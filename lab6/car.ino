#define IN1 2
#define IN2 4
#define IN3 5
#define IN4 6
#define ENA 3
#define ENB 9
//define HC_SR04 PIN
#define ECHOPIN_L 7        // Orange line
#define TRIGPIN_L 8        // Red line

#define ECHOPIN_R 10        // Orange line
#define TRIGPIN_R 11
float rotary;
int right_cm,left_cm;
void setup() {
  // put your setup code here, to run once:
  pinMode(IN1,OUTPUT);pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);pinMode(ENB,OUTPUT);

  analogWrite(ENA,100); analogWrite(ENB,100); 
  
  Serial.begin (115200);
  //define HC_SR04 PIN
  pinMode(ECHOPIN_R, INPUT);
  pinMode(TRIGPIN_R, OUTPUT);
  
  pinMode(TRIGPIN_L, OUTPUT);
  pinMode(ECHOPIN_L, INPUT);
}

void ultrasound(void){
  //digitalWrite(TRIGPIN_L, LOW); 
  digitalWrite(TRIGPIN_R, LOW);
  delayMicroseconds(2); 
  //digitalWrite(TRIGPIN_L, HIGH); 
  digitalWrite(TRIGPIN_R, HIGH);
  delayMicroseconds(10); 
  //digitalWrite(TRIGPIN_L, LOW);
  digitalWrite(TRIGPIN_R, LOW);
  
  // Distance Calculation
  float timestop_R = pulseIn(ECHOPIN_R, HIGH);
 

  
  digitalWrite(TRIGPIN_L, LOW); 
  delayMicroseconds(2); 
  digitalWrite(TRIGPIN_L, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TRIGPIN_L, LOW);
  
  float timestop_L = pulseIn(ECHOPIN_L, HIGH);
  
  
  left_cm = (timestop_L *0.0336) / 2; // return in microseconds, 1cm requires 29.4ms
  right_cm = (timestop_R *0.0336) / 2; // return in microseconds, 1cm requires 29.4ms
  //if (left_cm>=100 ) left_cm = 100;
  if (right_cm>=100 ) right_cm = left_cm;   
  }
void motor_stop(void){
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
  }
void motor_backward(void){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
  }
void motor_forward(void){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  }
void motor_turn_right(void){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  }
void motor_turn_left(void){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
  }  
void loop() {
  // put your main code here, to run repeatedly:
  ultrasound();
  analogWrite(ENA,100); analogWrite(ENB,100);
  Serial.print(" right_cm :");
  Serial.print(right_cm);
  Serial.print(" left_cm :");
  Serial.print(left_cm);
  Serial.print(" distance :");
  Serial.print(rotary);  
  if((left_cm>50)&&(right_cm>50)){
    motor_stop();
  }else if((left_cm<=50&&left_cm>=20)&&(right_cm<=50&&right_cm>=20)){
    motor_forward();
  }else if((left_cm<20 &&left_cm >=10)&&(right_cm<20&&right_cm>=10)){
    motor_stop();
  }else if((left_cm<10)||(right_cm<10)){
    motor_backward();
  }
  rotary = left_cm - right_cm;
  
  
  if(abs(rotary)>30){
    analogWrite(ENA,50); analogWrite(ENB,50);
    if(rotary>0){
      motor_turn_right();
        Serial.print("turn right ");
      }else if(rotary<0){
        motor_turn_left();
        Serial.print("turn left ");
    } 
  }
  Serial.println();
}
