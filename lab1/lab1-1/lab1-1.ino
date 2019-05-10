int index = 0;
int offset = 1;
void setup() {
  DDRD = B01111100;
}
void loop() {
  PORTD = 0x04 << index;
  index = index + offset;
  if (index == 4){
    offset = -1;
  }
  if (index == 0){
    offset = 1;
  }
  delay(50);
}
