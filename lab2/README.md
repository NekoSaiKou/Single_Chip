# Arduino講義：多工七段顯示器與掃描鍵盤

## 七段顯示器
![](https://i.imgur.com/rJDI9BN.jpg)

* **共陽** ( Common Anode ) 為在 COM 端接 HIGH，想亮的腳位接 LOW。
* **共陰** ( Common Cathode ) 為在 COM 端接 LOW，想亮的腳位接 HIGH。

* 如何知道共陽(陰)?
  * 可使用電表歐姆檔先將**正棒**接至七段中心腳(3 or 8)，**負棒**掃過其他腳，若會亮則是共陽極，反之則為共陰極
* 如何使用 Arduino 測共陽(陰)。
  * 將 Vcc 接到 COM 腳(串接電阻)，再將 GND 接出處碰其他腳位，若會亮則次共陽極，反之則是共陰極。圖中為共陽極。
![](https://i.imgur.com/wYAxRZM.png)
* 顯示方法：
![](https://i.imgur.com/xO9qmLO.png)
* **<font color="red">注意!!需額外增加限流電阻，否則內部可能會燒毀!!</font>**

## 多工七段顯示器
![](https://i.imgur.com/EhJVvCa.png)

![](https://i.imgur.com/LLDa3GN.png)

顯示原理：
![](https://i.imgur.com/gWcKghj.png)

### Lab1 - 多工七段顯示器



* 實驗目的：學習對 Arduino 的 GPIO 與多工七段顯示器掃描基本控制方式。
* 實驗範例：控制 4 顆七段顯示器顯示 "2019"
* 實作接線圖:
![](https://i.imgur.com/tSO913N.png)



* **<font color="red">A~G需接限流電阻。!!</font>**

* 實作腳位配置 : 

| 多工七段顯示器 | Arduino |
| -------- | -------- | 
| 0        | Pin10    | 
| 1        | Pin11    | 
| 2        | Pin12    | 
| 3        | Pin13    |
| a        | Pin2     |
| b        | Pin3     |
| c        | Pin4     |
| d        | Pin5     |
| e        | Pin6     |
| f        | Pin7     |
| g        | Pin8     |
| dp       | Pin9     |
* 實作結果:
![](https://i.imgur.com/33xbjS7.jpg)


* 程式碼
![](https://i.imgur.com/EtNbE9N.png)
![](https://i.imgur.com/rCpNz7V.png)
![](https://i.imgur.com/zR4LZwo.png)
![](https://i.imgur.com/Vi0FDiX.png)
![](https://i.imgur.com/LiHbfgm.png)





### Lab2 - 數字設定器

* 實作目的:學習對 Arduino 的 GPIO 控制多工七段顯示器及 button。
* 實作目標:利用 button 來控制四顆七段顯示器使其能上數，長按(約 1 秒)能連續上數。
* 實作影片:
<iframe width="640" height="360" src="https://www.youtube.com/embed/SW0twN1__x4?list=UUQmpO0IQCpe-DNwHk2z9jVQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

* 實作提示:長按功能需使用<font color="red"> millis() </font>函數。
*millis() 函式會回傳 Arduino 從開始執行程式一直到目前為止的千分之一秒數值(number of milliseconds)，這個數值在大約 50 天後會溢位(overflow)，屆時會從 0 開始計數。*
          
### Bonus -   數字設定器V2
* 實作目標：承接 Lab2 ，但必須讓為零的高位數不顯示。( 提示:從副程式 Number_Transfer( )下手 )

* 實作影片:<iframe width="640" height="360" src="https://www.youtube.com/embed/U3Vr__5zoZ8?list=UUQmpO0IQCpe-DNwHk2z9jVQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>          
          
## 4 x 4 Keypad
![](https://i.imgur.com/p5uUq6x.png)

* 掃描原理：
<!-- ![](https://i.imgur.com/1muhJRO.png) -->


![](https://i.imgur.com/wPz5yiU.png)



### Lab3 - 小鍵盤
* 實作目的：學習使用掃描式鍵盤。
* 實作目標：使用 Keypad，達成鍵盤輸入資料，將資料透過 UART 傳輸至內建之 Serial monitor。
* 實作腳位配置 : 

    | 4 x 4 KeyPad | Arduino |
    | -------- | -------- | 
    | 1(ROW1)  | Pin A0   | 
    | 2(ROW2)  | Pin A1   | 
    | 3(ROW3)  | Pin A2   | 
    | 4(ROW4)  | Pin A3   |
    | 5(COL1)  | Pin 2     |
    | 6(COL2)  | Pin 3     |
    | 7(COL3)  | Pin 4     |
    | 8(COL4)  | Pin 5     |


* 實作影片:
<iframe width="640" height="360" src="https://www.youtube.com/embed/K-w15EEFD2E?list=UUQmpO0IQCpe-DNwHk2z9jVQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

* 部分程式碼

![](https://i.imgur.com/zAl4TfV.png)
![](https://i.imgur.com/CPQalar.png)
![](https://i.imgur.com/I36HxL3.png)
![](https://i.imgur.com/MQX88kp.png)




  
### Lab4 - 小鍵盤(Library)  
* 實作目的：學習使用 Library 實現鍵盤掃描。
* 實作目標：使用 Keypad Library，達成鍵盤輸入資料，將資料透過 UART 傳輸至內建之 Serial monitor。
* 實作步驟：
    * [Keypad Library](http://playground.arduino.cc/code/Keypad#.UxlzUOWzLlw)
    * Download here : <font color="red">keypad.zip</font>
    * Put the Keypad folder in “arduino\libraries”.
    * Once the library is imported, an add “<font color="red">#include <Keypad.h></font>”at the top of your Sketch,you can use the library it provided.
    
    ![](https://i.imgur.com/rRap3tc.png)



## 課後問題 (三題)
:::info
* Q1. Arduino UNO 是一款基於 Microchip ATmega328P 微控制器所組成開源微控制器開發板，請問 ATmega328P 這顆微控制晶片有幾支腳位可以用來當作 digital I/O 使用。請參考: [<font color="red"> ATmega328P datasheet</font>](https://www.sparkfun.com/datasheets/Components/SMD/ATMega328.pdf)
* Q2.請簡述多工七段顯示器與掃描式 KeyPad 的工作原理。
* Q3.承接 Lab3，當你按下一顆按鈕不放，再去按其他按鈕時，會發生甚麼況狀? 該如何解決此問題 (Lab3 與 Lab4 的差別)?
:::
