#include <Keypad.h>
#include "Password.h"

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
Password pwd = Password("0711");

void setup(){
  Serial.begin(115200);
  Serial.println("請輸入密碼");
}

void loop(){
  char key = keypad.getKey();
  if (key != NO_KEY){
      Serial.print('*');
    switch(key){
      case 'A':
        bool pwd_pass;
        pwd_pass = True_OR_False();
        if (!pwd_pass){
          pwd.reset();
          Serial.println("Wrong passwords!");
        }
        else{
          Serial.println("Unlock!");
        }
        break;
      default:
        keyNumber(key);
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

