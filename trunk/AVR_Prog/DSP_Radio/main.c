//=============================================================================
// File Name    : main.c
//
// Title        : DSP���W�I���W���[��NS9542��p�������W�I����v���O����
// Revision     : 0.1
// Notes        :
// Target MCU   : Atmel AVR series ATMega328p
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2012/08/11   �΂��      �J���J�n
// 2012/08/20   �΂��      Ver0.1����
//=============================================================================
//
//�yFUSE�z
//   Low Fuse       0xFF
//   High Fuse      0xDA
//   Extended Fuse  0x05
//
//==============================================================================


/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "main.h"
#include "uart.h"
#include "i2c.h"
#include "xitoa.h"
#include "NS9542.h"
#include "eep_data.h"
#include "lcd_lib.h"
#include "ir_ctrl.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
#define debug 0

/* macro ---------------------------------------------------------------------*/
#define LED_ON()    LED_PORT |= _BV(LED)
#define LED_OFF()   LED_PORT &= ~_BV(LED)

/* variables -----------------------------------------------------------------*/
uint8_t SwCond;
CHANNEL channel;    // �I�𒆂̃`�����l�����
uint16_t ch_count;  // �o�^����ă`�����l����
uint16_t ch_no;     // �I���`�����l�����
uint8_t lcd_no;     // LCD�̕\����ʂ̔ԍ�(1�`3) : �����l
bool is_mute;       // �~���[�g
IrMode ir_mode;
uint16_t am_seek_freq;
uint16_t fm_seek_freq;

/* functions ----------------------------------------------------------------*/

/*======================================*/
/*  �^�C�}�[2 ��rA��v���荞��         */
/*  100Hz                               */
/*======================================*/
ISR( TIMER2_COMPA_vect )
{
    uint8_t keybuf;

    /* �L�[��Ԏ擾 */
    SwCond = 0;
    keybuf = ~SW_IN;

    if (keybuf & _BV(SW1))  SwCond |= SW_1;
    if (keybuf & _BV(SW2))  SwCond |= SW_2;
    if (keybuf & _BV(SW3))  SwCond |= SW_3;
    if (keybuf & _BV(SW4))  SwCond |= SW_4;
}

/*======================================*/
/* LCD AM���g���\��                     */
/*======================================*/
static void lcd_disp_am_freq( uint16_t freq )
{
    char tmp_disp[10];

    itoa( freq, tmp_disp, 10 );
    lcd_puts( tmp_disp );
    lcd_puts("KHz");
}

/*======================================*/
/* LCD FM���g���\��                     */
/*======================================*/
static void lcd_disp_fm_freq( uint16_t freq )
{
    char tmp_disp[10];
    int tmp;

    tmp = freq / 100;
    itoa(tmp, tmp_disp, 10);
    lcd_puts(tmp_disp);
    lcd_puts(".");

    tmp = freq % 100;
    if ( tmp != 0 )
    {
        tmp /= 10;
    }
    itoa(tmp, tmp_disp, 10);
    lcd_puts( tmp_disp );
    lcd_puts("MHz");
}

/*======================================*/
/*  LCD ���C�A�E�g�\��                  */
/*======================================*/
static void lcd_disp_ch( void )
{
    char tmp_disp[10];
    int8_t sensi;

    lcd_clear();
    lcd_pos( 1, 1 );
    lcd_puts( channel.name );

    if ( lcd_no == 1 )
    {
        // CH�\��
        lcd_pos( 2, 1 );
        lcd_puts("CH");
        itoa(ch_no, tmp_disp, 10);
        lcd_puts(tmp_disp);

        lcd_pos( 2, 7 );
        if ( channel.band == AM )
        {
            lcd_puts("AM");
        }
        else
        {
            lcd_puts("FM");
        }
    }
    else if ( lcd_no == 2 )
    {
        lcd_pos( 2, 1 );
        if ( channel.band == FM )
        {
            lcd_disp_fm_freq( channel.freq );
        }
        else
        {
            lcd_disp_am_freq( channel.freq );
        }
    }
    else if ( lcd_no == 3 )
    {
        lcd_pos( 2, 1 );
        if ( channel.band == FM )
        {
            if ( NS9542_is_stereo() )
            {
                lcd_puts("STEREO");
            }
            else
            {
                lcd_puts("MONO");
            }

            sensi = NS9542_signal_strength( FM );
            itoa( sensi, tmp_disp, 10 );
            lcd_pos( 2, 7 );
            lcd_puts( tmp_disp );
        }
        else
        {
            lcd_puts("MONO");
            sensi = NS9542_signal_strength( AM );
            itoa( sensi, tmp_disp, 10 );
            lcd_pos( 2, 7 );
            lcd_puts( tmp_disp );
        }
    }
}

/*======================================*/
/*  LCD�\���y�[�W�ύX                   */
/*======================================*/
static void lcd_change(void)
{
    lcd_no++;
    if (lcd_no > MAX_LCD_NO)
    {
        lcd_no = 1;
    }
    lcd_disp_ch();
}

/*======================================*/
/*  LCD�\���y�[�W������                 */
/*======================================*/
void init_lcd_disp_ch( void )
{
    lcd_no = 1;
    lcd_disp_ch();
}

/*======================================*/
/*  ���W�I�ǑI��                        */
/*======================================*/
static void set_radio( uint16_t ch_no, CHANNEL channel )
{
    // for UART
    xprintf(PSTR("CH(%d): %s(%d) %s => Select OK!\n"), ch_no, channel.band == AM ? "AM": "FM", channel.freq, channel.name);

    // for LCD
    init_lcd_disp_ch();

    // Set CH
    NS9542_band_freq_change( channel.band, channel.freq, 0 );

    // �~���[�g�ݒ�
    NS9542_mute( is_mute );
}

/*======================================*/
/*  �ド�W�I�ǑI��                      */
/*======================================*/
static void pre_channel( void )
{
    if (!( ch_no == MAX_CHANNEL || ch_count == 1 ))
    {
        if (ch_no > 1)
        {
            eep_read( --ch_no, &channel );
            set_radio( ch_no, channel );
            init_lcd_disp_ch();
        }
    }
}

/*======================================*/
/*  �����W�I�ǑI��                      */
/*======================================*/
static void next_channel( void )
{
    if (!( ch_no == MAX_CHANNEL || ch_count == 1 ))
    {
        if (ch_count > ch_no)
        {
            eep_read(++ch_no, &channel);
            set_radio(ch_no,channel);
            init_lcd_disp_ch();
        }
    }
}

/*======================================*/
/*  �~���[�g                            */
/*======================================*/
static void toggle_mute(void)
{
    is_mute ^= 1;       // �~���[�g�t���O�l���g�O��

    NS9542_mute( is_mute );

    if ( is_mute )
    {
        LED_ON();
        xputs(PSTR("MUTE ON.\n"));
    }
    else
    {
        LED_OFF();
        xputs(PSTR("MUTE OFF.\n"));
    }
}

/*======================================*/
/*  �L�[���͏���                        */
/*======================================*/
static void keyin( void )
{
    if( SwCond )
    {
        // 1�`4 SW
        if (SwCond & SW_1)          /* LCD Display Change */
        {
            lcd_change();
        }
        else if (SwCond & SW_2)     /* Pre CH */
        {
            pre_channel();
        }
        else if (SwCond & SW_3)     /* mute */
        {
            toggle_mute();
            if ( ir_mode != NO_IR )
            {
                init_lcd_disp_ch();
            }
        }
        else if (SwCond & SW_4)     /* next ch */
        {
            next_channel();
        }
        ir_mode = NO_IR;            /* �L�[���͂��ꂽ��ԊO�������R���̑���𖳌��ɂ���*/

        // �X�C�b�`���������܂ő҂�
        while(SwCond)
        {
            _delay_ms(100);
        }
    }
}

/*======================================*/
/* �ԊO�������R������                   */
/*======================================*/
static void ir_in ( void )
{
    uint8_t l;
    char tmp_disp[10];
    static uint16_t recieve_num;        // �����R�������M��������

    l = IrCtrl.len;

    switch (IrCtrl.fmt)
    {
    case NEC:   /* NEC format data frame */
        if (l == 32)    /* Only 32-bit frame is valid */
        {
#if debug
            xprintf(PSTR("Recieve NEC CODE:[0x%02X 0x%02X 0x%02X 0x%02X]\n"), IrCtrl.buff[0], IrCtrl.buff[1], IrCtrl.buff[2], IrCtrl.buff[3]);
#endif
            if (IrCtrl.buff[0] == 0x00 && IrCtrl.buff[1] == 0xFF)       // �����R���̃J�X�^���R�[�h
            {
                if (IrCtrl.buff[2] == 0x12)             // (MENU)
                {
                    ir_mode = NO_IR;
                    lcd_change();
                }
                else if(IrCtrl.buff[2] == 0x16)         // (L1)
                {
                    ir_mode = NO_IR;
                    pre_channel();
                }
                else if(IrCtrl.buff[2] == 0x17)         // (L2)
                {
                    ir_mode = NO_IR;
                    next_channel();
                }
                else if(IrCtrl.buff[2] == 0x18)         // (L3)
                {
                    ir_mode = NO_IR;

                    lcd_clear();
                    lcd_pos(1,1);
                    lcd_puts("Seek AM");

                    NS9542_band_setting( AM );
                    am_seek_freq = NS9542_am_pluse_seek( am_seek_freq, 0 );

                    if (am_seek_freq == 0)  // �Ō�܂ŃV�[�N�����̂ŁA�ēx�ŏ�����
                    {
                        am_seek_freq = am_low_limit_freq;
                        am_seek_freq = NS9542_am_pluse_seek( am_seek_freq, 0 );
                    }
                    NS9542_mute( is_mute );             // �V�[�N����ƃ~���[�g�����ɂȂ��Ė߂��Ă���̂�

                    lcd_pos( 2, 1 );
                    lcd_disp_am_freq( am_seek_freq );

                    am_seek_freq += am_step;
                }
                else if(IrCtrl.buff[2] == 0x19)         // (L4)
                {
                    ir_mode = NO_IR;

                    lcd_clear();
                    lcd_pos(1,1);
                    lcd_puts("Seek FM");

                    NS9542_band_setting( FM );
                    fm_seek_freq = NS9542_fm_pluse_seek( fm_seek_freq, 0 );

                    if (fm_seek_freq == 0)  // �Ō�܂ŃV�[�N�����̂ŁA�ēx�ŏ�����
                    {
                        fm_seek_freq = fm_low_limit_freq;
                        fm_seek_freq = NS9542_fm_pluse_seek( fm_seek_freq, 0 );
                    }
                    NS9542_mute( is_mute );             // �V�[�N����ƃ~���[�g�����ɂȂ��Ė߂��Ă���̂�

                    lcd_pos(2,1);
                    lcd_disp_fm_freq( fm_seek_freq );

                    fm_seek_freq += fm_step;
                }
                else if(IrCtrl.buff[2] == 0x11)         // (Flash)
                {
                    ir_mode = NO_IR;
                    toggle_mute();
                }
                else if (IrCtrl.buff[2] == 0x13)        // (Set)
                {
                    ir_mode = TUNE_CH;
                    recieve_num = 0;
                    lcd_clear();
                    lcd_pos(1,1);
                    lcd_puts("Tune");
                    lcd_pos(2,1);
                    lcd_puts("FQ");
                }
                else if (IrCtrl.buff[2] == 0x14)        // (OSD)
                {
                    ir_mode = SELECT_CH;
                    recieve_num = 0;
                    lcd_clear();
                    lcd_pos(1,1);
                    lcd_puts("Select");
                    lcd_pos(2,1);
                    lcd_puts("CH");
                }
                else if (IrCtrl.buff[2] == 0x0D)        //  (ESC)
                {
                    ir_mode = NO_IR;
                    init_lcd_disp_ch();
                }

                else if(IrCtrl.buff[2] >= 0x00 && IrCtrl.buff[2] <= 0x09)   // (0)�`(9)
                {
                    if (ir_mode != NO_IR && recieve_num <= 999) // 4���܂�
                    {
                        recieve_num *= 10;      // 10�{(���グ)
                        recieve_num += (uint16_t)IrCtrl.buff[2];

                        itoa( recieve_num, tmp_disp, 10 );
                        lcd_pos( 2, 3 );
                        lcd_puts( tmp_disp );
                    }
                }
                else if(IrCtrl.buff[2] >= 0x0F)         // (Enter)
                {
#if debug
                    xprintf(PSTR("Recieve No:%d\n"), recieve_num);
#endif
                    if (ir_mode == SELECT_CH)
                    {
                        ch_count  = eep_count();
                        if (0 < recieve_num && recieve_num <= ch_count )
                        {
                            ch_no = recieve_num;
                        }
                        eep_read( ch_no, &channel );
                        set_radio( ch_no, channel );
                        init_lcd_disp_ch();

                        ir_mode = NO_IR;
                    }
                    else if (ir_mode == TUNE_CH)
                    {
                        if ( am_low_limit_freq <= recieve_num  && recieve_num <= am_high_limit_freq )
                        {
                            channel.band = AM;
                            channel.freq = recieve_num;
                            strcpy(channel.name, "Manual");

                            NS9542_band_freq_change( channel.band, channel.freq, 0 );
                        }
                        else if (fm_low_limit_freq <= recieve_num  && recieve_num <= fm_high_limit_freq )
                        {
                            channel.band = FM;
                            channel.freq = recieve_num;
                            strcpy(channel.name, "Manual");

                            NS9542_band_freq_change( channel.band, channel.freq, 0 );
                        }
                        init_lcd_disp_ch();

                        ir_mode = NO_IR;
                    }
                }
            }
        }
        break;

    case NEC|REPT:  /* NEC repeat frame */
#if debug
        xputs(PSTR("Recieve NEC CODE:[repeat]\n"));
#endif
        break;
    }

    IrCtrl.state = IR_IDLE;     /* Ready to receive next frame */
}

/*======================================*/
/*  �V���A�����C�����j�^                */
/*======================================*/
static void get_line (char *buff, int len)
{
    char c;
    int idx = 0;


    xputc('>');
    for (;;)
    {
        /* �L�[���͏��� */
        keyin();
        /* �����R������ */
        if (IrCtrl.state == IR_RECVED) ir_in();  /* ��M�t���[���̏��� */
        if (!uart_test()) continue;

        c = uart_get();
        if (c == '\r') break;
        if ((c == '\b') && idx)
        {
            idx--;
            xputc(c);
        }
        if (((uint8_t)c >= ' ') && (idx < len - 1))
        {
            buff[idx++] = c;
            xputc(c);
        }
    }
    buff[idx] = 0;
    xputc('\n');
}


/*======================================*/
/*  ���C��                              */
/*======================================*/
int main(void)
{
    int  i;
    long tmp;
    char buf[32], *p, *words[MAX_LEN];

    cli();                  // ���荞�݋֎~

    // �ԊO����M��̃|�[�g�̏�����
    IR_DDR &= ~_BV(IR_SENSOR);                              // ����
    IR_PORT |= _BV(IR_SENSOR);                              // �|�[�g�̃v���A�b�v

    // �X�C�b�`�̃|�[�g�̐ݒ�
    SW_DDR &= ~(_BV(SW1) | _BV(SW2) | _BV(SW3) | _BV(SW4));
    SW_PORT |= _BV(SW1) | _BV(SW2) | _BV(SW3) | _BV(SW4);   // �X�C�b�`�̃|�[�g�̃v���A�b�v

    // LED�̃|�[�g�̏�����
    LED_DDR |= _BV(LED);

    // �^�C�}�[2�ݒ� 10ms����
    TCCR2A = _BV(WGM21);                                    // CTC����
    TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);             // �v���X�P�[�� 1/1024 16MHz/1024=64us
    OCR2A = TIMER2_CMPA;
    TIMSK2 |= _BV(OCIE2A);

    // �d�̓J�b�g
//    PRR = _BV(PRTIM0) | _BV(PRTIM1) | _BV(PRUSART0) | _BV(PRADC) | _BV(PRSPI);

    ACSR = _BV(ACD);                                        // �A�i���O�R���p���[�^�֎~

    I2CMsInit();
    lcd_init();

    uart_init(38400);
    xfunc_out = (void(*)(char))uart_put;                    /* Join xitoa module to communication module */

    eep_init();

    /* �ԊO���@�\�̏����� */
    IR_initialize();

    sei();                                                  //�����݋���

    // �����p�����[�^�ݒ�
    is_mute = false;                                        // �~���[�gOff
    ir_mode = NO_IR;

    ch_no = 1;
    lcd_no = 1;

    ch_count = eep_count();
    if(ch_count == 0)
    {
        channel.band = AM;
        channel.freq = 666;
        strcpy(channel.name, "NHK-R1");
    }
    else
    {
        eep_read(ch_no, &channel);
    }

    NS9542_set_reqion( JAPAN );
    am_seek_freq = am_low_limit_freq;
    fm_seek_freq = fm_low_limit_freq;

    NS9542_Starting( channel.band, channel.freq, 0 );
    lcd_disp_ch();

    xputs(PSTR("Radio control program ver 0.1\n"));

    while(1)
    {
        get_line(buf, sizeof(buf));

        p = buf;
        for (i = 0; i < MAX_LEN; i++)
        {
            if ((words[i] = strtok(p, " ")) == NULL)
                break;
            p = NULL;
        }

        if (strcmp(words[0],"SET")==0)
        {
            if(xatoi(&words[1], &tmp)==0)
            {
                xputs(PSTR("FREQ ERROR\n"));
                continue;
            }
            if (words[2] == NULL)
            {
                xputs(PSTR("STATION NAME ERROR!\n"));
                continue;
            }

            if ( am_low_limit_freq <= (uint16_t)tmp  && (uint16_t)tmp <= am_high_limit_freq )
            {
                channel.band = AM;
            }
            else if (fm_low_limit_freq <= (uint16_t)tmp && (uint16_t)tmp <= fm_high_limit_freq )
            {
                channel.band = FM;
            }
            else
            {
                xputs(PSTR("FREQ ERROR!\n"));
                continue;
            }

            channel.freq = (uint16_t)tmp;
            strcpy(channel.name, words[2]);

            if ( (ch_no =eep_save(&channel)) == -1 )
            {
                xputs(PSTR("EEPROM SAVE ERROR!\n"));
                continue;
            }
            else
            {
                ch_count  = eep_count();
                xprintf(PSTR("CH(%d): %s(%d) %s => Save OK!\n"),ch_no,channel.band == AM ? "AM": "FM",channel.freq, channel.name);
                NS9542_band_freq_change( channel.band, channel.freq, 0 );

                init_lcd_disp_ch();
            }
        }
        else if (strcmp(words[0],"CH")==0)
        {
            xatoi(&words[1], &tmp);

            ch_no = (uint16_t)tmp;
            eep_read(ch_no, &channel);
            set_radio( ch_no, channel );

            init_lcd_disp_ch();
        }
        else if (strcmp(words[0],"MUTE")==0)
        {
            if (strcmp(words[1],"ON")==0)
            {
                is_mute = true;          // �~���[�gON
                NS9542_mute( is_mute );
                LED_ON();
                xputs(PSTR("MUTE ON.\n"));
            }
            else if (strcmp(words[1],"OFF")==0)
            {
                is_mute = false;        // �~���[�gOff
                NS9542_mute( is_mute );
                LED_OFF();
                xputs(PSTR("MUTE OFF.\n"));
            }
        }
        else if (strcmp(words[0],"CLEAR")==0)
        {
            eep_clear();
            ch_count = eep_count();
            if(ch_count == 0)
            {
                channel.band = AM;
                channel.freq = 666;
                strcpy(channel.name, "NHK-R1");
                ch_no = 1;
                lcd_no = 1;
                xputs(PSTR("CLEAR OK!\n"));
            }
            else
            {
                xputs(PSTR("EEPROM CLEAR ERROR!\n"));
            }
        }
    }
    return 0;
}
