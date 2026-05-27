#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =========================
// OLED
// =========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =========================
// SENSOR
// =========================
MAX30105 particleSensor;

// =========================
// PINES
// =========================
#define BUZZER_PIN 3
#define LED_PIN 2

// =========================
// VARIABLES
// =========================
long irValue;

float beatsPerMinute;
int beatAvg;

byte rates[4];
byte rateSpot = 0;

long lastBeat = 0;

float spo2 = 0;

bool fingerDetected = false;

// =========================
// SETUP
// =========================
void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED ERROR");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(15, 10);
  display.println("INICIANDO...");
  display.display();

  delay(1000);

  // SENSOR
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("MAX30102 ERROR");
    display.display();

    while (1);
  }

  // CONFIGURACION SENSOR
  particleSensor.setup();

  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeIR(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);

  display.clearDisplay();
  display.setCursor(10, 10);
  display.println("SENSOR OK");
  display.display();

  delay(1000);
}

// =========================
// LOOP
// =========================
void loop()
{
  irValue = particleSensor.getIR();

  // =========================
  // DETECCION DE DEDO
  // =========================
  if (irValue > 50000)
  {
    fingerDetected = true;
  }
  else
  {
    fingerDetected = false;
  }

  // =========================
  // SIN DEDO
  // =========================
  if (!fingerDetected)
  {
    display.clearDisplay();

    display.setTextSize(1);

    display.setCursor(18, 5);
    display.println("COLOQUE DEDO");

    display.setCursor(28, 18);
    display.println("MAX30102");

    display.display();

    beatAvg = 0;
    spo2 = 0;

    return;
  }

  // =========================
  // DETECTAR LATIDO
  // =========================
  if (checkForBeat(irValue))
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 220 && beatsPerMinute > 40)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= 4;

      beatAvg = 0;

      for (byte x = 0; x < 4; x++)
        beatAvg += rates[x];

      beatAvg /= 4;

      // BEEP
      tone(BUZZER_PIN, 2000, 40);

      // LED
      digitalWrite(LED_PIN, HIGH);
      delay(20);
      digitalWrite(LED_PIN, LOW);
    }
  }

  // =========================
  // CALCULO SIMPLE SpO2
  // =========================
  spo2 = map(irValue, 50000, 100000, 90, 99);

  if (spo2 > 99)
    spo2 = 99;

  if (spo2 < 90)
    spo2 = 90;

  // =========================
  // OLED
  // =========================
  display.clearDisplay();

  // TITULO
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print("PULSIOXIMETRO");

  // BPM
  display.setTextSize(2);

  display.setCursor(0, 12);
  display.print(beatAvg);

  display.setTextSize(1);

  display.setCursor(55, 18);
  display.print("BPM");

  // OXIGENO
  display.setTextSize(2);

  display.setCursor(80, 12);
  display.print((int)spo2);

  display.setTextSize(1);

  display.print("%");

  // =========================
  // ESTADO
  // =========================
  display.setTextSize(1);

  display.setCursor(0, 26);

  if (beatAvg < 60)
  {
    display.print("BRADICARDIA");
  }
  else if (beatAvg > 100)
  {
    display.print("TAQUICARDIA");
  }
  else
  {
    display.print("NORMAL");
  }

  display.display();

  // SERIAL
  Serial.print("IR=");
  Serial.print(irValue);

  Serial.print(" BPM=");
  Serial.print(beatAvg);

  Serial.print(" SpO2=");
  Serial.println(spo2);

  delay(30);
}