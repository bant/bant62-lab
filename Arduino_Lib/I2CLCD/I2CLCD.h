#ifndef I2CLCD_h
#define I2CLCD_h

#include <inttypes.h>

#define ESC (0x1B)

class I2CLCD
{
private:

public:
    I2CLCD();
    void begin(void);
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

extern I2CLCD I2clcd;

#endif
