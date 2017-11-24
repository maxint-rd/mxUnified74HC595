# mxUnified74HC595
mxUnifiedIO child library for Arduino. Use device specific drivers with 74HC595 shift registers using a unified API.

This library (Arduino IDE version 1.6 and higher) is a child library of the mxUnifiedIO library. It provides a unified
API to drive 74HC595 shift registers via the SPI pins or using software SPI. 

By using this library, setting a pin of the shift register is as easy as calling Arduino's digitalWrite().
The library implements shiftOut() to allow device specific drivers using the expanded pins of the shift register
to be used as easy as those on the MCU.

# Installation
- This library requires the mxUnifiedIO base library: https://github.com/maxint-rd/mxUnifiedIO
- To support the ATtiny85, the tinySPI library is required: http://github.com/JChristensen/tinySPI

You can download the .ZIP library files from the locations mentioned above. Use the Arduino IDE to add the .ZIP library file via the menu. Alternatively copy all library files into a folder under your /libraries/ folder and restart the IDE.

The ATtiny85 can be programmed using a programmer such as the USBasp. This library was tested with an ATtiny85 running at 8MHz, using the attiny core by David A. Mellis. It was installed using the Arduino IDE boardmanager. For more information see also [his github page](https://github.com/damellis/attiny), this [tutorial by SparkFun](https://learn.sparkfun.com/tutorials/tiny-avr-programmer-hookup-guide/attiny85-use-hints) and this [YouTube video by Make:](https://www.youtube.com/watch?v=30rPt802n1k).

# Pinouts & connections
The 74HC595 shift register requires three pins plus VCC/GND.
The shift register can be connected to the hardware SPI pin for best speed.
- Default pins for ESP8266:   SS=15, MOSI=13, SCLK=14
- Default pins for ATmega328: SS=10, MOSI=11, SCLK=13
- Default pins for ATtiny85:  SS=0, MOSI=1, SCLK=2

Notes:
- With hardware SPI the MISO pin isn't used but will still be read and written to during SPI transfer. Be careful when sharing this pin!
- Software SPI pins are slower, but offer pin-freedom.
- Suggested pins for ESP-01: SS=3 (RX), MOSI=2, SCLK=1

This fine piece of ASCII-art shows the pinout of the 74HC595. Of course you can also review the [TI datasheet](http://www.ti.com/lit/ds/symlink/sn74hc595.pdf) (PDF).
```
Pinout 74HC595 shift register DIP and SMD model:

     +--v--+          PINS 1-8       PINS 9-16
  1 -+     +- 16      1: output Qb   16: VCC
  2 -+     +- 15      2: output Qc   15: output Qa   - expanded pin P0
  3 -+     +- 14      3: output Qd   14: Serial SER  - connect to SPI-MOSI
  4 -+     +- 13      4: output Qe   13: Enable OE   - active low, so connect to GND
  5 -+     +- 12      5: output Qf   12: Latch RCLK  - connect to SPI-SS
  6 -+     +- 11      6: output Qg   11: Clock SRCLK - connect to SPI-SCK
  7 -+     +- 10      7: output Qh   10: Clear SRCLR - active low, so connect to VCC
  8 -+     +- 9       8: GND          9: Cascade SER Qh' - to SER of next chip
     +-----+
```

The used pins are specified in the constructor. Hardware SPI is fastest. The number of shift registers used also effects the speed.
Two cascaded registers is slightly slower than one, but you have more pins available.
These are some constructor examples:
```C++
mxUnified74HC595 unio = mxUnified74HC595();                  // default hardware SPI pins, no cascading
//mxUnified74HC595 unio = mxUnified74HC595(2);               // default hardware SPI pins, two cascaded shift-registers
//mxUnified74HC595 unio = mxUnified74HC595(10, 11, 13);      // alternative software SPI pins: SS, MOSI, SCLK
//mxUnified74HC595 unio = mxUnified74HC595(10, 11, 13, 2);   // alternative software and two cascaded shift-registers
//mxUnified74HC595 unio = mxUnified74HC595(3, 2, 0);         // alternative software SPI pins for ESP-01:
//mxUnified74HC595 unio = mxUnified74HC595(3, 2, 0, 3);      // alternative software SPI pins for ESP-01, three cascaded
```

# Features & limitations
- The current version of this library supports ESP8266 and Atmel ATmega328 and ATmega168 MCUs. Support for ATtiny85 was just added. Other Atmel processors may work as well, but they've not been tested yet. For some MCUs the library will require modification. Please let me know if you've successfully used this library with other MCUs.
- Using hardware SPI to drive the shift register(s) is fastest, but not available on each MCU module. On ATtiny85 hardware SPI is supported u sing the tinySPI library (see http://github.com/JChristensen/tinySPI), which only supports MSBFIRST. Therefor on ATtiny using hardware SPI, setBitOrder() is not supported.
- For ATtiny and ESP-01 software SPI is available. Update171123: in the current version setBitOrder() is implemented for software SPI on both ESP8266 and on Atmel. Note that the shiftOut() method (for shifting data out to one expanded pin) still only supports the default MSBFIRST option, not LSBFIRST.
- As the 74HC595 shifts data to its output pins, only OUTPUT mode is supported. digitalWrite() and shiftOut() are used to switch the output pins high/low. digitalRead() can be used to query the status of an output pin.
- Using digitalWrite() to change one expanded pin requires sending a whole byte to the shift register (or more when cascaded). Therefor the maximum speed that can be achieved is much lower than using direct MCU pins.
- Best speeds can be obtained by connecting the shift register to the hardware SPI pins and by using a fast MCU. The ESP8266 has a higher clock-speed than an ATmega328. To optimize speed on the Atmel MCU, port-manipulation techniques are applied when using software SPI to drive the shift register. This may result in less compatibility with untested MCUs.
- shiftOut() can be used as software SPI on expanded pins, where you can choose which pin to use for MOSI and SCK. Please note that shifting one byte out requires 16 changes of the clockpin, so this is much slower than using direct MCU pins. However, the Nokia LCD examples shows that the results can still be quite usable. See https://github.com/maxint-rd/mxUnifiedPCD8544_Nokia_5110_LCD for more info and the specific examples.
- Currently a maximum of four cascaded shift registers is supported. Please note each added shift register will require more bits to be shifted out, resulting in slower handling of digitalWrite() and shiftOut() calls.

# Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.
