#ifndef I2CLCD_h
#define I2CLCD_h

#include <inttypes.h>
#include <Wire.h>

#define ESC (0x1B)
#define I2CLCD_SLAVE_ADRS (0x64 >> 1)

class I2CLCD : public TwoWire
{
private:
    int address;

public:
    I2CLCD(void);
    I2CLCD(int set_address);
    ~I2CLCD(void);     // destructor
    void gotoCursor(uint8_t x, uint8_t y);
    void home(void);
    void clear(void);
    void saveCustomCharacter(uint8_t romCharNum, uint8_t lcdCharData[]);
    void mapCustomCharacter(uint8_t romCharNum, uint8_t lcdCharNum);
    void offDisplay(void);
    void onDisplay(void);
    void blinkCharacter(void);
    void dispCursor(void);
    void blinkCursor(void);
    void hideCursor(void);
    void moveLeftCursor(void);
    void moveRightCursor(void);
    void moveLeftDisplay(void);
    void moveRightDisplay(void);
    void printChar(uint8_t chr);
    void printStr(char *str);
};

#endif
