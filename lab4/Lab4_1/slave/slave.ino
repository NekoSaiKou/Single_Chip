#include <Wire.h>

#define SLAVE_ADDRESS 0x12
#define BAUDRATE 115200
void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(getData);
  Serial.begin(BAUDRATE);
  Serial.println("Slave Start \n");
}

void getData(){
  if( Wire.available() ){
    while ( Wire.available() )
      Serial.print((char) Wire.read());
    Serial.print("\n");
  }
}

void loop() {
  getData();
}
