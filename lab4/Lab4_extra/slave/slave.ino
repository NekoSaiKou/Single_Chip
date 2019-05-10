#include <Wire.h>

#define SLAVE_ADDRESS 0x12
#define BAUDRATE 115200
#define LED_PIN 9
int message=0;
void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(getData);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  Serial.begin(BAUDRATE);
  Serial.println("Slave Start \n");
}

void getData(int i){
  if( Wire.available() )
    message = Wire.read();  // get message
  else
    return 0;
}

void loop() {
  if(message)   //if message is 1, light up the LED
    digitalWrite(LED_PIN, HIGH);
  else
    digitalWrite(LED_PIN, LOW);
}
