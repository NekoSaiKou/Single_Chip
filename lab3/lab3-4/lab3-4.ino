#include <Keypad.h>
#include "Password.h"
#include <string.h>

#define input_pwd 0
#define reset_pwd 1
#include<math.h>

/* Seven-seg setup */
//set port 10,11,12,13 to be the ground of the 8-segment led display
int SEG_COM[4] = {10,11,12,13};
//for each SEG_data represent the port needed to set high to make led display the number respectively
int SEG_Data[10][7] = {{1,1,1,1,1,1,0},//0
                       {0,1,1,0,0,0,0},//1
                       {1,1,0,1,1,0,1},//2
                       {1,1,1,1,0,0,1},//3
                       {0,1,1,0,0,1,1},//4
                       {1,0,1,1,0,1,1},//5
                       {1,0,1,1,1,1,1},//6
                       {1,1,1,0,0,0,0},//7
                       {1,1,1,1,1,1,1},//8  
                       {1,1,1,0,0,1,1},//9
  };
int good[4][7] = {{0,1,1,1,1,0,1},//d
                {0,0,1,1,1,0,1},//o
                {0,0,1,1,1,0,1},//o
                {1,0,1,1,1,1,0}//G
                };
int bad[4][7] = {
               {0,1,1,1,1,0,1},//d
               {1,1,1,0,1,1,1},//a
               {0,0,1,1,1,1,1},//b
               
               {0,0,0,0,0,0,0},//
                };
                
// the array that stores the number coresponding to each 8-segment led display
int disp[4] = {0,0,0,0};

//Keyboard setting
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {{'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'}};
byte rowPins[ROWS] = {A5, A4, A3, A2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A1, A0, 9, 1}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Password setting
Password pwd = Password("7711");
String key_in_string = "";
char charBuf[4] = "";
int number_converted = 0;
int mode = 0;
int input_status = input_pwd;

void setup(){
  for(int i=2; i<=13; i++)
  {
   if(i ==9){
    continue;}
   pinMode(i,OUTPUT);
   digitalWrite(i,HIGH);   
  }
}

void loop(){
  char key = keypad.getKey();
  if (key != NO_KEY){
    switch(key){
      //Enter
      case 'A':
        /* If in normal mode then check password */
        if(input_status == input_pwd){
          if (!True_OR_False()){
            pwd.reset();
            mode = -2;/* Print 'bad' with seven-seg */
          }
          else{
            mode = -1;/* Print 'good' with seven-seg */
          }
        }
        /* If in setting mode then change password */
        else{
          /* We change state into normal mode after modify the password. */
          input_status = input_pwd;
          /* To use password library, we should convert string type data into character array type */
          key_in_string.toCharArray(charBuf,5);
          pwd.set(charBuf);
          pwd.reset();
        }
        /* Clear input data */
        key_in_string = "";
        number_converted = 0;
        break;
      //Erase and enter again
      case 'B':
        pwd.reset();
        /* Clear input data */
        key_in_string = "";
        number_converted = 0;
        break;
      //Reset Password
      case 'C':
        input_status = reset_pwd;
        /* Clear input data */
        key_in_string = "";
        number_converted = 0;
        break;
      default:
      /* Set sevenseg print mode into normal mode */
      mode = 0;
      if ( key_in_string == ""){
        key_in_string =  key;
      }else{
        key_in_string = key_in_string + key;
      }
      number_converted = key_in_string.toInt();
      if(input_status == input_pwd){
        keyNumber(key);
        }
    }
  }
  Number_Transfer(number_converted);
  led_Drive(mode,number_converted); 
}
void keyNumber(char key){
  pwd.append(key);
}
bool True_OR_False(){
  bool a = pwd.evaluate();
  return a;
}


//transfer the Number to seperated interger and store into array
void Number_Transfer(int Num)
{   
 for (int i=0;i<=3;i++){
   disp[i] = Num%10;
   Num = Num/10;}
}
void SEG_Drive(int Number)
{  //according to the number we choose correspond to the number index we want to display
  // and write each output pin attached to leddisplay to HIGH
  for(int i=0;i<=6;i++)
   digitalWrite(i+2,SEG_Data[Number][i]);}
 
void Good_Drive(int Number)
{  //according to the number we choose correspond to the number index we want to display
  // and write each output pin attached to leddisplay to HIGH
  for(int i=0;i<=6;i++)
   digitalWrite(i+2,good[Number][i]);}
   
void bad_Drive(int Number)
{  //according to the number we choose correspond to the number index we want to display
  // and write each output pin attached to leddisplay to HIGH
  for(int i=0;i<=6;i++)
   digitalWrite(i+2,bad[Number][i]);}
   
void led_Drive(int mode,int number){
  for(int i=0;i<=3;i++)
  {
     if(mode>=0){
      if (i > key_in_string.length()-1){
        continue;
      }
     }
        
    //select the led diaplay by set the Common Cathode led gound to LOW
    digitalWrite(SEG_COM[i],LOW);
    if(mode==-1)
      Good_Drive(i);
    else if(mode == -2)
      bad_Drive(i);
    else
      SEG_Drive(disp[i]);
    delay(5);
    digitalWrite(SEG_COM[i],HIGH);
  }
}


