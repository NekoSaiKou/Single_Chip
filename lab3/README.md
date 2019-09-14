# Arduino講義：Keypad密碼鎖
## Intoduction of UART
* **UART** 全文為 **Universal Asynchronous Receiver/Transmitter** ，即**通用非同步收發傳輸器**。
* 通用非同步收發傳輸器是一種[異步收發傳輸器](https://en.wikipedia.org/wiki/Asynchronous_serial_communication)
* 將資料由串列傳輸 (Serial communication) 與平行傳輸 (Parallel communication) 間作傳輸轉換。
![](http://wiki.csie.ncku.edu.tw/embedded/USART_DataTransmission.gif)
* **單工、半雙工與全雙工**

| 單工 | 半雙工 | 全雙工 |
| -------- | -------- | -------- |
|![](https://i.imgur.com/NkVAamw.jpg)|![](https://i.imgur.com/rICbdJp.jpg)  |![](https://i.imgur.com/RBbG5hN.jpg)|
|只支持信號在一個方向上傳輸(正向或反向)，任何時候不能改變訊號的方向。 EX:計算機和印表機之間的通信、B.B.Call。 | 允許信號在兩個方向上傳輸，但同一時間只允許信號在一個信道上單向傳輸。EX:傳統的對講機。 |允許數據同時在兩個方向上傳輸，即有兩個信道，因此允許同時進行雙向傳輸。 EX:電話、手機、RS232。 |
* 原文網址：https://kknews.cc/zh-tw/other/y2av3rj.html 
* **串列 v.s 並列**
  * 串列
  ![](https://i.imgur.com/iR4wqGW.jpg)
    * 一次傳送一個 bit，傳輸速率較慢、成本較低、可遠距離傳輸。
    * EX :滑鼠、鍵盤、電腦網路傳輸、RS-232 介面。
  * 並列
  ![](https://i.imgur.com/WqHNHVN.jpg)
    * 一次傳送多個 bit，傳輸速率快、成本高、僅限短距離使用。
    * EX :25Pin D-Type印表機。
* **非同步 (Asynchronous) v.s 同步 (Synchronous)**
  * 非同步
  ![](https://i.imgur.com/gvCSISx.jpg)

    * 雙方有各自的時脈， 在傳送資料時插入額外資訊，表示資料起始、結束。優點是設定時間短、硬體成本低、機器時脈不同也能傳資料，缺點是單次傳輸的資料量較少。
    * EX : RS-232 (實現方式簡單低廉)、 RS-422 (較長距離的傳輸)、RS-485 (較多的裝置連接數目)。
  * 同步
  ![](https://i.imgur.com/cDlmMqx.jpg)

    * 雙方共用時脈(一次只做一件事)，額外提供時脈訊號，使兩端機器在溝通時能夠藉此同步收發資料。比起非同步傳輸，同步傳輸不需要 start/stop bit ，因此能夠一次傳較多的資料。
    * EX : I2C 、 SPI 。
* **UART**
  * 全雙工
  * 串列
  * 非同步
* 由2個獨立的接收 (RX) 、發送 (TX) 緩衝器構成，可同時發送和接收資料。
* 若 baud 為 9600 (bits/sec)，表示每秒可以傳 960 Bytes 的資料量。
* UART 傳送1 Byte (=8 bits)資料，加上頭尾共為10 bits。

 $$\frac{9600(bits)}{startbit(1 bits)+data(8 bits)+stop bit(1 bits)}= 960Bytes$$
 
[參考資料](https://zh.wikipedia.org/wiki/%E6%B3%A2%E7%89%B9%E7%8E%87)


## UART Protocol
![](https://i.imgur.com/EFrYOMa.png)
* 每個符號由三種資料共10個位元所組成，共分為：
    1. 起始位元 (Start Bit)
    2. 資料內容 (Data)
    3. 停止位元 (Stop Bit)
* 每個資料在傳送前都會有一個低電位的起始位 (start bit) ；資料傳完之後會接著一組停止位元 (stop bit)。
* 因此，若假設資料長度為 8bits，加上頭尾，每傳 1byte 至少需要 10bits。
:::info
在起始位 (start bit) 期間，資料發送端 (TX) 處於低電位；在停止位 (stop bit) 期間，資料發送端 (TX) 處於高電位。
:::
* [UART歷史](http://makerpro.cc/2016/04/understand-what-is-uart/)
* USART_BaudRate:

    * **Baud** 指的是有效數據信號調製載波的速率，**1鮑即指每秒傳輸1個符號**，而通過不同的調製方式，可以在一個碼元符號上負載多個bit位信息。
    * $$Baud = \frac{f_{osc}}{16\times (UBRRn+1) }$$
    * **Baud(bits/sec)** ： Baud rate 
    * **f~OSC~** ： System oscillator clock frequency
    * **UBRRn** ： Contents of the UBRRnH and UBRRnL Registers(用來設定 Baud Rate )。![](https://i.imgur.com/Xgarr18.jpg)
    * Asynchronous Normal Mode:
     $$UBRR = \frac{f_{OSC}}{16\times baud } – 1$$


---
## UART register description
   * **Receive buffer** : 只能讀出不能寫入。
   * **Transmitter buffer** :只能寫入不能讀出。
   * **Interrupt Enable Register** : CPU寫入暫存器告訴UART何時中斷。
### UCSR0A(USART Control and Status Register 0 A)
![](https://i.imgur.com/DSOuooD.png)
* 它是一個暫存器，用於指示串行通信的控制和狀態。
* **RXC0** ( USART Receive Complete ) : 當**接收緩衝區**中存在未讀取的數據時，旗標設為1。用來知道何時能收到一個位元組。
* **TXC0**( USART Transmit Complete ) : 當**發送緩衝區**中發送數據時，**TXC0**（ USART 發送完成 ）變為1。
* **UDRE0** ( USART Data Register Empty ) : 當數據可寫入**傳輸緩衝區**時，**UDRE0**（USART數據暫存器為空）變為1。
### UCSR0B(USART Control and Status Register 0 B)
![](https://i.imgur.com/H73A9Qc.png)
* 它同樣是一個暫存器，用於指示串行通信的控制和狀態。
* **UCSR0B** 暫存器比較重要的是 **RXEN0** 和 **TXEN0**，這兩個位元用來啟用接收器 (receiver) 和發射器 (transmitter)，一般來說，我們會啟用這兩個元，可以用來開啟傳送、接收的位元。
* **UDRIE0** (USART Data Register Empty Interrupt Enable 0) : 決定要不要啟用中斷的位元。如果 **UDRIE0** 設置為 1，則允許UDRIE0標誌中斷。
### UDR0(USART I/O Data Register 0)
![](https://i.imgur.com/4BfEyf4.png)
* 它是用來存放資料的暫存器。
* **TXB** 和 **RXB** 具有相同的 I/O address (共用UDR0)。
* 當寫入資料到 **UDR0** 時，資料會寫入**TXB register**。
* 當要讀取 **UDR0** 的內容時，實際上是從 **RXB register** 讀取內容。


---

## Bluetooth 

* 藍芽模組 HC-05，使用 UART 介面與 Ardunio 溝通。
* 藍芽模組也可以視為一塊 MCU 並且可以透過 UART 介面進行設定。
* 設定方式:
  * 透過 USB to UART 轉換電路 ，從電腦開啟 Serial Port 做寫入動作。
  * 利用 Ardunio uno 開發板本身就有的 IC 來做轉換即可寫入 AT 指令。
* 藍芽設定接線方式
  * **同時按住紅框鈕及接上 5v 即可進入 AT 模式。** 
   ![](https://i.imgur.com/K11ITRH.png)
  * 接線:
    | Arduino  | 藍芽模組  | 備註 |
    | -------- | -------- | -------- |
    | 3.3V     | KEY      ||
    | 5V       | VCC      |注意電源不可接錯 |
    | GND      | GND      |注意電源不可接錯 |
    | RXD(pin0)| TXD      |         |
    | TXD(pin1)| RXD      |      |
    
    ![](https://i.imgur.com/pDl8ZEn.png)
    *  程式碼:
  ```c++=
  #include  <SoftwareSerial.h>
  SoftwareSerial BTSerial(10, 11); // RX 、 TX
  void setup()
   {
    Serial.begin(9600);
    BTSerial.begin(38400);  // HC-05 default speed in AT command more
   }
  void loop()
   {
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available())
    Serial.write(BTSerial.read());
  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
     BTSerial.write(Serial.read());
     }
  ```


* HC-05 AT 指令設定
  * 打開 Serial Port。
  * Serial Monitor 的鮑率選**9600**，另外一個選項要選 **NL&CR**，即可下 AT 指令。
  * 輸入 AT 指令第一次會出現 ERROR:(0)，第二次輸入 AT 出現 OK ，即可進入 AT 模式。

  
 ![](https://i.imgur.com/9zBsLA4.jpg)



  * **指令查詢** :
     - **查詢 Baud rate:**
       AT+UART?。
       
     - **查詢 address:**
       AT+ADDR?
       
     - **查名字:**
       AT+NAME?

  * **設定名稱 ：** 
    AT + NAME=XXXX 
  * **設定密碼 ：** 
    AT + PSWD=XXXX 
  * **設定 Baud Rate (bit/sec) ：** (若設定為9600)
    AT + UART=9600,0,0  (鮑率 , 停止位元 , 同位位元)
* 其他參數設定可以參考 [HC-05	Datasheet](http://www.techbitar.com/uploads/2/0/3/1/20316977/hc-05_at_commands.pdf)
   
* 利用 Android 手機下載“Bluetooth Terminal”。![](https://i.imgur.com/oDo5zqU.jpg)
* 進入藍芽設定介面，按掃描裝置，找尋自己的的藍芽名稱。
* 輸入密碼(若無設定，預設為1234)。
* 開啟 app ，選定裝置後按下 connect 即可連線。
* <font color="red">成功連線時，藍芽模組上的警示燈呈快閃兩次循環。</font>
:::info
藍芽燈狀態:
  1. 不停閃爍:正常模式/等待連接模式
  2. 2秒亮2秒滅: AT 模式/配置模式
  3. 每秒閃爍2次:連接成功
:::

---



# Lab1:
  ### 藍芽實作
  * 實驗目的：學習使用藍芽控制。
  * 實驗目標：透過手機藍芽來控制led明滅燈。
  
  
  * 結果:<iframe width="668" height="376" src="https://www.youtube.com/embed/adcjwJ0fhdE" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


  
# Lab2:
### 密碼鎖實作
  * [Password Library](https://playground.arduino.cc/code/password)
  
https://github.com/phrac/arduino/tree/master/tutorials

  * 實驗目的：學習使用Keypad及序列埠視窗。
  * 實驗目標：使用Keypad來輸入4位數密碼(如:1234)，輸入正確即印出Unlock!，錯誤即印出Wrong passwords!。
  * 程式碼:
    ![](https://i.imgur.com/W4U061U.png)

  
# Lab3:
### 密碼鎖實作
  * 實驗目的：學習使用 Keypad 及序列埠視窗。
  * 實驗目標：呈上題，增加可以修改密碼及 reset (若密碼輸錯，可重新輸入密碼)的功能。
  * 結果:![](https://i.imgur.com/62dknAu.png)


# Lab4:
### 密碼鎖實作
  * 實驗目的：學習使用 Keypad 及多工七段顯示器。
  * 實驗目標：使用 Keypad 來輸入4位密碼，並顯示於七段顯示器上，輸入正確即印出 good ，錯誤及印出 bAd (需有清除輸入密碼的功能)。
  * 結果:<iframe width="668" height="376" src="https://www.youtube.com/embed/g4ZLTIdOPXY" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
  
 ## 課後問題 (3個)
:::info
* **Q1.** 請問串列及並列的差別為何?裡面的 **UART的溝通模式** 又屬於何種?
* **Q2.** 試說明 **UCSR0A** 以及 **UCSR0B** 的功用。
* **Q3.** 請說明 **HC-05** 作為 **master** 以及 **slave** 的功用是什麼?
:::
