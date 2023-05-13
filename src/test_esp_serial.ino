#include<SoftwareSerial.h>
#include<ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#define Thingspeak_report_ms 15000

WiFiClient  client;
SoftwareSerial s(D6, D5);

char* ssid = "AR3FIN";   // your network SSID (name) 
char* password = "12345678";   // your network password

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "9VD28QNK3Y7LKVAQ";
uint32_t tsLastReportTS=0;
float spo2;
float heartrate;
float BPM_ecg;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  s.begin(9600);

  ThingSpeak.begin(client);  // Initialize ThingSpeak
  if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
          WiFi.begin(ssid, password); 
          delay(5000);     
      } 
      Serial.println("\nConnected.");
  }
 //WiFi.begin(ssid, password); 
}

void loop() {
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  //if(root == JsonObject ::invalid())
  //return;

  //Serial.println("JSON received and parsed");
  root.prettyPrintTo(s);

  Serial.print(" BPM from ppg : ");
  heartrate = root["data1"];
  Serial.print(heartrate);

  Serial.print(" spo2 : ");
  spo2 = root["data2"];
  Serial.print(spo2);
  
  Serial.print(" BPM from ecg : ");
  BPM_ecg = root["data3"];
  Serial.print(BPM_ecg);
  
  Serial.println(" ");
  
  if ((millis() - tsLastReportTS) > Thingspeak_report_ms) {  
      ThingSpeak.setField(1, heartrate);
      ThingSpeak.setField(2, spo2);
      ThingSpeak.setField(3, BPM_ecg);  
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      
      if (x == 200){
          Serial.println("Channel update successful.");
      }
      else{
          Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
      tsLastReportTS = millis();
  }
}
