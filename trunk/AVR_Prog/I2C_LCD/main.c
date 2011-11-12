//*****************************************************************************
// File Name    : main.c
//
// Title        : Tiny2313によるLCD I2Cスレーブデバイス
// Revision     : 1.0
// Notes        :
// Target MCU   : Atmel AVR series ATtiny2313
// Tool Chain   : WinAVR-20100110
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2010/10/26   ばんと      開発開始
// 2010/10/28   ばんと      Ver0.1完了
//*****************************************************************************

/* ****************************************************************************
   ピンアサイン
*******************************************************************************
                      ___ ___
                 RST# |  U  |Vcc
           UART....TX |     |PB7 ... SLCK　
            "  ....RX |     |PB6 ... MISO
             　  Xtl2 |  　 |PB5 ... MOSI
                 Xtl1 |     |PB4 ... NC
       (LCD)DB4...PD2 |     |PB3 ... TIMER
          "   5...PD3 |     |PB2 ... JP
          "   6...PD4 |     |PB1 ... (LCD)RS
          "   7...PD5 |     |PB0 ...   "  R/W
                  GND |     |PD6 ...   "  E
                      ~~~~~~~
        LCD .... SC1602BS-B

--------------------------------------------------------------------------------
  I2C/UART コマンドの説明
    ESC'L'<x><y>.... カーソルをx y の位置に移動
    ESC'H'      .... カーソルをホームポジションへ移動
    ESC'C'      .... LCDをクリア
    ESC'S'<n><d1><d2><d3><d4><d5><d6><d7><d8>
                .... EEPROMのn番目の位置へカスタムフォントを登録
    ESC'M'<n>   .... EEPROMのn番目のカスタムフォントをLCDに登録

    ESC'X'      .... 画面非表示
	ESC'N'      .... 画面表示・カーソル消去
	ESC'B'      .... 画面表示・カーソル非表示・文字ブリンク
    ESC'D'      .... 画面表示・カーソル表示
    ESC'E'      .... 画面表示・カーソル表示・文字ブリンク

    ESC'-'      .... カーソル左移動
	ESC'+'      .... カーソル右移動

	ESC'<'      .... 画面左移動
	ESC'>'      .... 画面右移動
-------------------------------------------------------------------------------*/


//----- Include Files ---------------------------------------------------------
#include <avr/io.h>     // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>  // include interrupt support
#include <avr/eeprom.h>
#include <util/delay.h>


#include "global.h"     // include our global settings
#include "lcd.h"
#include "UART2313.h"
#include "usiTwiSlave.h"


#define ESC (0x1B)
#define FONT_SIZE 8

static const uint8_t TWI_slaveAddress = 0x32;

enum { S_NORMAL = 0, S_SEQUENCE };
enum { S_L0 = 10, S_L1, S_L2 };
enum { S_M0 = 20, S_M1, S_M2 };
enum { S_S0 = 30, S_S1, S_S2, S_S3, S_S4, S_S5, S_S6, S_S7, S_S8, S_S9 };

typedef struct
{
    unsigned char font[FONT_SIZE];
} custom_char_t;

// EEPROM カスタムフォントの初期データ
custom_char_t eCustom[] __attribute__((section(".eeprom"))) =
{
    {{0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00}}, // 0. 0/5 full progress block
    {{0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00}}, // 1. 1/5 full progress block
    {{0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00}}, // 2. 2/5 full progress block
    {{0x00, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00}}, // 3. 3/5 full progress block
    {{0x00, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00}}, // 4. 4/5 full progress block
    {{0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00}}, // 5. 5/5 full progress block

	{{0x1f, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x1f, 0x00}},	// 6.  日
	{{0x1f, 0x11, 0x1f, 0x11, 0x1f, 0x11, 0x11, 0x00}}, // 7.  月
	{{0x04, 0x15, 0x15, 0x15, 0x04, 0x0a, 0x11, 0x00}}, // 8.  火
	{{0x04, 0x04, 0x1d, 0x06, 0x0d, 0x14, 0x04, 0x00}}, // 9.  水
	{{0x04, 0x04, 0x1f, 0x04, 0x0e, 0x15, 0x04, 0x00}}, // 10. 木
	{{0x04, 0x0a, 0x1f, 0x04, 0x1f, 0x0e, 0x1f, 0x00}}, // 11. 金
	{{0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x1f, 0x00}}, // 12. 土
	{{0x08, 0x0f, 0x02, 0x0f, 0x0a, 0x1f, 0x02, 0x00}}, // 13. 年

    {{0x00, 0x0E, 0x15, 0x15, 0x15, 0x0E, 0x00, 0x00}}, // 14. animated play icon frame 1
    {{0x00, 0x0E, 0x13, 0x15, 0x19, 0x0E, 0x00, 0x00}}  // 15. animated play icon frame 2

/*
    {{0x03, 0x07, 0x0F, 0x1F, 0x0F, 0x07, 0x03, 0x00}}, // 6. rewind arrow
    {{0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00}}, // 7. stop block
    {{0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x00}}, // 8. pause bars
    {{0x18, 0x1C, 0x1E, 0x1F, 0x1E, 0x1C, 0x18, 0x00}}, // 9. fast-forward arrow
    {{0x00, 0x04, 0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x00}}, // 10. scroll up arrow
    {{0x00, 0x1F, 0x1F, 0x0E, 0x0E, 0x04, 0x04, 0x00}}, // 11. scroll down arrow
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // 12. blank character
    {{0x00, 0x0E, 0x19, 0x15, 0x13, 0x0E, 0x00, 0x00}}, // 13. animated play icon frame 0
    {{0x00, 0x0E, 0x15, 0x15, 0x15, 0x0E, 0x00, 0x00}}, // 14. animated play icon frame 1
    {{0x00, 0x0E, 0x13, 0x15, 0x19, 0x0E, 0x00, 0x00}}  // 15. animated play icon frame 2
*/
};

unsigned char rCustom[FONT_SIZE];


void lcdMapCustomChar(uint8_t romCharNum, uint8_t lcdCharNum)
{
    register uint8_t i;

    // EEPROM よりデータを取り出す。
    eeprom_busy_wait();
    eeprom_read_block(&rCustom, (void *)(sizeof(custom_char_t)*romCharNum),sizeof(custom_char_t));
    eeprom_busy_wait();

    // set CG RAM アドレス設定
    lcd_command((1<<LCD_CGRAM) | (lcdCharNum << 3));
    for (i=0; i<8; i++)
    {
        lcd_data(rCustom[i]);
    }
}

void lcdSaveCustomChar(uint8_t romCharNum)
{
    // EEPROM にデータを書き出す。
    eeprom_busy_wait();
    eeprom_write_block(&rCustom, (void *)(sizeof(custom_char_t)*romCharNum), sizeof(custom_char_t));
    eeprom_busy_wait();

    // 書き込みに時間が必要なのかも、ウェイト入れる
//	_delay_ms(10000);
}

/*-----------------------------------------
 *
 * 動作モード設定ジャンパの確認
 *
 *-----------------------------------------*/
uint8_t check_I2C(void)
{
    PORTB |= (1 << PB2);       // ポートB2のプルアップ
    _delay_us(1000);

    // ポートB2を入力に設定
    DDRB &= ~(1 << PB2);

    return PINB & (1<<PB2);
}

/*-----------------------------------------
 *
 * TIMER1 初期化(PWM設定)
 *
 *-----------------------------------------*/
void init_timer1(void)
{
	DDRB |= (1<<PB3);

	TCCR1A = (1<<COM1A0);				// 比較一致でトグル
	TCCR1B = (1<<WGM12) | (1<<CS10);	// 比較一致タイマー、カウンタクリア.

										//  TIMER1 の周期を設定する.
	OCR0A = 39;							//  (8MHz / 2) / (39+1) = 100KHz

}

//----- Begin Code ------------------------------------------------------------
int main(void)
{
    static uint8_t f_i2c;
    static bool f_recieve;
    static uint8_t rbuf[8], *p, temp;
    static uint8_t state = S_NORMAL;


	// TIMER1の設定
	init_timer1();

    f_i2c = check_I2C();
    if (f_i2c)
    {
        // I2C スレーブ初期化
        usiTwiSlaveInit( TWI_slaveAddress );
    }
    else
    {
        // 2313標準UARTの初期化
        uart2313_init();
    }

    //  LCD初期化
    lcd_init(LCD_DISP_ON);
    lcdMapCustomChar(0, 0);
    lcdMapCustomChar(1, 1);
    lcdMapCustomChar(2, 2);
    lcdMapCustomChar(3, 3);
    lcdMapCustomChar(4, 4);
    lcdMapCustomChar(5, 5);
    lcdMapCustomChar(6, 6);
    lcdMapCustomChar(7, 7);
    lcd_gotoxy(0, 0);

    // any logos?
    // write something

    lcd_puts_P("I2CLCD");
    if (f_i2c)
    {             //1234567890123456
        lcd_puts_P("-I");
    }
    else
    {             //1234567890123456
        lcd_puts_P("-S");
    }

    sei();

    while (1)
    {
        if (f_i2c)
        {
            f_recieve = usiTwiDataInReceiveBuffer();
            if (f_recieve)
            {
                temp = usiTwiReceiveByte();
            }
        }
        else
        {
            f_recieve = xreadOK();
            if (f_recieve)
            {
                temp = xread();
            }

        }

        if (f_recieve)
        {
            switch (state)
            {
            case S_NORMAL:
                if (temp == ESC)
                {
                    state = S_SEQUENCE;
                }
                else
                {
                    lcd_putc(temp);
                }
                break;
            case S_SEQUENCE:
                p = rbuf;
                *p = temp;
                switch (temp)
                {
                case 'L': // L for Locate (X), (Y)
                    state = S_L1;
                    break;

                case 'H': // ホームポジションへカーソル移動
                    lcd_home();
                    state = S_NORMAL;
                    break;

                case 'C': // 画面クリア
                    lcd_clrscr();
                    state = S_NORMAL;
                    break;

                case 'S':  // S for Save Custom Character to EEPROM
                    state = S_S1;
                    break;

                case 'M': // M for Custom Character Mapping
                    state = S_M1;
                    break;

                case 'X': // 画面非表示
                    /* display off                            */
                    lcd_command(LCD_DISP_OFF);
                    state = S_NORMAL;
                    break;

                case 'N': // 画面表示・カーソル消去
                    /* display on, cursor off                 */
                    lcd_command(LCD_DISP_ON);
                    state = S_NORMAL;
                    break;

                case 'B': // 画面表示・カーソル非表示・ブリンク文字
                    /* display on, cursor off, blink char     */
                    lcd_command(LCD_DISP_ON_BLINK);
                    state = S_NORMAL;
                    break;

                case 'D': // 画面表示・カーソル表示
                    /* display on, cursor on                  */
                    lcd_command(LCD_DISP_ON_CURSOR);
                    state = S_NORMAL;
                    break;

                case 'E': // 画面表示・カーソル表示・ブリンク文字
                    /* display on, cursor on, blink char      */
                    lcd_command(LCD_DISP_ON_CURSOR_BLINK);
                    state = S_NORMAL;
                    break;

                case '-':  // カーソル左移動
                    /* move cursor left  (decrement)          */
                    lcd_command(LCD_MOVE_CURSOR_LEFT);
                    state = S_NORMAL;
                    break;
                case '+':  // カーソル右移動
                    /* move cursor right (increment)          */
                    lcd_command(LCD_MOVE_CURSOR_RIGHT);
                    state = S_NORMAL;
                    break;

                case '<': // 画面左移動
                    /* shift display left                     */
                    lcd_command(LCD_MOVE_DISP_LEFT);
                    state = S_NORMAL;
                    break;
                case '>': // 画面右移動
                    /* shift display right                    */
                    lcd_command(LCD_MOVE_DISP_RIGHT);
                    state = S_NORMAL;
                    break;

                }
                break;

            case S_L1:
                *++p = temp;
                state = S_L2;
                break;
            case S_L2:
//                *++p = temp;
                lcd_gotoxy(rbuf[1], temp);
                state = S_NORMAL;
                break;
            case S_M1:
                *++p = temp;
                state = S_M2;
                break;
            case S_M2:
//              *++p = temp;
                lcdMapCustomChar(rbuf[1],temp);
                state = S_NORMAL;
                break;

                // -----------------------------

            case S_S1:
                *++p = temp;
                state = S_S2;
                break;
            case S_S2:
                rCustom[0] = temp;
//                *++p = temp;
                state = S_S3;
                break;
            case S_S3:
                rCustom[1] = temp;
//                *++p = temp;
                state = S_S4;
                break;
            case S_S4:
                rCustom[2] = temp;
//                *++p = temp;
                state = S_S5;
                break;
            case S_S5:
                rCustom[3] = temp;
//                *++p = temp;
                state = S_S6;
                break;
            case S_S6:
                rCustom[4] = temp;
//                *++p = temp;
                state = S_S7;
                break;
            case S_S7:
                rCustom[5] = temp;
//                *++p = temp;
                state = S_S8;
                break;
            case S_S8:
                rCustom[6] = temp;
//                *++p = temp;
                state = S_S9;
                break;
            case S_S9:
                rCustom[7] = temp;
//                *++p = temp;
                lcdSaveCustomChar(rbuf[1]);
                state = S_NORMAL;
                break;
            }
        }
    }

    return 0;
}
