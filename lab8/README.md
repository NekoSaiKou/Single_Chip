
# 2019單晶片課程：Mbed 平台

***mbed*** 是基於 ARM 架構下, 針對 IoT 所發展的**服務框架**, ARM 和其他硬體的夥伴 (例如 CSR, Nordic 等等) 合作, 期望是發展出一套**商用化**, **標準化**的 IoT 裝置, mbed 包含了 **OS, 軟體元件以及工具組**, 建立 IoT 生態系

mbed 包含了以下三個部分:
1. mbed OS
* 基於 ARM Cortex-M, 一個全端的作業系統
* 針對耗電量, 連接性, 安全性, **可複用性優化**的軟體元件
* 適合用於 IoT, 且免費
2. mbed Device Server
* 類似瀏覽器或手機和 Web Server 建立連線, Device Server 則是處理 IoT 裝置來的連線
* 支援 IoT 所需要的通訊協定, 行為, 安全等需求
* 可以透過 REST API 來存取企業軟體, Web 應用程式, 以及雲端平台
3. mbed Tools
* 藉由 Web 開發的啟發, 將嵌入式系統開發也導入 Web 開發方式
* 提供 command-line 的 build, 元件管理以及測試工具
* 提供 **Web IDE**, 以及開發者 Web 服務, 協助你的開發可以有 accessibility 和 productivity, 就跟你預期在其他領域可以有的一樣



進入mbed 官網(https://os.mbed.com/)
點選hardware->boards
# Mbed OS boards

![](https://i.imgur.com/ydK4kac.png)
:::warning
右下角有ARM Mbed 標示的才是支援Mbed OS的喔~~~~
:::
 

## NUCLEO-F207ZG
![](https://i.imgur.com/sHexfIY.png)
### spec:
* Core: ARM® 32-bit Cortex®-M3 CPU (120 MHz max) 
* 1 Mbyte of Flash memory
* 32 kHz oscillator for RTC with calibration
* Up to 15 communication interfaces
* Up to 3 × I2 C interfaces (SMBus/PMBus)
* Up to 4 USARTs and 2 UARTs (7.5 Mbit/s, ISO 7816 interface, LIN, IrDA, modem ctrl)
* Up to 3 SPIs (30 Mbit/s), 2 with muxed I2 S to achieve audio class accuracy via audio PLL or external PLL
* 2 × CAN interfaces (2.0B Active)

### pinout
![](https://i.imgur.com/AQqpDd4.png)
![](https://i.imgur.com/e7r85p2.png)
![](https://i.imgur.com/ZC8DUiR.png)
![](https://i.imgur.com/zp8ldC8.png)
![](https://i.imgur.com/aGMTg5Y.png)

## 燒錄
線上基本燒錄流程
1. 板子插上電腦(如果電腦讀不到com請安裝這個驅動程式 [stlink-driver](https://drive.google.com/open?id=1tP8tPG9vj4hwtsCAQbrY8T_rRa5wiLm-))
2. 將自己寫的code在線上IDE  compile
3. compile完下載.bin檔
4. 放進板子,成功!!!!

Mbed 官網:https://os.mbed.com/
1.註冊帳號
![](https://i.imgur.com/NlM8jvW.png)

2.點選compile
![](https://i.imgur.com/S6qex4v.jpg)

3.IDE頁面,點選左上角的new->new program
![](https://i.imgur.com/RnCRcz2.png)
 
4.選擇板子與專案
![](https://i.imgur.com/ejI3hfF.png)

5.點選compile並下載.bin檔
![](https://i.imgur.com/0E8VNJB.png)

6.將.bin檔丟入裝置的資料夾
![](https://i.imgur.com/HOuMCQf.png)

7.成功!!
![](https://i.imgur.com/WOfhrJa.png)



## 範例

### LED+Button

使用button控制LED亮暗

每當按下按鈕便將LED狀態toggle
```cpp=
#include "mbed.h"

DigitalOut led1(LED1);//declare output pin
InterruptIn button1(USER_BUTTON);//declare interupt pin

volatile bool button1_pressed = false; // Used in the main loop
volatile bool button1_enabled = true; // Used for debouncing
Timeout button1_timeout; // Used for debouncing

// Enables button when bouncing is over
void button1_enabled_cb(void)
{
    button1_enabled = true;
}

// ISR handling button pressed event
void button1_onpressed_cb(void)
{
    if (button1_enabled) { // Disabled while the button is bouncing
        button1_enabled = false;
        button1_pressed = true; // To be read by the main loop
        button1_timeout.attach(callback(button1_enabled_cb), 0.3); // Debounce time 300 ms
    }
}

int main()
{
    button1.fall(&button1_onpressed_cb); // Attach ISR to handle button press event
    while(1) {
        if (button1_pressed) { // Set when button is pressed
            button1_pressed = false;
            led1 = !led1;
        }
    }
}
```


### uart+ticker

每秒變換燈號，並將經過秒數顯示於螢幕上
```cpp=
#include "mbed.h"


Ticker ticker1;//declare a timer
Serial pc(SERIAL_TX, SERIAL_RX);// declare serial object

//declare three led object
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
int i = 1; //counter


void change() // run horse light
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
    pc.printf("This program runs since %d seconds.\n", i++); // seconds passed
       change();
}

int main()
{
    
    pc.printf("Hello World !\n");
    myled1=1;
    ticker1.attach(&uart_print,1); // start the timer
    while(1) {
        
        
    }
}
```
:::info
可以使用Arduino IDE中的序列埠監控式窗工具來顯示所傳資料與傳遞資料

:::
### lab1
請用實作出此影片中的裝置


{%youtube 1DMaGcuo4RI %}

1. 按下button會停止狀態
2. 再次按下會倒著進行
3. 再按下會停止狀態
4. 最後回歸原狀


### 用uart 控制跑馬燈 (terminal)

輸入“p”停止跑馬燈
輸入“s”開始跑馬燈
```cpp=
#include "mbed.h"

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

char s=' '; // data from pc

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
    
    pc.printf("This program runs since %d seconds.\n", i++);
   if(lock==false){  //control the light
       change();
    }
}

int main()
{
    
    pc.printf("Hello World !\n");
    myled1=1;
    ticker1.attach(&uart_print,1);// start the timer
    while(1) {
        if(pc.readable()){//check if data received
            s=pc.getc(); //receive data
            if(s=='p'){// type 'p' to lock
                lock=true;
                pc.putc(s); //print
            }
            else if(s=='s'){//type 's' to unlock
                lock=false;   
                pc.putc(s); //print
            }
        }
        
    }
}

```
### lab2
分別輸入
**"High"
"Middle" 
"Low"**
來調控跑馬燈變換速度

## 加入libary
如同arduino一樣,使用外掛模組時往往會需要額外的libary來驅動該模組,mbed的Web IDE可以直接搜尋libary並導入專案中

1. 點選左上角的import
![](https://i.imgur.com/7rbKdQo.png)
2. 搜尋關鍵字,雙擊想要的libary來導入專案中
![](https://i.imgur.com/Ty53G4g.png)

### 使用MPU6050-讀取加速度值與角速度值
```cpp=
#include "mbed.h"
#include "MPU6050.h"

MPU6050 mpu; //create acc object
Serial pc(USBTX,USBRX); //create serial object

int16_t ax, ay, az; //accleration
int16_t gx, gy, gz; //gyroscope

int main()
{
    pc.baud(115200); //set baud
    mpu.initialize(); // intialize
    pc.printf(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed"); //test connection
    
    while(1) {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //get sensor data
      pc.printf("ax:%d;ay:%d;az:%d;gx:%d;gy:%d;gz:%d\n",ax,ay,az,gx,gy,gz); // print data
      wait(0.2);
    }
}


```

### lab3
使用MPU6050 控制跑馬燈(如影片)
{%youtube mQ2yPg-pgmc %}

### 加分題
基於lab3 加入呼吸燈(如影片)
{%youtube HvLGD3bXpiY %}

:::success
參考資料

PIN_name.h
https://github.com/ARMmbed/mbed-os/tree/master/targets/TARGET_STM/TARGET_STM32F2/TARGET_NUCLEO_F207ZG

STM32f207 datasheet
http://pdf1.alldatasheet.com/datasheet-pdf/view/551708/STMICROELECTRONICS/STM32F207.html
:::
:::info
## 課後練習
1. 請說明在使用Ticker.attach這個function時為何後面引數某function_A時需要使用 
**＆function_A**或
**callback(function_A)**?
若直接使用Ticker.attach(function_A)可以嗎？

2.  請觀察以下程式碼
```cpp=0
#include "mbed.h"

//------------------------------------
Ticker ticker1;//declare a timer
Serial pc(SERIAL_TX, SERIAL_RX);// declare serial object

DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
int i = 1;
volatile bool lock=false;
char s=' ';
void change()
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
    
   // pc.printf("This program runs since %d seconds.\n", i++);
   if(lock==false){
       change();
    }
}

int main()
{
    
    pc.printf("Hello World !\n");
    myled1=1;
    ticker1.attach(&uart_print,1);
    while(1) {
        if(pc.readable()){
        //method1
        /*
            s=pc.getc();
            if(s=='p'){
                lock=true;
                pc.putc(s);
            }
            else if(s=='s'){
                lock=false;   
                pc.putc(s);
            }*/
            
        //method2
        /*
            if(pc.getc()=='p'){
                lock=true;
                pc.putc(pc.getc());
            }
            else if(pc.getc()=='s'){
                lock=false;   
                pc.putc(pc.getc());
        
            }*/
        }
        
    }
}

```
說明使用method1與method2兩種不同方式來讀取uart的讀值是一樣的結果嗎？
若不是，請解釋造成差異之因素

3.請說明AVR架構與ARM架構為何?

:::
