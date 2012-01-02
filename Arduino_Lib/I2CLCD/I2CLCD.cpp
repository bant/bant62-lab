extern "C"
{
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
}
#include <WConstants.h>
#include <Wire.h>
#include "I2CLCD.h"

#define I2CLCD_SLAVE_ADRS (0x64 >> 1)

// Constructors ////////////////////////////////////////////////////////////////
I2CLCD::I2CLCD()
{
}

// Public Methods //////////////////////////////////////////////////////////////

void I2CLCD::begin(void)
{
    Wire.begin();
}

void I2CLCD::gotoCursor(uint8_t x, uint8_t y)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('L');
    Wire.send(x);
    Wire.send(y);
    Wire.endTransmission();
}

void I2CLCD::home(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('H');
    Wire.endTransmission();
}

void I2CLCD::clear(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('C');
    Wire.endTransmission();
}

void I2CLCD::saveCustomCharacter(uint8_t romCharNum, uint8_t lcdCharData[])
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('S');
    Wire.send(romCharNum);
    for (int i=0; i<8;i++)
    {
        Wire.send(lcdCharData[i]);
    }
    Wire.endTransmission();
    delay(100);
}

void I2CLCD::mapCustomCharacter(uint8_t romCharNum, uint8_t lcdCharNum)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('M');
    Wire.send(romCharNum);
    Wire.send(lcdCharNum);
    Wire.endTransmission();
}

//   ESC'X'      .... 画面非表示
void I2CLCD::offDisplay(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('X');
    Wire.endTransmission();
}

//	ESC'N'      .... 画面表示・カーソル消去
void I2CLCD::onDisplay(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('N');
    Wire.endTransmission();
}

//	ESC'B'      .... 画面表示・カーソル非表示・文字ブリンク
void I2CLCD::blinkCharacter(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('B');
    Wire.endTransmission();
}

//    ESC'D'      .... 画面表示・カーソル表示
void I2CLCD::dispCursor(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('D');
    Wire.endTransmission();
}

//    ESC'E'      .... 画面表示・カーソル表示・文字ブリンク
void I2CLCD::blinkCursor(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('E');
    Wire.endTransmission();
}

//	ESC'N'      .... 画面表示・カーソル消去
void I2CLCD::hideCursor(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('N');
    Wire.endTransmission();
}

//    ESC'-'      .... カーソル左移動
void I2CLCD::moveLeftCursor(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('-');
    Wire.endTransmission();
}

//	ESC'+'      .... カーソル右移動
void I2CLCD::moveRightCursor(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('+');
    Wire.endTransmission();
}

//	ESC'<'      .... 画面左移動
void I2CLCD::moveLeftDisplay(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('<');
    Wire.endTransmission();
}

//	ESC'>'      .... 画面右移動
void I2CLCD::moveRightDisplay(void)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(ESC);
    Wire.send('>');
    Wire.endTransmission();
}

// 一文字表示
void I2CLCD::printChar(uint8_t chr)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    Wire.send(chr);
    Wire.endTransmission();
}

// 文字列表示
void I2CLCD::printStr(char *str)
{
    Wire.beginTransmission(I2CLCD_SLAVE_ADRS);
    for (int i = 0; i < strlen(str); i++ )
    {
        Wire.send(str[i]);
    }
    Wire.endTransmission();
}


I2CLCD I2clcd;
