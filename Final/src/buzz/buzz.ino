#define Do  523
#define Re  587
#define Mi  659
#define Fa  698
#define So  784
#define La  880
#define Si  988
int melody[7] = {Do, Re, Mi, Fa, So, La, Si};
const int buzzer = 9;
void setup(){
  pinMode(buzzer, OUTPUT);
}
void loop() {  
  for (int i = 0;i < 8; i++) {
    tone(buzzer, melody[i]);
    delay(1000);  
  }
  noTone(buzzer);//不發出聲音
  delay(2000);
}