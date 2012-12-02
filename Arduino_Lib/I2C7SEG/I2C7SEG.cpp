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

#include "I2C7SEG.h"

// Constructors ////////////////////////////////////////////////////////////////
I2C7SEG::I2C7SEG(void)
{
    address = I2C7SEG_SLAVE_ADRS;
}

I2C7SEG::I2C7SEG(int set_address)
{
    address = set_address;
}

// Destructor //////////////////////////////////////////////////////////////////
I2C7SEG::~I2C7SEG(void) 
{
  //  Serial.print("Deleted file I2C7SEG");
}

// Public Methods //////////////////////////////////////////////////////////////

void I2C7SEG::clear(void)
{
    beginTransmission(address);
    write(ESC);
    write('C');
    endTransmission();
}

void I2C7SEG::shift_left(void)
{
    beginTransmission(address);
    write(ESC);
    write('<');
    endTransmission();
}

void I2C7SEG::shift_right(void)
{
    beginTransmission(address);
    write(ESC);
    write('>');
    endTransmission();
}

void I2C7SEG::rotato_left(void)
{
    beginTransmission(address);
    write(ESC);
    write('[');
    endTransmission();
}

void I2C7SEG::rotato_right(void)
{
    beginTransmission(address);
    write(ESC);
    write(']');
    endTransmission();
}

// 一文字表示
void I2C7SEG::printChar(uint8_t chr)
{
    beginTransmission(address);
    write(chr);
    endTransmission();
}

// 文字列表示
void I2C7SEG::printStr(char *str)
{
    beginTransmission(address);
    for (int i = 0; i < strlen(str); i++ )
    {
        write(str[i]);
    }
    endTransmission();
}

// 一文字表示(位置付き)
void I2C7SEG::printPosChar(uint8_t pos, uint8_t chr)
{
    beginTransmission(address);
    write(ESC);
    write('[');
    write(pos);
    write(chr);
    endTransmission();
}
