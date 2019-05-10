#include<Wire.h>
#include<LiquidCrystal_I2C.h>
#include <Keypad.h>

/* There is four state in this Calculator */
#define iniA    0 // A is not entered
#define enterA 1  // At list a digit of A is entered
#define iniB 2    // B is not entered
#define enterB 3  // At list a digit of B is entered
#define syntaxerr 4 // Syntax error
#define next_operate 5 //Prepare for next operation

/* Keyboard mapping */
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {{'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'}};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/* Calculator */
float A = 0;
float B = 0;
float ans = 0;
int cal_state = iniA;
int operation = 0;
char operate[4] = {'+','-','x','/'};

LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
 //lcd.setCursor(0,0);
  //lcd.printstr("NCKU");
  Serial.begin(115200);
}
int cursor_pos;
void loop() {
  // put your main code here, to run repeatedly:
int i;

char key = keypad.getKey();
  if (key != NO_KEY){
    Serial.print(key);
    switch(key){
      case 'A':/* + */
      case 'B':/* - */
      case 'C':/* * */
      case 'D':/* / */
        operation = key - 'A';
        if(cal_state == iniA){
          /* If A is not entered, then * and / operator are not allowed. */
          if(operation == 2 || operation == 3 ){
            cal_state = syntaxerr;
          }else{
            lcd.setCursor(cursor_pos,0);
            lcd.print(operate[operation]);
            cal_state = iniB;
            A = 0;
          }
        }
        /* Multiple operation is not allowed, like  A*B*C in a line. */
        else if(cal_state == enterB){
          cal_state = syntaxerr;
        }
        else if (cal_state == enterA){ /*If A is enteted then enter B*/
          lcd.setCursor(cursor_pos,0);
          lcd.print(operate[operation]);
          cal_state = iniB;
        }else if(cal_state == next_operate){
          lcd.clear();
          cursor_pos = 0;
          lcd.setCursor(cursor_pos,0);
          lcd.print(A);
          lcd.print(operate[operation]);
          cal_state = enterB;
          }
        break;
      case '#':/* = */
        /* If error already happened or if number B is not yet entered. */
        if(cal_state == syntaxerr || cal_state == iniB){
          Serial.println("Syntax Error");
          lcd.setCursor(0,1);
          lcd.print("Syntax Error");
          cal_state = iniA;
        }else{/* normal operation */
          cal_state = iniA;
          switch (operation){
            case 0:
              ans = A + B;
              break;
            case 1:
              ans = A - B;
              break;
            case 2:
              ans = A * B;
              break;
            case 3:
              ans = A / B;
              break;
            default:
              break;
          }
          if(ans > 32767){
            Serial.println("Error!!");
            cal_state = iniA;
            lcd.setCursor(0,1);
            lcd.print("Error!!");
          }else{
            lcd.setCursor(0,1);
            lcd.print("=");
            lcd.setCursor(1,1);
            lcd.print(ans);
          }
        }
        Serial.print(A);
        Serial.print(' ');
        Serial.print(B);
        Serial.print(' ');
        Serial.println(ans);
        A = ans;
        B = 0;
        cal_state = next_operate;//next_operate
        break;
      case '*':/* Clear */
        A = 0;
        B = 0;
        cal_state = iniA;
        lcd.clear();
        cursor_pos=0;
        break;
      default: /* Enter Number */
        int num = key - '0';
        if (cal_state == iniA||cal_state == next_operate){
          cal_state = enterA;
          lcd.clear();
          cursor_pos=0;
          lcd.setCursor(cursor_pos,0);
          lcd.print(key);
          cursor_pos++;
          A=num;
        }else if(cal_state == enterA){
          lcd.print(key);
          cursor_pos++;
          A = 10*A + num;
        }else{
           if(cal_state == syntaxerr){
            cal_state = syntaxerr;
          }else{
            cal_state = enterB;
          }
          lcd.print(key);
          B = 10*B + num;
          cursor_pos++;
        }
        break;
    }
  }
}
