#define led 13
#define button 8
boolean ledState = LOW;
boolean lastBTNstate = LOW;
long lastBTNtime = 0;
boolean toogle = false;

void setup() {
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  pinMode(button,INPUT_PULLUP);
  digitalWrite(led,ledState);
}

void loop() {
  boolean btnState = digitalRead(button);
  if (lastBTNstate != btnState){
    lastBTNstate = btnState;
    lastBTNtime = millis();
  }
  if(btnState == LOW and lastBTNstate == LOW){
    if(millis()-lastBTNtime > 50){
      if(toogle == false){
        toogle = true;
        ledState = !ledState;
        Serial.println(ledState);
        digitalWrite(led,ledState);
      }
    }
  }
  else{
    toogle = false;
  }
}
