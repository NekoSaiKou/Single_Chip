#include "mbed.h"
#include "rtos.h"
 
DigitalOut LEDs[3] = { PB_0_ALT0, PB_7, PB_14 };
bool ledStatus[3] = {true, true, true};
bool button_hit = 0;
int start=0;
InterruptIn button(USER_BUTTON); // Configure P1_14 pin as input
Mutex mut;

Semaphore one_slots(1); //宣告兩執行緒的Semaphore
Semaphore two_slots(2); //宣告兩執行緒的Semaphore
 
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

void lightUp(void const *led) {
    while (true) {
        int state = -1;
        mut.lock();
        state = button_hit;
        mut.unlock();

        if(state==1) {
            int a = two_slots.wait();
            printf("a %d led: %d\r\n",a,(int*)led);
            if(a == 1 && state == 1){
                LEDs[(int)(int*)led] = 1;
            }
            Thread::wait(500);
            two_slots.release();
        }
        else if(state == 0) {
            int b = one_slots.wait();
            printf("b %d state: %d   led: %d\r\n",b,state, (int*)led);
            if(state == 0){
                LEDs[(int)(int*)led] = 1;
            }
            Thread::wait(500);
            one_slots.release();
        }
        else {}      
//        mut.unlock();
        LEDs[(int)(int*)led] = 0;
    }
}
void hitButn() {
    button.fall(&button1_onpressed_cb); // Attach ISR to handle button press event
    while(true) {
        if( button1_pressed ){
            
            button1_pressed = false;
            mut.lock();
            button_hit = !button_hit;
            mut.unlock();
            
            //two_slots.release();
//            one_slots.release();
//            wait_ms(500);
//            LEDs[0] = 0;
//            LEDs[1] = 0;
//            LEDs[2] = 0;
        }
    }
}



 
int main(void) {
    Thread but_thread;
    Thread led_1;
    Thread led_2;
    Thread led_3;
    printf("Thread begin\n\r");
    but_thread.start(hitButn);
    printf("but_thread Thread Initialized\n\r");
    led_1.start(callback(lightUp,(void *)0)); 
    printf("led_1 Thread Initialized\n\r");
    led_2.start(callback(lightUp,(void *)1)); 
    printf("led_2 Thread Initialized\n\r");
    led_3.start(callback(lightUp,(void *)2)); 
    printf("led_3 Thread Initialized\n\r");
    
    printf("Thread Initialized\n\r");
    
//    led_1_timer.start(500);
//    led_2_timer.start(500);
//    led_3_timer.start(500);
    
    Thread::wait(osWaitForever);
}
