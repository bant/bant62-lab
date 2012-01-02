//*****************************************************************************
// File Name    : main.c
//
// Title        : Tiny2313�ɂ��LCD I2C�X���[�u�f�o�C�X
// Revision     : 1.0
// Notes        :
// Target MCU   : Atmel AVR series ATtiny2313
// Tool Chain   : WinAVR-20100110
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2010/10/26   �΂��      �J���J�n
// 2010/10/28   �΂��      Ver0.1����
//*****************************************************************************

/* ****************************************************************************
   �s���A�T�C��
*******************************************************************************
                      ___ ___
                 RST# |  U  |Vcc
           UART....TX |     |PB7 ... SLCK�@
            "  ....RX |     |PB6 ... MISO
             �@  Xtl2 |  �@ |PB5 ... MOSI
                 Xtl1 |     |PB4 ... NC
       (LCD)DB4...PD2 |     |PB3 ... TIMER
          "   5...PD3 |     |PB2 ... JP
          "   6...PD4 |     |PB1 ... (LCD)RS
          "   7...PD5 |     |PB0 ...   "  R/W
                  GND |     |PD6 ...   "  E
                      ~~~~~~~
        LCD .... SC1602BS-B

--------------------------------------------------------------------------------
  I2C/UART �R�}���h�̐���
    ESC'L'<x><y>.... �J�[�\����x y �̈ʒu�Ɉړ�
    ESC'H'      .... �J�[�\�����z�[���|�W�V�����ֈړ�
    ESC'C'      .... LCD���N���A
    ESC'S'<n><d1><d2><d3><d4><d5><d6><d7><d8>
                .... EEPROM��n�Ԗڂ̈ʒu�փJ�X�^���t�H���g��o�^
    ESC'M'<n>   .... EEPROM��n�Ԗڂ̃J�X�^���t�H���g��LCD�ɓo�^

    ESC'X'      .... ��ʔ�\��
	ESC'N'      .... ��ʕ\���E�J�[�\������
	ESC'B'      .... ��ʕ\���E�J�[�\����\���E�����u�����N
    ESC'D'      .... ��ʕ\���E�J�[�\���\��
    ESC'E'      .... ��ʕ\���E�J�[�\���\���E�����u�����N

    ESC'-'      .... �J�[�\�����ړ�
	ESC'+'      .... �J�[�\���E�ړ�

	ESC'<'      .... ��ʍ��ړ�
	ESC'>'      .... ��ʉE�ړ�
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

// EEPROM �J�X�^���t�H���g�̏����f�[�^
custom_char_t eCustom[] __attribute__((section(".eeprom"))) =
{
    {{0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00}}, // 0. 0/5 full progress block
    {{0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00}}, // 1. 1/5 full progress block
    {{0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00}}, // 2. 2/5 full progress block
    {{0x00, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00}}, // 3. 3/5 full progress block
    {{0x00, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00}}, // 4. 4/5 full progress block
    {{0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00}}, // 5. 5/5 full progress block

	{{0x1f, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x1f, 0x00}},	// 6.  ��
	{{0x1f, 0x11, 0x1f, 0x11, 0x1f, 0x11, 0x11, 0x00}}, // 7.  ��
	{{0x04, 0x15, 0x15, 0x15, 0x04, 0x0a, 0x11, 0x00}}, // 8.  ��
	{{0x04, 0x04, 0x1d, 0x06, 0x0d, 0x14, 0x04, 0x00}}, // 9.  ��
	{{0x04, 0x04, 0x1f, 0x04, 0x0e, 0x15, 0x04, 0x00}}, // 10. ��
	{{0x04, 0x0a, 0x1f, 0x04, 0x1f, 0x0e, 0x1f, 0x00}}, // 11. ��
	{{0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x1f, 0x00}}, // 12. �y
	{{0x08, 0x0f, 0x02, 0x0f, 0x0a, 0x1f, 0x02, 0x00}}, // 13. �N

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

    // EEPROM ���f�[�^�����o���B
    eeprom_busy_wait();
    eeprom_read_block(&rCustom, (void *)(sizeof(custom_char_t)*romCharNum),sizeof(custom_char_t));
    eeprom_busy_wait();

    // set CG RAM �A�h���X�ݒ�
    lcd_command((1<<LCD_CGRAM) | (lcdCharNum << 3));
    for (i=0; i<8; i++)
    {
        lcd_data(rCustom[i]);
    }
}

void lcdSaveCustomChar(uint8_t romCharNum)
{
    // EEPROM �Ƀf�[�^�������o���B
    eeprom_busy_wait();
    eeprom_write_block(&rCustom, (void *)(sizeof(custom_char_t)*romCharNum), sizeof(custom_char_t));
    eeprom_busy_wait();

    // �������݂Ɏ��Ԃ��K�v�Ȃ̂����A�E�F�C�g�����
//	_delay_ms(10000);
}

/*-----------------------------------------
 *
 * ���샂�[�h�ݒ�W�����p�̊m�F
 *
 *-----------------------------------------*/
uint8_t check_I2C(void)
{
    PORTB |= (1 << PB2);       // �|�[�gB2�̃v���A�b�v
    _delay_us(1000);

    // �|�[�gB2����͂ɐݒ�
    DDRB &= ~(1 << PB2);

    return PINB & (1<<PB2);
}

/*-----------------------------------------
 *
 * TIMER1 ������(PWM�ݒ�)
 *
 *-----------------------------------------*/
void init_timer1(void)
{
	DDRB |= (1<<PB3);

	TCCR1A = (1<<COM1A0);				// ��r��v�Ńg�O��
	TCCR1B = (1<<WGM12) | (1<<CS10);	// ��r��v�^�C�}�[�A�J�E���^�N���A.

										//  TIMER1 �̎�����ݒ肷��.
	OCR0A = 39;							//  (8MHz / 2) / (39+1) = 100KHz

}

//----- Begin Code ------------------------------------------------------------
int main(void)
{
    static uint8_t f_i2c;
    static bool f_recieve;
    static uint8_t rbuf[8], *p, temp;
    static uint8_t state = S_NORMAL;


	// TIMER1�̐ݒ�
	init_timer1();

    f_i2c = check_I2C();
    if (f_i2c)
    {
        // I2C �X���[�u������
        usiTwiSlaveInit( TWI_slaveAddress );
    }
    else
    {
        // 2313�W��UART�̏�����
        uart2313_init();
    }

    //  LCD������
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

                case 'H': // �z�[���|�W�V�����փJ�[�\���ړ�
                    lcd_home();
                    state = S_NORMAL;
                    break;

                case 'C': // ��ʃN���A
                    lcd_clrscr();
                    state = S_NORMAL;
                    break;

                case 'S':  // S for Save Custom Character to EEPROM
                    state = S_S1;
                    break;

                case 'M': // M for Custom Character Mapping
                    state = S_M1;
                    break;

                case 'X': // ��ʔ�\��
                    /* display off                            */
                    lcd_command(LCD_DISP_OFF);
                    state = S_NORMAL;
                    break;

                case 'N': // ��ʕ\���E�J�[�\������
                    /* display on, cursor off                 */
                    lcd_command(LCD_DISP_ON);
                    state = S_NORMAL;
                    break;

                case 'B': // ��ʕ\���E�J�[�\����\���E�u�����N����
                    /* display on, cursor off, blink char     */
                    lcd_command(LCD_DISP_ON_BLINK);
                    state = S_NORMAL;
                    break;

                case 'D': // ��ʕ\���E�J�[�\���\��
                    /* display on, cursor on                  */
                    lcd_command(LCD_DISP_ON_CURSOR);
                    state = S_NORMAL;
                    break;

                case 'E': // ��ʕ\���E�J�[�\���\���E�u�����N����
                    /* display on, cursor on, blink char      */
                    lcd_command(LCD_DISP_ON_CURSOR_BLINK);
                    state = S_NORMAL;
                    break;

                case '-':  // �J�[�\�����ړ�
                    /* move cursor left  (decrement)          */
                    lcd_command(LCD_MOVE_CURSOR_LEFT);
                    state = S_NORMAL;
                    break;
                case '+':  // �J�[�\���E�ړ�
                    /* move cursor right (increment)          */
                    lcd_command(LCD_MOVE_CURSOR_RIGHT);
                    state = S_NORMAL;
                    break;

                case '<': // ��ʍ��ړ�
                    /* shift display left                     */
                    lcd_command(LCD_MOVE_DISP_LEFT);
                    state = S_NORMAL;
                    break;
                case '>': // ��ʉE�ړ�
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
