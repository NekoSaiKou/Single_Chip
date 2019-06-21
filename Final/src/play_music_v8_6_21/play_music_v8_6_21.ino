#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
#include  <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11); // RX 、 TX

MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions 
//#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed.
#define OUTPUT_READABLE_REALACCEL

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
//#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
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
VectorInt16 last_aaReal;// [x, y, z]            use to store the last acc state

// Pose vars
#define IDLE  0
#define ACCEL 1
#define WAIT_DACCE 2
#define DACCE 3
#define SAM_SIZE 6
int state = IDLE;
int state_count = 0;
int dacc_thres = 800;
int acc_thres = 5000;
long X_ave = 0;
long Z_ave = 0;

// Melody vars
#define Do  523
#define Re  587
#define Mi  659
#define Fa  698
#define So  1568
#define La  880
#define Si  988
#define Doh 1046
int melody[8] = {Do, Re, Mi, Fa, So, La, Si, Doh};

// Mapping
/*
#define DO  '0'
#define RE  '1'
#define MI  '2'
#define FA  '3'
#define SO  '4'
#define LA  '5'
#define SI  '6'
#define DOH '7'
*/
#define DO  'a'
#define RE  'b'
#define MI  'c'
#define FA  'd'
#define SO  'e'
#define LA  'f'
#define SI  'g'
#define DOH 'h'
uint8_t cmd_map[8] = {SO, FA, MI, RE, DO, DOH, SI, LA};

#define BUZZER_PIN 9
unsigned long buzz_time = 0;

// LED vars       [R, G, B]
const uint16_t LED_PIN[3] = { 5, 4, 3 };

// Pitch vars
#define PITCH_PIN A0
int pitch_high=0;

// Button vars
#define BUTTON_PIN 12
// 0 => not pressed
// 1 => not debounced delay yet
// 2 => pressed and debounced
// 3 => press a long time
//uint16_t chore_state = 0; 
bool long_press_start = false; 
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long confirmPressStart = 0; // conferm that the button has been pressed, start counting the time pressed
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long longPressThres = 600;    // the debounce time; increase if the output flickers
bool lastButtonState = LOW;   // the previous reading from the input pin
bool button_state = false;
uint16_t send_count = 0;
uint64_t send_timer = 0;


bool isChord = false;

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void dmpDataReady() {
    mpuInterrupt = true;
}

// ======================================================================
// ===  Use value in buffer to calculate the accel related to world   ===
// ======================================================================
void calWorldAcc() {
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
      Serial.print("\t");   
  #endif
}

// ======================================================================
// ===  Light up the LED, using different color at different status   ===
// ===  0 => Turn off the light                                       ===
// ===  1 => Red                                                      ===
// ===  2 => Green                                                    ===
// ===  3 => Blue                                                     ===
// ===  4 => Orange                                                   ===
// ======================================================================
void lightLed(int color) {
  digitalWrite(LED_PIN[0], LOW);
  digitalWrite(LED_PIN[1], LOW);
  digitalWrite(LED_PIN[2], LOW);
  
  if(color == 1) {       //  RED
    digitalWrite(LED_PIN[0], HIGH);
  }
  else if(color == 2) {       //  GREEN
    digitalWrite(LED_PIN[1], HIGH);
  }
  else if(color == 3) {       //  BLUE
    digitalWrite(LED_PIN[2], HIGH);
  }
  else if(color == 4) {       //  YELLOW
    digitalWrite(LED_PIN[0], HIGH);
    digitalWrite(LED_PIN[2], HIGH);
    digitalWrite(LED_PIN[1], HIGH);
  }
  else {
    Serial.println("Error color");
  }
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void setup() {
    // Use Pin 13 as 5v power supply
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    // Resistance Initial
    pinMode(PITCH_PIN, INPUT);
    
    // Initialize led output pin
    pinMode(LED_PIN[0], OUTPUT);
    pinMode(LED_PIN[1], OUTPUT);
    pinMode(LED_PIN[2], OUTPUT);

    // BUTTON_PIN Initialized
    // Turn up red light for not ready
    lightLed(2);
  
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // BUZZER_PIN Initialized
    pinMode(BUZZER_PIN, OUTPUT);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Serial open
    Serial.begin(115200);
    //Bluetooth serial open
    BTSerial.begin(9600); 
    while (!BTSerial); // wait for Leonardo enumeration, others continue immediately
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

    //wait mpu
    mpu_wait();
    // configure LED for output
    lightLed(3);
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
            // Acquire data from fifobuffer
            calWorldAcc();
            
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
// ===                 Judge which pitch is now                 ===
// ================================================================
int judgePitch() {
  int reading = analogRead(PITCH_PIN);
  if(reading < 450) {
    return 8;
  }
  else if(reading > 800) {
    return 10; 
  }
  else {
    return 9;
  }
}

// ================================================================
// ===          Judge if the button is hit, with debounce       ===
// ================================================================
void buttonHit() {
  // read the state of the switch into a local variable:
  bool reading = digitalRead(BUTTON_PIN);
  reading = !reading; // inverse, for using input pullup

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Debounce finished, able to do the judgement
    button_state = reading;
  }
  
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}

// ================================================================
// ===         Use button state to judge the chore state        ===
// ================================================================
void pressState() {
  // run button status
  buttonHit();
  //Serial.print(millis() - confirmPressStart);
  // If the button is pressed and is the first time pressed 
  if(button_state && !long_press_start) {
    confirmPressStart = millis();
    long_press_start = true;
    lightLed(1);
    Serial.println("Long press start");
  }
  // If the button is released immediately
  else if(!button_state && long_press_start && ((millis() - confirmPressStart) < longPressThres) ) {
    isChord = !isChord;
    long_press_start = false;
    Serial.println("It's a short press");
    if(isChord)
      //BTSerial.write("11");
      BTSerial.write(11+'a');
    else
      //BTSerial.write("12");
      BTSerial.write(12+'a');
  //delay(1000);
    lightLed(3);
  }
  else if(!button_state){
    long_press_start = false;
    lightLed(3);
  }
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
void loop() {
    /*
     * For debug, able to get Serial and BlurTooth to work
     */
    if (BTSerial.available())
      Serial.write(BTSerial.read());
    // Keep reading from Arduino Serial Monitor and send to HC-05
    if (Serial.available())
      BTSerial.write(Serial.read());

    // indicate start
    //digitalWrite(LED_PIN,HIGH);
    
    // If buzz buzzing for 500ms then stop
    if(millis() - buzz_time > 500){
      noTone(BUZZER_PIN);
    }
  
    // if programming failed, don't try to do anything
    if (!dmpReady) return;


    /***************************************************
     *                                                 *
     *                Get button status                *
     *                                                 *
     ***************************************************/
    pressState();
    //Serial.print("Chord: ");
    //Serial.print(isChord);
    //Serial.print("long_press_start");
    //Serial.println(long_press_start);
    /***************************************************
     *                                                 *
     *            Get resistance status                *
     *                                                 *
     ***************************************************/
    int tmp_pitch = judgePitch();
    if(tmp_pitch != pitch_high) {
      pitch_high = tmp_pitch;
      send_count = 3;
      send_timer = millis();
    }
    if(send_count != 0) {
      if( (millis() - send_timer) > 80) {
        send_timer = millis();
        send_count--;
        BTSerial.write(pitch_high+'a');
        /*
        if(pitch_high < 10)
          BTSerial.write( pitch_high+'0' );
        else if(pitch_high == 10 ) 
          BTSerial.write( "10" );
        else{}
        */
      }
    }

    /***************************************************
     *                                                 *
     *                  Get MPU status                 *
     *                                                 *
     ***************************************************/
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
    } 
    else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        /* Acquire data from fifobuffer */
        calWorldAcc();
                
        /* Calculate acceleration */
        long aa_x = aaReal.x;
        //long aa_y = aaReal.y;
        long aa_z = aaReal.z;  
        long aa_total = sqrt( aa_x*aa_x +  aa_z*aa_z);
        Serial.print("aa_total: ");
        Serial.print(aa_total);
        Serial.print("\t");
        
        /* If the force isn't large enough then skip this procedure */
        /* Usually, deacceleration value is smaller than acceleration */
        if(aa_total >= dacc_thres && state == WAIT_DACCE){
            Serial.print("Deacceleration detected  ");
            Serial.println(state_count);
            if(state_count == 4){
              state = DACCE;  
            }
            state_count++;
        }
        /* If acceleration is large enough than detect motion */
        else if(aa_total >= acc_thres){             
            /* If idling and sense an accelleration then go to accelerate state */
            if(state == IDLE){
              Serial.println("Acceleration detected");
              state = ACCEL;
            }
            /* Record several accel state acceleration data and its moving average */
            if(state == ACCEL){
                /* Get five samples of acceleration */
                if(state_count < SAM_SIZE){
                    /* Calculate swing acceleration */
                    X_ave = X_ave + aa_x;
                    Z_ave = Z_ave + aa_z;
                    state_count++;
                }
                /* After getting 4 sample then calculate average and obtain groups */
                else if(state_count == SAM_SIZE && !long_press_start){

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
                    Serial.print("  angle: ");
                    Serial.print(divide_angle);
                    Serial.print("\t");
                    int group = divide_angle/45;
                    char send_group = group + 'a';
                    //char send_group = group + '0';
                    //BTSerial.write(send_group);     
                    BTSerial.write((char) cmd_map[group]);     // Send the command after mapping
                    play(group);
                    Serial.print("Group  ");
                    Serial.println(cmd_map[group]);                 
                }
            }
        }
        /* 
        If acceleration isn't that big but previous state is acceleration, then wait for deacceleration.
        If acceleration isn't that big but previous state is deacceleration, then a cycle finish --idle.
        */
        else{
            if(state == ACCEL){
                Serial.println("go wait deacceleration");
                state = WAIT_DACCE;
                state_count = 0;
                X_ave = 0;
                Z_ave = 0;
            }
            if(state == DACCE){
                Serial.println("After deacceleration go idle");
                state = IDLE;
                state_count = 0;
            }
        }
    }
    else{}
    Serial.println("");
}

// ================================================================
// ===                    Play tone                             ===
// ================================================================
void play(int group){
    buzz_time = millis();
    tone(BUZZER_PIN, melody[(int)(cmd_map[group]-'0')]);
}

