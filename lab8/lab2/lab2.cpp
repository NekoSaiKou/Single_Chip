#include "mbed.h"
#include <string.h>
//------------------------------------
// Hyperterminal configuration
// 9600 bauds, 8-bit data, no parity
//------------------------------------
Ticker ticker1;//declare a timer
Serial pc(SERIAL_TX, SERIAL_RX);// declare serial object

//declare three led object
DigitalOut myled1(LED1); 
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);

int i = 1;//counter

volatile bool lock=false; //locker

char s[10]; // data from pc

void change()// run horse light
{
    if(myled1)
    {   myled1=0;
        myled2=1;
        myled3=0;
    }
    else if(myled2)
    { 
        myled1=0;
        myled2=0;
        myled3=1;
        
    }
    else if(myled3)
    { 
        myled1=1;
        myled2=0;
        myled3=0;
        
    }
    
}
void uart_print()
{
    
    //pc.printf("This program runs since %d seconds.\n", i++);
   if(lock==false){  //control the light
       change();
    }
}

int main()
{
    pc.printf("Hello world");
    myled1=1;
    ticker1.attach(&uart_print,1);// start the timer
    while(1) {
        if(pc.readable()){//check if data received
            pc.gets(s,10); //receive data
            pc.printf("%s",s);
            if( strcmp(s,"High\r\n")==0){// type 'p' to lock
                ticker1.attach(&uart_print,0.1);// start the timer
                lock=false;
                pc.puts(s); //print
                pc.printf("\n");
            }
            else if(strcmp(s,"Middle\r\n")==0){//type 's' to unlock
                ticker1.attach(&uart_print,0.5);// start the timer
                lock=false;   
                pc.puts(s); //print
                pc.printf("\n");
            }
            else if(strcmp(s,"Low\r\n")==0){//type 's' to unlock
                ticker1.attach(&uart_print,1);// start the timer
                lock=false;   
                pc.puts(s); //print
                pc.printf("\n");
            }
        }
        
    }
}
