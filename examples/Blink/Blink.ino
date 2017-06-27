#include <mxUnified74HC595.h>

#if defined(__AVR_ATtiny85__)
  // ATtiny85 has no regular hardware serial. You can use TinyDebugSerial (TX=3) or software serial
  // see also http://www.ernstc.dk/arduino/tinycom.html
  #include <SoftwareSerial.h>
  SoftwareSerial mySerial(4, 3); // RX, TX
  //TinyDebugSerial mySerial= TinyDebugSerial(); 
  #define Serial mySerial
  #define LED_BUILTIN 1
#endif



// create the 74HC595 output object
mxUnified74HC595 unio = mxUnified74HC595();                  // use hardware SPI pins, no cascading
//mxUnified74HC595 unio = mxUnified74HC595(2);               // use hardware SPI pins, two cascaded shift-registers (slightly slower, but more pins)
//mxUnified74HC595 unio = mxUnified74HC595(10, 11, 13);      // alternative software SPI pins: SS, MOSI, SCLK, no cascading (slow, but pin-freedom)
//mxUnified74HC595 unio = mxUnified74HC595(10, 11, 13, 3);   // alternative software SPI pins: SS, MOSI, SCLK, three cascaded shift-registers (slow, but pin-freedom)
//mxUnified74HC595 unio = mxUnified74HC595(0, 2, 3);   // alternative software SPI pins for ESP-01: SS=3(RX), MOSI=2, SCLK=0, three cascaded shift-registers (slow, but pin-freedom)
//mxUnified74HC595 unio = mxUnified74HC595(0, 1, 2);   // alternative software SPI pins for ATtiny85: SS=3(RX), MOSI=2, SCLK=0, three cascaded shift-registers (slow, but pin-freedom)

// Hardware SPI pins:
//   ATmega328: SS=10, MOSI=11, SCLK=12
//   ESP8266:   SS=15, MOSI=13, SCLK=14
//   ATtiny85:  SS=0, MOSI=1, SCLK=2

// clock-timings on 3.3V 8MHz 328 using software SPI shiftOut: 7+21 us per SCLK tick, latch-pulse digitalWrite: 10us
// clock-timings on 3.3V 8MHz 328 using software SPI port manipulation: 3.5+2 us per SCLK tick, latch-pulse, latch-pulse digitalWrite: 10us
// clock-timings on 3.3V 8MHz 328 using hardware SPI: 0.25+0.25 us per SCLK tick, latch-pulse port manipulation: 4us


void setup()
{
  Serial.begin(115200);
  Serial.println(F("mxUnified74HC595 Blink"));

  unio.begin();     // start using the mxUnified74HC595 I2C shift register
  unio.digitalWrite(1, HIGH);   // set expanded pin 3 high
}

void loop()
{
  Serial.print(F("."));
  unio.digitalWrite(3, HIGH);   // set expanded pin 3 high
  unio.digitalWrite(4, LOW);   // set expanded pin 3 high
  delay(500);
  unio.digitalWrite(3, LOW);   // set expanded pin 3 low
  unio.digitalWrite(4, HIGH);   // set expanded pin 3 high
  delay(500);
}
