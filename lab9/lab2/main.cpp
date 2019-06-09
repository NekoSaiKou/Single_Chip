#include "mbed.h"
#include "rtos.h"
 
/* Mail */
typedef struct {
  uint32_t year;    /* year               */
  uint32_t month;   /* month               */
  uint32_t day;     /* day               */
  uint32_t hour;    /* hour               */
  uint32_t minute;  /* minute               */
  uint32_t second;  /* second               */
} mail_t;
 
Mail<mail_t, 16> mail_box;

int get_day(int m, int y){
    if(m >= 8){
        return (m%2==0)?31:30;
    }
    else{
        if(m == 2){
            if((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0 && y % 3200 != 0)){
                return 29;
            }
            else{
                return 28;
            }
        }
        else{
            return (m%2==1)?31:30;
        }
    }
}

void send_thread (void) {
    uint32_t current_y = 2019;
    uint32_t current_m = 5;
    uint32_t current_d = 31;
    uint32_t current_h = 9;
    uint32_t current_min = 50;
    uint32_t current_s = 0;
    while (true) {
        mail_t *mail = mail_box.alloc();
        
        /* Update timer*/
        current_s = current_s + 1;
        
        /* Update date */
        if(current_s == 60){
            current_s = 0;
            current_min = current_min + 1;
            
            if(current_min == 60){
                current_min  = 0;
                current_h = current_h + 1;
                
                if(current_h == 24){
                    current_h  = 0;
                    current_d = current_d + 1;
                    
                    if(current_d > get_day(current_m, current_y)){
                        current_d = 1;
                        current_m = current_m + 1;
                        
                        if(current_m == 13){
                            current_m  = 1;
                            current_y = current_y + 1;
                        } 
                    }
                }
            }
        }

        mail->second = current_s; 
        mail->minute = current_min; 
        mail->hour = current_h; 
        mail->day = current_d; 
        mail->month = current_m; 
        mail->year = current_y; 
        
        mail_box.put(mail);
        Thread::wait(1000);
    }
}
 
int main (void) {
    Thread thread;
    thread.start(callback(send_thread));
    
    while (true) {
        osEvent evt = mail_box.get();
        if (evt.status == osEventMail) {
            mail_t *mail = (mail_t*)evt.value.p; // value.p value 的 pointer
            
            printf(" %4d-"   , mail->year);
            printf("%2d-"     , mail->month);
            printf("%2d"     , mail->day);
            printf(" , ");
            printf("%2d:"     , mail->hour);
            printf("%2d:"     , mail->minute);
            printf("%2d\r\n"     , mail->second);
            
            mail_box.free(mail);
        }
    }
}