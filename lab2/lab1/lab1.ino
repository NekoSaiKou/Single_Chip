boolean ledState = LOW;
int SEG_COM[4] = {10,11,12,13};
int SEG_Data[10][7] = {{1,1,1,1,1,1,0},//0
                       {0,1,1,0,0,0,0},//1
                       {1,1,0,1,1,0,1},//2
                       {1,1,1,1,0,0,1},//3
                       {0,1,1,0,0,1,1},//4
                       {1,0,1,1,0,1,1},//5
                       {1,0,1,1,1,1,0},//6
                       {1,1,1,0,0,0,0},//7
                       {1,1,1,1,1,1,1},//8  
                       {1,1,1,0,0,1,1},//9
  };
int number = 2019;
int disp[4] = {0,0,0,0};
byte i = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for(int i=2; i<=13; i++)
  {
   pinMode(i,OUTPUT);
   digitalWrite(i,HIGH);   
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Number_Transfer(number);
  for(i=0;i<=3;i++)
  {
    digitalWrite(SEG_COM[i],LOW);
    SEG_Drive(disp[i]);
    
    delay(5);
    digitalWrite(SEG_COM[i],HIGH);
  }
  
}
void Number_Transfer(int Num)
{
 disp[0] = Num % 10;
 Num = Num/10;
 disp[1] = Num  % 10;
 Num = Num/10;
 disp[2] = Num % 10;
 Num = Num/10;
 disp[3] = Num % 10; 
}
void SEG_Drive(int Number)
{ 
  for(int i=0;i<=6;i++)
   digitalWrite(i+2,SEG_Data[Number][i]);
  Serial.print(Number);
}
