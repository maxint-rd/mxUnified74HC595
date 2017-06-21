#include "mxUnified74HC595.h"

mxUnified74HC595::mxUnified74HC595(uint8_t nNumRegisters)		// default: nNumRegisters=1 
{	// constructor for hardware SPI
	// Hardware SPI pins:
	//   ATmega328: SS=10, MOSI=11, SCLK=12
	//   ESP8266:   SS=15, MOSI=13, SCLK=14
	
	_nConstr++;
	_nNumRegisters=nNumRegisters;
	_nNumRegisters=min(MXUNIFIED74HC595_MAX_REGISTERS, nNumRegisters);
	_nNumPins=_nNumRegisters*8;

	_spi_SS=MXUNIFIED74HC595_SPI_PIN_SS;
	_spi_MOSI=-1;
	_spi_SCLK=-1;
//	_spi_MOSI=11;
//	_spi_SCLK=13;
}

mxUnified74HC595::mxUnified74HC595(uint8_t nPinSS, uint8_t nPinMOSI, uint8_t nPinSCLK, uint8_t nNumRegisters)		// default: nNumRegisters=1 
{	// constructor for software SPI
	_nConstr++;
	_nNumRegisters=nNumRegisters;
	_nNumRegisters=min(MXUNIFIED74HC595_MAX_REGISTERS, nNumRegisters);
	_nNumPins=_nNumRegisters*8;

	_spi_SS=nPinSS;
	_spi_MOSI=nPinMOSI;
	_spi_SCLK=nPinSCLK;
}

#if defined(ESP8266)
void mxUnified74HC595::begin(uint32_t spi_speed)		// default:  spi_speed=4000000
#else
void mxUnified74HC595::begin(uint8_t spi_speed_div)	// default: spi_speed_div=SPI_CLOCK_DIV4
#endif
{
	// Call without parameters assumes using the default pins.
	// For the base mxUnifiedIO class the default pins are tied directly to the MCU pins

	// The standard pin definition allows the following devices connected as example
	// ATmega328, PCD8544+Max7219 panel:
	//  P3;  D13 - led
	//  P2:  D3 - RST
	//  P4:  D4 - CE
	//  P5:  D5 - DC
	//  P6:  D6 - DIN
	//  P7:  D7 - CLK	
	
	//  P0:  D10 - CS  (SS)
	//  P1:  D11 - DIN (MOSI)
	//       D12 - not connected (MISO)
	//  P3:  D13 - CLK (SCK)
  if (isHardwareSPI())
  {
    // Setup hardware SPI.
    SPI.begin();
#ifdef ESP8266
		// see https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library/pull/27/commits
    // Datasheet says 4 MHz is max SPI clock speed
    SPI.setFrequency(spi_speed);			// 4MHz works on some devices, but may be too fast for 74HC595 using regular breadboard and wires
#else
    SPI.setClockDivider(spi_speed_div);	// DIV4=4 Mhz on 5V@16MHz or 2MHz on 3.3V@8MHz
#endif
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
  }
  else
  {
    // Setup software SPI.

    // Set software SPI specific pin outputs.
		::pinMode(_spi_MOSI, OUTPUT);
		::pinMode(_spi_SCLK, OUTPUT);
/*
		::digitalWrite(_spi_MOSI, LOW);
		::digitalWrite(_spi_SCLK, LOW);
*/

// see https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library/pull/27/commits
#ifndef ESP8266
    // Set software SPI ports and masks.
    clkport     = portOutputRegister(digitalPinToPort(_spi_SCLK));
    clkpinmask  = digitalPinToBitMask(_spi_SCLK);
    mosiport    = portOutputRegister(digitalPinToPort(_spi_MOSI));
    mosipinmask = digitalPinToBitMask(_spi_MOSI);

#if(_MXUNIFIEDIO_DEBUG)
  Serial.print("sw-SPI: SS=");
  Serial.print(_spi_SS);
  Serial.print(", MOSI=");
  Serial.print(_spi_MOSI);
  Serial.print(", SCLK=");
  Serial.println(_spi_SCLK);

  Serial.print("clkport:");
  Serial.println(*clkport);
  Serial.print("Mask:");
  Serial.println(clkpinmask);
  Serial.print("mosiport:");
  Serial.println(*mosiport);
  Serial.print("Mask:");
  Serial.println(mosipinmask);
#endif

#endif
  }

#ifdef ESP8266
	::pinMode(_spi_SS, OUTPUT);
//  ::digitalWrite(_spi_SS, LOW);
#else
  ssport    = portOutputRegister(digitalPinToPort(_spi_SS));
  sspinmask = digitalPinToBitMask(_spi_SS);
#if(_MXUNIFIEDIO_DEBUG)
  Serial.print("SSPort:");
  Serial.println(*ssport);
  Serial.print("Mask:");
  Serial.println(sspinmask);
#endif
#endif


#if(_MXUNIFIEDIO_DEBUG)
  Serial.print(F("mxUnified74HC595::begin("));
  Serial.print(" ");
  Serial.print(F(") constr:"));
  Serial.print(_nConstr);
  Serial.print(F(", nNumRegisters:"));
  Serial.print(_nNumRegisters);
  Serial.print(F(", _nNumPins:"));
  Serial.print(_nNumPins);
  Serial.println(F("."));
#endif
}

void mxUnified74HC595::send8Bits(bool fClosedTransmission)		// default: fClosedTransmission=true
{
//	if(fClosedTransmission)
//		startTransmission();
	sendBits();
//	if(fClosedTransmission)
//	  endTransmission();
}

void mxUnified74HC595::sendBits()
{
	// shift the 8 output pins using the set pins
	//::shiftOut(_spi_MOSI, _spi_SCLK, MSBFIRST, _dataOut);
	uint32_t uOut=_dataOut;
#if(_MXUNIFIEDIO_DEBUG)
  Serial.print(F("mxUnified74HC595::sendBits("));
  Serial.print(_dataOut, BIN);
  Serial.print(F("):"));
  
#endif
	latch(true);
	for(uint8_t n=0; n<_nNumRegisters; n++)
	{
#if(_MXUNIFIEDIO_DEBUG)
  Serial.print(F("["));
  Serial.print(n);
  Serial.print(F(": "));
  Serial.print(uOut&0xFF, BIN);
  Serial.print(F("]"));
#endif
		spiWrite(uOut&0xFF);
		if(_nNumPins>8) uOut>>=8;
	}
	latch(false);
#if(_MXUNIFIEDIO_DEBUG)
  Serial.println(F("."));
#endif
}

void mxUnified74HC595::shiftOut(uint8_t nDataPin, uint8_t nClockPin, uint8_t bitOrder, uint8_t nValue)
{	// shiftOut the bits of one byto to the stated exended data and clock pins
	// TODO: bitOrder can be MSBFIRST or LSBFIRST, not implemented yet!
	// TODO: currently shiftOut will not start/end transmission. Would be nice if automatic?
	uint8_t aI2C_bytestream[16];
	uint8_t uCnt=0;
	//uint8_t dataOut=_pUniOut->getBits();
#if(_MXUNIFIEDIO_DEBUG)
	Serial.print(F("O"));
#endif

  for(uint8_t bit = 0x80; bit; bit >>= 1)
  {
		// Using the PCF8574 library for bulk writes is WAY TOO SLOW! (1400 ms to display the screen)
 		// Each SPI DIN databyte transfer requires 16 CLK toggles, i.e. 16 I2C bytes to write.
 		// Therefor bundle the writing of each byte into one Wire.write() call and minimize the begin/end calls.
 		// This saves writing the address for each bit and speeds screen display up to 380 ms.
 		// Still slow, but acceptable when display() function is not called after drawing each primitive. 
		// BTW. Setting bits is more readable using the i2cSetBit function and doesn't cost much performance.
  	setBit(nClockPin, LOW);
  	setBit(nDataPin, (nValue & bit));
  	sendBits();
    //aI2C_bytestream[uCnt++]=_dataOut;
  	setBit(nClockPin, HIGH);
  	sendBits();
  	//aI2C_bytestream[uCnt++]=_dataOut;
  }

#if(_MXUNIFIEDIO_DEBUG)
	Serial.print(F("t"));
#endif

//	Serial.print(F("W"));
//  Wire.write(aI2C_bytestream, sizeof(aI2C_bytestream));		// writing the bytes of all 16 bits in one go (still takes 302-480ms)
//	Serial.print(F("e"));
}

/* no transaction support (yet) 
void mxUnified74HC595::startTransmission(void)
{    // Overwrite in subclasses if desired!
}

void mxUnified74HC595::endTransmission(void)
{    // Overwrite in subclasses if desired!
}
*/

 	
void mxUnified74HC595::digitalWrite(uint8_t nPin, uint8_t nVal)
{ // Overwrite in subclasses if desired!
#if(_MXUNIFIEDIO_DEBUG)
  Serial.print(F("mxUnified74HC595::digitalWrite("));
  Serial.print(nPin);
  Serial.print(F(","));
  Serial.print(nVal);
  Serial.println(F(")"));
#endif

	if (nPin >= _nNumPins) return;
	setBit(nPin, nVal);
//	::digitalWrite(_aPins[nPin], nVal);		// call the regular digitalWrite using the global scope resolution operator
	sendBits();
}	

inline void mxUnified74HC595::spiWrite(uint8_t d)
{
  if (isHardwareSPI())
  {
#if(_MXUNIFIEDIO_DEBUG_SPI)
  Serial.print("hw-SPI: SS=");
  Serial.print(_spi_SS);
  Serial.print(", MOSI=");
  Serial.print(_spi_MOSI);
  Serial.print(", SCLK=");
  Serial.println(_spi_SCLK);
#endif


    // Hardware SPI write.
		// On 328@8MHz: hw-SPI SCLK L+H=0.25+0.25us=0.5us (8MHz/DIV4=2MHz SPI)
		// On ESP@80MHz: hw-SPI SCLK L+H=0.125+0.125us=0.25us (4MHz SPI)
    SPI.transfer(d);
  }
  else
  {
#if(_MXUNIFIEDIO_DEBUG_SPI)
  Serial.print("sw-SPI: SS=");
  Serial.print(_spi_SS);
  Serial.print(", MOSI=");
  Serial.print(_spi_MOSI);
  Serial.print(", SCLK=");
  Serial.println(_spi_SCLK);
#endif

    // Software SPI write with bit banging. Note: sw-SPI has clock active low, data active high
    // On ESP8266: hw-SPI has MOSI and SCLK active high, fixed sw-SPI by first setting data and then pulsing clock
#ifdef ESP8266
    // see https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library/pull/27/commits
		// On ESP8266@80Mzh: digitalWrite SCLK L+H=0.5+1.0us=1.5us (667KHz)
    for(uint8_t bit = 0x80; bit; bit >>= 1)
    {
      //::digitalWrite(_spi_SCLK, LOW);
      if (d & bit) ::digitalWrite(_spi_MOSI, HIGH);
      else         ::digitalWrite(_spi_MOSI, LOW);
      //::digitalWrite(_spi_SCLK, HIGH);
      ::digitalWrite(_spi_SCLK, HIGH);
      ::digitalWrite(_spi_SCLK, LOW);
    }
 #else
    for(uint8_t bit = 0x80; bit; bit >>= 1)
    {
/*
			// On 328@8Mzh: digitalWrite SCLK L+H=8+17us=25us
      ::digitalWrite(_spi_SCLK, LOW);
      if (d & bit) ::digitalWrite(_spi_MOSI, HIGH);
      else         ::digitalWrite(_spi_MOSI, LOW);
      ::digitalWrite(_spi_SCLK, HIGH);
*/
			// On 328@8Mzh: port-mani SCLK L+H=3.5+2us=5.5us
      //*clkport &= ~clkpinmask;
      if(d & bit) *mosiport |=  mosipinmask;
      else        *mosiport &= ~mosipinmask;
      *clkport |=  clkpinmask;
      *clkport &= ~clkpinmask;			// changed clock to resemble hardware SPI: active high and pulse clock after setting data
    }
#endif
  }
}

bool mxUnified74HC595::isHardwareSPI()
{
  return (_spi_MOSI == -1 && _spi_SCLK == -1);
}

void mxUnified74HC595::latch(bool fBegin)
{
	if(fBegin)
	{
#ifdef ESP8266
		::digitalWrite(_spi_SS, LOW);
#else
	  *ssport &= ~sspinmask;
#endif
	}
	else
	{
#ifdef ESP8266
		::digitalWrite(_spi_SS, HIGH);
#else
	  *ssport |=  sspinmask;
#endif
	}
}
