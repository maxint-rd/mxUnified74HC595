#ifndef _MXUNIFIED74HC595_H
#define _MXUNIFIED74HC595_H

#define _MXUNIFIEDIO_DEBUG 0		// Note: Serial debugging not supported on ATtiny85 

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
#endif
#include "mxUnifiedIO.h"
#if defined(MXUNIFIED_ATTINY)
	#include <tinySPI.h>               //http://github.com/JChristensen/tinySPI
#else
	#include <SPI.h>
#endif

#ifdef ESP8266
	#define MXUNIFIED74HC595_SPI_PIN_SS 15			// SS=15 by default on ESP?
#elif defined(MXUNIFIED_ATTINY)
	#define MXUNIFIED74HC595_SPI_PIN_SS 0			// SS=0 by default on ATtiny
	#define SPI_CLOCK_DIV4 4			// setting SPI clock is not supported by tinySPI
#else
	#define MXUNIFIED74HC595_SPI_PIN_SS 10
#endif
#define MXUNIFIED74HC595_MAX_REGISTERS 4

// Default to max SPI clock speed for PCD8544 of 4 mhz (16mhz / 4) for normal Arduinos.
// This can be modified to change the clock speed if necessary (like for supporting other hardware).
#define MXUNIFIED74HC595_SPI_CLOCK_DIV SPI_CLOCK_DIV4


#ifdef __SAM3X8E__
  typedef volatile RwReg PortReg;
  typedef uint32_t PortMask;
#elif defined(ESP8266)
  typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#else
  typedef volatile uint8_t PortReg;
  typedef uint8_t PortMask;
#endif

class mxUnified74HC595 : public mxUnifiedIO
{
 public:
 	mxUnified74HC595(uint8_t nNumRegisters=1);				// constructor for hardware SPI and one or more shift registers
	mxUnified74HC595(uint8_t nPinSS, uint8_t nPinMOSI, uint8_t nPinSCLK, uint8_t nNumRegisters=1);		// constructor for software SPI

  // These are be overridden by this subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
#if defined(ESP8266)
  void begin(uint32_t spi_speed=4000000);
#else
  void begin(uint8_t spi_speed_div=SPI_CLOCK_DIV4);
#endif
  void send8Bits(bool fClosedTransmission=true);
  void sendBits(void);
  void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value);
/* no transaction support (yet) 
  void startTransmission(void);
  void endTransmission(void);
*/
  
 	void digitalWrite(uint8_t nPin, uint8_t nVal);
	void setBitOrder(uint8_t bitOrder);		// default bitorder is MSBFIRST

 private:
 	uint8_t _nNumRegisters;
 	int8_t _spi_SS, _spi_MOSI, _spi_SCLK;
 	uint8_t _spi_error;
 	uint8_t _spi_bitorder;

  volatile PortReg  *mosiport, *clkport, *ssport;
  PortMask mosipinmask, clkpinmask, sspinmask;

	bool isHardwareSPI();
  void spiWrite(uint8_t c);
	void latch(bool fBegin);
};

#endif // _MXUNIFIED74HC595_H
