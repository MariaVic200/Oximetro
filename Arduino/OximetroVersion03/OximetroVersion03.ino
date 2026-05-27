#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =========================
// CONFIGURACIÓN OLED
// =========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =========================
// CONFIGURACIÓN SENSOR MAX30102
// =========================
MAX30105 particleSensor;

// =========================
// VARIABLES PARA RITMO CARDIACO (BPM)
// =========================
const byte RATE_SIZE = 4; // Cantidad de muestras para promediar
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

// =========================
// VARIABLES PARA OXÍGENO EN SANGRE (SpO2)
// =========================
const byte SPO2_SIZE = 4;
float spo2Values[SPO2_SIZE];
byte spo2Spot = 0;
float redValue, irValue;
float ratio, spo2, spo2Avg;

// =========================
// SETUP
// =========================
void setup() {
  Serial.begin(115200);

  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error: Pantalla OLED no encontrada");
    while (1);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Inicializar sensor MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("ERROR SENSOR");
    display.display();
    while (1);
  }

  // Configuración óptima del sensor
  particleSensor.setup(
    60,      // Potencia del LED (0=mín, 255=máx)
    2,       // Modo: 2 = solo Rojo + Infrarrojo
    100,     // Tasa de muestreo (100 muestras/segundo)
    411,     // Tiempo de integración del LED
    16       // Frecuencia de pulso
  );

  // Pantalla de inicio
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("OXIMETRO");
  display.display();
  delay(2000);
}

// =========================
// FUNCIÓN PARA CALCULAR SpO2
// =========================
float calcularSpO2(float red, float ir) {
  if (ir <= 0 || red <= 0) return 0;

  // Fórmula estándar para estimación de SpO2
  ratio = (red / ir);
  float spO2 = 110 - (25 * ratio);

  // Limitar valores dentro de rangos fisiológicos
  if (spO2 > 100) spO2 = 100;
  if (spO2 < 0) spO2 = 0;

  return spO2;
}

// =========================
// LOOP PRINCIPAL
// =========================
void loop() {
  // Leer valores del sensor
  redValue = particleSensor.getRed();
  irValue = particleSensor.getIR();

  // Verificar si hay dedo colocado
  boolean hayDedo = (irValue > 50000);

  if (hayDedo) {
    // --------------------------
    // Cálculo de BPM
    // --------------------------
    if (checkForBeat(irValue)) {
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute > 20 && beatsPerMinute < 250) {
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;

        // Calcular promedio de BPM
        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++) {
          beatAvg += rates[x];
        }
        beatAvg /= RATE_SIZE;
      }
    }

    // --------------------------
    // Cálculo de SpO2
    // --------------------------
    spo2 = calcularSpO2(redValue, irValue);

    if (spo2 > 0) {
      spo2Values[spo2Spot++] = spo2;
      spo2Spot %= SPO2_SIZE;

      // Calcular promedio de SpO2
      spo2Avg = 0;
      for (byte x = 0; x < SPO2_SIZE; x++) {
        spo2Avg += spo2Values[x];
      }
      spo2Avg /= SPO2_SIZE;
    }

  } else {
    // Reiniciar valores si no hay dedo
    beatAvg = 0;
    spo2Avg = 0;
  }

  // =========================
  // ACTUALIZAR PANTALLA OLED
  // =========================
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("BIOSENSOR MAX30102");

  // Mostrar BPM
  display.setTextSize(2);
  display.setCursor(0, 20);
  if (beatAvg > 0) {
    display.print("BPM:");
    display.print(beatAvg);
  } else {
    display.print("BPM: --");
  }

  // Mostrar SpO2
  display.setCursor(70, 20);
  if (spo2Avg > 0) {
    display.print("O2:");
    display.print((int)spo2Avg);
    display.print("%");
  } else {
    display.print("O2: --");
  }

  // Mensaje de estado
  display.setTextSize(1);
  display.setCursor(0, 50);
  if (!hayDedo) {
    display.println("→ Ponga el dedo");
  } else {
    display.println("→ Leyendo datos...");
  }

  display.display();

  // =========================
  // SALIDA MONITOR SERIAL
  // =========================
  Serial.print("IR:");
  Serial.print(irValue);
  Serial.print(" | Rojo:");
  Serial.print(redValue);
  Serial.print(" | BPM:");
  Serial.print(beatAvg);
  Serial.print(" | SpO2:");
  Serial.println(spo2Avg, 0);

  delay(20);
}