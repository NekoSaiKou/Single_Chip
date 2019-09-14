# Arduino講義:跟隨自走車




## 超音波模組(HC-SR04):

* 超音波模組工作原理:

     ![](https://i.imgur.com/ODTMfKF.png)
     
    * 觸發 : 採用I/O觸發測距，給至少為 <font color="red">10uS</font> 的高電位信號。
    * 發射 : 模組自動發送 8 個 40KHz 的方波，自動檢測是否有信號返回。
    * 接收 : 有信號返回，通過模組之 I/O 輸出高電位，<font color="red">高電位持續的時間</font>就是超聲波 從發射到返回的時間。
    
    ![](https://i.imgur.com/qci8enf.png)
    * 距離計算 = (高電位時間 / 2) * 聲速 。
        * 讀取高電位時間 : 使用 <font color="red">pulseIn( )</font> 此Function，單位為microsecond。 ([Arduino pulseln()](https://www.arduino.cc/en/Reference.PulseIn))
        * 聲速 =  331.5 + 0.607 * t(攝氏溫度)。
          Ex : 溫度為攝氏 20 度，則聲速為 331.5 + 0.607 * 20 = 343.64 m/s。
        * 將聲速還算成 cm/microsecond => 343.64 * 100 / 1000000 = 0.034364 cm/microsecond。
        
* 超音波模組規格與腳位 : 
    * 電源需求 : DC 5V / 2mA
    * 輸出電位( 1/ 0)：5V/ 0V
    * 精度：3mm
    * <font color="red">距離範圍：2 ~ 450cm</font>
    * 有效的角度：<15℃
        
         ![](https://i.imgur.com/LWgbM7Z.png)
             
### Lab1 - 超聲波測距器

* 實作目的 : 學習使用超聲波模組。
* 實作範例 : 使用超聲波模組搭配 LCD 顯示器，當物體在 15 公分內時，將測得之距離顯示於 LCD 上，超過 15 公分，則顯示 Exceed 。
* 實作電路圖 : 
 
 
 ![](https://i.imgur.com/Ue8brEF.png)
* 實作腳位配置 :


| HC-SR04 超聲波模組 | Arduino |
| -------- | -------- | 
| Vcc      | 5V       | 
| Trig     | Pin12    | 
| Echo     | Pin11    | 
| Gnd      | GND      |

 | LCM 1602 | Arduino |
| -------- | -------- | 
| GND      | GND      | 
| Vcc      | 5V       | 
| SDA      | A4       | 
| SCL      | A5       |

* 實作影片 : 

<iframe width="640" height="361" src="https://www.youtube.com/embed/D3kzD2FmErQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


* 部分程式提示 : 

![](https://i.imgur.com/T7PfhuS.png)
![](https://i.imgur.com/ydbR6Qz.png)




## PWM (Pulse Width Modulation) 脈衝寬度調變

* PWM : 
    利用數位訊號模擬類比訊號的方式。
* PWM原理 : 
    藉由數位訊號高頻率的切換，調整開關的比例，就可以模擬出需要的類比訊號。
    
    ![](https://i.imgur.com/JzH4Ugy.png)


    ![](https://i.imgur.com/g1XaOdl.png)
*    Timer Registers
     *    ATmega328 內與 Timer 相關的 Register 大略如下(n 代表 Timern)：
            * Timer Counter Control Register A (TCCRnA)：決定 timer 的運作模式（operating mode）。

            * Timer Counter Control Register B (TCCRnB)：決定 timer 的 prescale value。
            * Timer Counter Register (TCNTn)：儲存 timer 目前的累積計數 (timer count)。
            * Output Compare Register A (OCRnA)：當 TCNTn 累積計數到 OCRnA 的計數時，觸發中斷（interrupt）。
            * Ouput Compare Register B (OCRnB)：當 TCNTn 累積計數到 OCRnB 的計數時，觸發中斷（interrupt）。

        *    Timer0 與 Timer2 皆為 8-bit 的 Timer，表示 TCNT0, 2 範圍為 0 ~ 255；Timer1 的TCNT1則為 16-bit，可以計數 0 ~ 65535。

* 如何使用 Arduino PWM : 
    ![](https://i.imgur.com/0rBZBoi.png)
    Pin 3、9、10、11 頻率為 490 Hz，Pin 5，6 頻率為 980 Hz
* Arduino PWM輸出函數 :
    <font color="red" size="5">analogWrite(pin, value)</font>
    -Pin:輸出PWM腳位。
    -Value:Duty cycle，數值範圍(0~255)。
    -Duty Cycle = Ton / (Ton + Toff) (%)，為 ON 的時間與週期相除的百分比，例如：0=0%，127=50%，255=100%。
    
### Lab2 -  測距警示燈

* 實作目的 : 學習控制 Arduino PWM 以及 超聲波模組。
* 實作範例 : 控制PWM與上節所使用到超音波模組，Duty Cycle會隨超音波測得距離越近而越高，反之則相反。
* 實作電路圖 : 
![](https://i.imgur.com/0JuCStZ.png)

* 實作影片 :
<iframe width="640" height="360" src="https://www.youtube.com/embed/K_ivwCI-eMM" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

<iframe width="640" height="361" src="https://www.youtube.com/embed/HmjG65rJGTc?list=UUQmpO0IQCpe-DNwHk2z9jVQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## L298N馬達驅動模組 : 
* L298N為ST公司所生產的，主要用途在直流馬達的訊號控制與電壓提升，若馬達由Arduino直接供電會電壓不足，但如果經由L298就可以外接供應電5V~35V使馬達得到更大的驅動功率，最大峰值電流可達3A，持續工作電流為2A。
* 雙路H橋馬達驅動模組。
* 驅動電壓為7V-12V（+12V位置）的時候，可以引出5V電壓（+5V位置）供外部使用。
* <font color="red">由此模組供給Arduino的5V需要接至Vin。</font>
* 模組週邊腳位及端子台說明:
![](https://i.imgur.com/0otn2JP.png)
* 模組控制訊號說明:
![](https://i.imgur.com/LcMsOD8.png)
* ENA和ENB腳位，可用<font color="red">PWM</font>來改變馬達轉速。
### Lab3 -  跟隨自走車
![](https://i.imgur.com/HiKbNMm.png)

### 實作說明:

* 實作目的：完成跟隨自走車。
* 實作方法：利用<font color="red">兩組超音波測距模組</font>，並搭配<font color="red">L298N驅動馬達</font>
* L298N接腳：

| L298N馬達模組 | Arduino |
| -------- | --------  | 
| INT1     | Pin2      | 
| INT2     | Pin4      | 
| INT3     | Pin5      | 
| INT4     | Pin6      |
| ENA      | Pin3(PWM) |
| ENB      | Pin9(PWM) |
* 超音波模組接腳：

| 超音波模組 | Arduino |
| -------- | --------  | 
| trigPin_left     | Pin13      | 
| trigPin_right     | Pin12     | 
| echoPin_left     | Pin11      | 
| echoPin_right     | Pin10     |
 * 實作步驟：
    * 組裝自走車。
    * 先試著建立好副程式，讓自走車轉動：
     
    ![](https://i.imgur.com/GgW7hHa.png)
    ![](https://i.imgur.com/k5bSDMq.png)

    * 加入超音波感測，並從序列埠監控室窗觀察。
    * 並藉由超音波測距判斷決策來操控馬達，決策條件說明: 
<font color="red">★50cm範圍外為機器人停止。
★50~20cm範圍內為機器人前進。
★10cm範圍內為機器人倒退。
★20~10cm範圍內為機器人停止。
★兩組超音波測距比較去決策機器人轉動方向。
—以上可依照感測器架設狀況做微調—</font>
    *  本實作部分程式提點:
    ![](https://i.imgur.com/ch9Gm51.png)
    ![](https://i.imgur.com/wq3UAb0.png)
    ![](https://i.imgur.com/yfek1Ds.png)
    ![](https://i.imgur.com/PRflCaW.png)

## 課後問題 (三題)
:::info
* Q1.試說明 超聲波模組感測原理。
* Q2.試說明 L298N馬達驅動模組的雙路H橋(與控制正反轉有關)。
* Q3.試說明 Arduino是如何產生PWM訊號(與內部Timer有關)。
:::