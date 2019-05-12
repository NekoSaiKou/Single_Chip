#include "mbed.h"

#define Forward  0
#define Pause   1
#define Backward 2

DigitalOut led[] = {LED1,LED2,LED3};
Ticker ticker1;//declare a timer
InterruptIn button1(USER_BUTTON);//declare interupt pin

int user_status = Pause;
int user_status_increment = -1;
int led_index = 0;
int led_index_increment = 0;

volatile bool button1_pressed = false; // Used in the main loop
volatile bool button1_enabled = true; // Used for debouncing
Timeout button1_timeout; // Used for debouncing

// Enables button when bouncing is over
void button1_enabled_cb(void){
    button1_enabled = true;
}

// ISR handling button pressed event
void button1_onpressed_cb(void){
    if (button1_enabled) { // Disabled while the button is bouncing
        button1_enabled = false;
        button1_pressed = true; // To be read by the main loop
        button1_timeout.attach(callback(button1_enabled_cb), 0.1); // Debounce time 300 ms
    }
}

// Run horse light
void run_horse(void){
    led_index = led_index + led_index_increment;
    if(led_index == 3){
        led_index = 0;
    }
    else if(led_index == -1){
        led_index = 2;
    }

    for(int i = 0 ; i < 3 ; i++){
        if(i == led_index){
            led[i] = 1;
        }
        else{
            led[i] = 0;
        }
    }
}
int main(){
    button1.fall(&button1_onpressed_cb); // Attach ISR to handle button press event
    ticker1.attach(&run_horse,0.5);// start the timer
    while(1) {
        if (button1_pressed) { // Set when button is pressed
            button1_pressed = false;
            user_status = user_status + user_status_increment;
            if(user_status == Forward){
                user_status_increment = 1;
                led_index_increment = 1;
            }
            else if(user_status == Backward){
                user_status_increment = -1;
                led_index_increment = -1;
            }
            else{
                led_index_increment = 0;
            }
        }
    }
}