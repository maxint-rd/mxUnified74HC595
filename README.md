# mxUnified74HC595
mxUnifiedIO child library for Arduino. Use device specific drivers with 74HC595 shift-registers using a unified API.

This library (Arduino IDE version 1.6 and higher) is a child library of the mxUnifiedIO library. It provides a unified
API to drive 74HC595 shift-registers via the SPI pins or using software SPI. 

By using this library, setting a pin of the shift-register is as easy as calling Arduino's digitalWrite().
The library implements shiftOut() to allow device specific drivers using the expanded pins of the shift-register
to be used as easy as those on the MCU.

# Features & limitations
- The current version of this library supports ESP8266 and Atmel ATmega328 and ATmega168 MCUs. Other Atmel processors may work as well, but they've not been tested yet. For some MCUs the library will require modification. Please let me know if you've successfully used this library with other MCUs.
- As the 74HC595 shifts data to its output pins, only OUTPUT mode is supported. digitalWrite() and shiftOut() are used to set the output pins. digitalRead() can be used to query the status of an output pin.
- Using digitalWrite() to change one expanded pin requires sending a whole byte to the shift-register (or more when cascaded). Therefor the maximum speed that can be achieved is much lower than using direct MCU pins.
- Best speeds can be obtained by connecting the shift register to the hardware SPI pins and by using a fast MCU. The ESP8266 has a higher clock-speed than an ATmega328. To optimize speed on the Atmel MCU, port-manipulation techniques are applied when using software SPI to drive the shift register. This may result in less compatibility with untested MCUs.
- shiftOUt() can be used as software SPI on expanded pins, where you can choose which pin to use for MOSI and SCK. Please note that shifting one byte out requires 16 changes of the clockpin, so this is much slower than using direct MCU pins. However, the Nokia LCD examples shows that the results can still be quite usable. See https://github.com/maxint-rd/mxUnifiedPCD8544_Nokia_5110_LCD for more info and the specific examples.
- Currently a maximum of four cascaded shift registers is supported. Please note each added shift register will require more bits to be shifted out, resulting in slower handling of digitalWrite() and shiftOut() calls.

# Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.
