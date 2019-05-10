const int led_pin = 13;

int ms = 0;
int s = 0;
int m = 0;
int hr = 0;

ISR(TIMER1_COMPA_vect){
  PORTB = PORTB^B100000;
  ms = ms + 1;
  
  if(ms == 100){
    ms = 0;
    s = s + 1;
  }
  if(s == 60){
    s = 0;
    m = m + 1;
  }
  if(m == 60){
    m = 0;
    hr = hr + 1;
  }
  String time_string = String("")+hr+" : "+m+" : "+s+" : "+ms;
  //  /r character is only work on normal serial monitor like 'putty' or 'MobaXterm'.
  //  This doesn't work on Arduino Built-in serial monitor
  Serial.print("\r");         
  Serial.print(time_string);
}
void setup(){
    DDRB = B100000;           // set the regisster of pin 13 as output mode by DDR
    TCCR1A = 0;               // reset TIMER1 control register by TCCR1
    TCCR1B = 0;
    TCCR1B |= (1<<WGM12);     // Set Clear Timer on Compare Match (CTC) Mode

    /* 10ms: 160000clk for 16MHz clk */
    /* 256 prescalar -> 625 count in 10ms */
    TCCR1B |= (1<<CS12);      // set the prescaler
    TCNT1=0;                  // reset Timer1 by TCNT
    OCR1A = 625;              // set compare value by OCR1
    TIMSK1 |= (1 << OCIE1A);  // enable  Timer1 compare interrupt mode by TIMSK
    sei();                    // enable global interrupt
    PORTB = B000000;
    Serial.begin(115200);
}
void loop(){
    // ISR will handle everything.
}
