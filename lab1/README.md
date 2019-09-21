# Install the Arduino
* 至Arduino官方網站下載開發環境: [click here](https://www.arduino.cc/en/Main/Software)

* 將檔案開啟

![](https://i.imgur.com/Dwc2efw.png)


* 開起「裝置管理員」，將 Arduino 接上，看一下是否有新增 USB 序列裝置(COM6)，**不一定為 COM6**。

![](https://i.imgur.com/7n0TLcm.png)


* **假如「USB序列裝置」出現無法辨識的裝置:
  方法一、按滑鼠右鍵，選擇「更新驅動程式」，並點選「自動搜尋更新的驅動程式軟體」，可以排除問題。
  方法二、按滑鼠右鍵，選擇「更新驅動程式」，並點選「瀏覽電腦上的驅動程式軟體」，瀏覽下載Arduino的地方(arduino-1.8.8)，找到drivers檔，點選並安裝。**![](https://i.imgur.com/SSvHWRm.png)

  
  

* 按下工具→序列埠，同時將arduino接上，選擇 COM6，即可以使用。

![](https://i.imgur.com/BPaazpM.png)


## Arduino Commonly Used Function
語法與我們平常使用的 **C/C++** 相同，不過Arduino有一些常用的函式。
* **setup()**
    * 在 setup 中的程式只會執行一次，之後進入 loop 的無限迴圈。
* **loop()**
    * 在 loop 中的程式會不斷執行。
* **pinMode(pin,mode)**
    * 用來設定某一腳位為輸入或輸出。
    * pin:腳位
    * mode:輸出(OUTPUT)/輸入(INPUT)
* **digitalWrite(pin,value)**
    * 用來控制某一腳位的電壓高低。
    * pin:腳位(D0~D13)
    * value:狀態(HIGH/LOW)
* **digitalRead(pin)**
    * 讀取某一腳位的電壓高低值。
    * pin:腳位(D0~D13)
* **delay()**
    * 設定程式運行停止時間長度。
    * 單位為 ms，delay(1000) 為延遲1秒。
* **millis()**
    * 回傳 Arduino 從開始執行程式一直到目前為止的千分之一秒數值 (number of milliseconds) ，這個數值在大約 50 天後會溢位 (overflow) ，屆時會從 0 開始計數。

* **Serial library**
    * **Serial.begin()** : 開啟 Serial Port 並設定通訊速率 (baud rate)。
    * **Serial.print()** : 傳送資料到外部電腦。
    * **Serial.println()** : 傳送資料到外部電腦並切換至下一行。
    ![](https://i.imgur.com/yLNqwrz.png)

    * **Serial.read()** : 接收來自外部電腦的資料。
    * **Serial.write()** : 以二進位碼 (即 byte) 輸出。
    ![](https://i.imgur.com/86s2XBp.png)

    * **Serial.available()** :從 RX 接收對方傳來的資料時, 會儲存在Buffer, 可以用 **available()** 函式檢查緩衝區是否已經有資料。
    
    
    

* 其他語法介紹: [Arduino語言手冊](https://h2maker.wordpress.com/arduino/)


# Arduino講義：GPIO

## Arduino UNO Pinout Diagram
![](https://i.imgur.com/IhqkN0O.png)


## Introduction of GPIO
全名 General Purpose Input Output ，通用輸入/輸出，每個 GPIO 端口可透過軟件分別配置成 INPUT 或者 OUTPUT 。

* **Input mode**： CPU 透過 GPIO 腳從外面收資料/信號，可以進行外接感測器(溫度、紅外線感測器等)狀態、高低電平等資訊的獲取。

* **Output mode**： CPU 透過 GPIO 腳將資料/信號送出，可用於控制LED、蜂鳴器及伺服馬達...等。

* GPIO 端口可以當 I2C、SPI (10~13)、UART...等多種用途，也能作為 IRQ (中斷要求)(2、3)的信號。 

## Arduino Hello world：Blink!
```c++=
void setup() {
  // 將腳位D13設為OUTPUT
  pinMode(13, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  // D13輸出高電位，點亮LED
  digitalWrite(13, HIGH);
  // 等待1000ms(一秒鐘)，這期間不動作
  delay(1000);                      
  // D13輸出低電位，LED熄滅
  digitalWrite(13, LOW);    
  // 等待1000ms(一秒鐘)，這期間不動作
  delay(1000);                       
}
```


---
## 模組說明
### LED
  ![](https://i.imgur.com/HhtiO5k.png)
  ![](https://i.imgur.com/JoETTXO.png)
**注意需要限流電阻(大概 220Ω 或 330Ω)，使 LED 流進電流不超過20mA。**

---
### 按鈕(Button)
![](https://i.imgur.com/JCcJ1hl.png)

---
## 程式說明
### LED Control  
  * 實驗目的：學習對 Arduino 的 GPIO 與 LED 基本控制方式。
  * 實驗目標：設定10個 GPIO 來實現跑馬燈。
  *  Schematic
![](https://i.imgur.com/oe2W3b0.png)

* 程式碼
```c++=
#define time 50 //設定延遲時間
int led;     //宣告腳位變數
void setup(){
  for(led=2;led<10;led++)
  {                         
    pinMode(led,OUTPUT); //設定pin2~pin9腳為輸出模式
}
}
void loop(){
  for(led=2; led<12; led++)//pin腳位由低到高
    {
      digitalWrite(led,HIGH);  //給指定之pin腳高電壓(LED亮)
      delay(time);            //延遲時間
      digitalWrite(led,LOW); //給指定之pin腳低電壓(LED暗)
      delay(time);
     }
}

```
# Lab1:
### 跑馬燈實作
  * 實驗目的：學習對 Arduino 的 GPIO 與 Led 基本控制方式。
  * 實驗目標：從右至左，輪流點亮每一個 Led(5顆)，一次只亮一個 Led，再從左至右亮回，不斷循環。
  <iframe width="668" height="376" src="https://www.youtube.com/embed/92EaoSYEFg4" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
  
# Lab2:
### Button&Led
  * 實驗目的：使用 Button 來控制 Led 的明滅狀態。
  * 實驗目標：按一下 Button，Led亮，再按一下，Led 暗。
  * 程式碼:
  ![](https://i.imgur.com/cDTjx0R.png)
  
  
### 彈跳問題:
![](https://i.imgur.com/4HPGYmI.png)


* **當接觸點碰撞在一起時，機械式按鈕的慣性會造成彈跳，將產生不乾淨的訊號。**
  會發生接觸彈跳乃因接觸點之間有抖動。在生活中接觸彈跳的作用是不明顯的，因其發生太快之所以不會影響大部份的設備，但對於反應快速的類比和邏輯電路就將產生問題，會導致將這些 on-off 脈衝被誤解為資料串流。
  
![](https://i.imgur.com/DzuwTLi.png)
### 解決方法:
* 硬體:透過連接一個簡單的 **RC 電路以抑制電壓的快速變化**，可容易解決這個問題。
  由於彈跳的期間沒有定義，這些元件的值決定得不是很準確。
  ![](https://i.imgur.com/cieckB0.png)
* 軟體:當程式在檢查輸入腳位的狀態並且偵測到變化時，應當**在一定時間的延遲後再進行一次檢查工作。** 如果程式確認狀態有變化，代表開關/按鈕已經改變了位置。

# Lab3:
### Button彈跳
  * 實驗目的：解決 Lab2 的彈跳問題。
  * 實驗目標：使 Led 能穩定的開關(按一下 Button，led 亮，再按一下，led暗)。
  * 程式碼:
  
  <iframe width="668" height="376" src="https://www.youtube.com/embed/EyU2tKRmi2o" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


## 課後問題 (3個)
:::info
* Q1.哪兩個腳位是用來序列埠傳送與接收?其功能及名稱分別為何?
* Q2.請說明機械彈跳的現象原理。
* Q3.承上題，請詳述如何解決其問題。 
:::

