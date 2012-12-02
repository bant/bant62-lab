extern "C"
{
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
}

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "I2CLCD.h"

// Constructors ////////////////////////////////////////////////////////////////
I2CLCD::I2CLCD(void)
{
    address = I2CLCD_SLAVE_ADRS;
}

I2CLCD::I2CLCD(int set_address)
{
    address = set_address;
}

// Destructor //////////////////////////////////////////////////////////////////
I2CLCD::~I2CLCD(void) 
{
  //  Serial.print("Deleted file I2C7SEG");
}

void I2CLCD::gotoCursor(uint8_t x, uint8_t y)
{
    beginTransmission(address);
    write(ESC);
    write('L');
    write(x);
    write(y);
    endTransmission();
}

void I2CLCD::home(void)
{
    beginTransmission(address);
    write(ESC);
    write('H');
    endTransmission();
}

void I2CLCD::clear(void)
{
    beginTransmission(address);
    write(ESC);
    write('C');
    endTransmission();
}

void I2CLCD::saveCustomCharacter(uint8_t romCharNum, uint8_t lcdCharData[])
{
    beginTransmission(address);
    write(ESC);
    write('S');
    write(romCharNum);
    for (int i=0; i<8;i++)
    {
        write(lcdCharData[i]);
    }
    endTransmission();
    delay(100);
}

void I2CLCD::mapCustomCharacter(uint8_t romCharNum, uint8_t lcdCharNum)
{
    beginTransmission(address);
    write(ESC);
    write('M');
    write(romCharNum);
    write(lcdCharNum);
    endTransmission();
}

//   ESC'X'      .... 画面非表示
void I2CLCD::offDisplay(void)
{
    beginTransmission(address);
    write(ESC);
    write('X');
    endTransmission();
}

//  ESC'N'      .... 画面表示・カーソル消去
void I2CLCD::onDisplay(void)
{
    beginTransmission(address);
    write(ESC);
    write('N');
    endTransmission();
}

//  ESC'B'      .... 画面表示・カーソル非表示・文字ブリンク
void I2CLCD::blinkCharacter(void)
{
    beginTransmission(address);
    write(ESC);
    write('B');
    endTransmission();
}

//    ESC'D'      .... 画面表示・カーソル表示
void I2CLCD::dispCursor(void)
{
    beginTransmission(address);
    write(ESC);
    write('D');
    endTransmission();
}

//    ESC'E'      .... 画面表示・カーソル表示・文字ブリンク
void I2CLCD::blinkCursor(void)
{
    beginTransmission(address);
    write(ESC);
    write('E');
    endTransmission();
}

//  ESC'N'      .... 画面表示・カーソル消去
void I2CLCD::hideCursor(void)
{
    beginTransmission(address);
    write(ESC);
    write('N');
    endTransmission();
}

//    ESC'-'      .... カーソル左移動
void I2CLCD::moveLeftCursor(void)
{
    beginTransmission(address);
    write(ESC);
    write('-');
    endTransmission();
}

//  ESC'+'      .... カーソル右移動
void I2CLCD::moveRightCursor(void)
{
    beginTransmission(address);
    write(ESC);
    write('+');
    endTransmission();
}

//  ESC'<'      .... 画面左移動
void I2CLCD::moveLeftDisplay(void)
{
    beginTransmission(address);
    write(ESC);
    write('<');
    endTransmission();
}

//  ESC'>'      .... 画面右移動
void I2CLCD::moveRightDisplay(void)
{
    beginTransmission(address);
    write(ESC);
    write('>');
    endTransmission();
}

// 一文字表示
void I2CLCD::printChar(uint8_t chr)
{
    beginTransmission(address);
    write(chr);
    endTransmission();
}

// 文字列表示
void I2CLCD::printStr(char *str)
{
    beginTransmission(address);
    for (int i = 0; i < strlen(str); i++ )
    {
        write(str[i]);
    }
    endTransmission();
}

//I2CLCD I2clcd;
