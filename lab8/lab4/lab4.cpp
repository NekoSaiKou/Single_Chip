#include "mbed.h"
#include <cstdlib>
#include "MPU6050.h"

MPU6050 mpu; //create acc object
Serial pc(USBTX,USBRX); //create serial object

PwmOut led[] = {PB_0,PB_7,PB_14_ALT0};

int16_t ax, ay, az; //accleration
int16_t gx, gy, gz; //gyroscope

int main()
{
    pc.baud(115200); //set baud
    mpu.initialize(); // intialize
    pc.printf(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed"); //test connection
    led[0].period(0.004f);      // 1/256 second period
    led[1].period(0.004f);      // 1/256 second period
    led[2].period(0.004f);      // 1/256 second period
    

    while(1) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //get sensor data
//        pc.printf("ax:%d;ay:%d;az:%d;gx:%d;gy:%d;gz:%d\n",ax,ay,az,gx,gy,gz); // print data
        float tmp = ((float)abs(az)-1000)/16500;
        pc.printf("%f\n", tmp);
        led[1] = tmp;
        if (ax>0){
            // specify period first
            tmp = ((float)abs(ax))/16500;
            led[0]=0;
            led[2]=tmp;
        }
        else {
            // specify period first
            tmp = ((float)abs(ax))/16500;
            led[2]=0;
            led[0]=tmp;
        }
        

        wait(0.2);
    }
}