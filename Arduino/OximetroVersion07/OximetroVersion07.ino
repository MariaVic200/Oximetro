#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// OLED
// =====================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =====================================================
// SENSOR MAX30102
// =====================================================
MAX30105 particleSensor;

// =====================================================
// PINES
// =====================================================
#define BUZZER_PIN 3
#define LED_PIN 2

// =====================================================
// VARIABLES
// =====================================================
long irValue;

float beatsPerMinute;
int beatAvg = 0;

byte rates[4];
byte rateSpot = 0;

long lastBeat = 0;

float spo2 = 0;

bool fingerDetected = false;

// Scroll horizontal
int scrollX = SCREEN_WIDTH;

// =====================================================
// SETUP
// =====================================================
void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // =====================================================
  // INICIAR OLED
  // =====================================================
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("ERROR OLED");

    while (1);
  }
  display.setRotation(2);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(20, 10);
  display.println("INICIANDO");

  display.display();

  delay(1500);

  // =====================================================
  // INICIAR SENSOR
  // =====================================================
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    display.clearDisplay();

    display.setCursor(0, 10);
    display.println("ERROR MAX30102");

    display.display();

    while (1);
  }

  // =====================================================
  // CONFIGURACION SENSOR
  // =====================================================
  particleSensor.setup();

  // POTENCIA LEDs
  particleSensor.setPulseAmplitudeRed(0x7F);
  particleSensor.setPulseAmplitudeIR(0x7F);
  particleSensor.setPulseAmplitudeGreen(0);

  display.clearDisplay();

  display.setCursor(28, 10);
  display.println("SENSOR OK");

  display.display();

  delay(1500);
}

// =====================================================
// LOOP PRINCIPAL
// =====================================================
void loop()
{
  // =====================================================
  // LEER SENSOR
  // =====================================================
  irValue = particleSensor.getIR();

  // =====================================================
  // DETECCION DE DEDO
  // =====================================================
  if (irValue > 30000)
  {
    fingerDetected = true;
  }
  else
  {
    fingerDetected = false;
  }

  // =====================================================
  // SI NO HAY DEDO
  // =====================================================
  if (!fingerDetected)
  {
    display.clearDisplay();

    display.setTextSize(1);

    display.setCursor(20, 8);
    display.println("COLOQUE");

    display.setCursor(38, 20);
    display.println("DEDO");

    display.display();

    beatAvg = 0;
    spo2 = 0;

    scrollX = SCREEN_WIDTH;

    delay(50);

    return;
  }

  // =====================================================
  // ESTABILIZAR BPM
  // =====================================================
  static long lastValidIR = 0;

  if (irValue > 2000)
  {
    lastValidIR = irValue;
  }

  // =====================================================
  // DETECTAR LATIDO
  // =====================================================
  if (checkForBeat(lastValidIR))
  {
    long delta = millis() - lastBeat;

    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    // FILTRO BPM
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

      // =====================================================
      // BEEP
      // =====================================================
      tone(BUZZER_PIN, 2000, 40);

      // =====================================================
      // LED
      // =====================================================
      digitalWrite(LED_PIN, HIGH);

      delay(20);

      digitalWrite(LED_PIN, LOW);
    }
  }

  // =====================================================
  // CALCULO SIMPLE SPO2
  // =====================================================
  spo2 = map(irValue, 30000, 120000, 90, 99);

  if (spo2 > 99)
  {
    spo2 = 99;
  }

  if (spo2 < 90)
  {
    spo2 = 90;
  }

  // =====================================================
  // ESTADO CARDIACO
  // =====================================================
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

  // =====================================================
  // TEXTO PARA SCROLL
  // =====================================================
  String mensaje =
    " BPM:" + String(beatAvg) +
    "   SPO2:" + String((int)spo2) + "%" +
    "   ESTADO:" + estado +
    "     ";

  // =====================================================
  // OLED
  // =====================================================
 display.clearDisplay();
 display.setTextSize(1);
 // BPM
 display.setCursor(0,0);
 display.print("BPM:");
 display.print(beatAvg);
 //SPO2
 display.setCursor(70,0);
 display.print("Spo2:");
 if(spo2 > 70 && spo2 <= 100)
 {
  display.print((int)spo2);
 display.print("%");
 }
else
{ display.print("---");
}
 display.setCursor(0,20);
 display.print(estado);

 display.setcursor(70,20);
display.print(irValue);
 display.display();
  // =====================================================
  // MONITOR SERIAL
  // =====================================================
  Serial.print("IR=");
  Serial.print(irValue);

  Serial.print(" BPM=");
  Serial.print(beatAvg);

  Serial.print(" SPO2=");
  Serial.println(spo2);

  // =====================================================
  // VELOCIDAD SCROLL
  // =====================================================
  delay(40);
}