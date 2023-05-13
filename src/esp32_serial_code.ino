#include<SoftwareSerial.h>
#include<ArduinoJson.h>

SoftwareSerial s(D6, D5);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  s.begin(9600);
}

void loop() {
  StaticJsonBuffer<1000>jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  //if(root == JsonObject ::invalid())
  //return;

  //Serial.println("JSON received and parsed");
  root.prettyPrintTo(s);
  
  Serial.print(" BPM : ");
  int BPM=root["data1"];
  Serial.print(BPM);
  
  Serial.print("    ");
  
 /* Serial.print("SPO2 : ");
  int SPO2=root["data2"];
  Serial.print(SPO2);
  */

  
  Serial.println("");
  Serial.println("");
  

}
