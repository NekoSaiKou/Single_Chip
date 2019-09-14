# Arduino講義：Serial Communication (SPI)
## Introduction of SPI
* SPI 全名 Serial Peripheral Interface ，中文叫做序列周邊介面。
* 一種主從式架構的同步資料協定，可以讓多個裝置在短距離互相通訊。
* SPI 裝置之間使用<font color="red">全雙工模式通訊</font>，包含一個<font color="red">主機(master)</font>和一個或<font color="red">多個從機(slave)</font>。
*   **SPI** : <font color="red"> 全雙工、串列、同步</font>
* 主要用以連接 ADC、DAC、EEPROM、通訊傳輸 IC...等週邊晶片。
### SPI 接腳名稱及意義
|     接腳名稱    |  腳位   |  說明   |
| -------- | --------|--------| 
| MOSI (Master Output,Slave Input)|11| master 數據輸出，slave 數據輸入|
| MISO (Master Input,Slave Output) |12| master 數據輸入，slave 數據輸出| 
| SCLK (Serial Clock)|13| 時脈信號，由 master 產生並控制     |
| SS (Slave Selected)  |10| LOW(低電位)時表示裝置可以與Master通訊|
* SPI 匯流排：
    * 單一主機對單一從機
![](https://i.imgur.com/Icd0Jwy.png)
    * 單一主機對複合從機 
     主機將欲操作之從機選擇線拉低(SS to LOW)，再分別透過 MOSI/MISO 啟動數據發送/接收。

        ![](https://i.imgur.com/Hjxqgtj.png)                
#### 傳輸方式
* 當 Master 對 Slave 做 select 之後 (連接到該 slave 的 SS 拉 low)， Master 開始送出 clock， 同時 Master 的資料 (MSB, 最高位元)， 也由 shift register 推出，以在 MOSI 上維持住它的值，而 Slave 的資料也是在同一時間送到 MISO，說明了 SPI 是一個全雙工同步的訊號系統。

![](https://i.imgur.com/3FOvziL.png)


---
## Data Modes
* CPOL
    * 寫入1時，時脈閒置時為高準位；寫入0時，時脈閒置時為低準位。
    write 1   ![](https://i.imgur.com/asip4a8.png) 
     write 0   ![](https://i.imgur.com/tuS19Vb.png) 
        |  CPOL0   | Leading edge     | Trailing edge     |
        | -------- | --------| --------|
        | 0  | Rising    | Falling  |
        | 1   | Falling   | Rising   |
        
* CPHA
    * 決定要在時脈的前緣或後緣取樣
    
        |  CPHA0   | Leading edge     | Trailing edge     |
        | -------- | --------| --------|
        | 0  | Sample  | Setup  |
        | 1  | Setup  | Sample   |
   
### The Table of SPI Modes
|  SPI Mode   | Conditions   | Leading Edge  |Trailing Edge|
| -------- | --------| --------|--------|
|0|  CPOL=0, CPHA=0   | Sample (Rising)  | Setup (Falling)|
|1|  CPOL=0, CPHA=1  | Setup (Rising)  | Sample (Falling)|
|2|  CPOL=1, CPHA=0  | Sample (Falling)  | Setup (Rising)|
|3|  CPOL=1, CPHA=1  |  Setup (Falling) |Sample (Rising)|
:::success
舉例：CPOL =0 表示 clock 原本在 low，CPHA =0 表示資料在第1個 edge 被讀取.  也就是 rising edge 被讀取。其他以此類推。
:::

<!-- * 平常使用 SPI Library 時，SPI Mode 預設值為0，SCLK 為4Mhz。
``` c++=
SPISettings() {
    init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0);
  }
```

### SPI Transfer Format with CPHA = 0
![](https://i.imgur.com/br3jTsg.png)

### SPI Transfer Format with CPHA = 1
![](https://i.imgur.com/uLrqhYw.png) -->

---
### 相關函數


| 函數 | 作用 | 
| -------- | -------- |
| SPI.begin()| 初始化 SPI Bus，設置SCK、MOSI和SS為輸出：將SCK和MOSI拉低，將SS拉高|
|SPI.end()|結束 SPI Bus|
|SPI.setBitOrder(order)|設定數據在 SPI Bus 上傳送順序為低位元(`LSBFIRST`)優先或高位元(`MSBFIRST`)優先|
|SPI.setClockDivider(divider)|設定 SPI 時脈的除數(Arduino硬體時脈為16Mhz)|
|SPI.transfer(val)|由 SPI 介面發送並接收 1 個 byte 資料|


---
## LAB1
* 實驗目的 :利用 SPI 進行 Arduino 間通訊
* Arduino UNO 內定 10,11,12,13 為 SPI 通信界面使用
  * Pin 10 ：SS chip select從設備致能信號，由主設備控制
  * Pin 11 ： MOSI  主設備數據輸出，從設備數據輸入
  * Pin 12 ： MISO  主設備數據輸入，從設備數據輸出
  * Pin 13 ： SCLK ，由主設備產生
* schematic

![](https://i.imgur.com/tJEPZp5.png)

* Master example
![](https://i.imgur.com/YMFoZYD.png)

<!-- 
// #include <SPI.h>

// void setup (void)
// {
//   digitalWrite(SS, HIGH);              // 確保SS初始狀態為HIGH 
//   SPI.begin (); 
//   SPI.setClockDivider(SPI_CLOCK_DIV8); //設定時脈為16/8 = 2 Mhz
// }  


// void loop (void)
// {
//   char c;
//   digitalWrite(SS, LOW);               //開始與從機通訊 //SS pin為10
  
//   // 傳送字串
//   for (const char * p = "Hello, world!\n" ; c = *p; p++)
//     SPI.transfer (c);

//   digitalWrite(SS, HIGH);              // 關閉與從機的通訊
//   delay (1000);  
// }   -->


* Slave example
![](https://i.imgur.com/DpVxmIH.png)


<!-- #include <SPI.h>

char buf [100];
volatile byte pos;
volatile bool process_it;

void setup (void)
{
  Serial.begin (115200);   
  SPCR |= bit (SPE);        //開啟從機的SPI通訊
  pinMode (MISO, OUTPUT);   //設定主入從出

  pos = 0;                  // buffer 裡頭為空
  process_it = false;

  SPI.attachInterrupt();    //啟用中斷函式
} 

ISR (SPI_STC_vect)          //SPI中斷程序
{
    byte c = SPDR;          //從SPI Data Register獲取資料(byte)
    if (pos < sizeof buf)   //若buffer空間足夠就寫入buffer裡頭
    {
        buf [pos++] = c;
        if (c == '\n')      // 收到換行字元表示要處理buffer
        process_it = true;
    }
}

void loop (void)            // 等待中斷函式回傳true
{
    if (process_it)
    {
        buf [pos] = 0;
        Serial.println (buf);
        pos = 0;
        process_it = false;
    } 
} 
``` -->
* SPCR 
![](https://i.imgur.com/enGoXKH.png)
[參考資料](https://www.sparkfun.com/datasheets/Components/SMD/ATMega328.pdf)
* ISR 介紹 [click here](http://programmermagazine.github.io/201407/htm/article1.html)

<!-- :::info
關於中斷函式：[**點我**](http://coopermaa2nd.blogspot.tw/2011/04/attachinterrupt.html)
::: -->


# Arduino講義：計步器
## MPU6050模組 
* 這次要練習的是 GY-521 的 Sensor，其核心的晶片是 MPU-6050。
* MPU-6050 是一個內包含<font color="red">三軸陀螺儀</font>以及<font color="red">三軸加速計</font>結合在一起的數位運動處理器(簡稱DMP)，以  <font color="red">  I²C </font>  輸出6軸的旋轉矩陣的數位資料。
 ### GY-521規格  
  * 核心晶片：MPU-6050 模組(三軸陀螺儀 + 三軸加速度計)
  * 供電電源：3-5v（內部低壓差穩壓）
  * 通信方式：標準 I²C 通信協議
  * 晶片內置 : 16bit AD 轉換器,16位數據輸出
  * 陀螺儀範圍：±250 500 1000 2000 °/s
  * 加速度範圍：±2 , ±4 , ±8 , ±16g
* MPU6050 晶片座標定義 :
將晶片平放在桌面上，將其表面文字轉至正確角度，此時，以晶片內部中心為原點，水平向右為x軸，水平向上為Y軸，指向天花板為Z軸。
![](https://i.imgur.com/oJZmbiu.png)


- 三軸加速度計 :
- 加速度計的三軸分量 ACC_X、ACC_Y 和 ACC_Z 均為<font color="red">16位有符號整數</font>，分別表示物件在三個軸向上的加速度，取負值時加速度沿座標軸負向，取正值時沿正向。
- 三個加速度分量均以<font color="red">重力加速度 g 的倍數為單位</font>，能夠表示的加速度範圍，即倍率可以統一設定，有4個可選倍率：2g、4g、8g、16g。
- 以 ACC_X 為例，若<font color="red">倍率設定為 2g （默認）</font>，則意味著 ACC_X 取最小值-32768時，當前加速度為沿 X 軸正方向2倍的重力加速度；若設定為4g，取-32768時表示沿 X 軸正方向4倍的重力加速度，以此類推。顯然，<font color="red">倍率越低精度越好，倍率越高表示的範圍越大</font>，這要根據具體的應用來設定。
- 這三個加速度分量是16位的二進制補碼值，且是有符號的。故而其輸出範圍 -32768~32767。  32767/2 = <font color="red">16384 即加速度計的靈敏度</font>。

* <font color="red"> Q&A </font> 那這個靈敏度怎麼使用呢？我們使用 Serial Port 顯示的一組數據來舉個例子：
     - ACC_X=03702 , ACC_Y=12456 , ACC_Z=06268 , G_X=-00023 , G_Y=-00059 , G_Z=00005 
     - 加速度計 X 軸獲取原始數據位 03702，那麼它對應的加速度數據是：03702/16384 = 0.23g。 g 為加速度的單位，重力加速度定義為1g, 等於9.8米每平方秒。
     - 具體的加速度公式 : <font color="red"> 加速度數據 = 加速度軸原始數據/加速度靈敏度</font>。
* 三軸陀螺儀 : 
* 繞 X、Y 和 Z 三個座標軸旋轉的角速度分量 GYR_X、GYR_Y 和 GYR_Z 均為16位有符號整數。從原點向旋轉軸方向看去，<font color="red">取正值時為順時針旋轉，取負值時為逆時針旋轉</font>。
  * 三個角速度分量均以“度/秒”為單位，能夠表示的角速度範圍，即倍率可統一設定，有4個可選倍率：250度/秒、500度/秒、1000度/秒、2000度/秒。
  *  以 GYR_X 為例，若倍率設定為250度/秒，則意味著 GYR_X 取正最大值32768時，當前角速度為順時針250度/秒；若設定為500度/秒，取32768時表示當前角速度為順時針500度/秒。顯然，倍率越低精度越好，倍率越高表示的範圍越大。
  *  這三個陀螺儀分量是16位的二進制補碼值，且是有符號的。故而其輸出範圍 -32768~32767。32767/2000 = <font color="red">16.4 即陀螺儀的靈敏度</font>。
 
### 下載 MPU-6050 library 
* MPU-6050 因為是靠 I2C 溝通的，所以是需要到 I2Cdevlib 去找相關的函式庫 library 。
* 但怕各位太辛苦，直接下載此處2個檔案即可 [MPU-6050](https://drive.google.com/open?id=1oLRlk2aqHdkppUU8AvR3iSkq-6myVYdo)  、  [I2Cdev](https://drive.google.com/file/d/1EDCvJBOozDBpI7-4ffF62k8mr7yetxEQ/view?usp=sharing) 。
* 將 MPU-6050 與 I2Cdev 資料夾, 丟進 Arduino 的 libraries 資料夾。
  * (Windows 預設路徑是: C:\Program Files (x86)\Arduino\libraries)
### 學習如何讀出 MPU-6050 量測之值
* MPU-6050 腳位 : 
    | Arduino  | MPU-6050  | 備註 |
    | -------- | -------- | -------- |
    | 3.3-5V   | VCC      |<font color="red">注意電源不可接錯</font> |
    | GND      | GND      |<font color="red">注意電源不可接錯</font> |
    | A5       | SCL      |I2C 時脈線|
    | A4       | SDA      |I2C 數據線|
    | 本次不使用 | XDA      |      |
    | 本次不使用 | XCL      |      |
    | 本次不使用 | AD0      |      |
    | 2         | INT     |中斷腳位(本次不使用)|

* 線路接法
![](https://pic1.zhimg.com/ad102a2c0e2f903d86a0c8dc79d81dc4_r.jpg) 

* code 
![](https://i.imgur.com/NT3FoLI.png)
## Lab2
### 計步器實作
* 實驗目標 : 利用 MPU-6050 做出簡易計步器之功能
* 實驗要求 : <font color="red">靜止時或輕輕移動時 </font>，計步器數值不會加一，需<font color="red">超過一定幅度</font>數值才會加一。 
* 實驗步驟參考 :
<font color="red">! ! 提醒 : 此實驗步驟為只使用三軸加速度之值來進行運算</font>
  * Step 1 : 利用講義上面介紹的加速度公式，得量化之加速度數據
  * Step 2 : 透過下方公式將加速度數據代入，回推出變化之角度
     ![](https://i.imgur.com/elp403K.png)
  * Step 3 : 為避免雜訊干擾，需透過前面量測出來之數據來設立閥值進行校準
  * Step 4 : 定義怎樣的移動算一步
* 示範影片 : 
<iframe width="640" height="360" src="https://www.youtube.com/embed/Y54LS3jHnQk" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

## Lab3
### 計步器實作
* 實驗目標 : 完善你的簡易計步器，為其加上LCD以及清除按鈕
* 實驗要求 : LCD 第一行顯示文字，第二行顯示步數。按下清除開關時，步數歸零。
* 示範影片 : 
<iframe width="640" height="360" src="https://www.youtube.com/embed/iNj8Kv7kHRk" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>




<!-- ### 加分 LAB SPI -- 接收 slave 端的回饋值
-   實驗目的：SPI用 master 端與 slave 端溝通方式。
    
-   實驗目標：將 master 端的數值丟入 slave 端進行運算後，slave 端將計算後的數值送回 master 端並於 Serial Monitor 顯示計算結果。

-   schematic：
![](https://i.imgur.com/av0BHIm.png)

-   master：
```c=
#include <SPI.h>

void setup (void)
{
    Serial.begin (115200);
    Serial.println ();

    digitalWrite(SS, HIGH);  
    SPI.begin ();
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    } 

    byte transferAndWait (const byte what) //數值傳送至Slave端
    {
    byte a = ;
    delayMicroseconds (20);
    return a;
    } 

void loop (void)
{
    byte a, b, c, d;   
    digitalWrite(SS, LOW);
    
    /*在此填入加法指令與數值*/
    transferAndWait ('a');
    transferAndWait (0);
    a = transferAndWait (17);
    
    digitalWrite(SS, HIGH);

    /*回傳值顯示在Serial Monitor上*/
    Serial.println ("Adding results:");
    Serial.println (a, DEC);
    
    delay(1000);     
    digitalWrite(SS, LOW);

    /*在此填入減法指令與數值*/
    transferAndWait ('s');
    transferAndWait (0);
    a = transferAndWait (17);
   
    digitalWrite(SS, HIGH);

    /*回傳值顯示在Serial Monitor上*/
    Serial.println ("Subtracting results:");
    Serial.println (a, DEC);
    
    delay (1000); 
} 

```

-   slave：
```c=
volatile byte command = 0;
void setup (void)
{
  /*設定主入從出*/

  /*啟用slave模式*/
  
  /*啟用中斷*/
} 

ISR (SPI_STC_vect)      //SPI中斷程序
{
    byte c = SPDR;
    switch (command)
    {
    case 0:
      command = c;      //將暫存器內的資料設給command
      SPDR = 0;         //清空暫存器
      break;
      
    /*收到加法or減法指令並在此進行計算*/
    
    }
} 

void loop (void)
{
  if (digitalRead (SS) == HIGH)  //若SPI通訊關閉，清除目前command
    command = 0;
}
``` -->
## 課後問題 (三題)
:::info
* Q1.請簡述 URAT、I2C、SPI三介面之特性與傳輸差異。
* Q2.MPU-6050 的接腳 XDA、XCL、AD0 功能為何?
* Q3.下方為不呼叫 MPU-6050 library  來讀出 MPU-6050 數值之程式，
請為其標上 <font color="red">完整的註解</font>。 
```c=
#include<Wire.h>

const int MPU_addr=0x68; 

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  
  Wire.write(0);     
  Wire.endTransmission(true);
  Serial.begin(38400);
}
void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  
  AcX=Wire.read()<<8|Wire.read();  
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read(); 
  GyX=Wire.read()<<8|Wire.read();  
  GyY=Wire.read()<<8|Wire.read(); 
  GyZ=Wire.read()<<8|Wire.read();  
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);  
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  delay(333);
  
}
```
:::