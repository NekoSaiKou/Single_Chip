/*Keypad define*/
#define row_num 4
#define col_num 4
byte rows[row_num] = {2, 3, 4, 5};
byte cols[col_num] = {6, 7, 8, 9};
char key[row_num][col_num] = {{'1','2','3','A'},
                           {'4','5','6','B'},
                           {'7','8','9','C'},
                           {'*','0','#','D'}};
/*Btn state*/
#define no 0                 //unpressed
#define yes 1                //pressed
#define debounce_start_on  2  //start debounce process(check press)
#define debounce_start_off 3  //start debounce process(check released)

/*Button data container*/
typedef struct _button_status{
  int ispress;              //Btn state mentioned above
  uint64_t press_time;
  uint64_t release_time;
} button_status;

/*Build data for each button*/
static button_status keyboard_table[row_num][col_num] = { 0 };

void setup() {
  Serial.begin(115200);
  for(int i = 0 ; i < 4 ; i++){
    pinMode(rows[i],OUTPUT);
    pinMode(cols[i],INPUT_PULLUP);
  }
}
void keypad(){
  int keypad_index = 0;
  // Scan each rows
  for(int row_index = 0; row_index < row_num ; row_index++){
   /*SET row-to-scan into low level.*/
    for(int i = 0 ; i < 4 ; i++){
      digitalWrite(rows[i],(i == row_index)?LOW:HIGH);
      delay(1);
    }
    // Scan each columns
    for(int col_index = 0; col_index < col_num ; col_index++){
      int btn = digitalRead(cols[col_index]);
        if (btn == 1) {// Release debounce
          if (keyboard_table[row_index][col_index].ispress == yes) {                        // If this button is in "pressed" state, then start debounce process
            keyboard_table[row_index][col_index].release_time = micros();
            keyboard_table[row_index][col_index].ispress = debounce_start_off;
          } else if (keyboard_table[row_index][col_index].ispress == debounce_start_on) {  //If this button is in "check if pressed" state and it get "release" signal, then stop debounce process and enter "released" mode
            keyboard_table[row_index][col_index].ispress = no;
          } else if (keyboard_table[row_index][col_index].ispress == debounce_start_off) { //If this button is in "start released debounce process", then check timing
            if (micros()- keyboard_table[row_index][col_index].release_time>= 10000) {
              keyboard_table[row_index][col_index].ispress = no;
            }
          } else {                                                                          //If this button is in "released" state, then stay in same state.
            keyboard_table[row_index][col_index].ispress = no;
          }
        } else { //Press debounce
          if (keyboard_table[row_index][col_index].ispress == no) {                          // If this button is in "un-pressed" state, then start debounce process
            keyboard_table[row_index][col_index].press_time = micros();
            keyboard_table[row_index][col_index].ispress = debounce_start_on;
          } else if (keyboard_table[row_index][col_index].ispress == debounce_start_on) {     //If this button is in "start pressed debounce process", then check timing
            if (micros() - keyboard_table[row_index][col_index].press_time >= 10000) {
              keyboard_table[row_index][col_index].ispress = yes;
              Serial.println(key[row_index][col_index]);
              return;
            }
          } else if (keyboard_table[row_index][col_index].ispress == debounce_start_off) { //If this button is in "check if released" state and it get "press" signal, then stop debounce process and enter "pressed" mode
            keyboard_table[row_index][col_index].ispress = yes;
          } else {                                                                          //If this button is in "pressed" state, then stay in same state.
            keyboard_table[row_index][col_index].ispress = yes; 
          }
        }
    }
   }
}
void loop() {
  keypad();
}
