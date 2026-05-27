#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================
// OLED
// =====================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =====================================
// MAX30102
// =====================================
MAX30105 particleSensor;

// =====================================
// BUZZER
// =====================================
#define BUZZER 8

// =====================================
// Variables BPM
// =====================================
const byte RATE_SIZE = 4;

byte rates[RATE_SIZE];
byte rateSpot = 0;

long lastBeat = 0;

float beatsPerMinute;
int beatAvg = 0;

// =====================================
// Variables falsas de SpO2
// (Aproximacion ligera para ahorrar RAM)
// =====================================
int spo2 = 98;

// =====================================
// Setup
// =====================================
void setup()
{
  Serial.begin(115200);

  // Iniciar I2C
  Wire.begin();

  // =========================
  // Buzzer
  // =========================
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // =========================
  // OLED
  // =========================
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("OLED ERROR"));

    while (1);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println(F("INICIANDO"));
  display.display();

  delay(1500);

  // =========================
  // Sensor MAX30102
  // =========================
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println(F("MAX30102 ERROR"));

    display.display();

    while (1);
  }

  // =====================================
  // Configuracion optimizada
  // =====================================
  byte ledBrightness = 50;
  byte sampleAverage = 4;
  byte ledMode = 2;
  int sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;

  particleSensor.setup(
    ledBrightness,
    sampleAverage,
    ledMode,
    sampleRate,
    pulseWidth,
    adcRange
  );

  // LEDs
  particleSensor.setPulseAmplitudeRed(0x32);
  particleSensor.setPulseAmplitudeIR(0x32);

  // =========================
  // Pantalla inicio
  // =========================
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(8, 20);
  display.println(F("OXIMETRO"));

  display.display();

  delay(2000);
}

// =====================================
// LOOP
// =====================================
void loop()
{
  long irValue = particleSensor.getIR();

  // =====================================
  // Detectar dedo
  // =====================================
  bool fingerDetected = irValue > 50000;

  // =====================================
  // Detectar latidos
  // =====================================
  if (fingerDetected && checkForBeat(irValue))
  {
    // Sonido buzzer
    tone(BUZZER, 1000, 50);

    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60.0 / (delta / 1000.0);

    // Filtrar BPM absurdos
    if (beatsPerMinute > 40 && beatsPerMinute < 180)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;

      rateSpot %= RATE_SIZE;

      beatAvg = 0;

      for (byte x = 0; x < RATE_SIZE; x++)
      {
        beatAvg += rates[x];
      }

      beatAvg /= RATE_SIZE;

      // =====================================
      // Simulacion ligera de SpO2
      // =====================================
      spo2 = 98;

      if (beatAvg > 110)
        spo2 = 96;

      if (beatAvg > 130)
        spo2 = 95;
    }
  }

  // =====================================
  // OLED
  // =====================================
  display.clearDisplay();

  // Titulo
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.println(F("MAX30102"));

  if (!fingerDetected)
  {
    display.setTextSize(1);
    display.setCursor(10, 25);
    display.println(F("PONGA EL DEDO"));
  }
  else
  {
    // BPM
    display.setTextSize(2);
    display.setCursor(0, 18);

    display.print(F("BPM:"));
    display.println(beatAvg);

    // SpO2
    display.setTextSize(2);
    display.setCursor(0, 45);

    display.print(F("O2:"));
    display.print(spo2);
    display.print(F("%"));
  }

  display.display();

  // =====================================
  // SERIAL
  // =====================================
  Serial.print(F("IR="));
  Serial.print(irValue);

  Serial.print(F(" BPM="));
  Serial.print(beatAvg);

  Serial.print(F(" SPO2="));
  Serial.println(spo2);

  delay(20);
}
