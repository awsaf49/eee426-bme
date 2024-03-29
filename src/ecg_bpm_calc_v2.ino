
// -------------ECG Part------------------
int UpperThreshold = 518;
int LowerThreshold = 490;
int reading = 0;
float BPM = 0.0;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;
const unsigned long delayTime = 10;
const unsigned long delayTime2 = 1000;
const unsigned long baudRate = 9600;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

const int BufferSize = 3;
double Buffer[BufferSize] = {0};
int BufferIdx = 0;

void setup(){
  Serial.begin(baudRate);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop(){

  // Get current time
  unsigned long currentMillis = millis();

  // First event
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
        Buffer[BufferIdx] = (double)(PulseInterval);
        BufferIdx += 1;
        if (BufferIdx >= BufferSize){
          BufferIdx = 0;
        }
      }
      IgnoreReading = true;
      digitalWrite(LED_BUILTIN, HIGH);
    }

    // Heart beat trailing edge detected.
    if(reading < LowerThreshold && IgnoreReading == true){
      IgnoreReading = false;
      digitalWrite(LED_BUILTIN, LOW);
    }  

    // Calculate Average Pulse Interval
    double AvgPulseInterval = 0;
    for (int i = 0; i < BufferSize; i++){
      AvgPulseInterval += (Buffer[i] / BufferSize);
    }

    // Calculate Beats Per Minute.
    BPM = (1.0/AvgPulseInterval) * 60.0 * 1000;
  }

  // Second event
  // Showing everything in Serial Monitor
  if(myTimer2(delayTime2, currentMillis) == 1){
    Serial.print(reading);
    Serial.print("\t");
    Serial.print(PulseInterval);
    Serial.print("\t");
    Serial.print(BPM);
    Serial.println(" BPM");
    Serial.flush();
  }
}

// First event timer
int myTimer1(long delayTime, long currentMillis){
  if(currentMillis - previousMillis >= delayTime){previousMillis = currentMillis;return 1;}
  else{return 0;}
}

// Second event timer
int myTimer2(long delayTime2, long currentMillis){
  if(currentMillis - previousMillis2 >= delayTime2){previousMillis2 = currentMillis;return 1;}
  else{return 0;}
}
