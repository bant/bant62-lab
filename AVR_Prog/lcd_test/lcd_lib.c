//=======================================================
// File Name    : lcd_lib.c
//
// Title        : AVR LCD ���C�u�����B
// Revision     : 0.1
// Notes        :
// Target MCU   : Atmel AVR series
// Tool Chain   : 
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2012/02/19   �΂��      �J���J�n
// 2011/02/19   �΂��      Ver0.1����
//=======================================================


#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "lcd_lib.h"
#include "common.h"

/*======================================*/
/*  LCD�ڑ��|�[�g�̏�����               */
/*======================================*/
void lcd_init_port(void)
{
#ifdef USE_LOW_NIBBLE
    DATA_DDR |= 0x0F;
    DATA_PORT &= ~0x0F;
#else
    DATA_DDR |= 0xF0;
    DATA_PORT &= ~0xF0;
#endif
    CTRL_DDR |=(_BV(LCD_RS) | _BV(LCD_E));
    CTRL_PORT &= ~(_BV(LCD_RS) | _BV(LCD_E));
}

/*======================================*/
/*  LCD�f�[�^�o�͊֐�                   */
/*======================================*/
void lcd_out(uint8_t code,uint8_t rs)
{
#ifdef USE_LOW_NIBBLE
    DATA_PORT = (code & 0x0F) | (DATA_PORT & 0xF0);
#else
    DATA_PORT = (code & 0xF0) | (DATA_PORT & 0x0F);
#endif
    if(rs==RS_ON)
    {
        CTRL_PORT |= _BV(LCD_RS);
    }
    else
    {
        CTRL_PORT &= ~_BV(LCD_RS);
    }

    wait_ms(1);
    CTRL_PORT |= _BV(LCD_E);
    wait_ms(1);
    CTRL_PORT &= ~_BV(LCD_E);
}

/*======================================*/
/*  4�r�b�g�R�}���h�o�͊֐�             */
/*======================================*/
void lcd_cmd(uint8_t cmd)
{
#ifdef USE_LOW_NIBBLE
    lcd_out(cmd>>4,RS_OFF);
    lcd_out(cmd,RS_OFF);
#else
    lcd_out(cmd,RS_OFF);
    lcd_out(cmd<<4,RS_OFF);
#endif

    if(cmd & 0xFC)
    {
        wait_us(50);
    }
    else
    {
        wait_ms(2);
    }
}

/*======================================*/
/*  4�r�b�g�f�[�^�o�͊֐�               */
/*======================================*/
void lcd_data(uint8_t data)
{
#ifdef USE_LOW_NIBBLE
    lcd_out(data>>4,RS_ON);
    lcd_out(data,RS_ON);
#else
    lcd_out(data,RS_ON);
    lcd_out(data<<4,RS_ON);
#endif

    wait_us(50);
}

/*======================================*/
/*  4�r�b�g�f�[�^�o�͊֐�               */
/*======================================*/
void lcd_init(void)
{
    /* �|�[�g�̏����� */
    lcd_init_port();
#ifdef USE_LOW_NIBBLE
    #ifdef USE_ACM0802C
   /* ACM0802C */

    wait_ms(35);            // 30mS�ȏ�̃E�F�C�g(���ۂ�35ms)

    lcd_out(0x02,RS_OFF);   // Function set
    lcd_out(0x02,RS_OFF);   //
    lcd_out(0x08,RS_OFF);   //

    wait_us(50);            // 39us�ȏ�̃E�F�C�g(���ۂ�50us)

    lcd_out(0x00,RS_OFF);   // Display ON/OFF Command
    lcd_out(0x0C,RS_OFF);   //

    wait_us(50);            // 39us�ȏ�̃E�F�C�g(���ۂ�50us)

    lcd_out(0x00,RS_OFF);   // Display Clear
    lcd_out(0x01,RS_OFF);   //

    wait_ms(2);             // 1.53ms�ȏ�̃E�F�C�g(���ۂ�2ms)

    lcd_out(0x00,RS_OFF);   // Entry Mode Set
    lcd_out(0x06,RS_OFF);   //

    wait_ms(50);            //
    #else
    /* 1602 */
    wait_ms(15);            // 15ms�ȏ�̃E�F�C�g

    lcd_out(0x03,RS_OFF);   //
    wait_ms(5);             // 4.1ms�ȏ�̃E�F�C�g

    lcd_out(0x03,RS_OFF);   //
    wait_us(150);           // 100us�ȏ�̃E�F�C�g

    lcd_out(0x03,RS_OFF);   //
    wait_us(50);            // 40us�ȏ�̃E�F�C�g

    lcd_out(0x02,0);        // 4�r�b�g���[�h
    wait_us(100);

    lcd_cmd(0x28);
    lcd_cmd(0x08);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x02);
    lcd_cmd(0x01);
    #endif
#else
    #ifdef USE_ACM0802C
   /* ACM0802C */

    wait_ms(35);            // 30mS�ȏ�̃E�F�C�g(���ۂ�35ms)

    lcd_out(0x20,RS_OFF);   // Function set
    lcd_out(0x20,RS_OFF);   //
    lcd_out(0x80,RS_OFF);   //

    wait_us(50);            // 39us�ȏ�̃E�F�C�g(���ۂ�50us)

    lcd_out(0x00,RS_OFF);   // Display ON/OFF Command
    lcd_out(0xC0,RS_OFF);   //

    wait_us(50);            // 39us�ȏ�̃E�F�C�g(���ۂ�50us)

    lcd_out(0x00,RS_OFF);   // Display Clear
    lcd_out(0x10,RS_OFF);   //

    wait_ms(2);             // 1.53ms�ȏ�̃E�F�C�g(���ۂ�2ms)

    lcd_out(0x00,RS_OFF);   // Entry Mode Set
    lcd_out(0x60,RS_OFF);   //

    wait_ms(50);            //
    #else
    /* 1602 */
    wait_ms(15);            // 15ms�ȏ�̃E�F�C�g

    lcd_out(0x30,RS_OFF);   //
    wait_ms(5);             // 4.1ms�ȏ�̃E�F�C�g

    lcd_out(0x30,RS_OFF);   //
    wait_us(150);           // 100us�ȏ�̃E�F�C�g

    lcd_out(0x30,RS_OFF);   //
    wait_us(50);            // 40us�ȏ�̃E�F�C�g

    lcd_out(0x20,0);        // 4�r�b�g���[�h
    wait_us(100);

    lcd_cmd(0x28);
    lcd_cmd(0x08);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x02);
    lcd_cmd(0x01);
    #endif
#endif
}

/*======================================*/
/*  ������o�͊֐�                      */
/*======================================*/
void lcd_puts(const char *s)
{
    register char c;

    while ((c = *s++))
    {
        lcd_data(c);
    }
}

/*======================================*/
/*  ������o�͊֐�2                     */
/*======================================*/
void lcd_puts_p(const char *progmem_s)
/* print string from program memory on lcd (no auto linefeed) */
{
    register char c;

    while ( (c = pgm_read_byte(progmem_s++)) )
    {
        lcd_data(c);
    }

}/* lcd_puts_p */

/*======================================*/
/*  �\���ʒu�w��֐�                    */
/*======================================*/
void lcd_pos(uint8_t line, uint8_t col)
{
    uint8_t pos;

    switch(line)
    {
        case 1:
            pos = 0x80;
            break;
        case 2:
            pos = 0xC0;
            break;
#ifdef LINE4
        case 3:
            pos = 0x94;
            break;
        case 4:
            pos = 0xD4;
            break;
#endif
        default:
            pos = 0x80;
            break;
    }
    lcd_cmd(pos + col - 1);
}
