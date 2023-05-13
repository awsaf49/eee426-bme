
#include<SoftwareSerial.h>
#include<ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial s(5,6);


PulseOximeter pox;
uint32_t tsLastReport = 0;

void onBeatDetected()
{

  Serial.println("Beat!!!");

}

void setup()
{
  s.begin(9600);
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("      Pluse");
  lcd.setCursor(0, 1);
  lcd.print("    Oximeter");
  delay(2000);

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  //pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
    
    pox.update();
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

      lcd.clear();
      lcd.setCursor(0 , 0);
      lcd.print("BPM : ");
      lcd.print(pox.getHeartRate());
      lcd.setCursor(0 , 1);
      lcd.print("Sp02: ");
      lcd.print(pox.getSpO2());
      lcd.print("%");
      tsLastReport = millis();
      
      root["data1"] = pox.getHeartRate();
      root["data2"] = pox.getSpO2();
      root.printTo(s);
      jsonBuffer.clear();
      Serial.flush();
      
    }
}
