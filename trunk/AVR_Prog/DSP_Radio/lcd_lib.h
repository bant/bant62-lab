//=======================================================
// File Name    : lcd_lib.h
//
// Title        : AVR LCD ���C�u�����B�w�b�_
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

#ifndef _LCD_LIB_H_
#define _LCD_LIB_H_

/* ===[�g�pLCD�̑I��]================================== */
/* SC1602    : USE_SC1602                               */
/* ACM0802C  : USE_ACM0802C                             */
/* �f�t�H���g: USE_SC1602                               */
/* ==================================================== */
/* ��������C�� */
#define USE_ACM0802C
//#define USE_SC1602

/* ===[�ڑ��|�[�g�̏�ʉ��ʃr�b�g�ݒ�]================= */
/* ���4�r�b�g���f�[�^�|�[�g : USE_HIGH_NIBBLE          */
/* ����4�r�b�g���f�[�^�|�[�g : USE_LOW_NIBBLE           */
/* �f�t�H���g  : USE_LOW_NIBBLE                         */
/* ==================================================== */
/* ��������C�� */
#define USE_LOW_NIBBLE
//#define USE_HIGH_NIBBLE

/* ===[�g�p�|�[�g�̐錾]=============================== */
/*  LCD��DB4�`Db7���|�[�g�̏��4�r�b�g�܂���            */
/*  ����4�r�b�g�ɐڑ����邱��                           */
/* ==================================================== */
/* ��������C�� */
#define DATA_DDR        DDRC
#define DATA_PORT       PORTC

#define CTRL_DDR        DDRD
#define CTRL_PORT       PORTD
#define LCD_RS          PD3
#define LCD_E           PD2

#define LINE2
//#define LINE4


/* ====[�ȍ~�͕ύX����K�v�Ȃ�]======================== */

/* ==================================================== */
/*  �^�Ԑ錾�I��                                        */
/* ==================================================== */
#ifdef USE_SC1602
    #undef USE_ACM0802C
#elif defined USE_ACM0802C
    #undef USE_SC1602
#else
    #define USE_SC1602
#endif

/* ==================================================== */
/*  ���4�r�b�g����4�r�b�g�錾�I��                      */
/* ==================================================== */
#ifdef USE_HIGH_NIBBLE
    #undef USE_LOW_NIBBLE
#elif defined USE_LOW_NIBBLE
    #undef USE_HIGH_NIBBLE
#else
    #define USE_HIGH_NIBBLE
#endif

/* ==================================================== */
/*  �ϐ���`                                            */
/* ==================================================== */
#define RS_OFF  0
#define RS_ON   1


/* ==================================================== */
/*  �֐��v���g�^�C�v                                    */
/* ==================================================== */
void lcd_init(void);
void lcd_out(uint8_t code, uint8_t rs);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_puts(const char *s);
void lcd_puts_p(const char *progmem_s);
void lcd_pos(uint8_t line, uint8_t col);

/* ==================================================== */
/*  �}�N���֐�                                          */
/* ==================================================== */
#define lcd_clear()         lcd_cmd(0x01)
#define lcd_home()          lcd_cmd(0x02)
#define lcd_display_on()    lcd_cmd(0x0C)
#define lcd_display_off()   lcd_cmd(0x08)

#endif
