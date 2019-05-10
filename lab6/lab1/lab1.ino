#include<Wire.h>
#include<LiquidCrystal_I2C.h>

// Set the pins on the I2C chip used for LCD connections:
// addr,lcd cols,lcd rows
LiquidCrystal_I2C lcd(0x27,16,2);

#define ECHOPIN 6        // Orange line
#define TRIGPIN 5        // Red line

void setup() {
  Serial.begin (115200);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);

  lcd.init(); //intitialize the lcd
  lcd.backlight(); //open the backlight of the lcd
  Wire.begin();
}

// Get the distance from the barrer
int getDis() {
  digitalWrite(TRIGPIN, LOW); 
  delayMicroseconds(2); 
  digitalWrite(TRIGPIN, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TRIGPIN, LOW);
  
  // Distance Calculation
  float timestop = pulseIn(ECHOPIN, HIGH);
  float distance = ( timestop *0.0336) / 2; // return in microseconds, 1cm requires 29.4ms

  Serial.println(distance);
  return (distance);
}

void loop() {
  int dis = getDis();
//  lcd.clear(); //clear the screen
  lcd.setCursor(0,0);//set the cursor to make the word move right
  lcd.print("Distance: ");
  lcd.setCursor(0,1);//set the cursor to make the word move right
  
  lcd.print("       ");
  lcd.setCursor(0,1);//set the cursor to make the word move right
  if(dis <= 15)
    lcd.print(dis);
  else
    lcd.print("Exceed");
  delay(50);
}
