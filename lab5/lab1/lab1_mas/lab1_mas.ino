#include <SPI.h>
void setup() {
  
  Serial.begin(115200);
    Serial.println("in master");
  // put your setup code here, to run once:
  digitalWrite(SS, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  char c;
  digitalWrite(SS, LOW);

  for(const char *p = "Hello, World!\n" ; c = *p; p++)  {
    SPI.transfer(c);
//    delay(10);
  }

   digitalWrite(SS, HIGH);
   delay(1000);
  
}
