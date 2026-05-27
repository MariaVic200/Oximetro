
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

MAX30105 particleSensor;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

long irValue;
int bpm = 0;

void setup()
{
  Serial.begin(115200);

  // OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED no encontrada");
    while(1);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  // Sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    Serial.println("MAX30102 no encontrado");
    while (1);
  }

  particleSensor.setup();

  display.setTextSize(2);
  display.setCursor(10,20);
  display.println("OXIMETRO");
  display.display();

  delay(2000);
}

void loop()
{
  irValue = particleSensor.getIR();

  if (checkForBeat(irValue))
  {
    bpm = random(70,90); // simulacion simple
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Monitor Cardiaco");

  display.setTextSize(2);
  display.setCursor(0,20);
  display.print("BPM:");
  display.println(bpm);

  display.setTextSize(2);
  display.setCursor(0,45);
  display.print("SpO2:");
  display.print("98%");
  
  display.display();

  delay(100);
}
