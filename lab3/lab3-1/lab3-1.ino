//#include  <SoftwareSerial.h>
//SoftwareSerial BTSerial(10, 11); // RX 、 TX
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);  // HC-05 baudrate, match serial in order to print correctly
  pinMode(5, OUTPUT);    // Set the led pin mode
  digitalWrite(5, LOW);
}
void loop()
{
  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (BTSerial.available()) {
    char recieve = BTSerial.read(); // get command from android via bluetooth
    Serial.println(recieve);
    if (recieve == '1') {           // If we set led to on
      digitalWrite(13, HIGH);
      BTSerial.write("LED ON\r\n");
    }
    else if (recieve == '0'){
      digitalWrite(13, LOW);
      BTSerial.write("LED OFF\r\n");
    } else{}
  }
}
