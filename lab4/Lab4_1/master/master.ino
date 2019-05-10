#include <Wire.h>

#define SLAVE_ADDRESS 0x12
#define BAUDRATE 115200
void setup() {
  Wire.begin();
  Serial.begin(BAUDRATE);
  Serial.println("Master Start \n");
}

// Used to send data to slave
void sendData(int *data){
  Wire.beginTransmission(SLAVE_ADDRESS);
    Wire.write("");
  
  for(int i = 0; i<40; i++) {
    Serial.print((char)data[i]);
    Wire.write(data[i]);
  }
  Serial.println("");
  Wire.endTransmission();
}

void loop() {
  int data[40] = {};
  int i =0;
  //get input
  if(Serial.available()){
    delay(1);   //serial need time to get all the stuff into buffer
    while (Serial.available() ) {
      data[i] = Serial.read();
      i++;
    }
    sendData(data);
  }
}
