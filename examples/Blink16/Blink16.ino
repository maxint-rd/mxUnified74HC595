//#define _MXUNIFIEDOUTPUT_DEBUG 0
#include <mxUnified74HC595.h>

// create the 74HC595 output object
//mxUnified74HC595 unio = mxUnified74HC595();                  // use hardware SPI pins, no cascading
//mxUnified74HC595 unio = mxUnified74HC595(2);               // use hardware SPI pins, two cascaded shift-registers (slightly slower, but more pins)
//mxUnified74HC595 unio = mxUnified74HC595(10, 11, 13);      // alternative software SPI pins: SS, MOSI, SCLK, no cascading (slow, but pin-freedom)
//mxUnified74HC595 unio = mxUnified74HC595(10, 11, 13, 4);   // alternative software SPI pins: SS, MOSI, SCLK, three cascaded shift-registers (slow, but pin-freedom)
mxUnified74HC595 unio = mxUnified74HC595(3, 2, 0, 2);      // alternative software SPI pins for ESP-01: SS, MOSI, SCLK, two cascaded shift-registers (slow, but pin-freedom)

// Hardware SPI pins:
//   ATmega328: SS=10, MOSI=11, SCLK=12
//   ESP8266:   SS=15, MOSI=13, SCLK=14

// Software SPI pins:
//   Suggested pins for ESP-01: SS=3 (RX), MOSI=2, SCLK=1

// clock-timings on 3.3V 8MHz 328 using software SPI shiftOut: 7+21 us per SCLK tick, latch-pulse digitalWrite: 10us
// clock-timings on 3.3V 8MHz 328 using software SPI port manipulation: 3.5+2 us per SCLK tick, latch-pulse, latch-pulse digitalWrite: 10us
// clock-timings on 3.3V 8MHz 328 using hardware SPI: 0.25+0.25 us per SCLK tick, latch-pulse port manipulation: 4us


void setup()
{
  Serial.begin(115200);
  Serial.print(F("mxUnified74HC595 Blink16: "));
  Serial.print(unio.getNumPins());
  Serial.println(F(" pins"));

  unio.begin();     // start using the mxUnified74HC595 I2C shift register
  unio.setBitOrder(LSBFIRST);        // change bitOrder to match pin layout of 595 shiftregister with display (LSB next to GND is Qh of 595)

  for(int nPin=0; nPin<unio.getNumPins(); nPin++)
    unio.digitalWrite(nPin, LOW);   // set expanded pin low
}

void loop()
{
  for(int nPin=0; nPin<unio.getNumPins(); nPin++)
  {
    Serial.print(nPin);
    Serial.print(F(" "));
    unio.digitalWrite(nPin, HIGH);   // set expanded pin high
    delay(10);
    unio.digitalWrite(nPin, LOW);   // set expanded pin low
    delay(40);
  }
  Serial.println(F("."));
  delay(50);
}