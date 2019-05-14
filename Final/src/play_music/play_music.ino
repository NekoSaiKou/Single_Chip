#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions 
//#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed.
//#define OUTPUT_READABLE_REALACCEL

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// Pose vars
#define idle  0
#define accel 1
#define wait_dacce 2
#define dacce 3
int state = idle;
int state_count = 0;
long X_ave = 0;
long Z_ave = 0;

// Melody vars
#define Do  523
#define Re  587
#define Mi  659
#define Fa  698
#define So  784
#define La  880
#define Si  988
#define Doh 1046
int melody[8] = {Do, Re, Mi, Fa, So, La, Si, Doh};
const int buzzer = 9;
unsigned long buzz_time = 0;

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // Buzzer pinout
    pinMode(buzzer, OUTPUT);

    // Serial open
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    // initialize device
    Serial.println(F("Initializing I2C devices..."));

    //Mpu initialize
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    //mpu.setXGyroOffset(220);
    //mpu.setYGyroOffset(76);
    //mpu.setZGyroOffset(-85);
    //mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN,LOW);
    //wait mpu
    mpu_wait();
    // configure LED for output
    
}
// ================================================================
// ===                    Wait for mpu stable                   ===
// ================================================================
void mpu_wait(){
    int loopcount = 1;
    while(true){
        // if programming failed, don't try to do anything
        if (!dmpReady) return;
    
        // wait for MPU interrupt or extra packet(s) available
        while (!mpuInterrupt && fifoCount < packetSize) {
            if (mpuInterrupt && fifoCount < packetSize) {
              // try to get out of the infinite loop 
              fifoCount = mpu.getFIFOCount();
            }  
        }
        
        // reset interrupt flag and get INT_STATUS byte
        mpuInterrupt = false;
        mpuIntStatus = mpu.getIntStatus();
    
        // get current FIFO count
        fifoCount = mpu.getFIFOCount();
    
        // check for overflow (this should never happen unless our code is too inefficient)
        if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
            // reset so we can continue cleanly
            mpu.resetFIFO();
            fifoCount = mpu.getFIFOCount();
            Serial.println(F("FIFO overflow!"));
    
        // otherwise, check for DMP data ready interrupt (this should happen frequently)
        } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
            // wait for correct available data length, should be a VERY short wait
            while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    
            // read a packet from FIFO
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            
            // track FIFO count here in case there is > 1 packet available
            // (this lets us immediately read more without waiting for an interrupt)
            fifoCount -= packetSize;
            
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            if(loopcount >1000){
                if(abs(aaReal.x) < 200 && abs(aaReal.y) < 200 && abs(aaReal.z) < 200){
                  break;
                }
            }    
        }

        Serial.print("Loop count:");
        Serial.println(loopcount);
        loopcount++;
    }
}
// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
void loop() {
    // indicate start
    digitalWrite(LED_PIN,HIGH);
    
    // If buzz buzzing for 500ms then stop
    if(millis() - buzz_time > 500){
      noTone(buzzer);
    }
  
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        if (mpuInterrupt && fifoCount < packetSize) {
          // try to get out of the infinite loop 
          fifoCount = mpu.getFIFOCount();
        }  
    }
    
    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        fifoCount = mpu.getFIFOCount();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        /* Acquire data from fifobuffer */
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&aa, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            
        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[2] * 180/M_PI);
            Serial.print("\t");  
        #endif

        #ifdef OUTPUT_READABLE_REALACCEL
            Serial.print("areal\t");
            Serial.print(aaReal.x);
            Serial.print("\t");
            Serial.print(aaReal.y);
            Serial.print("\t");
            Serial.print(aaReal.z);
            Serial.println("\t");   
        #endif

        /* Calculate acceleration */
        long aa_x = aaReal.x;
        //long aa_y = aaReal.y;
        long aa_z = aaReal.z;  
        long aa_total = sqrt( aa_x*aa_x +  aa_z*aa_z);
        
        /* If the force isn't large enough then skip this procedure */
        /* Usually, deacceleration value is smaller than acceleration */
        if(aa_total >= 800 && state == wait_dacce){
            Serial.print("Deacceleration detected  ");
            Serial.println(state_count);
            if(state_count == 4){
              state = dacce;  
            }
            state_count++;
        }
        /* If acceleration is large enough than detect motion */
        else if(aa_total >= 3000){             
            /* If idling and sense an accelleration then go to accelerate state */
            if(state == idle){
              Serial.println("Acceleration detected");
              state = accel;
            }
            /* Record several accel state acceleration data and its moving average */
            if(state == accel){
                /* Get five samples of acceleration */
                if(state_count <4){
                    X_ave = X_ave + aa_x;
                    Z_ave = Z_ave + aa_z;
                    state_count++;
                }
                /* After getting 4 sample then calculate average and obtain groups */
                else if(state_count == 4){
                    state_count++;
                    X_ave = X_ave/state_count;
                    Z_ave = Z_ave/state_count;
                    /* Calculate swing acceleration */
                    long x_z = sqrt( X_ave*X_ave + Z_ave*Z_ave); 
                    /* Calculate force angle relative to sensor and map to 360 degree */
                    int angle = acos(float(X_ave)/x_z)*180/M_PI;
                    if(aa_z <0){
                        angle = 360-angle;
                    }
                    /* Take sensor angle into consideration */
                    int final_angle = angle + ypr[1] * 180/M_PI;
                    if(final_angle >= 360){
                      final_angle = final_angle - 360;
                    }else if(final_angle < 0){
                      final_angle = final_angle + 360;
                    }

                    /* Divide into 8 groups */
                    int divide_angle = final_angle + 22.5;
                    if(divide_angle >= 360){
                      divide_angle = divide_angle - 360;
                    }
                    int group = divide_angle/45;
                    play(group);
                    Serial.print("Group  ");
                    Serial.println(group);                 
                }
            }
        }
        /* 
        If acceleration isn't that big but previous state is acceleration, then wait for deacceleration.
        If acceleration isn't that big but previous state is deacceleration, then a cycle finish --idle.
        */
        else{
            if(state == accel){
                Serial.println("go wait deacceleration");
                state = wait_dacce;
                state_count = 0;
                X_ave = 0;
                Z_ave = 0;
            }
            if(state == dacce){
                Serial.println("After deacceleration go idle");
                state = idle;
                state_count = 0;
            }
        }
    }
}
// ================================================================
// ===                    Play tone                             ===
// ================================================================
void play(int group){
    buzz_time = millis();
    tone(buzzer, melody[group]);
}

