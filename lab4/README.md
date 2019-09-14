# Arduino講義：Serial Communication (I2C)
---

<!-- > **重點 or 縮寫** 請標**黑粗** 且 英文記得空格ㄚㄚㄚㄚ -->
<!-- > -->
<!-- > [name=YuanHaoHo][color=#c8ff05] -->

## Introduction of I2C
* I2C bus 為 Inter-Integrated Circuit Bus 的縮寫，中文叫做積體電路匯流排，其功用為提供 IC 之間的連接與通訊。
* 只有兩條訊號線，<font color="red">SCL(Serial CLock Line) 負責傳時脈，SDA(Serial Data line)負責傳資料</font>。
*  **I2C** : <font color="red"> 半雙工、串列、同步</font>
* 允許有多個 slave。

![](https://i.imgur.com/jOQudK1.png)
* 須接<font color="red">上拉電阻</font>，將浮接狀態( float )轉變為邏輯高準位( High )狀態。
* 閒置( idle )時兩者皆為高電位。
![](https://i.imgur.com/X4RwG5W.gif)
* START condition：<font color="red">SCL為 HIGH時，SDA發生下緣</font>

* 先傳地址( slave address，決定跟哪個 slave 溝通)，再傳資料。地址有 7bit跟 10bit兩種。 
  * 7bit：最大可以有 128個 slave (但實際上有些地址是保留的)。
  * 10bit：最大可以有 1024個 slave 。(但很少見，不是所有使用 I2C 的裝置都支援 10bit mode)。
* 地址：由於地址為 7bit，因此在傳第一個 byte的時候，前 7bit(MSB 為 address，最後的 bit 為 R/W bit，指定要"讀"或"寫"資料(0：write，1：Read)。
 * 只在 <font color="red">SCL 為 HIGH 時，才會對 SDA 取樣。 SCL 為 LOW 時，不會取樣，但可改變資料</font>。 
* STOP condition：<font color="red">SCL為 HIGH時，SDA發生上緣</font> 


* 寫資料的情況：
![](https://i.imgur.com/ThlOXjN.gif)
    * R/W為0時，代表 slave 端要讀取master端資料，此時 slave 會發出一個 ==acknowledge bit==(就是由 slave 把 SDA 拉低)，讓 master 確認 slave 是否有收到資料。
* 讀資料的情況：
![](https://i.imgur.com/PRWtSDN.gif)
    * R/W 為1時，代表 master 端要讀取 slave 端資料，此時 master 為 Receiver，slave 為 transmiter，因此 acknowledge bit 會是由 master 進行發送。
:::info
note：傳地址後的 acknowledge bit 必為 slave 傳送給 master；傳送 Data 後的 acknowledge bit 則須視前面 R/W 的值而定。
::: 

* 當 master 讀到 ACK 回應, 即可以繼續傳輸下一個 Byte。
* 當 master 讀到 NACK 回應, 意思是 **slave 無回應**，此時 master 應送出 (結束) 訊號放棄該命令。
* 參考資料：
    * [資料來源](http://i2c.info/i2c-bus-specification)
---
### 相關函數

<!-- > 函數端應該並非只有 wire 可以使用，可以稍微提及其他函式庫
> [name=YuanHaoHo][color=#c8ff05] -->

#### Master端：
| 函數 | 作用 |
| --- | --- |
| Wire.begin();| 啟動 Wire 作為 master  |
| Wire.beginTransmission(address);| 開始對 address 的連線 |
| Wire.endTransmission(); |關閉之前的連線 |
| Wire.write(data); | 在連線上送出 一個 byte 的資料 |

#### Slave端：
| 函數  | 作用 |
| --- | --- |
| Wire.begin(address); | 啟動 Wire  |
| Wire.onReceive(function) | 設定用來接收資料的函數 |
| Wire.available(); |  檢查連線上是否有可接收的資料 |
| Wire.read(); |  讀取連線上的一個 byte 的資料 |

#### LiquidCrystal_I2C:
| 函數  | 作用 |
| --- | --- |
| lcd.begin(cols, rows); | 定義LCD的長寬（n列×n行）  |
| lcd.clear() | 清空LCD |
| lcd.setCursor(col, row); |  設定輸入值起始位置 |
| lcd.print(data); |  在螢幕上顯示資料 |


:::info
note：在無(address)時，Arduino 是作為I2C的 master；有(address)時，是作為 I2C 的 slave 。
:::
<!-- :::info
note： Arduino 沒有 I2C ，只有**2 wire serial interface(TWI)** ，但是這個TWI**相容於飛利浦的I2C協定**。詳見 [ATmega328P datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf) p.260。
::: -->
## LAB1
* 實驗目標:利用I2C進行Arduino間通訊
* schematic：

![](https://lh3.googleusercontent.com/Zuw2QyEtARMvNPyQhm72J0vKZbImM-SgRYTSvgxM_vK2aZaJSFz80pgbLX-CxtcEfyTvyur7rJBPvsqy19RRLYB-49Q0CsMxrd2Ygr1VRxxlyDRu8R2J_v2MvzWFI_iLIxEEUv-ppXdPGpPDfU0j5hzEeOyPlAZy78A-5by_OYQgobjxNRHDdEkDBpoaGpu3vdg3qsbXvBQihFtwOCf9cUvcYH26-bDkTCXM_WvvChB7QyY-vsmyhk_bJTb36AbqxnTcxlTY392h-PM0fRj0Sv-d5aXFYoxPgee8KlnGzP2yVS5E0xQfTRMcWl04polf8nYHATMgbF2JqHKa_b2o5kra9LAID0sARbarG42-hMdeFmz-Y_EnAefclJ0jOMWV3S7WaRizZG3JBXi9KDz66GjlzuwSrk7w2ogbBAHqfAJGeyDOWlcabNk8JON_REXswwdF4IHfYn72Yo7fF-O5FsaeGjgFrWl9erxqvgAhX6JknX6PYhSBMZt6uXwS_OgxCZ-tz6mntN5y27mjjVSvfZkjJ6pnVemA6m9ivJfQkWeDsSzEslzneN4CiHhauXeeM1Glly42qNm973rxCADrB1KYeSR2--4ym61XbOYuDz1NEWU0bUuCyS3X95I8NH7w7eiEccBbQYGaPRd74r5GOuxTepcNqMTf1kXbydyBhvvOHbFT1pSXxrtT=w941-h740-no)
* master 把串口收到的資料，發送給 slave：
![](https://i.imgur.com/DlWNGZv.png)
 

* slave 端直接處理接收回來的資料：

![](https://i.imgur.com/jZSH5V9.png)

:::warning
若使用的 Arduino IDE 版本不能同時開兩個 Serial Monitor，請[點此](http://ttssh2.osdn.jp/)下載Tera Term後，點選設定做為第二個Serial Monitor。
使用方法：開啟Tera Term後，點選**設定** -->**連接埠** -->**更改端口與鮑率**
:::


# Arduino講義：LCD
## LCD介紹
* 目前市面上的 LCD 模組可分文字型及繪圖型兩種。文字型 LCD 模組常見的規格有16 字x1 行，20 字x1 行，40 字x1 行，16 字x2 行，20 字x2 行，40 字x2行等多種。

* 文字型 LCD 顯示器內部具有字元產生器，因此可以接受 ASCII 字元碼，並將字元顯示在 LCD 上。
## LCD架構
* LCD 模組的結構如圖所示，由 LCD 控制器、LCD 驅動器及 LCD 顯示器所組成。目前市售 LCD 模組所採用的 LCD 控制器都相容，使用方法也相同，因此幾乎所有的 LCD 模組都具有相同的控制方法。
## I2C 16x2 LCD介紹
![](https://i.imgur.com/AOFuvA4.png)
* SDA – 接 Arduino 的 Analog Pin 4 (Arduino Mega 為 Pin 20)
* SCL – 接 Arduino 的 Analog Pin 5 (Arduino Mega 為 Pin 21)
* GND – 接 GND
* VCC – 接 +5V
## LCD接法
 ![](https://i.imgur.com/1YCyEvb.png)

## 學習使用LCD貼出字串
* LCD I2C Libraries下載：[here](https://drive.google.com/file/d/11hV6xfWycVe5Hg2bahq_UgWgoPMLBCK9/view?usp=sharing)
* Keypad Libraries下載：[here](https://drive.google.com/file/d/1mnzf--VuktjYytxyOUm0J5sxz0J_YV6g/view?usp=sharing)
--> 解壓縮後將資料夾放進Arduino libraries當中
* code：

![](https://i.imgur.com/VnLFbL9.png)

## LAB2
* 實驗目標：將上個code中的字串進行繞圈的方式顯示在LCD上。
* 參考影片：
 <iframe width="560" height="315" src="https://www.youtube.com/embed/ZUuxAuS7Lds" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

* code：
![](https://i.imgur.com/XWV35hC.png)
## LAB3
### 簡易計算機
* 實驗目標：利用薄膜鍵盤與LCD實現簡易計算機
* 實驗要求:
   *  A 鍵為加號、B 鍵為減號、C 為乘號、D 為除號、# 為等於、* 為清除鍵。
   *  由薄膜鍵盤輸入數字後，LCD 第一列顯示運算式，第二列顯示等號與運算結果。
* 示範影片:
<iframe width="560" height="315" src="https://www.youtube.com/embed/0YtXmMf3lpk" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

## LAB4
### 可累加簡易計算機
* 實驗目標：改善計算機程式碼中的BUG，以及額外增加新功能
* 實驗要求:
  *    由薄膜鍵盤輸入數字後，LCD 第一列顯示加數被加數與加號，第二列顯示等號與結果，**並且可以儲存上一次的結果再進行下一次的運算**。
  *   需**處理字串溢位問題**。當相加總和超過32767時需**顯示ERROR**! 
* 示範影片:
<iframe width="560" height="315" src="https://www.youtube.com/embed/cz9xQI6KBqE" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>






### 加分LAB 
-   實驗目的：I2C用 master端與slave端溝通方式，透過按鈕控制LED輸出。
    
-   實驗目標：在master端設定讀取按鈕狀態並將狀態傳至slave端對LED進行輸出。
    
-   Schematic：請參照I2C與GPIO講義自行接腳。

* 參考：
<iframe width="560" height="315" src="https://www.youtube.com/embed/VcGYAjxjA9Y" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

* master
```c++=
int x=0;
int slaveAddress = 9;
const int buttomPin = 4;
int buttomState = 0;

void setup(){
    pinMode(buttomPin,INPUT);
    Wire.begin();
    Serial.begin(9600);
} 
void loop(){
    buttomState =digitalRead(buttomPin);
    Serial.println(buttomState);
    if(buttomState ==HIGH){
        x=1;
    }
    else{
        x=0;
        }
    Wire.beginTransmission(slaveAddress);
    Wire.write(x);
    Wire.endTransmission();
    delay(200);
}

```

* Slave
```c++=
#include <Wire.h>

int LED = 12;
int x =0;

void setup(){
    Serial.begin(9600);
    pinMode(LED,OUTPUT);
    Wire.begin(9);
    Wire.onReceive(receiveEvent);

}

void receiveEvent(int bytes){
x = Wire.read();

}

void loop(){
    if (x == 1){
        digitalWrite(LED,HIGH);
        delay(200);
    }
    if (x == 0){
        digitalWrite(LED,LOW);
        delay(200);
    }
}
```
## 課後問題
:::info
Q1.簡述 I2C 如何藉由 SCL 與 SDA 傳送資料 ?
Q2. I2C ACK/NACK bit 與 R/W bit 功能為何?
Q3.電算機 LAB4 為何總和超過 32767 時字串會溢位?如何解決 ?

:::