
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
#include <Keypad.h>

/* There is four state in this Calculator */
#define ini    0    // A is not entered
#define enterA 1    // At list a digit of A is entered
#define enterB 2    // Enter B
#define syntaxerr 3 // Suntax error

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
LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
}
int cursor_pos = 0;
void loop() {
  // put your main code here, to run repeatedly:
int i;

char key = keypad.getKey();
  if (key != NO_KEY){
    Serial.print(key);
          lcd.setCursor(cursor_pos,0);
          lcd.print(key);
          cursor_pos++;
  }
}

