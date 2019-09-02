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

/* LCD cursor definition*/
int lcd_show_current = 0;
int lcd_current_line = 0;
int lcd_show_enter = 0;
int lcd_enter_line = 1;

/* Keyboard index */
int key_index = 0;
int prev = 0;
int count = 0;

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
    int num = key - '0';
    if(num > 0 && num <= 9){/* Key 1 to 9 */
      if(num == prev){
        count ++;
        if(count == 3 && num == 9){
          count = 1;
        }
        else if(count == 4){
          count = 1;
        }
      }
      else{
        count = 1;
        prev = num;
      }
      key_index = (num-1)*3 + count -1 ; /* relative index to char 'A' */
      Serial.println(key_index);
      lcd.setCursor(lcd_show_current,lcd_current_line);
      lcd.print(char('A'+key_index));
    }
    else{
      if (key == 'A'){
        lcd.setCursor(lcd_show_enter,lcd_enter_line);
        lcd.print(char('A'+key_index));
        lcd_show_enter++;
      }
      else if (key == 'B'){
        lcd_show_enter--;
        lcd.setCursor(lcd_show_enter,lcd_enter_line);
        lcd.print(' ');
      }
    }
  }
}

