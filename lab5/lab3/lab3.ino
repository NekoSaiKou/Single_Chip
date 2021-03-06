#include <MPU6050.h>
#include <Wire.h>
#include<LiquidCrystal_I2C.h>
MPU6050 mpu;
int16_t acc_x,acc_y,acc_z,gx,gy,gz;
long acc_total_vector;
long loop_timer;
float angle_pitch_acc;
int state = 0; /*0 1 2 */
int state_count = 0;
int step_count = 0;

/* LCD SETUP*/
// Set the pins on the I2C chip used for LCD connections:
// addr,lcd cols,lcd rows
LiquidCrystal_I2C lcd(0x27,16,2);
int x=0;
const int buttomPin = 4;
int buttomState = 0;
int i=0;// number to show

void setup(){
  Wire.begin();
  Serial.begin(9600);
  mpu.initialize();
  printstep();
  loop_timer = micros();  

  /*LCD SETUP*/
  lcd.init(); //intitialize the lcd
  lcd.backlight(); //open the backlight of the lcd
  pinMode(buttomPin,INPUT);
}
void loop(){
    mpu.getMotion6(&acc_x, &acc_y, &acc_z, &gx, &gy, &gz);
    //Accelerometer angle calculations
    acc_total_vector = sqrt(((long)acc_x*(long)acc_x)+((long)acc_y*(long)acc_y)+((long)acc_z*(long)acc_z));  //Calculate the total accelerometer vector
    //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
    angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
    
    /* walk detect*/
    switch(state){
      case 0:/* Wait for user swing his hand from back to front */
        if(angle_pitch_acc < -30){
          state_count++;    
          if(state_count>30){ /* If angle <-30 degree continuously, we believe his hand is in 'front' state. */
            loop_timer = millis(); /* If he has his hand in front, then we believe he will swing his hand to back. */
            state = 1;
            state_count = 0;    
          }
          }
          else{ /* If state count is interrupt, we believe that we got the noise so we reset counter. */
            state_count = 0;
          }
        break;
      case 1:/* Wait for user swing his hand from front to back */
        if(angle_pitch_acc > 30){ /* Same counter logic as case 0 */
          state_count++;
          if(state_count>30){
            state = 2;
            state_count = 0;    
          }
        }
          else{
            state_count = 0;
            if(millis()-loop_timer > 500){/* If stuck in this state too long, we abort this step. */
              state = 0;
            }
          }
        break;
      case 2:
        state = 0;
        if(millis()-loop_timer > 300){/* The procedure to swing hand back and front should not be done instantly.  */
          step_count++;
          printstep();
        }
        
    }

    buttomState =digitalRead(buttomPin);
    if(buttomState == HIGH){
      step_count = 0;
      printstep();
    }
}
void printstep(){
  Serial.print("Step");Serial.println(step_count);
  lcd.clear(); //clear the screen
  lcd.setCursor(0,0);
  lcd.print("STEP COUNT: ");
  lcd.setCursor(0,1);
  lcd.print(step_count);
}
