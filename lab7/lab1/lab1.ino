const int led_pin = 13;

// everytime interrupt happen -> Change state
ISR(TIMER1_COMPA_vect){
  PORTB = PORTB^B100000;
}

void setup(){
    DDRB = B100000;           // set the regisster of pin 13 as output mode by DDR
    TCCR1A = 0;               // reset TIMER1 control register by TCCR1
    TCCR1B = 0;
    TCCR1B |= (1<<WGM12);     // Set Clear Timer on Compare Match (CTC) Mode

    /* 0.25s: 4000000clk for 16MHz clk */
    /* 256 prescalar -> 15265 count in 0.25s */
    TCCR1B |= (1<<CS12);      // set the prescaler to adjust 16Mhz to map to 0.25s by TCCR1
    TCNT1=0;                  // reset Timer1 by TCNT
    OCR1A = 15625;            // set compare value by OCR1
    TIMSK1 |= (1 << OCIE1A);  // enable  Timer1 compare interrupt mode by TIMSK
    sei();                    // enable global interrupt 
    PORTB = B000000;
}
void loop(){
    // ISR will handle the interrupt
}
