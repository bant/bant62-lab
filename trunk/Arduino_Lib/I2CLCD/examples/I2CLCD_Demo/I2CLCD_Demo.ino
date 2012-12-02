//===================================================================
// File Name    : I2CLCD_Demo.ino
//
// Title        : Arduino I2CLCD デモスケッチ
// Revision     : 0.1
// Notes        : I2CLCD におさるの顔を表示させるデモ
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2012/12/1    ばんと      作成
//==================================================================

#include <Wire.h>
#include <I2CLCD.h>

I2CLCD I2clcd;

//==================================================================
//  LCDの外字として登録する文字のビットマップ(データは、おさるの顔)
//==================================================================
uint8_t osamru_bitmap[6][8] =
{
    {   4,  3, 15,  7, 15, 18, 18, 14},             // おさる　その１
    {  31, 31, 31, 31,  3,  0, 12, 18},             // おさる　その２
    {   0, 24, 28, 30, 18,  2, 12, 18},             // おさる　その３
    {  30, 15, 30, 12,  6,  2,  1,  0},             // おさる　その４
    {  22, 12,  0, 17, 24, 30,  1, 30},             // おさる　その５
    {  22, 12,  4, 18,  2,  4, 24,  0}              // おさる　その６
};

void setup()
{
    I2clcd.begin();

    // LCD の外字を登録
    for (int i=0; i < 6;i++)
    {
        I2clcd.saveCustomCharacter(i, osamru_bitmap[i]);//
        I2clcd.mapCustomCharacter(i,i);
    }

    I2clcd.clear();
    I2clcd.gotoCursor(0, 0);

    I2clcd.printChar(0);            // おさる　その１
    I2clcd.printChar(1);            // おさる　その２
    I2clcd.printChar(2);            // おさる　その３
    I2clcd.printStr(">Monkee LCD<");
    I2clcd.gotoCursor(0, 1);
    I2clcd.printChar(3);            // おさる　その４
    I2clcd.printChar(4);            // おさる　その５
    I2clcd.printChar(5);            // おさる　その６

    I2clcd.printStr("[\265\273\331\300\336 \263\257\267\267!]"); // 8進表記すればカタカナも表示可

    I2clcd.gotoCursor(0, 0);
    I2clcd.hideCursor();

}
void loop()
{
}
