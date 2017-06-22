# mxUnified74HC595
mxUnifiedIO child library for Arduino. Use device specific drivers with 74HC595 shift-registers using a unified API.

This library (Arduino IDE version 1.6 and higher) is a child library of the mxUnifiedIO library. It provides a unified
API to drive 74HC595 shift-registers via the SPI pins or using software SPI. 

By using this library, setting a pin of the shift-register is as easy as calling Arduino's digitalWrite().
The library implements shiftOut() to allow device specific drivers using the expanded pins of the shift-register
to be used as easy as those on the MCU.

# Features & limitations
- The current version of this library supports ESP8266 and Atmel ATmega328 and ATmega168 MCUs. Other Atmel processors may work as well, but they've not been tested yet. For some MCUs the library will require modification. Please let me know if you've succesfully used this library with other MCUs. 
- Currently a maximum of four cascaded shift registers is supported. Please note each added shift register will require more bits to be shifted out, resulting in slower handling of digitalWrite() and shiftOut() calls

# Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.
