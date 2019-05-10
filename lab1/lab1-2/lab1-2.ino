#define led 13
#define button 8
boolean ledState = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  pinMode(button,INPUT_PULLUP);
  digitalWrite(led,ledState);
}

void loop() {
  boolean btnState = digitalRead(button);
  if(btnState == LOW){
    ledState = !ledState;
    Serial.println(ledState);
    digitalWrite(led,ledState);
  }
}
