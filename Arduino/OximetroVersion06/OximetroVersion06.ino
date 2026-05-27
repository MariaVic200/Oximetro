#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ======================================
// OLED
// ======================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ======================================
// SENSOR MAX30102
// ======================================
MAX30105 particleSensor;

// ======================================
// PINES
// ======================================
#define BUZZER_PIN 3
#define LED_PIN 2

// ======================================
// VARIABLES
// ======================================
long irValue;

float beatsPerMinute;
int beatAvg = 0;

byte rates[4];
byte rateSpot = 0;

long lastBeat = 0;

float spo2 = 0;

bool fingerDetected = false;

// Scroll
int scrollX = SCREEN_WIDTH;

// ======================================
// SETUP
// ======================================
void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // ======================================
  // INICIAR OLED
  // ======================================
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("ERROR OLED");

    while (1);
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(18, 10);
  display.println("INICIANDO");

  display.display();

  delay(1500);

  // ======================================
  // INICIAR SENSOR
  // ======================================
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    display.clearDisplay();

    display.setCursor(0, 10);
    display.println("MAX30102 ERROR");

    display.display();

    while (1);
  }

  // ======================================
  // CONFIGURACION SENSOR
  // ======================================
  particleSensor.setup();

  // Brillo LEDs
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeIR(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);

  display.clearDisplay();

  display.setCursor(30, 10);
  display.println("SENSOR OK");

  display.display();

  delay(1500);
}

// ======================================
// LOOP
// ======================================
void loop()
{
  // ======================================
  // LEER SENSOR
  // ======================================
  irValue = particleSensor.getIR();

  // ======================================
  // DETECTAR DEDO
  // ======================================
  if (irValue > 50000)
  {
    fingerDetected = true;
  }
  else
  {
    fingerDetected = false;
  }

  // ======================================
  // SIN DEDO
  // ======================================
  if (!fingerDetected)
  {
    display.clearDisplay();

    display.setTextSize(1);

    display.setCursor(20, 5);
    display.println("COLOQUE");

    display.setCursor(35, 18);
    display.println("DEDO");

    display.display();

    beatAvg = 0;
    spo2 = 0;

    scrollX = SCREEN_WIDTH;

    delay(50);

    return;
  }

  // ======================================
  // DETECCION DE LATIDO
  // ======================================
  if (checkForBeat(irValue))
  {
    long delta = millis() - lastBeat;

    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    // FILTRO
    if (beatsPerMinute < 220 && beatsPerMinute > 40)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;

      rateSpot %= 4;

      beatAvg = 0;

      for (byte x = 0; x < 4; x++)
      {
        beatAvg += rates[x];
      }

      beatAvg /= 4;

      // ======================================
      // BEEP
      // ======================================
      tone(BUZZER_PIN, 2000, 40);

      // ======================================
      // LED
      // ======================================
      digitalWrite(LED_PIN, HIGH);

      delay(20);

      digitalWrite(LED_PIN, LOW);
    }
  }

  // ======================================
  // CALCULO SIMPLE DE SPO2
  // ======================================
  spo2 = map(irValue, 50000, 100000, 90, 99);

  if (spo2 > 99)
  {
    spo2 = 99;
  }

  if (spo2 < 90)
  {
    spo2 = 90;
  }

  // ======================================
  // ESTADO CARDIACO
  // ======================================
  String estado = "";

  if (beatAvg < 60)
  {
    estado = "BRADICARDIA";
  }
  else if (beatAvg > 100)
  {
    estado = "TAQUICARDIA";
  }
  else
  {
    estado = "NORMAL";
  }

  // ======================================
  // TEXTO SCROLL
  // ======================================
  String mensaje =
    " BPM:" + String(beatAvg) +
    "   SPO2:" + String((int)spo2) + "%" +
    "   ESTADO:" + estado +
    "     ";

  // ======================================
  // MOSTRAR EN OLED
  // ======================================
  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(scrollX, 12);

  display.print(mensaje);

  display.display();

  // ======================================
  // MOVER SCROLL
  // ======================================
  scrollX -= 2;

  // Ancho aproximado del texto
  int textWidth = mensaje.length() * 6;

  // Reiniciar scroll
  if (scrollX < -textWidth)
  {
    scrollX = SCREEN_WIDTH;
  }

  // ======================================
  // SERIAL
  // ======================================
  Serial.print("IR=");
  Serial.print(irValue);

  Serial.print(" BPM=");
  Serial.print(beatAvg);

  Serial.print(" SPO2=");
  Serial.println(spo2);

  // ======================================
  // VELOCIDAD SCROLL
  // ======================================
  delay(40);
}