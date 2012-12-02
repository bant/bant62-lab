#include <Wire.h>
#include <I2C7SEG.h>

I2C7SEG I2c7seg;

void setup()
{
    I2c7seg.begin();
    I2c7seg.clear();
}

void loop()
{
    I2c7seg.printChar('1');
    delay(500);
    I2c7seg.printChar('2');
    delay(500);
    I2c7seg.printChar('3');
    delay(500);
    I2c7seg.printChar('4');
    delay(500);
    I2c7seg.printChar('5');
    delay(500);
    I2c7seg.printChar('6');
    delay(500);
    I2c7seg.printChar('7');
    delay(500);
    I2c7seg.printChar('8');
    delay(500);

    delay(500);
    I2c7seg.rotato_right();
    delay(500);
    I2c7seg.rotato_right();
    delay(500);
    I2c7seg.rotato_right();
    delay(500);
    I2c7seg.rotato_right();
    delay(500);
    I2c7seg.rotato_right();
    delay(500);
    I2c7seg.rotato_right();
    delay(500);
    I2c7seg.rotato_right();
    delay(500);
    I2c7seg.rotato_right();
    delay(500);

    I2c7seg.rotato_left();
    I2c7seg.printChar('.');
    delay(500);
    I2c7seg.rotato_left();
    I2c7seg.printChar('.');
    delay(500);
    I2c7seg.rotato_left();
    I2c7seg.printChar('.');
    delay(500);
    I2c7seg.rotato_left();
    I2c7seg.printChar('.');
    delay(500);
    I2c7seg.rotato_left();
    I2c7seg.printChar('.');
    delay(500);
    I2c7seg.rotato_left();
    I2c7seg.printChar('.');
    delay(500);
    I2c7seg.rotato_left();
    I2c7seg.printChar('.');
    delay(500);
    I2c7seg.rotato_left();
    I2c7seg.printChar('.');
    delay(500);

    I2c7seg.shift_right();
    delay(500);
    I2c7seg.shift_right();
    delay(500);
    I2c7seg.shift_right();
    delay(500);
    I2c7seg.shift_right();
    delay(500);
    I2c7seg.shift_right();
    delay(500);
    I2c7seg.shift_right();
    delay(500);
    I2c7seg.shift_right();
    delay(500);
    I2c7seg.shift_right();
    delay(500);

    I2c7seg.clear();

    I2c7seg.printChar('a');
    delay(500);
    I2c7seg.printChar('b');
    delay(500);
    I2c7seg.printChar('c');
    delay(500);
    I2c7seg.printChar('d');
    delay(500);
    I2c7seg.printChar('e');
    delay(500);
    I2c7seg.printChar('f');
    delay(500);
    I2c7seg.printChar('g');
    delay(500);
    I2c7seg.printChar('h');
    delay(500);
    I2c7seg.printChar('i');
    delay(500);
    I2c7seg.printChar('j');
    delay(500);
    I2c7seg.printChar('k');
    delay(500);
    I2c7seg.printChar('l');
    delay(500);
    I2c7seg.printChar('m');
    delay(500);
    I2c7seg.printChar('n');
    delay(500);
    I2c7seg.printChar('o');
    delay(500);
    I2c7seg.printChar('p');
    delay(500);
    I2c7seg.printChar('q');
    delay(500);
    I2c7seg.printChar('r');
    delay(500);
    I2c7seg.printChar('s');
    delay(500);
    I2c7seg.printChar('t');
    delay(500);
    I2c7seg.printChar('u');
    delay(500);
    I2c7seg.printChar('v');
    delay(500);
    I2c7seg.printChar('w');
    delay(500);
    I2c7seg.printChar('x');
    delay(500);
    I2c7seg.printChar('y');
    delay(500);
    I2c7seg.printChar('z');
    delay(500);

    I2c7seg.clear();
}
