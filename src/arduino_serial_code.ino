
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

int UpperThreshold = 518;
int LowerThreshold = 490;
int reading = 0;
float BPM_ecg = 0.0;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;
const unsigned long delayTime = 10;
const unsigned long delayTime2 = 1000;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
unsigned long currentMillis = 0;

void onBeatDetected()
{

  Serial.println("Beat!!!");

}

int myTimer1(long delayTime, long currentMillis){
  if(currentMillis - previousMillis >= delayTime){previousMillis = currentMillis;return 1;}
  else{return 0;}
}

// Second event timer
int myTimer2(long delayTime2, long currentMillis){
  if(currentMillis - previousMillis2 >= delayTime2){previousMillis2 = currentMillis;return 1;}
  else{return 0;}
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
    
    currentMillis = millis();

    if(myTimer1(delayTime, currentMillis) == 1){
    
        reading = analogRead(0); 
    
        // Heart beat leading edge detected.
        if(reading > UpperThreshold && IgnoreReading == false){
          if(FirstPulseDetected == false){
            FirstPulseTime = millis();
            FirstPulseDetected = true;
          }
          else{
            SecondPulseTime = millis();
            PulseInterval = SecondPulseTime - FirstPulseTime;
            FirstPulseTime = SecondPulseTime;
          }
          IgnoreReading = true;
          digitalWrite(LED_BUILTIN, HIGH);
        }
    
        // Heart beat trailing edge detected.
        if(reading < LowerThreshold && IgnoreReading == true){
          IgnoreReading = false;
          digitalWrite(LED_BUILTIN, LOW);
        }  
    
        // Calculate Beats Per Minute.
        BPM_ecg = (1.0/PulseInterval) * 60.0 * 1000;
      
      }

    
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

      Serial.print(" BPM from ppg : ");
      //heartrate = root["data1"];
      Serial.print(pox.getHeartRate());
    
      Serial.print(" spo2 : ");
      //spo2 = root["data2"];
      Serial.print(pox.getSpO2());
      
      Serial.print(" BPM from ecg : ");
      //BPM_ecg = root["data3"];
      Serial.print(BPM_ecg);
      
      Serial.println(" ");

      lcd.clear();
      lcd.setCursor(0 , 0);
      lcd.print("BPM :");
      lcd.print(pox.getHeartRate());
      lcd.setCursor(0 , 1);
      lcd.print("Ox:");
      lcd.print(pox.getSpO2());
      lcd.print("%");
      lcd.setCursor(7, 1);
      lcd.print("BPMe:");
      lcd.print(BPM_ecg);
      Serial.println(" ");
      tsLastReport = millis();
      
      root["data1"] = pox.getHeartRate();
      root["data2"] = pox.getSpO2();
      root["data3"] = BPM_ecg;
      root.printTo(s);
      jsonBuffer.clear();
      Serial.flush();
      
    }
}
