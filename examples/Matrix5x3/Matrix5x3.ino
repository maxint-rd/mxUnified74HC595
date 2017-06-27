/*
 * 5x3 LED matrix connected to one 74HC595
 * 
 * The 5x3 matrix has 5 columns and 3 rows. Columns are combined with rows on one 74HC595,
 * The three rows (active low) are connected to the P0-P2 pins via current limiting resistors.
 * The five columns (active high) are directly connected to P3-P7 pins of the 74HC595.
 * P0, P1 and P2 are rows bottom, middle and top.
 * P3-P7 are columns left to right.
 * 
 * The 74HC595 is driven using the mxUnified74HC595 Univerfied Output library.
 * This sketch doesn't use a timer interrupt, but refreshed the matrix rows using a special delay function
 */

#include <mxUnified74HC595.h>

//#define _MXUNIFIEDOUTPUT_DEBUG 0
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

// Hardware SPI pins:
//   ATmega328: SS=10, MOSI=11, SCLK=12
//   ESP8266:   SS=15, MOSI=13, SCLK=14
//   ATtiny85:  SS=0, MOSI=1, SCLK=2
//   Suggested pins ESP-01:   SS=3 (RX), MOSI=2, SCLK=0

// clock-timings on 3.3V 8MHz 328 using software SPI shiftOut: 7+21 us per SCLK tick, latch-pulse digitalWrite: 10us
// clock-timings on 3.3V 8MHz 328 using software SPI port manipulation: 3.5+2 us per SCLK tick, latch-pulse, latch-pulse digitalWrite: 10us
// clock-timings on 3.3V 8MHz 328 using hardware SPI: 0.25+0.25 us per SCLK tick, latch-pulse port manipulation: 4us



////////////////////////////
// Definitions            //
////////////////////////////
#define ROWS_MAX 3
#define COLUMNS_MAX 6
#define ROWS 3
#define COLUMNS 5

// memory space for the matrix
byte barrMatrix[ROWS_MAX]; // = {0, 0, 0, 0, 0};

#define zero_code                  0b0011101000101110
#define one_code                   0b0000001111101000
#define two_code                   0b0011011001101001
#define three_code                 0b0010101010110001
#define four_code                  0b0111110010011100
#define five_code                  0b0100101010111001
#define six_code                   0b0100101010101110
#define seven_code                 0b0110001010010011
#define eight_code                 0b0010101010101010
#define nine_code                  0b0011101010101001
const uint16_t arrCodes[] PROGMEM={zero_code,one_code,two_code,three_code,four_code,five_code,six_code,seven_code,eight_code, nine_code};

const uint8_t PBitMap1[ROWS_MAX] PROGMEM = {   // store in program memory to save RAM
  0b11111,
  0b10001,
  0b11111
};

const uint8_t PBitMap2[ROWS_MAX] PROGMEM = {   // store in program memory to save RAM
  0b10101,
  0b01010,
  0b10101
};

const uint8_t PBitMap3[ROWS_MAX] PROGMEM = {   // store in program memory to save RAM
  0b01010,
  0b10101,
  0b01010
};

const uint8_t PBitMap4[ROWS_MAX] PROGMEM = {   // store in program memory to save RAM
  0b00010,
  0b11111,
  0b00010
};

const uint8_t PBitMap5[ROWS_MAX] PROGMEM = {   // store in program memory to save RAM
  0b00100,
  0b01010,
  0b10001
};

const uint8_t PBitMap6[ROWS_MAX] PROGMEM = {   // store in program memory to save RAM
  0b10001,
  0b01010,
  0b00100
};


void matrixLedOn(char row, char col)
{ // set a matrix led on, origin (0,0) is left top. 
  if(row >= 0 && row < ROWS && col >= 0 && col < COLUMNS)
  {
    //unsigned char bCol=1<<(COLUMNS-col-1);    // force origin left to right.
    unsigned char bCol=1<<col;
    barrMatrix[row]|=bCol;
  }
}

void matrixLedOff(char row, char col)
{ // set a led off
  if(row >= 0 && row < ROWS && col >= 0 && col < COLUMNS)
  {
    //unsigned char bCol=1<<(COLUMNS-col-1);    // force origin left to right.
    unsigned char bCol=1<<col;
    barrMatrix[row]&= ~bCol;
  }
}

void matrixOff(void)
{ // clear the entire matrix
  for(int i=0; i<ROWS; i++)
    barrMatrix[i]=0;
}

void matrixOn(void)
{ // switch the entire matrix on
  for(int i=0; i<ROWS; i++)
    barrMatrix[i]=0b00011111;
}


void matrixBitmap(const uint8_t *pBitmap)
{ // set a bitmap (assume array of five bytes containing five rows with columns in least significant bits)
  for(int i=0; i<ROWS_MAX; i++)
  {
    barrMatrix[i]=pBitmap[i];
//  sprintf(szDebug, "setBitmap: %u, %u", i, barrMatrix[i]);
//  Serial.println(szDebug);
  }

}

//void matrixBitmapP(const uint8_t *pBitmap, int nDelay=1000)
void matrixBitmapP(const uint8_t *pBitmap)
{ // set a PROGMEM bitmap (assume array of five bytes containing ROWS_MAX rows with columns in least significant bits)
  memcpy_P(barrMatrix, pBitmap, ROWS_MAX);
//  if(nDelay>0) delayR(nDelay);
}



void matrixBitmap16(const uint16_t uCode)
{ // set a bitmap using a 16 bit code
  // (assume packed array of three 5-bit values plus 1 bit filler containing three rows with five columns in least significant bits)
  uint16_t uCodeLeft=uCode;
  for(int i=0; i<ROWS; i++)
  {
    barrMatrix[i]=(uCodeLeft & 0b0111110000000000)>>10;
    uCodeLeft<<=5;
  }
}

void matrixBitmap16P(const uint16_t *pCode)
{ // set a bitmap using a 16 bit code
  uint16_t bmp16;
  memcpy_P(&bmp16, pCode, sizeof(uint16_t));
  matrixBitmap16(bmp16);
}

void matrixAnimation(const uint16_t *pAni, int nFrames)
{ // assume array of packed bitmaps
//  uint16_t bmp16;

  for(int i = 0; i<nFrames; i++)
  {
    matrixBitmap16P(pAni+i);
//    memcpy_P(&bmp16, pAni+i, sizeof(uint16_t));
//    matrixBitmap16(bmp16);
//    matrixBitmap16(*(pAni+i));
//    *sizeof(uint16_t)));
//    delayR(200);
  }
}

void matrixNumber(const uint8_t bCode)
{
   uint16_t bmp16;
   memcpy_P(&bmp16, arrCodes+bCode, sizeof(uint16_t));
   matrixBitmap16(bmp16);
}


void matrixSendRow(uint8_t bRow,uint8_t bCol)
{ // Send one row to the matrix using two chained 74HC595.
  // Note: for proper display only one row-pin should be on at a time
  // Timer interrupt can be used to use POV to seemingly display pixels on different rows simultaneously

/* the 5x3 matrix has 5 columns and 3 rows. Columns are combined with rows on one 74HC595,
 * bits 0, 1 and 2 are rows bottom, middle and top (active low)
 * bits 3-7 are columns left to right (active high)
 */
  // merge row and column bits
  uint8_t bCombined=(~bRow) & 0x07;   //negative is needed: to light up the led, the col should be high when the row is low
  bCombined|=(bCol<<3);

  // shift combined byte out
  //shiftOut(dataPin, clockPin, LSBFIRST, bCombined);
  //shiftOutFast(LSBFIRST, bCombined);      // faster and 28 bytes smaller!
  unio.digitalWrite8(bCombined);

  //take the latch pin high so the LEDs will light up:
  //digitalWriteFast(PIN_LATCH, HIGH);
  //delayNop2();

//  sprintf(szDebug, "sendRow: %u, %u", bRow, bCol);
//  Serial.println(szDebug);
}

volatile byte refreshRow = 0;
void matrixRefresh()
{ // show all columns of current row
  unsigned char bRow=1<<refreshRow;
  matrixSendRow(bRow, barrMatrix[refreshRow]);
  refreshRow++;
  if(refreshRow>=ROWS) refreshRow=0;
}

void delayR(int nDelay)
{
  for(int n=1; n<nDelay; n++)
  {
    matrixRefresh();
    delay(1);
  }
}



void setup()
{
  Serial.begin(115200);
  Serial.println(F("mxUnified74HC595 Matrix5x3"));

  unio.begin();     // start using the mxUnified74HC595 I2C shift register

/**/
  matrixOn();
  delayR(500);
  matrixOff();
  delayR(500);
/**/
}

void loop()
{
  Serial.print(F("."));

  matrixLedOn(0,0);
  delayR(500);
  matrixLedOff(0,0);
  delayR(500);



/*
  Blink(100);
  matrixSendRow(0b00010101, 0b00000001);

  delay(1000);               // wait for a second
  Blink(100);
  Blink(100);
  Blink(100);

  matrixSendRow(0b00001010, 0b00000001);

  delay(1000);               // wait for a second
  Blink(100);
  Blink(100);
  Blink(100);
*/
/*
    matrixOn();
    delayR(800);
    matrixOff();
    delayR(800);
    matrixOn();
*/
  for(byte i=1; i<=5; i++)
  {
    matrixOn();
    delayR(100*i);
    matrixOff();
    delayR(100);
  }

  //matrixAnimation(dreamMouth_code, sizeof(dreamMouth_code));


//  Blink(100);
  for(int row=0; row<ROWS; row++)
    for(int col=0; col<COLUMNS; col++)
    {
        matrixOff();
        matrixLedOn(row,col);
        delayR(50);
    }

  matrixOff();
//  Blink(100);
  for(int col=0; col<COLUMNS; col++)
    for(int row=0; row<ROWS; row++)
    {
        matrixOff();
        matrixLedOn(row,col);
        delayR(50);
    }
/*
*/

/*
//  matrixOff();
//  Blink(100);
  for(int row=0; row<ROWS; row++)
    for(int col=0; col<COLUMNS; col++)
    {
        matrixLedOn(ROWS-1-row,COLUMNS-1-col);
        delayR(25);
        matrixLedOff(ROWS-1-row,COLUMNS-1-col);
        //delay(25);
    }
*/

delayR(300);
//for(byte i=9; i>=0;i--)
for(int i=9; i>=0;i--)    // decresing byte doesn't compile allright
{
  matrixNumber(i);
  delayR(300);
}

//delayR(300);

/**/
  //Blink(100);
//  for(int j = 0; j<3; j++)
//  {
/*
    for(int i = 0; i<sizeof(wave_code)/sizeof(uint16_t); i++)
    //for(int i = 0; i<sizeof(wave_code); i++)
    {
//      matrixOff();
//      matrixBitmap16(wave_code[i]);
//      matrixBitmap16P(wave_code+i);
      delayR(200);
    }
*/
/*
    for(int i = 0; i<sizeof(adivinawi_code)/sizeof(uint16_t); i++)
    {
      matrixOff();
      matrixBitmap16(adivinawi_code[i]);
      delayR(200);
    }

    for(int i = 0; i<sizeof(littleUuh_code)/sizeof(uint16_t); i++)
    {
      matrixOff();
      matrixBitmap16(littleUuh_code[i]);
      delayR(200);
    }
*/

   //matrixAnimation(dreamMouth_code, sizeof(dreamMouth_code)/sizeof(uint16_t));
/*
    for(int i = 0; i<sizeof(dreamMouth_code)/sizeof(uint16_t); i++)
    {
//      matrixOff();
      matrixBitmap16(dreamMouth_code[i]);
      delayR(200);
    }
*/
//  }





//  Blink(100);
//  matrixOff();
  matrixBitmapP(PBitMap1);
  delayR(1000);
  matrixBitmapP(PBitMap2);
  delayR(1000);
  matrixBitmapP(PBitMap3);
  delayR(1000);
  matrixBitmapP(PBitMap4);
  delayR(1000);
  matrixBitmapP(PBitMap5);
  delayR(1000);
  matrixBitmapP(PBitMap6);
  delayR(1000);


//  Blink(100);
  matrixOff();
  for(int row=0; row<ROWS; row++)
    for(int col=0; col<COLUMNS; col++)
    {
        matrixLedOn(row,col);
        delayR(50);
    }
//  Blink(100);
  for(int row=0; row<ROWS; row++)
    for(int col=0; col<COLUMNS; col++)
    {
        matrixLedOff(row,col);
        delayR(50);
    }
  delay(1000);


  for(int n=0; n<256; n++)
  {
    matrixSendRow(n>>5, n&0b00011111);
    delay(50);
    //Blink(50);
  }

}
