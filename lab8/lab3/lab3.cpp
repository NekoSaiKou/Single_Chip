#include "mbed.h"
#include <cstdlib>
#include "MPU6050.h"

MPU6050 mpu; //create acc object
Serial pc(USBTX,USBRX); //create serial object

DigitalOut led[] = {LED1,LED2,LED3};

int16_t ax, ay, az; //accleration
int16_t gx, gy, gz; //gyroscope

void lightLED(int num) {
    for(int i=0; i<3; i++) {
        led[i] = 0;   
    }    
    led[num] = 1;
}

int main()
{
    pc.baud(115200); //set baud
    mpu.initialize(); // intialize
    pc.printf(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed"); //test connection

    while(1) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //get sensor data
//        pc.printf("ax:%d;ay:%d;az:%d;gx:%d;gy:%d;gz:%d\n",ax,ay,az,gx,gy,gz); // print data

        if ( (az/ abs(ax))>4 ) {
            pc.printf("1\n");
            lightLED(1);
        }
        else if ( (ax/ abs(az) ) > 1.5) {
            pc.printf("2\n");
            lightLED(2);
        } 
        else if ( (ax/ abs(az) ) < -1.5) {
            pc.printf("0\n");
            lightLED(0);
        } 
        else {}


        wait(0.2);
    }
}