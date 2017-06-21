# mxUnified74HC595
mxUnifiedIO child library for Arduino. Use device specific drivers with 74HC595 shift-registers using a unified API.

This library (Arduino IDE version 1.6 and higher) is a child library of the mxUnifiedIO library. It provides a unified
API to drive 74HC595 shift-registers via the SPI pins or using software SPI. 

By using this library, setting a pin of the shift-register is as easy as calling Arduino's digitalWrite().
The library implements shiftOut() to allow device specific drivers using the expanded pins of the shift-register
to be used as easy as those on the MCU.

The current version of this library supports ESP8266 and Atmel ATmega328 and ATmega168 MCU's.
