#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --------------------------
// CONFIGURACIÓN DE HARDWARE
// --------------------------
#define BuzzerPin     3
#define ModeButtonPin A0
#define LED_PIN       2

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

#define REPORTING_PERIOD_MS 5000 // Actualizar datos cada 5 segundos

// Objetos
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
PulseOximeter pox;

// --------------------------
// VARIABLES GLOBALES
// --------------------------
int mode = 0;                 // 0 = Modo Normal | 1 = Modo Analítico
int buttonOld = HIGH;
int buttonNew;

float sumHeartRate = 0;
float sumSpO2      = 0;
int count          = 0;
uint32_t tsLastReport = 0;

// Imagen de icono de dedo (matriz de píxeles)
const unsigned char PROGMEM finger[] = {
  0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x36,0x96,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x96,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x96,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x36,0xA0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0xA0,0xC0,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x02,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x30,0x02,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
  0x00,0x06,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0x00,0x00,0x00,0x00,0x00,
};

// --------------------------
// FUNCIONES
// --------------------------

// Se ejecuta cada vez que se detecta un latido
void onBeatDetected() {
  digitalWrite(LED_PIN, HIGH);
  tone(BuzzerPin, 2000, 50); // Sonido corto
  delay(50);
  digitalWrite(LED_PIN, LOW);
}

// Muestra mensaje e imagen para colocar el dedo
void displayFingerPrompt() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(18, 0);
  if (mode == 0) {
    display.print("Modo Normal");
  } else {
    display.print("Modo Analitico");
  }
  
  display.setCursor(18, 10);
  display.print("Coloque");
  display.setCursor(18, 20);
  display.print("su dedo");

  // Dibujar icono
  display.drawBitmap((display.width() - 84)/2 + 35, 
                     (display.height() - 22)/2 + 3, 
                     finger, 84, 22, WHITE);

  display.display();
}

// Reiniciar sensor al cambiar de modo
void resetSensor() {
  pox.shutdown();
  delay(100);
  pox.begin();
  pox.setIRLedCurrent(MAX30100_LED_CURR_11MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

// --------------------------
// SETUP
// --------------------------
void setup() {
  Serial.begin(115200);

  // Pines de entrada/salida
  pinMode(BuzzerPin, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(ModeButtonPin, INPUT_PULLUP);

  // Iniciar Pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED no encontrada"));
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 10);
  display.println("INICIANDO...");
  display.display();
  delay(2000);

  // Iniciar Sensor MAX30100
  if (!pox.begin()) {
    display.clearDisplay();
    display.setCursor(0,10);
    display.println("ERROR SENSOR");
    display.display();
    while(1);
  } else {
    Serial.println("Sensor OK");
  }

  // Configuración del sensor
  pox.setIRLedCurrent(MAX30100_LED_CURR_11MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

// --------------------------
// LOOP PRINCIPAL
// --------------------------
void loop() {
  // Lectura de botón para cambiar modo
  buttonNew = digitalRead(ModeButtonPin);
  if (buttonNew == LOW && buttonOld == HIGH) { // Flanco de bajada (presionado)
    mode = !mode;
    Serial.print("Modo cambiado a: ");
    Serial.println(mode == 0 ? "Normal" : "Analitico");
    resetSensor();
  }
  buttonOld = buttonNew;

  // Actualizar lecturas del sensor
  pox.update();

  float heartRate = pox.getHeartRate();
  float spo2      = pox.getSpO2();

  // Si no hay dedo o valores muy bajos, mostrar aviso
  if (heartRate < 30 || spo2 < 50) {
    displayFingerPrompt();
    sumHeartRate = 0;
    sumSpO2 = 0;
    count = 0;
    return;
  }

  // Acumular valores para promedio
  sumHeartRate += heartRate;
  sumSpO2 += spo2;
  count++;

  // Mostrar datos cada X milisegundos
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float avgHeartRate = (count > 0) ? sumHeartRate / count : 0;
    float avgSpO2      = (count > 0) ? sumSpO2 / count : 0;

    // --------------------------
    // MODO NORMAL
    // --------------------------
    if (mode == 0) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.print("BPM:");
      display.print(avgHeartRate, 0);

      display.setCursor(0, 18);
      display.print("O2:");
      display.print(avgSpO2, 0);
      display.print("%");
      display.display();
    }
    // --------------------------
    // MODO ANALÍTICO
    // --------------------------
    else {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("BPM:");
      display.print(avgHeartRate, 0);
      display.print(" | O2:");
      display.print(avgSpO2, 0);
      display.print("%");

      // Diagnóstico Frecuencia Cardíaca
      display.setTextSize(1);
      if (avgHeartRate < 60) {
        display.setCursor(20, 12);
        display.print("BRADICARDIA");
      } else if (avgHeartRate > 100) {
        display.setCursor(22, 12);
        display.print("TAQUICARDIA");
      } else {
        display.setCursor(15, 12);
        display.print("RITMO NORMAL");
      }

      // Diagnóstico Oxigenación
      if (avgSpO2 >= 95) {
        display.setCursor(10, 24);
        display.print("BUENA OXIGENACION");
      } else if (avgSpO2 >= 91 && avgSpO2 <= 94) {
        display.setCursor(15, 24);
        display.print("HIPOXIA LEVE");
      } else if (avgSpO2 >= 86 && avgSpO2 <= 90) {
        display.setCursor(12, 24);
        display.print("HIPOXIA MODERADA");
      } else if (avgSpO2 < 86 && avgSpO2 > 50) {
        display.setCursor(12, 24);
        display.print("HIPOXIA SEVERA");
      }
      
      display.display();
    }

    // Reiniciar acumuladores
    sumHeartRate = 0;
    sumSpO2      = 0;
    count        = 0;
    tsLastReport = millis();

    // Monitor Serial
    Serial.print("BPM Prom: "); Serial.print(avgHeartRate);
    Serial.print(" | SpO2 Prom: "); Serial.println(avgSpO2);
  }
}