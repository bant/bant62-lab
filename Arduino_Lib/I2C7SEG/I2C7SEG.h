#ifndef I2C7SEG_H
#define I2C7SEG_H

#include <inttypes.h>
#include <Wire.h>

#define ESC (0x1B)
#define I2C7SEG_SLAVE_ADRS (0x66 >> 1)

class I2C7SEG : public TwoWire
{
private:
    int address;

public:
    I2C7SEG();
    I2C7SEG(int address);
    ~I2C7SEG();     // destructor
    void clear(void);
    void shift_left(void);
    void shift_right(void);
    void rotato_left(void);
    void rotato_right(void);
    void printChar(uint8_t chr);
    void printStr(char *str);
    void printPosChar(uint8_t pos, uint8_t chr);
};

#endif
