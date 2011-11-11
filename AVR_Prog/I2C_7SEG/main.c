//=============================================================================
// File Name    : main.c
//
// Title        : Tiny2313によるI2C 7SEG LEDスレーブデバイスドライバ
// Revision     : 0.1
// Notes        :
// Target MCU   : Atmel AVR series ATtiny2313
// Tool Chain   : WinAVR-20100110
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2011/08/26   ばんと      開発開始
// 2011/08/28   ばんと      Ver0.1完了
//=============================================================================

/* ****************************************************************************
   ピンアサイン
*******************************************************************************
                      ___ ___
                 RST# |  U  |Vcc
           UART....TX |     |PB7 ... SLCK
            "  ....RX |     |PB6 ... MISO
             　  Xtl2 |  　 |PB5 ... MOSI
                 Xtl1 |     |PB4 ... JP
          LINE1...PD2 |     |PB3 ... LINE9
           "  2...PD3 |     |PB2 ...  "  8
           "  3...PD4 |     |PB1 ...  "  7
           "  4...PD5 |     |PB0 ...  "  6
                  GND |     |PD6 ...  "  5
                      ~~~~~~~
--------------------------------------------------------------------------------
  I2C/UART コマンドの説明

  ■特殊コマンド

    ESC'L'<x><c>.... xの位置に<c>の文字を書く
    ESC'C'      .... 表示クリア
    ESC'<'      .... 画面左移動(空白で埋める)
    ESC'>'      .... 画面右移動(空白で埋める)
    ESC'['      .... 画面左移動(文字をローテート)
    ESC']'      .... 画面右移動(文字をローテート)

  ■文字表示

    0～9 a～z(A～Z) を送信することにより表示、ただし.をつけるとひとつ前
    の文字に.がつく

-------------------------------------------------------------------------------*/

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>     // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>  // include interrupt support
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "global.h"     // include our global settings
#include "UART2313.h"
#include "usiTwiSlave.h"

#define ESC       (0x1B)

#define CHARLIE_1_D   PD2
#define CHARLIE_2_D   PD3
#define CHARLIE_3_D   PD4
#define CHARLIE_4_D   PD5
#define CHARLIE_5_D   PD6
#define CHARLIE_6_B   PB0
#define CHARLIE_7_B   PB1
#define CHARLIE_8_B   PB2
#define CHARLIE_9_B   PB3

#define NONE      0
#define HIGH      1

#define LEFT      0
#define RIGHT       1

#define NUMBER_START  0
#define ALFABET_START   10
#define HYPHEN      37
#define BLANK       53

#define LINE_1      1
#define LINE_2      2
#define LINE_3      3
#define LINE_4      4
#define LINE_5      5
#define LINE_6      6
#define LINE_7      7
#define LINE_8      8
#define LINE_9      9

#define LINE_SIZE   9

#define NUMBER_SATRT  0
#define ALFABET_START   10
#define HYPHEN      37
#define BLANK       53

enum { S_NORMAL = 0, S_SEQUENCE };
enum { S_L0 = 10, S_L1, S_L2 };

/*****************************************
 *            グローバル変数定義         *
 *****************************************/
const uint8_t eLINE[] __attribute__ ((progmem)) =
{
    LINE_1, LINE_2, LINE_3, LINE_4, LINE_5, LINE_6, LINE_7, LINE_8, LINE_9,  // 0 LINE8
    LINE_2, LINE_1, LINE_3, LINE_4, LINE_5, LINE_6, LINE_7, LINE_8, LINE_9,  // 1 LINE7
    LINE_3, LINE_2, LINE_1, LINE_4, LINE_5, LINE_6, LINE_7, LINE_8, LINE_9,  // 2 LINE6
    LINE_4, LINE_2, LINE_3, LINE_1, LINE_5, LINE_6, LINE_7, LINE_8, LINE_9,  // 3 LINE5
    LINE_5, LINE_2, LINE_3, LINE_4, LINE_1, LINE_6, LINE_7, LINE_8, LINE_9,  // 4 LINE4
    LINE_6, LINE_2, LINE_3, LINE_4, LINE_5, LINE_1, LINE_7, LINE_8, LINE_9,  // 5 LINE3
    LINE_7, LINE_2, LINE_3, LINE_4, LINE_5, LINE_6, LINE_1, LINE_8, LINE_9,  // 6 LINE2
    LINE_8, LINE_2, LINE_3, LINE_4, LINE_5, LINE_6, LINE_7, LINE_1, LINE_9   // 7 LINE1
};

const uint8_t eSEG[] __attribute__ ((progmem))  =
{
//    .gfedcba
    0b00111111, //  0:0   => NUMBER_START : 0
    0b00000110, //  1:1
    0b01011011, //  2:2
    0b01001111, //  3:3
    0b01100110, //  4:4
    0b01101101, //  5:5
    0b01111101, //  6:6
    0b00000111, //  7:7
    0b01111111, //  8:8
    0b01101111, //  9:9
    //                    ----------------------------------------
    0b01011111, // 10:a   => ALFABET_START : 10
    0b01111100, // 11:b
    0b01011000, // 12:c
    0b01011110, // 13:d
    0b01111011, // 14:e
    0b01110001, // 15:f
    0b00111101, // 16:g
    0b01110100, // 17:h
    0b00000100, // 18:i
    0b00011110, // 19:j
    0b01110101, // 20:k
    0b00111000, // 21:l
    0b00110111, // 22:m
    0b01010100, // 23:n
    0b01011100, // 24:o
    0b01110011, // 25:p
    0b01100111, // 26:q
    0b01010000, // 27:r
    0b01101100, // 28:s
    0b01111000, // 29:t
    0b00011100, // 30:u
    0b00111110, // 31:v
    0b01111110, // 32:w
    0b01110110, // 33:x
    0b01100110, // 34:y
    0b00011011, // 35:z
    //            --------------------------------------------
    0b01100011, // 36:ﾛ
    0b01000000, // 37:-
    0b01000110, // 38:|-
    0b01110000, // 39:-|
    0b01010010, // 40:
    0b01100100, // 41:

    0b00000001, // 42:~     ぐるぐる
    0b00000010, // 43:
    0b00000100, // 44:
    0b00001000, // 45:
    0b00010000, // 46:
    0b00100000, // 47:

    0b10000000, // 48:

    0b00001111, // 49:[
    0b00111001, // 50:]
    0b01000001, // 51:
    0b01001000, // 52:
    0b00000000  // 53:  空白
};

static const uint8_t TWI_slaveAddress = 0x33;

uint8_t count;
uint8_t segment;
uint8_t set_digit[8];

/*****************************************
 *  ポートの初期化                       *
 *****************************************/
void InitPort ( void )
{
    // ポートの初期化
    // set ports direction to 'input'
    DDRD &= ~(
                (1<<CHARLIE_1_D) |
                (1<<CHARLIE_2_D) |
                (1<<CHARLIE_3_D) |
                (1<<CHARLIE_4_D) |
                (1<<CHARLIE_5_D)
            );
    DDRB &= ~(
                (1<<CHARLIE_6_B) |
                (1<<CHARLIE_7_B) |
                (1<<CHARLIE_8_B) |
                (1<<CHARLIE_9_B)
            );

    // set ports value to 'LOW'
    PORTD &= ~(
                 (1<<CHARLIE_1_D) |
                 (1<<CHARLIE_2_D) |
                 (1<<CHARLIE_3_D) |
                 (1<<CHARLIE_4_D) |
                 (1<<CHARLIE_5_D)
             );
    PORTB &= ~(
                 (1<<CHARLIE_6_B) |
                 (1<<CHARLIE_7_B) |
                 (1<<CHARLIE_8_B) |
                 (1<<CHARLIE_9_B)
             );
}

/*****************************************
 *  7SEG LED出力ライン設定               *
 *****************************************/
void SetLine( uint8_t line, uint8_t mode )
{
    switch (line)
    {
    case LINE_1:
        if (mode == HIGH)
        {
            PORTD |= (1<<CHARLIE_1_D);
        }
        DDRD |= (1<<CHARLIE_1_D);
        break;

    case LINE_2:
        if (mode == HIGH)
        {
            PORTD |= (1<<CHARLIE_2_D);
        }
        DDRD |= (1<<CHARLIE_2_D);
        break;

    case LINE_3:
        if (mode == HIGH)
        {
            PORTD |= (1<<CHARLIE_3_D);
        }
        DDRD |= (1<<CHARLIE_3_D);
        break;

    case LINE_4:
        if (mode == HIGH)
        {
            PORTD |= (1<<CHARLIE_4_D);
        }
        DDRD |= (1<<CHARLIE_4_D);
        break;

    case LINE_5:
        if (mode == HIGH)
        {
            PORTD |= (1<<CHARLIE_5_D);
        }
        DDRD |= (1<<CHARLIE_5_D);
        break;

    case LINE_6:
        if (mode == HIGH)
        {
            PORTB |= (1<<CHARLIE_6_B);
        }
        DDRB |= (1<<CHARLIE_6_B);
        break;

    case LINE_7:
        if (mode == HIGH)
        {
            PORTB |= (1<<CHARLIE_7_B);
        }
        DDRB |= (1<<CHARLIE_7_B);
        break;

    case LINE_8:
        if (mode == HIGH)
        {
            PORTB |= (1<<CHARLIE_8_B);
        }
        DDRB |= (1<<CHARLIE_8_B);
        break;

    case LINE_9:
        if (mode == HIGH)
        {
            // high
            PORTB |= (1<<CHARLIE_9_B);
        }
        DDRB |= (1<<CHARLIE_9_B);
        break;

    default:
        break;
    }
}

/*****************************************
 *  7セグデータ出力                      *
 *****************************************/
void DigitOut( uint8_t digit,  uint8_t data )
{
    volatile int i;
    uint8_t * pos;
    bool dot_flag;

    pos = (uint8_t *)(eLINE + LINE_SIZE * digit);
    SetLine( pgm_read_byte(pos), HIGH );

    dot_flag = false;
    if (data & 0x80)
    {
        dot_flag = true;
        data &= 0x7F;
    }

    if ( data > 52 )
    {
        data = 53;
    }
    segment = pgm_read_byte(eSEG + data);

    if (dot_flag)
    {
        segment |= 0b10000000;
    }

    pos += 8;
    for ( i=0; i <8; i++)
    {
        if ( segment & (0b10000000 >> i))
        {
            SetLine(pgm_read_byte( pos - i ), NONE );
        }
    }
}

/*****************************************
 *  動作モード設定ジャンパの確認         *
 *****************************************/
bool check_I2C(void)
{
    PORTB |= (1 << PB4);       // ポートB4のプルアップ
    _delay_us(1000);

    // ポートB4を入力に設定
    DDRB &= ~(1 << PB4);

    return (PINB & (1<<PB4));
}

/*****************************************
 *  ７セグ書き込み(書き込み位置付き)     *
 *****************************************/
void led_write(uint8_t pos, uint8_t data)
{
    if (data == '.')
    {
        set_digit[pos] |= 0x80;
    }
    else
    {
        if (data >= '0' && data <= '9')
        {
            set_digit[pos] = NUMBER_START + data - '0';
        }
        else if (data >= 'a' && data <= 'z')
        {
            set_digit[pos] = ALFABET_START + data - 'a';
        }
        else if (data >= 'A' && data <= 'Z')
        {
            set_digit[pos] = ALFABET_START + data - 'A';
        }
        else if (data == '-')
        {
            set_digit[pos] = HYPHEN;
        }
        else if (data == ' ')
        {
            set_digit[pos] = BLANK;
        }
    }
}

/*****************************************
 *  ７セグ一文字書き込み                 *
 *****************************************/
void led_putc(uint8_t data)
{
    uint8_t i;

    if ((data >= '0' && data <= '9')  ||
        (data >= 'a' && data <= 'z')  ||
        (data >= 'A' && data <= 'Z')  ||
        (data == '-') || (data == ' ')  ||
        (data == '.')
    )
    {
        if (data != '.')
        {

            for (i=0; i<7; i++)
            {
                set_digit[i] = set_digit[i+1];
            }
        }
        led_write(7, data);
    }
}

/*****************************************
 *  ７セグ画面クリア                     *
 *****************************************/
void led_clr(void)
{
    uint8_t i;

    for ( i=0; i<8; i++ )
    {
        set_digit[i] = BLANK;
    }
}

/*****************************************
 *  ７セグ画面シフト                     *
 *****************************************/
void led_shift(uint8_t mode)
{
    uint8_t i;

    if ( mode == LEFT )
    {
        for ( i=0; i<7; i++ )
        {
            set_digit[i] = set_digit[i+1];
        }
        set_digit[7] = BLANK;
    }
    else
    {
        for ( i=7; i>0; i-- )
        {
            set_digit[i] = set_digit[i-1];
        }
        set_digit[0] = BLANK;
    }
}

/*****************************************
 *  ７セグ画面ローテート                 *
 *****************************************/
void led_rotate(uint8_t mode)
{
    uint8_t i, tmp;

    if ( mode == LEFT )
    {
        tmp = set_digit[0];

        for ( i=0; i<7; i++ )
        {
            set_digit[i] = set_digit[i+1];
        }
        set_digit[7] = tmp;
    }
    else
    {
        tmp = set_digit[7];

        for ( i=7; i>0; i-- )
        {
            set_digit[i] = set_digit[i-1];
        }
        set_digit[0] = tmp;
    }
}

/*****************************************
 *  タイマ0オーバーフロー割り込み処理    *
 *****************************************/
ISR(TIMER0_OVF_vect)
{
    InitPort();

    DigitOut( count, set_digit[ count ] );

    if ( ++count > 7 )
    {
        count = 0;
    }
}

/*****************************************
 *  メイン                               *
 *****************************************/
int main( void )
{
    volatile int i,j;
    static uint8_t pos;
    static bool f_recieve, f_i2c;
    static uint8_t rbuf[8], *p, temp;
    static uint8_t state = S_NORMAL;

    // 定数の初期化
    count = 0;
    led_clr();

    // ボートの初期化
    InitPort();

    // タイマー0設定
    TCCR0B = (1<<CS00)|(1<<CS01); // 64分周
    TIMSK |= (1<<TOIE0);      // タイマ0のオーバーフロー割り込み許可
    TCNT0 = 0xFF;

    f_i2c = check_I2C();
    if ( f_i2c )
    {
        // I2C スレーブ初期化
        usiTwiSlaveInit( TWI_slaveAddress );
    }
    else
    {
        // 2313標準UARTの初期化
        uart2313_init();
    }

    sei();

    // opening animation
    for ( i = 0; i < 8; i++ )
    {
        for ( j = 42; j < 48; j++ )
        {
            set_digit[i] = j;
            _delay_ms(30);
        }
        set_digit[i] = 0;
    }

    _delay_ms(100);
    led_clr();
    if ( f_i2c )
    {
        set_digit[7] = 49;
        set_digit[6] = 1;
        set_digit[5] = 50;
    }
    else
    {
        set_digit[7] = 49;
        set_digit[6] = 5;
        set_digit[5] = 50;
    }

    while (1)
    {
        sleep_mode();

        if ( f_i2c )
        {
            f_recieve = usiTwiDataInReceiveBuffer();
            if ( f_recieve )
            {
                temp = usiTwiReceiveByte();
            }
        }
        else
        {
            f_recieve = xreadOK();
            if ( f_recieve )
            {
                temp = xread();
            }
        }

        if ( f_recieve )
        {
            switch ( state )
            {
            case S_NORMAL:
                if (temp == ESC)
                {
                    state = S_SEQUENCE;
                }
                else
                {
                    led_putc(temp);
                }
                break;

            case S_SEQUENCE:
                p = rbuf;
                *p = temp;
                switch (temp)
                {
                case 'L': // L for Locate (X)
                    state = S_L1;
                    break;

                case 'C': // 画面クリア
                    led_clr();
                    state = S_NORMAL;
                    break;

                case '<': // 画面左シフト
                    led_shift(LEFT);
                    state = S_NORMAL;
                    break;

                case '>': // 画面右シフト
                    led_shift(RIGHT);
                    state = S_NORMAL;
                    break;

                case '[': // 画面左ローテート
                    led_rotate(LEFT);
                    state = S_NORMAL;
                    break;

                case ']': // 画面右ローテート
                    led_rotate(RIGHT);
                    state = S_NORMAL;
                    break;

                default:
                    state = S_NORMAL;
                    break;
                }
                break;

            case S_L1:
                *++p = temp;
                state = S_L2;
                break;

            case S_L2:
                pos = 7 - (rbuf[1] - '0');
                led_write(pos, temp);
                state = S_NORMAL;
                break;

            default:
                break;
            }
        }
    }
    return 0;
}
