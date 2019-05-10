const int led_pin = 13;
int brightness = 0;
int increment = 2;

// Counter A ISR
ISR(TIMER2_COMPA_vect){
  PORTB = PORTB^B100000;
  brightness = brightness + increment;
  // If the brightness is exceed lower or upper bound then change increment direction
  if(brightness < 0){
    brightness = 0;
    increment = increment * -1;
  }
  else if(brightness > 155){
    brightness = 155;
    increment = increment * -1;
  }
  // The higher brightness means the lower low level time.
  int duration = 156-brightness;
  // Set new duration time
  OCR2B = duration;
}

// Counter B ISR
ISR(TIMER2_COMPB_vect){
  PORTB = PORTB^B100000;
}
void setup(){
    DDRB = B100000;                                     // set the regisster of pin 13 as output mode by DDR
    TCCR2A = 0;                                         // reset TIMER2 control register by TCCR1
    TCCR2B = 0;
    TCCR2A |= (1<<WGM21) ;                              // Set Clear Timer on Compare Match (CTC) Mode

    /* 10ms: 160000clk for 16MHz clk */
    /* 1024 prescalar -> 156.25 count in 10ms */
    /* 100Hz */
    TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);        // set the prescaler to adjust 16Mhz to map to 0.01s by TCCR1
    TCNT2=0;                                            // reset Timer2 by TCNT
    OCR2A = 156;                                        // set compare value by OCR2 (for 100Hz)
    OCR2B = 155;                                        // set compare value by OCR2 (for duration)
    TIMSK2 |= (1 << OCIE2A) | (1 << OCIE2B) ;           // enable  Timer2 compare interrupt mode by TIMSK
    sei();                                              // enable global interrupt
    PORTB = B000000;
}
void loop(){
    //ISR will handle everything
}
