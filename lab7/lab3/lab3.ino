#include <avr/sleep.h>
#include <avr/wdt.h>

const byte ledPin = 13;    // LED腳位
byte times = 0;            // 記錄執行次數
volatile byte state = 0;   // 暫存執行狀態
ISR(WDT_vect){
    state = 2;
}
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  Serial.println("Running...");
  Serial.begin(9600);
  Serial.println("Initializing...");
  delay(100);  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).*/
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  /* set new watchdog timeout prescaler value */
  WDTCSR = (1<<WDP0) | (1<<WDP3);
  /* Enable the WD interrupt (note no reset). */
  WDTCSR = WDTCSR | (1<<WDIE);
  Serial.println("Initialisation complete.");
  delay(100);
}
void loop()
{
  if (state == 2) {
    Serial.println();
    Serial.println("Was awake...");
  }
  state = 0;
  digitalWrite(ledPin, !digitalRead(ledPin));
  Serial.println("Change LED state");
  times ++;
  if (times > 0) {
    times = 0;
    Serial.println("Go to sleep...");
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    delay(100);
    sleep_enable();
    sleep_mode();
    sleep_disable();
  }
}
