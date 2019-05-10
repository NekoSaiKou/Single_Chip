#include <Keypad.h>
#include "Password.h"
#include <string.h>

#define input_pwd 0
#define reset_pwd 1

int input_status = input_pwd;
//Keyboard setting
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {{'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'}};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Password setting
byte passwordlength = 4;
Password pwd = Password("0711");
String key_in_string = "";
char charBuf[4] = "";

void setup(){
  Serial.begin(115200);
  Serial.println("請輸入密碼");
}

void loop(){
  char key = keypad.getKey();
  if (key != NO_KEY){
    switch(key){
      case 'A':
        if(input_status == input_pwd){
          bool pwd_pass = True_OR_False();
          if (!pwd_pass){
            pwd.reset();
            Serial.println("\nWrong passwords!");
          }
          else{
            Serial.println("\nUnlock!");
          }
        }else{
          Serial.print("\n密碼已變更");
          Serial.println(key_in_string);
          input_status = input_pwd;
          key_in_string.toCharArray(charBuf,5);
          pwd.set(charBuf);
          pwd.reset();
        }
        break;
      case 'B':
        Serial.println("\n請重新輸入");
        pwd.reset();
        key_in_string = "";
        break;
       case 'C':
        Serial.println("\n請設定新的密碼");
        input_status = reset_pwd;
        key_in_string = "";
        break;
      default:
      Serial.print(key);
      if ( key_in_string == ""){
        key_in_string =  key;
      }else{
        key_in_string = key_in_string + key;
      }
      if(input_status == input_pwd){
        keyNumber( key);
       }
    }
  }
}
void keyNumber(char key){
  pwd.append(key);
}
bool True_OR_False(){
  bool a = pwd.evaluate();
  return a;
}

