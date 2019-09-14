# 2019單晶片課程：RTOS
## RTOS - 簡介
* 即時作業系統（Real-time operating system，RTOS），是指作業系統要在一個固定時間內，做出正確反應。
* RTOS相較一般的作業系統，最大的特色就在於「即時性」
* 即時作業系統可分類為「軟式（Soft）」與「硬式（Hard），此間差別主要在於完成 process 之間的時間差是否符合設定。

|          | **硬性即時作業系統**  | **軟性即時作業系統** |
| -------- | -------- | -------- |
| **簡介**  | 也可稱為安全臨界系統（Safety-Critical System），有最迫切的需求，必須在特定期限之前回應事件，保證在它們的期限內完成臨界即時任務。    | 跟硬性即時系統比較起來，限制的範圍比較少，簡單提供一件臨界的即時系統，將在其它的任務上接受優先權並保有優先權直到它完成。     |
|**工作延遲**| 所有工作都不能夠延遲 | 允許少量的工作延遲，若沒有在回應時間內完成，只是造成系統的效能變差，不會影響系統的執行 |
|**要求** | 非常嚴苛| 比較寬鬆|



## Mbed-OS RTOS
Mbed OS RTOS 的物件導向功能包括執行緒管理( thread )、同步處理( synchronization objects )和定時器 ( timer )。它還提供用於附加於特定應用程序 idle hook function 的介面，用來讀取操作系統計數並提供 RTOS 報錯功能。

:::info
Hook 是用來與作業系統掛勾進而攔截並處理某些訊息之用。idle hook function 即是由介面觸發後，使程式進入 Idle 狀態的函式。
:::
:::danger
以下範例函式多為介紹使用，正確語法以物件導向的寫法為主。
:::

---
### Thread
**執行緒**是作業系統能夠進行運算排程的最小單位，被包含在行程之中，式行程中的實際運作單位。在 Mbed OS RTOS 中以 thread 的函式庫來設定 thread的運作，而在 main 函式中，因為其為初始化程式本體的函式，所以他的初始排序優先權(os Priority)為`osPriorityNormal`。

以下為簡單的應用 : 
``` C++=
#include "mbed.h"
#include "rtos.h"
 
DigitalOut led1(LED1);
DigitalOut led2(LED2);
Thread thread;
 
void led2_thread() {
    while (true) {
        led2 = !led2;
        Thread::wait(1000);
    }
}
 
int main() {
    thread.start(led2_thread);
    
    while (true) {
        led1 = !led1;
        Thread::wait(500);
    }
}
```
:::info
The main function is already the first thread scheduled by the rtos
:::

 執行緒之間會在以下階段間轉換 : 
* **RUNNING** : 執行緒在執行的階段，若為單執行緒晶片，則只會有一執行緒在執行。
* **READY** : 執行緒準備好執行的階段，一旦執行的執行緒(RUNNING thread)結束或等待下一個準備好的執行緒(READY thread)完成(補進的執行緒必須有最高的執行優先權)，此執行緒才會進入執行的階段(RUNNING state)。
* **WAITING** : 此類執行緒會等待事件發生(event occur)才會觸發，因此歸類為等待階段 (WAITTING state)。
* **INACTIVE** : 以在程式碼內但未創造或者已經結束的執行緒會被歸類為此一階段，此階段基本上不會耗損任何系統資源。

![](https://i.imgur.com/mqGLfg5.png)

在執行緒中執行優先權可以有以下幾種可以設定 : 
``` javascript= 
typedef enum {
  osPriorityIdle          = -3,         ///< Priority: idle (lowest)
  osPriorityLow           = -2,         ///< Priority: low
  osPriorityBelowNormal   = -1,         ///< Priority: below normal
  osPriorityNormal        =  0,         ///< Priority: normal (default)
  osPriorityAboveNormal   = +1,         ///< Priority: above normal
  osPriorityHigh          = +2,         ///< Priority: high
  osPriorityRealtime      = +3,         ///< Priority: realtime (highest)
  osPriorityError         = 0x84,       ///< System cannot determine priority or illegal priority.
  osPriorityReserved      = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
} osPriority;
```
並以以下函式設定優先順序 : 
```
set_priority(osPriority priority)
```
當然也可以在宣告時便將所有執行緒要設定的參數設定好 :
```
Thread (osPriority priority=osPriorityNormal, uint32_t stack_size=DEFAULT_STACK_SIZE, unsigned char *stack_pointer=NULL)
//Allocate a new thread without starting execution. 
```
宣告範例
```
Thread t1(osPriorityHigh);
```
相關語法可以參考 : [Thread](https://os.mbed.com/docs/mbed-os/v5.11/apis/thread.html)。

---
### Mutex
`Mutex` 是一種可以將執行緒進行同步處理的方式，使用 lock 和 unlock 的方式來共享資源使用。
:::warning
`Mutex`的方法無法被 ISR (interrupt service routines)所使用。
:::

以下為 `Mutex` 的示意圖 : 
![](https://i.imgur.com/cKvXmC6.png)

以下為 `Mutex` 的範例程式 : 
```javascript=
#include "mbed.h"
#include "rtos.h"
 
Mutex stdio_mutex;
 
void notify(const char* name, int state) {
    stdio_mutex.lock();
    printf("%s: %d\n\r", name, state);
    stdio_mutex.unlock();
}
 
void test_thread(void const *args) {
    while (true) {
        notify((const char*)args, 0); Thread::wait(1000);
        notify((const char*)args, 1); Thread::wait(1000);
    }
}
 
int main() {
    Thread t2;
    Thread t3;
    t2.start(callback(test_thread, (void *)"Th 2"));
    t3.start(callback(test_thread, (void *)"Th 3"));
 
    test_thread((void *)"Th 1");
}
```
利用 lock() 來使 stdio 正常運作，並用 unlock()釋放資源。
:::info
在 ARM C 標準函式庫內已經存在 Mutex 。主要是可以讓 stdio 進行正常運作，因此以上的範例在實作時並不是必須，除非有些沒有以上保護機制的開發板才需要以上程式碼，如 ARM M0 系列。
:::

:::warning
正是因為 ARM C 標準函式庫在使用 stdio 時會使用到 Mutex，所以在 ISR 中變無法使用 printf, putc, getc, malloc 和 new 。
:::

::: info
Thread  class 中的 wait() function 中間是以毫秒為單位，使用 Thread::wait(1000) 是該 funciotn 下 全部 thread 暫停一秒， t1.wait(500) 是 t1 該物件暫停 0.5 秒。
:::
相關語法請參考 : [Mutex](https://os.mbed.com/docs/mbed-os/v5.11/apis/mutex.html)。

---
### Semaphore
`Semaphore` 可以用來管理多種特定線程使用共享資源 (a pool of shared resources) 的情況。

以下為 `Semaphore` 的示意圖 :
![](https://i.imgur.com/xeydSYc.png)

以下為 `Semaphore` 的範例程式 :
```javascript=
#include "mbed.h"
#include "rtos.h"
 
Semaphore two_slots(2); //宣告兩執行緒的Semaphore
 
void test_thread(void const *name) {
    while (true) {
        two_slots.wait();
        printf("%s\n\r", (const char*)name);
        Thread::wait(1000);
        two_slots.release();
    }
}
 
int main (void) {
    Thread t2;
    Thread t3;
 
    t2.start(callback(test_thread, (void *)"Th 2"));
    t3.start(callback(test_thread, (void *)"Th 3"));
 
    test_thread((void *)"Th 1");
}
```
可管理同時要釋放多少資源來處理執行緒。

相關語法請參考 : [Semaphore](https://os.mbed.com/docs/mbed-os/v5.11/apis/semaphore.html)。

---
### Signals
任何一個執行緒可以被標註為等待 signal 的執行緒。

以下為 `Signal` 範例程式 : 
```javascript=
#include "mbed.h"
#include "rtos.h"
 
DigitalOut led(LED1);
Thread thread;
void led_thread() {
    while (true) {
        // Signal flags that are reported as event are automatically cleared.
        thread.signal_wait(0x1);
        led = !led;
    }
}
 
int main (void) {
 
    thread.start(callback(led_thread));
 
    while (true) {
        Thread::wait(1000);
        thread.signal_set(0x1);
    }
}
```
---
### Queue
在資料結構中我們學過 `Queue`，即是 FIFO (Frist In Frist Out) 的結構，在實作執行緒排程時就會使用到此種結構方式。

以下為 `Queue` 的示意圖 : 
![](https://i.imgur.com/gr3DlME.png)

常用指令 : 
```javascript=
Queue () //Create and initialise a message Queue . 
Queue< T, queue_sz  >
osStatus put (T *data, uint32_t millisec=0) //Put a message in a Queue ; T : templete
osEvent get (uint32_t millisec=osWaitForever)//Get a message or Wait for a message from a Queue . 
```
---
### MemoryPool
`MemoryPool` 是用來定意並管理記憶體空間的程式。混合 `Queue` 的函式我們可以得到以下範例程式 : 
```javascript=
#include "mbed.h"
#include "rtos.h"
 
typedef struct {
    float    voltage;   /* AD result of measured voltage */
    float    current;   /* AD result of measured current */
    uint32_t counter;   /* A counter value               */
} message_t;
 
MemoryPool<message_t, 16> mpool;
Queue<message_t, 16> queue;
 
/* Send Thread */
void send_thread (void) {
    uint32_t i = 0;
    while (true) {
        i++; // fake data update
        message_t *message = mpool.alloc();
        message->voltage = (i * 0.1) * 33; 
        message->current = (i * 0.1) * 11;
        message->counter = i;
        queue.put(message);
        Thread::wait(1000);
    }
}
 
int main (void) {
    Thread thread;
    thread.start(callback(send_thread));
    
    while (true) {
        osEvent evt = queue.get();
        if (evt.status == osEventMessage) {
            message_t *message = (message_t*)evt.value.p; // value.p value 的 pointer
            printf("\nVoltage: %.2f V\n\r"   , message->voltage);
            printf("Current: %.2f A\n\r"     , message->current);
            printf("Number of cycles: %u\n\r", message->counter);
            
            mpool.free(message);
        }
    }
}
```

常用指令 : 
```javascript=
MemoryPool< T, pool_sz > //Create and Initialize a memory pool. 
MemoryPool ()
T* alloc (void) //Allocate a memory block of type T from a memory pool. 
T* calloc (void) //Allocate a memory block of type T from a memory pool and set memory block to zero.
osStatus free (T *block) //Return an allocated memory block back to a specific memory pool. 

```

---
### Mail
`Mail`的功能即是將上述的 `Queue` 和 `Memory pool` 功能整合起來。

以下為 `Mail` 的示意圖 : 
![](https://i.imgur.com/EURq6iV.png)

以下為 `Mail` 的範例程式 : 
```javascript=
#include "mbed.h"
#include "rtos.h"
 
/* Mail */
typedef struct {
  float    voltage; /* AD result of measured voltage */
  float    current; /* AD result of measured current */
  uint32_t counter; /* A counter value               */
} mail_t;
 
Mail<mail_t, 16> mail_box;
 
void send_thread (void) {
    uint32_t i = 0;
    while (true) {
        i++; // fake data update
        mail_t *mail = mail_box.alloc();
        mail->voltage = (i * 0.1) * 33; 
        mail->current = (i * 0.1) * 11;
        mail->counter = i;
        mail_box.put(mail);
        Thread::wait(1000);
    }
}
 
int main (void) {
    Thread thread;
    thread.start(callback(send_thread));
    
    while (true) {
        osEvent evt = mail_box.get();
        if (evt.status == osEventMail) {
            mail_t *mail = (mail_t*)evt.value.p; // value.p value 的 pointer
            printf("\nVoltage: %.2f V\n\r"   , mail->voltage);
            printf("Current: %.2f A\n\r"     , mail->current);
            printf("Number of cycles: %u\n\r", mail->counter);
            
            mail_box.free(mail);
        }
    }
}
```

常用指令 : 
```javascript=
 Mail< T, queue_sz > 	//Create and Initialise Mail queue.
 Mail ()
 T *alloc (uint32_t millisec=0) //Allocate a memory block of type T. 
 T *calloc (uint32_t millisec=0) //Allocate a memory block of type T and set memory block to zero.
osStatus put (T *mptr) //Put a mail in the queue. 
osEvent get (uint32_t millisec=osWaitForever) //Get a mail from a queue. 
osStatus free (T *mptr) //Free a memory block from a mai
```

---
### RTOS Timer
`RTOS Timer` 即是將系統內的計時器運用在 RTOS 上，一個 timer 的函式可以同步執行一至多個計時器，且 timer 具有正常計時器的功能，如 started 、 restrarted 和 stopped。而這些 timer 皆屬於 `osTimerThread`，也就是說 timer 的 callback function 即是屬於此執行緒下。

![](https://i.imgur.com/UYIAlld.png)

```javascript=
#include "mbed.h"
#include "rtos.h"
 
DigitalOut LEDs[3] = {
    DigitalOut(PB_0_ALT0), DigitalOut(PB_7), DigitalOut(PB_14)
};
 
void blink(void const *n) {
    LEDs[(int)n] = !LEDs[(int)n];
}
 
int main(void) {
    RtosTimer led_1_timer(blink, osTimerPeriodic, (void *)0); 
    //RtosTimer function_name(callback_function ,os_timer_type ,  RtosTimer(void(*func)(void const *argument) )
    RtosTimer led_2_timer(blink, osTimerPeriodic, (void *)1);
    RtosTimer led_3_timer(blink, osTimerPeriodic, (void *)2);
    
    led_1_timer.start(2000);
    led_2_timer.start(1000);
    led_3_timer.start(500);
    
    Thread::wait(osWaitForever);
}
```
在 `RTOSTimer` 中 `os_timer_type` 的參數設定值 : 
```javascript=
typedef enum {
  osTimerOnce             = 0,          ///< One-shot timer.
  osTimerPeriodic         = 1           ///< Repeating timer.
} os_timer_type;
```
---
### Status and Error Codes
The Status and Error Codes section lists all the return values that the `CMSIS-RTOS functions` will return:

:::info
CMSIS-RTOS 是 ARM 公司為統一操作系統、降低嵌入式門檻而發佈的操作系統標準軟件接口。通俗講，CMSIS-RTOS 將操作系統（不管是 FREE-RTOS 還是 RTX 等）屏蔽起來，然後提供 CMSIS-RTOS 接口函數給最終用户調用。 
:::

+ `osOK` : function completed ; no event occurred.
+ `osEventSignal` : function completed; signal event occurred.
+ `osEventMessage` : function completed; message event occurred.
+ `osEventMail` : function completed; mail event occurred.
+ `osEventTimeout` : function completed; timeout occurred.
+ `osErrorParameter` : parameter error: a mandatory parameter was missing or specified an incorrect object.
+ `osErrorResource` : resource not available: a specified resource was not available.
+ `osErrorTimeoutResource` : resource not available within given time : a specified resource was not available within the timeout period.
+ `osErrorISR` : not allowed in ISR context: the function cannot be called from interrupt service routines.
+ `osErrorISRRecursive` : function called multiple times from ISR with same object.
+ `osErrorPriority` : system cannot determine priority or thread has illegal priority.
+ `osErrorNoMemory` : system is out of memory: it was impossible to allocate or reserve memory for the operation.
+ `osErrorValue` : value of a parameter is out of range.
+ `osErrorOS` : unspecified RTOS error: run-time error but no other error message fits.

---
## RTOS - 上課練習
### Lab 1 
請利用上述 RTOS 中的 thread 和 Semaphore 來完成以下功能：
1. 每隔一秒亮一顆開發板上之LED，總共三顆輪流。
2. 當開發板上 Button 按下後，改變為 Semaphore的狀態，Semaphore 觸發內容為每次同時亮兩顆 LED 。
3. 再按一次開發板上的 Button 後，回歸每秒自動亮一顆LED。
:::info
**註 : button 的功能請用 thread 來完成，請上網自行找尋設定 button 的函數和使用方法。**
:::
{%youtube VaqEYOOMyQk %}

### Lab 2
請利用 Mail 的函式完成一組時間，時間24時制，呈現必須包含 `年-月-日  時 : 分 : 秒`，請自行撰寫時鐘的 struct 並使用 printf 呈現至電腦終端機於以檢查，過程中請注意大小月。

如下圖所示 :
![](https://i.imgur.com/c94zb6O.png)


### Lab 3
請利用上述 RTOS 之 RTOS Timer 與 Mail 來完成以下功能 : 
1. 使用開發板上的 Button 當作計時按鈕觸發，按一下後進入計時介面。
2. 進入介面後按一次按鈕即開始計時，在按一下即結束計時，結束後請呈現計時時間，時間以終端機呈現且具有小數點後兩碼。
3. 結束計時候在按一下按鈕即回歸等待計時的狀態。

:::info
1. 使用 Mail 的方式避免，快數計時記憶體混亂。
2. 降量減少在 RTOS Timer 中 callback function 裡的運算量，運算時間過長會導致混亂。
:::






---
## RTOS - 課後問題
1. 請回答以下程式碼是否可以運作，如果可以請解釋結果，如果無法運作請解釋原因。
```javascript=
#include "mbed.h"
#include "rtos.h"
 
Mutex stdio_mutex;
Thread t3(osPriorityAboveNormal);
 
void notify(const char* name, int state) {
    stdio_mutex.lock();
    printf("%s: %d\n\r", name, state);
    printf("\n");
    stdio_mutex.unlock();
}
 
void test_thread(void const *args) {
    while (true) {
        notify((const char*)args, 0); Thread::wait(1000);
        notify((const char*)args, 1); Thread::wait(1000);
    }
}

void test_thread2(void const *args) {
    while (true) {
       printf("%s: %d\n\r",(const char*)args, 0); Thread::wait(500);
       printf("%s: %d\n\r",(const char*)args, 1); Thread::wait(1500);
    }
}

int main() {
    Thread t2;
    t2.start(callback(test_thread, (void *)"Th 2"));
    t3.start(callback(test_thread2, (void *)"Th 3"));
    test_thread((void *)"Th 1");
}
```
2.請解釋 `mail_t *mail = (mail_t*)evt.value.p` 中`evt.value.p`的意思，與取用其內 data 為何要使用 `->` 此符號。

3.請敘述 `MCU` 專用的`RTOS` 在晶片系統中的應用，請舉 1~2 例說明，並強調其優缺點。

---
## RTOS - 相關知識
+ [Event](https://os.mbed.com/docs/mbed-os/v5.11/apis/event.html)
+ [EventFlags](https://os.mbed.com/docs/mbed-os/v5.11/apis/eventflags.html)
+ [ConditionVariable](https://os.mbed.com/docs/mbed-os/v5.11/apis/conditionvariable.html)
+ [Kernel interface functions](https://os.mbed.com/docs/mbed-os/v5.11/apis/kernel-interface-functions.html)