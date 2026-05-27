#include <Wire.h>
#include "MAX30105.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

MAX30105 particleSensor;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED no encontrada");
    while(1);
  }

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    Serial.println("MAX30102 no encontrado");
    while (1);
  }

  particleSensor.setup();

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.println("OXIMETRO");
  display.display();

  delay(2000);
}

void loop()
{
  long irValue = particleSensor.getIR();

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Sensor funcionando");

  display.setCursor(0,20);
  display.print("IR: ");
  display.println(irValue);

  display.display();

  delay(200);
}
