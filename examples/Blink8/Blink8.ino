//#define _MXUNIFIEDOUTPUT_DEBUG 0
#include <mxUnified74HC595.h>

// create the 74HC595 output object
//mxUnified74HC595 unio = mxUnified74HC595();                  // use hardware SPI pins, no cascading
//mxUnified74HC595 unio = mxUnified74HC595(2);               // use hardware SPI pins, two cascaded shift-registers (slightly slower, but more pins)
//mxUnified74HC595 unio = mxUnified74HC595(10, 11, 13);      // alternative software SPI pins: SS, MOSI, SCLK, no cascading (slow, but pin-freedom)
mxUnified74HC595 unio = mxUnified74HC595(3, 2, 0);      // alternative software SPI pins for ESP-01: SS, MOSI, SCLK, no cascading (slow, but pin-freedom)
//mxUnified74HC595 unio = mxUnified74HC595(10, 11, 13, 3);   // alternative software SPI pins: SS, MOSI, SCLK, three cascaded shift-registers (slow, but pin-freedom)
#define NUMPINS 8
// Hardware SPI pins:
//   ATmega328: SS=10, MOSI=11, SCLK=12
//   ESP8266:   SS=15, MOSI=13, SCLK=14
//   Suggested pins ESP-01:   SS=3 (RX), MOSI=2, SCLK=0


void setup()
{
  Serial.begin(115200);
  Serial.println(F("mxUnified74HC595 Blink8"));

  unio.begin();     // start using the mxUnified74HC595 I2C shift register
  for(int nPin=0; nPin<NUMPINS; nPin++)
    unio.digitalWrite(nPin, LOW);   // set expanded pin low
}

void loop()
{
  for(int nPin=0; nPin<NUMPINS; nPin++)
  {
    Serial.print(nPin);
    Serial.print(F(" "));
    unio.digitalWrite(nPin, HIGH);   // set expanded pin high
    delay(50);
    unio.digitalWrite(nPin, LOW);   // set expanded pin low
    delay(100);
  }
  Serial.println(F("."));
  delay(200);
}

