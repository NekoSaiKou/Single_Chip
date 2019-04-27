Arduino Library in this Project
==
## Source

[i2cdevlib project](https://www.i2cdevlib.com/)  
[github](https://github.com/jrowberg/i2cdevlib)  
MPU-9150 DMP9 example is copied from the [Pull Request](https://github.com/jrowberg/i2cdevlib/pull/182)   in i2cdev github.


## Copyright  
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

## Modify

### MPU9150_9Axis_MotionApps41

    typedef unsigned char prog_uchar;
I didn't use prog_uchar in header file. I use 'unsigned char' type directly.  
  

 
    #ifndef MPU9150_DMP_FIFO_RATE_DIVISOR 
    #define MPU9150_DMP_FIFO_RATE_DIVISOR 0x05
    #endif

The last value of dmpConfig is changed into the divisor above.

### MPU9150_DMP9

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    //flush fifo everytime we read
    mpu.resetFIFO();

Instead of FIFO overflow detection, I flush the FIFO everytime after reading.
