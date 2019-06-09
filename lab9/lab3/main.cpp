#include "mbed.h"
#include "rtos.h"

InterruptIn button1(USER_BUTTON);//declare interupt pin
DigitalOut led1(LED1);//declare output pin
volatile bool button1_pressed = false; // Used in the main loop
volatile bool button1_enabled = true; // Used for debouncing
Timeout button1_timeout; // Used for debouncing
uint32_t current_ms10 = 0;
uint32_t current_min = 0;
uint32_t current_s = 0; 

// Enables button when bouncing is over
void button1_enabled_cb(void){
    button1_enabled = true;
}

// ISR handling button pressed event
void button1_onpressed_cb(void){
    if (button1_enabled) { // Disabled while the button is bouncing
        button1_enabled = false;
        button1_pressed = true; // To be read by the main loop
        button1_timeout.attach(callback(button1_enabled_cb), 0.3); // Debounce time 300 ms
    }
}

/* Mail */
typedef struct {
  uint32_t ms10;    /* milli second               */
  uint32_t minute;  /* minute               */
  uint32_t second;  /* second               */
} mail_t;
 
Mail<mail_t, 16> mail_box;

void send_thread(void const *n){
        mail_t *mail = mail_box.alloc();       
        /* Update timer*/
        current_ms10 = current_ms10 + 1;        
        /* Update date */
        if(current_ms10 == 100){
            current_ms10 = 0;
            current_s = current_s + 1;            
            if(current_s == 60){
                current_s  = 0;
                current_min = current_min + 1;                
            }
        }
        mail->second = current_s; 
        mail->minute = current_min; 
        mail->ms10 = current_ms10;
        mail_box.put(mail);   
}

int main (void) {
    //Thread thread;
    //thread.start(callback(send_thread));
    bool start = false;
    //RtosTimer led_1_timer(blink, osTimerPeriodic, (void *)0); 
    RtosTimer c_timer(send_thread, osTimerPeriodic,(void *)0);
    button1.fall(&button1_onpressed_cb); // Attach ISR to handle button press event
    while(1) {
        if (button1_pressed) { // Set when button is pressed
            button1_pressed = false;
            led1 = !led1;
            start = !start;
            if(start){
                c_timer.start(10);
                printf("i'm in start\n");
                current_ms10 = 0;
                current_min = 0;
                current_s = 0;  
            }
            else{
                printf("i'm in stop\n");
                c_timer.stop();                        
            }
        }
        if(!mail_box.empty()){
            osEvent evt = mail_box.get();
            if(evt.status == osEventMail){
                mail_t *mail = (mail_t*)evt.value.p;
                int minute = mail->minute;
                int second = mail->second;
                int ms10 = mail->ms10;
                mail_box.free(mail);
                printf("%2d: ",minute);
                printf("%2d: ",second);
                printf("%2d\r\n",ms10);
            }
        }
    }
}