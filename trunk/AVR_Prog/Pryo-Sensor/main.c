//=============================================================================
// File Name    : main.c
//
// Title        : �œd�^�ԊO���Z���T AKE-1(RE-210)�e�X�g�v���O����
// Revision     : 0.1
// Notes        :
// Target MCU   : Atmel AVR ATtiny2313
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2012/09/29   �΂��      �J���J�n
//=============================================================================
//
//�yFUSE�z
//   Low Fuse       0xE4
//   High Fuse      0xDF
//   Extended Fuse  0xFF
//
//==============================================================================

/*
�y�s���z�u�z            ___ ___
                   RST# |  U  |Vcc
                 ....TX |     |PB7 ...
                 ....RX |     |PB6 ...
                   Xtl2 |     |PB5 ...
                   Xtl1 |     |PB4 ...
                ....PD2 |     |PB3 ...
                ....PD3 |     |PB2 ...
                ....PD4 |     |PB1 ...�m�FLED
                ....PD5 |     |PB0 ...
                    GND |     |PD6 ...AKE-1(RE-210)
                        +-----+
                       ATTiny2313
*/


/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

/* typedef ------------------------------------------------------------------- */
/* define -------------------------------------------------------------------- */
enum { WAVE_FALL=0, WAVE_RISE };

#define debug 0
/* macro --------------------------------------------------------------------- */
/*==================================*/
/* �L���v�`���[�֌W�}�N���֐�       */
/*==================================*/
#define IR_CAPT_TEST()  TCCR1B & _BV(ICES1)     /* Rx: Check which edge generated the capture interrupt */
#define IR_CAPT_RISE()  TCCR1B |= _BV(ICES1)    /* Rx: Set captureing is triggered on rising edge */
#define IR_CAPT_FALL()  TCCR1B &= ~_BV(ICES1)   /* Rx: Set captureing is triggered on falling edge */
#define IR_CAPT_CLEAR() TIFR |= _BV(ICF1)       /* Rx: Enable captureing interrupt */
#define IR_CAPT_ENA()   TIMSK |= _BV(ICIE1)     /* Rx: Enable captureing interrupt */
#define IR_CAPT_REG()   ICR1                    /* Rx: Returns the value in capture register */
#define IR_CAPT_DIS()   TIMSK &= ~_BV(ICIE1)    /* Tx && Rx: Disable captureing interrupt */

/*==================================*/
/* LED�̕\���}�N���֐�              */
/*==================================*/
#define LED_OFF()   PORTB |= _BV(PB1)
#define LED_ON()    PORTB &= ~_BV(PB1)


/* variables ----------------------------------------------------------------- */
volatile uint16_t pryo_count;
volatile uint8_t PryoState;

/* functions ----------------------------------------------------------------- */

/****************************************
* IO�̏�����                            *
****************************************/
void io_init(void)
{
    // LED �̐ݒ�
    // �|�[�g B0 B1���o�͂ɐݒ�
    DDRB |= (1<<PB1);          // �|�[�g B1  ���o�͐ݒ�

    // �ԊO���Z���T�ݒ�
    // �|�[�g D6����͂ɐݒ�
    DDRD &= ~(1 << PD6);
}


/****************************************
* �^�C�}�[0�I�[�o�[�t���[����           *
*****************************************/
ISR( TIMER0_OVF_vect )
{
    if ( pryo_count != 0)
    {
        pryo_count--;
    }

    if (pryo_count == 0)
    {
        LED_OFF();

    }
    else
    {
        LED_ON();
    }

}

/****************************************
* �^�C�}�[1����(�J�E���g�L���v�`���[)   *
****************************************/
ISR( TIMER1_CAPT_vect )
{
    uint16_t PulseWidth;

    PulseWidth = ICR1;          // �J�E���g�l�̓ǂݎ��
    TCNT1 = 0x0000;             // �J�E���^�l�̃N���A

    switch ( PryoState )
    {
    case WAVE_FALL:
        if (bit_is_clear(PIND,PIND6))       // (���_��)����������G�b�W���o
        {
            IR_CAPT_RISE();                 // �����オ��쓮�ɕύX
            PryoState = WAVE_RISE;
        }
        break;

    case WAVE_RISE:
        if ( bit_is_set(PIND, PIND6) )      // (���_��)�����オ��G�b�W���o(���[�_�[�̃L�����A��~)
        {
            IR_CAPT_FALL();                 // ����������쓮�ɕύX(ICP�̌��o�G�b�W��L)
            PryoState = WAVE_FALL;          // �͂��߂ɖ߂�
            if (PulseWidth > 1520)          // ��50mS�ȏ�̐M��
            {
                pryo_count += 32*10;
                if (pryo_count > 32*60)
                {
                    pryo_count = 32*60;
                }
            }
        }
        break;

    default:
        IR_CAPT_FALL();                     // ����������쓮�ɕύX(ICP�̌��o�G�b�W��L)
        PryoState = WAVE_FALL;
        break;
    }
}


/****************************************
* �^�C�}�[0�̏�����                    *
****************************************/
void timer0_init(void)
{
    TCCR0A =
        (0<<COM0A1) |
        (0<<COM0A0) |           /* �W���|�[�g���� (OC0A�ؒf) */
        (0<<COM0B1) |
        (0<<COM0B0) |           /* �W���|�[�g���� (OC0B�ؒf) */
        (0<<WGM01)  |
        (0<<WGM00);             /* �W������ */

    TCCR0B =
        (0<<FOC0A) |
        (0<<FOC0B) |            /* �s���� */
        (0<<WGM02) |            /* �W������ */
        (1<<CS02)  |            /* �v���X�P�[���� 1/1024 */
        (0<<CS01)  |
        (1<<CS00);

    TCNT0 = 0;              // �^�C�}0�̏����l
    TIMSK |= _BV(TOIE0);    // �^�C�}0�I�[�o�[�t���[�������荞�݋���
}


/****************************************
* �^�C�}�[1�̏�����                     *
* �L���v�`���[���荞�݂Ƃ��Đݒ�        *
*****************************************/
void timer1_init(void)
{
    //======================
    // TCCR1�ݒ�
    //======================
    TCCR1A =            //
        (0<<COM1A1) |   // COM1A1:COM1A0
        (0<<COM1A0) |   //  00  OC1A�ؒf(�g��Ȃ��Ƃ�������)
        (0<<COM1B1) |   // COM1B1:COM1B0
        (0<<COM1B0) |   //  00  OC1A�ؒf(�g��Ȃ��Ƃ�������)
        (0<<WGM11)  |   // WGM11:WGM10 �g�`�������(4bit�̉���2bit)
        (0<<WGM10);     //  00  �g��Ȃ�

    //======================
    // TCCR1B �ݒ�
    //======================
    TCCR1B =            //
        (1<<ICNC1)  |   // ICNC1 : �ߊl�N������1�G����������    (0:�s���� 1:����)
        (1<<ICES1)  |   // ICES1 : �ߊl�N�����͒[�I��           (0:�s�I�� 1:�I��)
        (0<<WGM13)  |   // WGM13 : �g�`�������(4bit�̏��2bit)
        (0<<WGM12)  |   // WGM12 : �g�`�������(4bit�̏��2bit)
        (1<<CS12)   |   // CS12:CS11:CS10 => �v���X�P�[��(256����)
        (0<<CS11)   |   //
        (1<<CS10);      //

    //======================
    // �J�E���^������
    //======================
    TCNT1 = 0x0000;
    OCR1A = 0xFFFF;
    OCR1B = 0xFFFF;

    //======================
    //   �O�����荞�ݐݒ�
    //======================
    // �O�����荞��(���g�p)
    GIMSK = 0x00;
    MCUCR = 0x00;

    // �R���p���[�^���g�p
    ACSR=0x80;

    // �^�C�}�[1�L���v�`���[��������
    TIMSK |= (1<<ICF1);
}

/*======================================*/
/* ���C��                               */
/*======================================*/
int main(void)
{

    uint8_t i;

    cli(); // ���荞�݋֎~

    io_init();

    // �Z���T�̋N����҂�
    for(i=0; i < 90; i++)
    {
        LED_ON();
        _delay_ms(500);
        LED_OFF();
        _delay_ms(500);
    }

    // LED�̕\���Ŏg�p
    timer0_init();

    // �œd�ԊO���Z���T�Ŏg�p
    timer1_init();

    IR_CAPT_FALL();     // ����������쓮�ɕύX
    IR_CAPT_CLEAR();    // �L���v�`���[�����v���N���A
    PryoState = WAVE_FALL;
    pryo_count = 0;     // 0�b

    set_sleep_mode(SLEEP_MODE_IDLE);// �p���[�_�E�����[�h��ݒ�

    sei();          // �S�̊�������

    while (1)
    {
        sleep_mode();
    }

    return 0;
}
