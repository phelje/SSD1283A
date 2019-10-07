// created by Jean-Marc Zingg to be a standalone SSD1283A library (instead of the GxCTRL_SSD1283A class for the GxTFT library)
// code extracts taken from https://github.com/lcdwiki/LCDWIKI_SPI
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// note: my only SSD1283A display is write only, read methods of this class don't work

#ifndef _SSD1283A_H_
#define _SSD1283A_H_

#include <Arduino.h>
#include <SPI.h>

#if defined(__AVR__) || defined(__arm__)
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif

// include the graphics rendering class you want to use (or both)
//#include <LCDWIKI_GUI.h>
#include <Adafruit_GFX.h>

#ifndef _ADAFRUIT_GFX_H
class SSD1283A
#else
class SSD1283A : public Adafruit_GFX
#endif
{
  public:
    SSD1283A(int8_t cs, int8_t cd, int8_t rst, int8_t led);
    void init(void);
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void setRotation(uint8_t r);
    uint8_t getRotation(void) const;
    void invertDisplay(boolean i);
    void setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void pushColors(uint16_t * block, int16_t n, bool first, uint8_t flags);
    void pushColors(const uint16_t* data, uint16_t n); // fast one
    void setVerticalScroll(int16_t top, int16_t scrollines, int16_t offset);
    int16_t getHeight(void) const;
    int16_t getWidth(void) const;
    void setBackLight(bool lit);
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
  private:
    void _startTransaction();
    void _endTransaction();
    void _writeCmd8(uint8_t cmd);
    void _writeData8(uint8_t data);
    void _writeCmd16(uint16_t cmd);
    void _writeData16(uint16_t data);
    void _writeCmdData16(uint16_t cmd, uint16_t data);
    void _writeCmdTransaction16(uint16_t cmd);
    void _writeDataTransaction16(uint16_t data);
    void _writeCmdDataTransaction16(uint16_t cmd, uint16_t data);
    void _init_table16(const void *table, int16_t size);
    void _pushColorsFast(const uint16_t* data, uint16_t n); // fast one
    void _pushColorsFastWithByteSwapping(const uint16_t* data, uint16_t n); // fast one
  protected:
    uint16_t WIDTH, HEIGHT, _width, _height, _rotation;
  private:
    SPISettings _spi_settings;
    int8_t _cs, _cd, _rst, _led;
};

#ifdef _LCDWIKI_GUI_H_

class SSD1283A_GUI: public SSD1283A, public LCDWIKI_GUI
{
  public:
    SSD1283A_GUI(int8_t cs, int8_t cd, int8_t rst, int8_t led) : SSD1283A(cs, cd, rst, led) {};
    uint16_t Color_To_565(uint8_t r, uint8_t g, uint8_t b)
    {
      return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
    }
    void Draw_Pixe(int16_t x, int16_t y, uint16_t color)
    {
      SSD1283A::drawPixel(x, y, color);
    }
    virtual void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
    {
      SSD1283A::fillRect(x, y, w, h, color);
    }
    virtual void Set_Addr_Window(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
    {
      SSD1283A::setWindowAddress(x1, y1, x2, y2);
    }
    virtual void Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags)
    {
      SSD1283A::pushColors(block, n, first, flags);
    }
    virtual int16_t Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h) {};
    virtual int16_t Get_Height(void) const
    {
      return _height;
    }
    virtual int16_t Get_Width(void) const
    {
      return _width;
    }
};

#endif

#endif
