#include <Wire.h>

#define SLAVE_ADDRESS 0x12
#define BAUDRATE 115200
#define BUTTON A0
void setup() {
  Wire.begin();
  Serial.begin(BAUDRATE);
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.println("Master Start \n");
}

void sendData(int data){
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(data);
  Wire.endTransmission();
}

void loop() {
  if(digitalRead(BUTTON))   // get button status 
    sendData(0);
  else
    sendData(1);
}
