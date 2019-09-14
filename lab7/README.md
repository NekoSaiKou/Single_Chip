

# **Arduino講義：Timer的使用** 
## 微控制器晶片：**ATmega328p**

* ATmega328p是Atmel發展的megaAVR產品線其中一個微控制器產品，也是目前Arduino Uno系列等常使用的8 bits AVR架構微控制器核心。
:::info
**AVR架構：**
特點：將Flash、EEPROM、SRAM通通整合在一塊晶片裡，還包含了其他功能，諸如輸出入腳位、計時器、序列傳輸埠、數位類比轉換。
**[其他參考1](https://andychen15577.blogspot.com/2016/10/avr-cpu-core_9.html?fbclid=IwAR1Tda-RZZQ9c63Y-UtTQlbZFBWqTEeqWXVfD31xY1VQ1Sb0gtCPQCsi0Zw)** / **[其他參考2](https://read01.com/zh-tw/kEMDP4.html#.XMbTBTAzaUk)**
:::

* **硬體規格摘要：** 32KB 快閃記憶體、1KB EEPROM、2KB SRAM、23個通用輸出輸入線、32個通用工作暫存器、三個具備比對能力的計時計數器、內部與外部中斷、UART、一組I2C、一組SPI串列埠、6通道10位元類比數位轉換器(TQFP及QFN/MLF包裝中只有8通道)、一組具有內部震盪器的可程式化看門狗計時器(128kHz)、五組可選擇的功率節約模式。





![](https://i.imgur.com/SFav7PT.png)

* **上圖為ATmega328p DIP 封裝晶片外觀，可觀察到共有28腳位。**

![](https://i.imgur.com/O6P4gyk.png)

* **上圖為ATmega328p 28支腳位功能及記號示意圖。**

![](https://i.imgur.com/zRqNRgN.png)

* **上圖為Arduino Uno開發板外觀及對應腳位功能及記號示意圖。**

藉由以上兩張圖示，我們可以互相連結功能相同之部分，並了解到當使用Arduino Uno腳位時，代表著控制ATmega328p晶片的哪一部份腳位功能。

---

## **Arduino 抽象層解析**

### **Uno板與晶片的關係：**

* 首先從ATmega328p的 [datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf) 開始
## 暫存器 (register)

* 以下將先行介紹最易理解的**輸出入暫存器**( I/O Ports)：
    * (Digital Output)**數位輸出**可將微處理器內部的資料或訊號傳送到外部的元件，或藉由輸出腳位驅動觸發外部元件 。
    * (Digital Input)**數位輸入**可將外部元件的訊號或狀態擷取到微處理器的內部暫存器，透過適當的運算處理以達成應用程式之目的。
    
    * 下表為Datasheet PORTB 所使用的暫存器位址及對應名稱及查詢頁數，通常底層的標頭檔或其他定義檔，一般來說會按照Datasheet的這些命名方式來命名。
    * 因ATmega328p為8 bits結構，因此我們對暫存器的選取以8 bits (一個byte)為單位，但也可以針對單一bit做存取，存取時需詳閱Datasheet以了解造成什麼功能上的變動與效果。
    
![](https://i.imgur.com/eyueEsw.png)

(此圖來自datasheet章節-"Register Summary")

---

* **ATmega328p微控制器總共有三組數位輸出入埠**，分別為**PORTB**、**PORTC**、**PORTD**。每一組輸出入埠都可作為一般的數位輸入（Digital Input）與數位輸出（Digital Output）使用。

* 每一組輸出入埠都有三個暫存器：

    * **DDRx register (資料方向暫存器) :** Configures pins as input or output. 
      (e.g . let the pin for a button or an led? (1=**OUTPUT** and 0=**INPUT**))
    * **PORTx register (輸出/輸入暫存器) :** Write data to pins turn on or off the led.
      (1=**HIGH** and 0=**LOW**, on **INPUT** pins ：1=**PULLUP**)
    * **PINx register (讀取腳位暫存器) :** Read data from pins. 
      (Is the button pressed or not? (1=**HIGH** and 0=**LOW**))
* 下圖為Datasheet中對應Arduino pin 13的PB5所屬腳位控制暫存器組，可從I/O章節的描述找到詳細說明。

![](https://i.imgur.com/C2lk1QK.png)



---

從這裡開始，我們將更深入地研究Arduino的韌體結構。首先，我們將討論之前的簡單實做
    - ***Blink Led***。

* 下圖紅線紅框意指Arduino Uno上內建之LED指示燈號腳位，其Pin 13與晶片的第19腳位相連，並且我們可以得知其佔存器編號為PB5，同時還可了解到該腳位本身也是SPI溝通之SCK腳位。



![](https://i.imgur.com/PDJWoGV.png)

![](https://i.imgur.com/1s8DkES.png)


**以下為Blink LED原始程式碼:**
``` c=0
//Original code of "blink LED" - look easy :)
void setup() {

  pinMode(13, OUTPUT);
}


void loop() {
  digitalWrite(13, HIGH);    
  delay(1000);                       
  digitalWrite(13, LOW);    
  delay(1000); 
}
```

---

* 通過上面介紹Arduino Uno及其ATmega328p晶片與暫存器映射的關係，我們可以像以前一樣用你的Arduino IDE測試下面修改過的“***Blink LED***”程式碼，它應該要會像以函式包裝的原始程式碼一樣地工作：


``` c=0
// a little abstract "Blink LED" code style ?! :)
void setup() {    
 DDRB = 0b00100000;     //set PB5 as output pin
}  
  
 
void loop() {  
 PORTB = 0b00100000;    //set PB5 as high state
 delay(250);            //delay
 PORTB = 0b00000000;    //set PB5 as low state
 delay(250);            //delay
}
```

* 或者可以使用Bitwise技巧來改寫代碼，如下所示：

``` c=0
//modify the above code with bitwise skill :)
void setup() {    
 DDRB = 0b00100000;             //set PB5 as output pin
 PORTB = 0b00000000;            //default set PB5 as low state
}  
  
void loop() {  
 PORTB = (1 << 5) | PORTB;      //or :PORTB |= (1 << 5)
 delay(250); 
 PORTB = ~(1 << 5) & PORTB;     //or :PORTB &= ~(1 << 5)
 delay(250); 
}
```

![](https://i.imgur.com/OwlnizD.png)

![](https://i.imgur.com/u2DlERT.png)

:::info
Arduino 的位元操作運算子：

![](https://i.imgur.com/pNlh4j0.png)

led toggle for example : PORTD ^= (1 << led_pin);


:::

---

## **Timer 暫存器**

![](https://i.imgur.com/F8Z7mLV.png)

看看Datasheet，我們會找到一些關於配置計時器(Timer)的暫存器。那麼，什麼是計時器？


* 計時器 (Timer)：
    * 用來計算時間的長短
    * **Time = Counter Value × Tcy**  (Tcy = 指令週期)  (**e.g.** 16Mhz的clock倒數為其指令週期)
* 計數器 (Counter)：
    * 用來計算事件發生的次數
    * Counter Value (計數值)

* 下圖為500ms計數器例子，在**TCNT1計數器**到一定數值後(**所花時間剛好等於500ms**)觸發事件，執行事件為重置為零、反轉LED燈。

   ![](https://i.imgur.com/6TxekeS.png)

* ATmega328p中有三個具備**比對能力**的計時計數器，其中各個Timer可以設定的模式各有不同，舉例以**Timer1**計時器而言，總共具備3種計時模式：

    * **TIMER1_COMMPA** (可以設定計數參數來觸發)
    * **TIMER1_OVF**    (只要overflow就觸發)
    * **TIMER1_CAPT**   (儲存輸入腳位的觸發瞬間時間)

![](https://i.imgur.com/cdccM6t.png)



---

## 如何計算可產生500ms的"***計數器***"和"***除頻數***"？

- ATmega328p內部晶振頻率為16Mhz ->週期：**1 / 16Mhz**（s）
- 如果我們想要500ms = 0.5（s）->意味著寄存器應該計數 0.5/（1 / 16M）= **8000000**（次）

- 但是我們的寄存器最大長度只有16 bits（**65535**）

- 所以我們需要**除頻**晶振頻率，下圖為Datasheet中ATmega328p的Timer1可用的除頻範圍及其暫存器設定。

-- ![](https://i.imgur.com/vZgaCih.png)
- 我們選擇256除頻16Mhz時脈並獲得8000000/256 = **31250**，這意味著寄存器可以在**256個時脈通過後計數一次**。
- 最後我們可以通過使用中斷模式來使用**500ms**定時器做特定的工作!!
- 其餘Timer 計時的使用方式以此類推，或詳閱Datasheet以獲得更多細節。


---
:::warning
**Warning**：當你更改一些關於計時器的寄存器時，你可能會影響Arduino庫中的原始功能，並可能導致錯誤，所以你應該仔細使用計時器。下圖為Arduino函式庫中有使用到ATmega328p Timer暫存器的分類整理。
:::


![](https://i.imgur.com/jWOKtvB.png)



---

### *Lab 1* - **運用Timer1**

* 實作目的： 學習使用Timer1 暫存器 及 I/O暫存器操作。
* 內容要求：使用**Timer1暫存器**來改變LED閃爍頻率，***禁止使用delay()函數***，取而代之請使用ISR()函數來呼叫對應之中斷向量。**請利用如下所示之範例及其提示(TODO：)進行修改**：

``` c=0
const int led_pin = 13;
//TODO：here you can set a variable t1_load = ?
//TODO：here you can set a variable  t1_comp =?
void setup(){
    pinMode(led_pin, OUTPUT);   //this line should change into register setting.
    //comment above funciton and do the "TODO" hint as follwoing
    //TODO: set the regisster of pin 13 as output mode by DDR?
    //TODO: reset TIMER1 control register by TCCR1?
    //TODO: set the prescaler to adjust 16Mhz to map to 0.25s by TCCR1?
    //TODO: reset Timer1 by TCNT?
    //TODO: set compare value by OCR1?
    //TODO: enable  Timer1 compare interrupt mode by TIMSK?
    //TODO: enable global interrupt by ? -> try to find this funciotn!!
}

void loop(){                   //the following content should be removed and keep blank in loop.
    digitalWrite(led_pin, HIGH);            
    delay(250);
    digitalWrite(led_pin, LOW);
    delay(250);
                               //loop() does nothing when using Timer1 interrupt 
}

//TODO: create a ISR() to control the routine of Timer1 interrupt and led toggled. 
```
:::  info
#### 小-提-示：
I.   請看[datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf) ***Timer/Counter1 with PWM - Register Description*** in page 108 ~ 112.
II. some parameters exist in the files of ***iom328p.h*** and ***interrupt.h*** in the following path:
* YourMainArduinoFolder ***\hardware\tools\avr\avr\include\avr***
:::
### ***Lab2*－簡易碼表**
* 內容要求：
    請運用上述Timer1暫存器設計方法，完成能夠顯示**00(hr):00(min):00(sec):00(10ms)** 之簡易碼表，**如同Lab1不可使用delay()**。



---


## **睡眠模式 和 看門狗計時器**

* 睡眠模式在ATmega規格書的Ch7中，它是開發人員節省電力的一種方法。
* 在睡眠模式下，系統幾乎完全停止，只留下基本的偵測功能，因此只消耗極少的電量。
* ATMega328p 微處理器具有**六種**睡眠模式。
* ![](https://i.imgur.com/j5y01hc.png)
* 睡眠模式耗電量測試圖：
    * ![](https://i.imgur.com/in2HFic.png)



---
**範例程式：**
* 本實驗程式的行為如下：
*  啟動時，每隔0.5秒點、滅三次位於第13腳的LED。
* LED閃爍完畢後，進入“Power-down（斷電）”睡眠模式。
* 當中斷0（第2腳）的訊號改變時，喚醒Arduino，再次閃爍LED三次，接著再進入睡眠模式。
* PS: 請先把Arduino的數位腳2接高電位（5V或3.3V）
* [Enerlib](https://playground.arduino.cc/Code/Enerlib/)
``` c=0
#include <Enerlib.h>
 
Energy energy;             // 宣告"Energy"程式物件
 
const byte swPin = 2;      // 開關腳位
const byte ledPin = 13;    // LED腳位
byte times = 0;            // 記錄執行次數
volatile byte state = 0;   // 暫存執行狀態
 
void wakeISR() {
   if (energy.WasSleeping()) {
    state = 1;
  } else {
    state = 2;
  }
}
 
void setup() {
  Serial.begin(9600);
 
  pinMode(ledPin, OUTPUT);
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH);
 
  attachInterrupt(0, wakeISR, CHANGE);  // 附加中斷服務常式
  
  Serial.println("Running...");
}
 
void loop()
{
  if (state == 1) {
    Serial.println("Was sleeping...");
  } else if (state == 2) {
    Serial.println("Was awake...");
  }
  state = 0;
  
  digitalWrite(ledPin, !digitalRead(ledPin));
  delay(500);
  times ++;
  Serial.println(times);
 
  if (times > 5) {
    times = 0;
    Serial.println("Go to sleep...");
    energy.PowerDown();
  }
}
```
:::info
**volatile**  definition:
volatile principle, please click [here](https://zh.wikipedia.org/wiki/Volatile%E5%8F%98%E9%87%8F).

:::


---


* **看門狗**是一種專為系統復位而設計的方法。

* Arduino微處理器上的看門狗定時器只有一個外部晶振：它有自己獨立的內部128kHz振盪器（而不是8/16位內部Timer使用的16Mhz內部晶振）。正是這個獨立的振盪器使WDT能夠在最低功耗模式下工作：SLEEP_MODE_PWR_DOWN。

* WDT還有一個除頻器，用於配置超時周期。它支持從16ms到8秒的超時時間：

* ![](https://i.imgur.com/QCEsVz3.png)![](https://i.imgur.com/8Cdop0R.png)
* 它還有三種操作模式：
    1. 中斷 - 當WDT超時時，將調用WDT_vect中斷向量。可用於喚醒微睡眠模式，包括最低功耗睡眠模式（SLEEP_MODE_PWR_DOWN），其他定時器不可用。
    2. 系統復位 - 將發生看門狗超時復位，即微控制器將重新啟動。
    3. 中斷和系統復位 - 首先將調用WDT_vect中斷向量，完成後將發生看門狗超時復位。
* ![](https://i.imgur.com/DgnnZJr.png) 


---

### **範例程式：**
**以下示例程式碼顯示了當我們故意製作記憶體溢位錯誤時，看門狗定時器如何重置整個系統。**


``` c=0
//Sample of watchdog timer

#include <avr/wdt.h>

volatile int f_wdt = 1;
volatile int f_condition = 0;

ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt = 1;
  }
  else
  {
    Serial.println("WDT Overrun");
    f_condition = 1;
    
  }
}

void setup() {

  Serial.begin(9600);
  Serial.println("Initializing...");
  delay(100);  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  /* set new watchdog timeout prescaler value */
  WDTCSR = (1<<WDP0) | (1<<WDP3);
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR = WDTCSR | (1<<WDIE);
  
  Serial.println("Initialisation complete.");
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(f_condition == 0)
  {
    int shut = 1/0;
  }
  else
  {
    Serial.println("watchdog has resetted the system");
  }
}
```


---
### *Lab 3*：睡眠喚醒
* 嘗試在上面的示例代碼中使用中斷模式，每隔8秒將Arduino從睡眠狀態喚醒，以切換LED的狀態，會使用到power.h以及sleep.h裡的函式，請同學按照下方提示並參照上方範例程式。**(hint: 綜合運用Watchdog 及 sleep mode)**
 ``` c=0
 #include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// start coding below 
 ```

---

:::info
### *加分題---Lab 4* ：PWM模擬

* PWM 是使用高、低電壓時間週期來為訊號編碼，以模擬類比訊號為例，基本原理是若在一個時間周期中，50% 的時間輸出高電位 5V，而 50% 輸出的時間輸出低電位 0V，那麼就整個時間周期來說，平均電壓可視為 2.5V，類似地，如果 90% 的時間輸出高電位 5V，那麼就整個時間周期來說，平均電壓可視為 4.5V，若 10% 的時間輸出高電位 5V，那麼就整個時間周期來說，平均電壓可視為 0.5V。
* ![](https://i.imgur.com/w3HNSXt.png)
* ![](https://i.imgur.com/Dw8I2FZ.png)
* 換言之，調整PWM的重要參數為Ton 及週期

---

* **試利用Timer2暫存器實現PWM去模擬出digitalWite()功能並以示波器呈現完成頻率為100Hz簡單的LED fade實驗，請善用datasheet第17章。**


:::

:::info
Hint：
![](https://i.imgur.com/NivP9N8.png)
-
![](https://i.imgur.com/RT1B8Oy.png)
![](https://i.imgur.com/hDebhK8.png)
![](https://i.imgur.com/0jWvf4f.png)
![](https://i.imgur.com/ujqTVlN.png)

PS:會用到以上暫存器&腳位。


:::

## 課後問題
:::info
* **Q1.** 試尋找出定義DDRB、PORTB、PINB之標頭檔相對路徑 ?

* **Q2.** 承上題，為何程式碼中沒有#include該標頭檔，但編譯時編譯器仍然認定編譯成功呢？(PS：委婉地說，可參考補充資料)

* **Q3.** 請觀察在LED fade實驗中，使用Timer2的PWM、使用digitalWrite() 兩者有何差別呢？其差別在LED blink改寫實驗中也有同樣現象，請舉出兩項。
:::

## 補充資料


:::info
### Arduino IDE 內部運作原理流程圖：
Note: How does Arduino Uno turn this code to AVR chip in Arduino IDE? [click here](https://github.com/arduino/Arduino/wiki/Build-Process)

![](https://i.imgur.com/UIykjwR.png)

:::




:::info
### *閱讀datasheet的方式及順序(可參考看看)：*
**這些參數會對應到datasheet的相應的暫存器:**

| Name | Bit7 | Bit6 | Bit5 | Bit4 | Bit3 | Bit2 | Bit1| Bit0|
| -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |
| **PORTB**     | PORTB7     | PORTB6     | PORTB5     | PORTB4     | PORTB3     | PORTB2     | PORTB1     | PORTB0     |
| **DDRB**    | DDRB7     | DDRB6     | DDRB5     | DDRB4     | DDRB3     | DDRB2     | DDRB1     | DDRB0     |
| **PINB**     | PINB7     | PINB6     | PINB5     | PINB4     | PINB3     | PINB2     | PINB1     | PINB0     |
**然後到對應章節閱讀這些暫存器的使用方式**:

![](https://i.imgur.com/gQT3x3Y.png)![](https://i.imgur.com/sbzutNh.png)


:::




:::success
實際上IC內部實現暫存器對應的腳位功能依然是硬體，例如上述I/O Port的硬體實現如下圖：

![](https://i.imgur.com/CxXhnLE.png)

或者像Timer各功能的實現如下圖：

![](https://i.imgur.com/jQy53ie.png)

Datasheet中有些暫存器命名方式可以透過所提供的硬體方塊圖來了解其操作原理，當不了解該暫存器的文字說明時，可以利用這些方塊圖來嘗試理解該暫存器運作原理，在此提供大家參考。
:::