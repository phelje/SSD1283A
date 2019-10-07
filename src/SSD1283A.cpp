// created by Jean-Marc Zingg to be a standalone SSD1283A library (instead of the GxCTRL_SSD1283A class for the GxTFT library)
// code extracts taken from https://github.com/lcdwiki/LCDWIKI_SPI
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// note: my only SSD1283A display is write only, read methods of this class don't work

#include <SPI.h>
#include "SSD1283A.h"

#define SPI_SPEED 27000000
//#define SPI_SPEED 4000000

#define TFTLCD_DELAY16  0xFFFF

#ifndef _ADAFRUIT_GFX_H
SSD1283A::SSD1283A(int8_t cs, int8_t cd, int8_t rst, int8_t led) : _spi_settings(SPI_SPEED, MSBFIRST, SPI_MODE0)
#else
SSD1283A::SSD1283A(int8_t cs, int8_t cd, int8_t rst, int8_t led) : Adafruit_GFX(130, 130), _spi_settings(SPI_SPEED, MSBFIRST, SPI_MODE0)
#endif
{
  _cs = cs;
  _cd = cd;
  _rst = rst;
  _led = led;
  digitalWrite(_cs, HIGH);
  digitalWrite(_cd, HIGH);
  pinMode(cs, OUTPUT);	  // Enable outputs
  pinMode(cd, OUTPUT);

  if (rst >= 0)
  {
    digitalWrite(rst, HIGH);
    pinMode(rst, OUTPUT);
  }
  if (led >= 0)
  {
    pinMode(led, OUTPUT);
  }
  _rotation = 0;
  WIDTH = 130;
  HEIGHT = 130;
  _width = WIDTH;
  _height = HEIGHT;
}

void SSD1283A::init(void)
{
  digitalWrite(_cs, HIGH);
  SPI.begin();
  //SPI.beginTransaction( { SPI_SPEED, MSBFIRST, SPI_MODE0 } );
  if (_rst >= 0)
  {
    digitalWrite(_rst, LOW);
    delay(2);
    digitalWrite(_rst, HIGH);
  }
  setBackLight(true);
  delay(200);
  static const uint16_t SSD1283A_regValues[] PROGMEM =
  {
    0x10, 0x2F8E,
    0x11, 0x000C,
    0x07, 0x0021,
    0x28, 0x0006,
    0x28, 0x0005,
    0x27, 0x057F,
    0x29, 0x89A1,
    0x00, 0x0001,
    TFTLCD_DELAY16, 100,
    0x29, 0x80B0,
    TFTLCD_DELAY16, 30,
    0x29, 0xFFFE,
    0x07, 0x0223,
    TFTLCD_DELAY16, 30,
    0x07, 0x0233,
    0x01, 0x2183,
    0x03, 0x6830,
    0x2F, 0xFFFF,
    0x2C, 0x8000,
    0x27, 0x0570,
    0x02, 0x0300,
    0x0B, 0x580C,
    0x12, 0x0609,
    0x13, 0x3100,
  };
  //Serial.println("init() 1");
  _init_table16(SSD1283A_regValues, sizeof(SSD1283A_regValues));
  //Serial.println("init() 2");
  setRotation(_rotation);
  //Serial.println("init() 3");
  invertDisplay(false);
  //Serial.println("init() 4");
}

void SSD1283A::setBackLight(bool lit)
{
  if (_led >= 0) digitalWrite(_led, lit ? HIGH : LOW);
}

void SSD1283A::_startTransaction()
{
  SPI.beginTransaction(_spi_settings);
  if (_cs >= 0) digitalWrite(_cs, LOW);
}

void SSD1283A::_endTransaction()
{
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void SSD1283A::_writeCmd8(uint8_t cmd)
{
  digitalWrite(_cd, LOW);
  SPI.transfer(cmd);
  digitalWrite(_cd, HIGH);
}

void SSD1283A::_writeData8(uint8_t data)
{
  digitalWrite(_cd, HIGH);
  SPI.transfer(data);
}

void SSD1283A::_writeCmd16(uint16_t cmd)
{
  digitalWrite(_cd, LOW);
  SPI.transfer(cmd >> 8);
  SPI.transfer(cmd);
  digitalWrite(_cd, HIGH);
}
void SSD1283A::_writeData16(uint16_t data)
{
  digitalWrite(_cd, HIGH);
  SPI.transfer(data >> 8);
  SPI.transfer(data);
}

void SSD1283A::_writeCmdData16(uint16_t cmd, uint16_t data)
{
  digitalWrite(_cd, LOW);
  SPI.transfer(cmd >> 8);
  SPI.transfer(cmd);
  digitalWrite(_cd, HIGH);
  SPI.transfer(data >> 8);
  SPI.transfer(data);
}

void SSD1283A::_writeCmdTransaction16(uint16_t cmd)
{
  _startTransaction();
  _writeCmd16(cmd);
  _endTransaction();
}

void SSD1283A::_writeDataTransaction16(uint16_t data)
{
  _startTransaction();
  _writeData16(data);
  _endTransaction();
}

void SSD1283A::_writeCmdDataTransaction16(uint16_t cmd, uint16_t data)
{
  _startTransaction();
  _writeCmdData16(cmd, data);
  _endTransaction();
}

// Sets the LCD address window
void SSD1283A::setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  _startTransaction();
  switch (_rotation)
  {
    case 0:
      _writeCmd8(0x44);
      _writeData8(x2 + 2);
      _writeData8(x1 + 2);
      _writeCmd8(0x45);
      _writeData8(y2 + 2);
      _writeData8(y1 + 2);
      _writeCmd8(0x21);
      _writeData8(y1 + 2);
      _writeData8(x1 + 2);
      break;
    case 1:
      _writeCmd8(0x44);
      _writeData8(HEIGHT - y1 + 1);
      _writeData8(HEIGHT - y2 + 1);
      _writeCmd8(0x45);
      _writeData8(WIDTH - x1 - 1);
      _writeData8(WIDTH - x2 - 1);
      _writeCmd8(0x21);
      _writeData8(WIDTH - x2 - 1);
      _writeData8(HEIGHT - y2 + 1);
      break;
    case 2:
      _writeCmd8(0x44);
      _writeData8(WIDTH - x1 + 1);
      _writeData8(WIDTH - x2 + 1);
      _writeCmd8(0x45);
      _writeData8(HEIGHT - y1 + 1);
      _writeData8(HEIGHT - y2 + 1);
      _writeCmd8(0x21);
      _writeData8(HEIGHT - y2 + 1);
      _writeData8(WIDTH - x2 + 1);
      break;
    case 3:
      _writeCmd8(0x44);
      _writeData8(y2 + 2);
      _writeData8(y1 + 2);
      _writeCmd8(0x45);
      _writeData8(x2);
      _writeData8(x1);
      _writeCmd8(0x21);
      _writeData8(x1);
      _writeData8(y1 + 2);
      break;
  }
  _writeCmd8(0x22);
  _endTransaction();
}

void SSD1283A::pushColors(uint16_t * block, int16_t n, bool first, uint8_t flags)
{
  uint16_t color;
  uint8_t h, l;
  bool isconst = flags & 1;
  //	bool isbigend = (flags & 2) != 0;
  _startTransaction();
  if (first)
  {
    _writeCmd8(0x22);
  }
  while (n-- > 0)
  {
    if (isconst)
    {
      color = pgm_read_word(block++);
    }
    else
    {
      color = (*block++);

    }
    _writeData16(color);
  }
  _endTransaction();
}

void SSD1283A::pushColors(const uint16_t* data, uint16_t n)
{
  _startTransaction();
  digitalWrite(_cd, HIGH);
#if (defined (ESP8266) || defined(ESP32) || (TEENSYDUINO == 147)) && true // faster
  _pushColorsFastWithByteSwapping(data, n);
#else
  while (n-- > 0)
  {
    uint16_t color = (*data++);
#if (defined (ESP8266) || defined(ESP32)) && true // faster
    SPI.write16(color);
#else
    SPI.transfer(color >> 8);
    SPI.transfer(color);
#endif
  }
#endif
  _endTransaction();
}

void SSD1283A::_pushColorsFast(const uint16_t* data, uint16_t n)
{
#if (defined(NOT_YET_KNOWN_PROCESSOR) || defined(TEENSYDUINO)) // just in case 
  SPI.transfer((uint8_t*)data, 0, 2 * n);
#endif
}

void SSD1283A::_pushColorsFastWithByteSwapping(const uint16_t* data, uint16_t n)
{
  static const uint16_t swap_buffer_size = 64; // optimal for ESP8266 SPI
  static const uint16_t max_chunk = swap_buffer_size / 2; // uint16_t's
  uint8_t swap_buffer[swap_buffer_size];
  const uint8_t* p1 = reinterpret_cast<const uint8_t*> (data);
  const uint8_t* p2 = p1 + 1;
  while (n > 0)
  {
    uint16_t chunk = min(max_chunk, n);
    n -= chunk;
    uint8_t* p3 = swap_buffer;
    uint8_t* p4 = p3 + 1;
    uint16_t ncopy = chunk;
    while (ncopy-- > 0)
    {
      *p3 = *p2; p3 += 2; p2 += 2;
      *p4 = *p1; p4 += 2; p1 += 2;
    }
#if (defined (ESP8266) || defined(ESP32))
    SPI.transferBytes(swap_buffer, 0, 2 * chunk);
#else
    SPI.transfer(swap_buffer, 0, 2 * chunk);
#endif
  }
}

void SSD1283A::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (y < 0) || (x > getWidth()) || (y > getHeight()))
  {
    return;
  }
  setWindowAddress(x, y, x, y);
  _startTransaction();
  _writeData16(color);
  _endTransaction();
}

void SSD1283A::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  int16_t end;
  if (w < 0)
  {
    w = -w;
    x -= w;
  }                           //+ve w
  end = x + w;
  if (x < 0)
  {
    x = 0;
  }
  if (end > getWidth())
  {
    end = getWidth();
  }
  w = end - x;
  if (h < 0)
  {
    h = -h;
    y -= h;
  }                           //+ve h
  end = y + h;
  if (y < 0)
  {
    y = 0;
  }
  if (end > getHeight())
  {
    end = getHeight();
  }
  h = end - y;
  setWindowAddress(x, y, x + w - 1, y + h - 1);
  _startTransaction();
  _writeCmd8(0x22);
  if (h > w)
  {
    end = h;
    h = w;
    w = end;
  }
  while (h-- > 0)
  {
    end = w;
    do
    {
      _writeData16(color);
    } while (--end != 0);
  }
  _endTransaction();
}

//Scroll display
void SSD1283A::setVerticalScroll(int16_t top, int16_t scrollines, int16_t offset)
{
  int16_t bfa = HEIGHT - top - scrollines;
  int16_t vsp;
  int16_t sea = top;
  if (offset <= -scrollines || offset >= scrollines)
  {
    offset = 0; //valid scroll
  }
  vsp = top + offset; // vertical start position
  if (offset < 0)
  {
    vsp += scrollines;          //keep in unsigned range
  }
  sea = top + scrollines - 1;
  _writeCmdDataTransaction16(0x41, vsp);
}

//get lcd width
int16_t SSD1283A::getWidth(void) const
{
  return _width;
}

//get lcd height
int16_t SSD1283A::getHeight(void) const
{
  return _height;
}

//set clockwise rotation
void SSD1283A::setRotation(uint8_t r)
{
#ifdef _ADAFRUIT_GFX_H
  Adafruit_GFX::setRotation(r);
#endif
  _rotation = r & 3;           // just perform the operation ourselves on the protected variables
  _width = (_rotation & 1) ? HEIGHT : WIDTH;
  _height = (_rotation & 1) ? WIDTH : HEIGHT;
  _startTransaction();
  switch (_rotation)
  {
    case 0:
    case 2:
      _writeCmdData16(0x01, 0x2183);
      _writeCmdData16(0x03, 0x6830);
      break;
    case 1:
    case 3:
      _writeCmdData16(0x01, 0x2283);
      _writeCmdData16(0x03, 0x6838);
      break;
  }
  _endTransaction();
  setWindowAddress(0, 0, _width - 1, _height - 1);
  setVerticalScroll(0, HEIGHT, 0);
}

//get current rotation
//0  :  0 degree
//1  :  90 degree
//2  :  180 degree
//3  :  270 degree
uint8_t SSD1283A::getRotation(void) const
{
  return _rotation;
}

//Anti color display
void SSD1283A::invertDisplay(boolean i)
{
  _startTransaction();
  uint8_t val = 1 ^ i;
  uint16_t reg;
  if ((_rotation == 0) || (_rotation == 2))
  {
    if (val)
    {
      reg = 0x2183;
    }
    else
    {
      reg = 0x0183;
    }
  }
  else if ((_rotation == 1) || (_rotation == 3))
  {
    if (val)
    {
      reg = 0x2283;
    }
    else
    {
      reg = 0x0283;
    }
  }
  _writeCmdData16(0x01, reg);
  _endTransaction();
}

void SSD1283A::_init_table16(const void *table, int16_t size)
{
  uint16_t *p = (uint16_t *) table;
  while (size > 0)
  {
    uint16_t cmd = pgm_read_word(p++);
    uint16_t d = pgm_read_word(p++);
    if (cmd == TFTLCD_DELAY16)
    {
      delay(d);
    }
    else
    {
      _writeCmdDataTransaction16(cmd, d);
    }
    size -= 2 * sizeof(int16_t);
  }
}

uint16_t SSD1283A::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
