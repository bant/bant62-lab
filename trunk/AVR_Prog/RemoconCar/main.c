/********************************************************
 *                                                      *
 *�@   Tiny2313�ɂ��ԊO�������R����M���W���[��       *
 *                                                      *
 *                                                      *
 *                      Created on: 2010/10/16          *
 *                          Author: bant                *
 *                                                      *
 ********************************************************

�y�s���z�u�z�@�@�@�@  �@___ ___
                   RST# |  U  |Vcc
             UART....TX |     |PB7 ...
              "  ....RX |     |PB6 ...
               �@  Xtl2 |  �@ |PB5 ...
                   Xtl1 |     |PB4 ...���[�^2��IN2
                ....PD2 |     |PB3 ...���[�^2��IN1
    ���[�^1��IN1....PD3 |     |PB2 ...OC0A(���[�^1PWM)
    ���[�^1��IN2....PD4 |     |PB1 ...�ʏ�LED
OC0B(���[�^2PWM)....PD5 |     |PB0 ...Switch
                    GND |     |PD6 ...ICP(input capture)
                        +-----+
                       ATTiny2313

�y�@�\�z
�@�@�@�E�^�C�}�[�̃L���v�`���[�@�\���g���āA�ԊO�����W���[��
 �@�@�@�@�����Pulse Position Modulation(�p���X�ʒu�ϒ��j
 �@�@�@�@�M�����f�R�[�h����B
      �E�M���͌J��Ԃ����̂ŁA�Ō�̉�̂ݗL���Ƃ���B
 �@�@ �E�f�R�[�h�����M����LCD�ɕ\������B

�y���������z
2011/2/7 V0.1: NEC�t�H�[�}�b�g�M���̃f�R�[�h���[�`������
--------------------------------------------------------*/


//----- Include Files ---------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "remocon2313.h"
#include "uart2313.h"

/****************************************
            �O���[�o���ϐ���`          *
****************************************/

volatile BYTE BitCount;             // �����R����M�r�b�g�J�E���^
volatile FLAGS ReqFlag;             // �v���t���O
volatile L4BYTE RemData;
volatile L4BYTE DispRemData;        // �����R����M�f�[�^,�\���o�b�t�@
volatile L4BYTE PreRemData;

volatile BYTE timer0_count = 32;

volatile BYTE flag;


/****************************************
*                                       *
* �^�C�}�[�O�I�[�o�[�t���[����          *
*                                       *
*  �������U8MHz����@����1024������     *
*  255��̃J�E���g�I�[�o�[�t���[��      *
*  ���荞�݂�����Ƃ���ƁA���荞��     *
*  ���Ԃ́A                             *
*  (1/8000000)*1024*255 = 0.03264(s)    *
*                        => 3.264(ms)   *
*                                       *
*  32�{����Ɩ�P�b�ɂȂ�               *
*                                       *
*****************************************/
ISR( TIMER0_OVF_vect )
{
    BYTE set_data;

    switch (flag)
    {
    case RECEIVE_DATA:
        set_data = 32;
        break;

    case REPEAT_DATA:
        set_data = 24;
        break;

    case RECEIVE_ERR_DATA:
        set_data = 16;
        break;

    case MAKER_CODE_ERROR:
        set_data = 8;
        break;

    default:
        set_data = 0;
        break;
    }

    if (set_data == 0)
    {
        LED_ON();
    }
    else
    {
        if (--timer0_count == 0)
        {
            timer0_count = set_data;
            PORTB ^= (1<<PB1);
        }
    }
}



/****************************************
* �^�C�}�[�P����(�J�E���g�L���v�`���[)  *
****************************************/
ISR( TIMER1_CAPT_vect )
{
    WORD PulseWidth;

    PulseWidth = ICR1;  // �J�E���g�l�ǂݎ��
    TCNT1=0x0000;   // �J�E���g�l�N���A

    switch (ReqFlag.RmStatus)
    {
    case RM_INTIAL :    // �������
        if (bit_is_clear(PIND, PIND6))
        { // (���_��)����������G�b�W���o�i���[�_�[�̃L�����A�J�n�j
            IR_CAPT_RISE();                     // �����オ��쓮�ɕύX(ICP�̌��o�G�b�W��H)
            ReqFlag.RmStatus = RM_READER_L;
        }
        break;
    case RM_READER_L :  // ���[�_�[�̃L�����AON�I��(9mS)
        if (bit_is_set(PIND, PIND6))
        { // (���_��)�����オ��G�b�W���o�i���[�_�[�̃L�����A��~�j
            IR_CAPT_FALL();                 // ����������쓮�ɕύX(ICP�̌��o�G�b�W��L)
            if ((PulseWidth >= 7200)  && (PulseWidth <= 10800))
            { // ���[�_�[���L�����AON���ԃ`�F�b�N(9mS)
                ReqFlag.RmStatus = RM_READER_H;
            }
            else
            {// ����`�t�H�[�}�b�g���o
                ReqFlag.ErrDisp = 1;
                BitCount = 0;
                ReqFlag.RmStatus = RM_INTIAL;
            }
        }
        break;
    case RM_READER_H :  // ���[�_�[�̃L�����AOFF�I��(4.5mS)
        if (bit_is_clear(PIND, PIND6))
        {// (���_��)����������G�b�W���o�i�f�[�^���̓X�g�b�v�r�b�g�̃L�����A�j
            if ((PulseWidth >= 3600)  && (PulseWidth <= 5400))
            {// ���[�_�[���L�����AOFF���ԃ`�F�b�N(4.5mS)
                ReqFlag.RmStatus = RM_DATA;         // �f�[�^�̊J�n
                IR_CAPT_FALL();             // ����������쓮�ɕύX(ICP�̌��o�G�b�W��L)
            }
            else if ((PulseWidth >= 1800)  && (PulseWidth <= 2700))
            {// ���s�[�g���[�_�[���L�����AOFF���ԃ`�F�b�N(2.25mS)
                ReqFlag.RmStatus = RM_STOP;         // �X�g�b�v�r�b�g
                IR_CAPT_RISE();                     // �����オ��쓮�ɕύX(ICP�̌��o�G�b�W��H)
            }
            else
            {// ����`�t�H�[�}�b�g���o
                ReqFlag.ErrDisp = 1;
                BitCount = 0;
                IR_CAPT_FALL();                     // ����������쓮�ɕύX(ICP�̌��o�G�b�W��L)
                ReqFlag.RmStatus = RM_INTIAL;
            }
        }
        break;
    case RM_DATA :  // �f�[�^�P�r�b�g��M�̏I��
        if (bit_is_clear(PIND, PIND6))
        {// (���_��)����������G�b�W���o�i�f�[�^���̓X�g�b�v�r�b�g�̃L�����A�j
            if ((PulseWidth >= 896)  && (PulseWidth <= 1344))
            {// �f�[�^0���o(1.12mS)
                RemData.DATA_LONG >>= 1;            // �f�[�^�E�V�t�g
                BitCount++;                         // ��M�J�E���g�X�V
                if (BitCount == NEC_BIT_COUNT)
                {
                    ReqFlag.RmStatus = RM_STOP;     // �X�g�b�v�r�b�g
                    IR_CAPT_RISE();                 // �����オ��쓮�ɕύX(ICP�̌��o�G�b�W��H)
                }
            }
            else if ((PulseWidth >= 1800)  && (PulseWidth <= 2700))
            {// �f�[�^1���o(1.68mS)
                RemData.DATA_LONG >>= 1;            // �f�[�^�E�V�t�g
                RemData.DATA_LONG |= 0x80000000;
                BitCount++;                         // ��M�J�E���g�X�V
                if (BitCount == NEC_BIT_COUNT)
                {
                    ReqFlag.RmStatus = RM_STOP;     // �X�g�b�v�r�b�g
                    IR_CAPT_RISE();                 // �����オ��쓮�ɕύX(ICP�̌��o�G�b�W��H)
                }
            }
            else
            {// ����`�t�H�[�}�b�g���o
                ReqFlag.ErrDisp = 1;
                BitCount = 0;
                IR_CAPT_FALL();                     // ����������쓮�ɕύX(ICP�̌��o�G�b�W��L)
                ReqFlag.RmStatus = RM_INTIAL;
            }
        }
        break;
    case RM_STOP :  // �X�g�b�v�r�b�g�̏I��
        if (bit_is_set(PIND, PIND6))
        {// (���_��)�����オ��G�b�W���o�i�X�g�b�v�r�b�g�̃L�����A��~�j
            DispRemData = RemData;
            ReqFlag.DispCode = 1;                   // �R�[�h�\���v���Z�b�g
            BitCount = 0;                           // ��M�J�E���g�N���A
            ReqFlag.RmStatus = RM_INTIAL;
            IR_CAPT_FALL();                         // ����������쓮�ɕύX(ICP�̌��o�G�b�W��L)
        }
        break;
    default:
        break;
    }
}

/****************************************
* IO�̏������@�@�@�@�@�@�@�@�@�@�@�@�@  *
****************************************/
void io_init(void)
{
    //==================//
    //   IO�̐ݒ�       //
    //==================//

    // LED �̐ݒ�
    // �|�[�g B0 B1���o�͂ɐݒ�
    DDRB |= (1<<PB1);          // �|�[�g B1�@���o�͐ݒ�

    // �ԊO���Z���T�ݒ�
    // �|�[�g D6����͂ɐݒ�
    DDRD &= ~(1 << PD6);

    // ���[�^�쓮IC����s��(�o��)
    DDRB |= (1<<PB3) | (1<<PB4);
    DDRD |= (1<<PD3) | (1<<PD4);
}

/*

�y�^�C�}�O�֘A���W�X�^�z

 1.���/����0����ڼ޽�A(TCCR0A)

 =================================================================
    ���/����0����ڼ޽�A(TCCR0A)�̃r�b�g����
    7       6       5       4       3       2       2       0
 ----------------------------------------------------------------
    COM0A1  COM0A0  COM0B1  COM0B0  -       -       WGM01   WGM00
 ================================================================

 1.1.COM0Ax�̏ڍ�

 ======================================================
    �\34. ��PWM�����rA�o�͑I��
 ======================================================
    COM0A1  COM0A0      �Ӗ�
    0       0           �W���߰ē��� (OC0A�ؒf)
    0       1           ��r��v��OC0A��� ĸ��(����)�o��
    1       0           ��r��v��OC0A��� Low���ُo��
    1       1           ��r��v��OC0A��� High���ُo��
 ======================================================

 ======================================================
    �\35. ����PWM�����rA�o�͑I�� (���ʒ��ӎQ��)
 ======================================================
    COM0A1  COM0A0      �Ӗ�
    0       0           �W���߰ē��� (OC0A�ؒf)
    0       1           WGM02=0 :   �W���߰ē��� (OC0A�ؒf)
                        WGM02=1 :   ��r��v��OC0A��� ĸ��
    1       0           ��r��v��Low�ABOTTOM��High��
                        OC0A��݂֏o�� (�񔽓]����)
    1       1           ��r��v��High�ABOTTOM��Low��
                        OC0A��݂֏o�� (���]����)
 ======================================================

 ======================================================
    �\36. �ʑ��PWM�����rA�o�͑I�� (���ʒ��ӎQ��)
 ======================================================
    COM0A1  COM0A0      �Ӗ�
    0       0           �W���߰ē��� (OC0A�ؒf)
    0       1           WGM02=0 :   �W���߰ē��� (OC0A�ؒf)
                        WGM02=1 :   ��r��v��OC0A��� ĸ��
                                    (����)�o��
    1       0           �㏸�v�����̔�r��v��Low�A���~�v��
                        ���̔�r��v��High��OC0A��݂֏o��
    1       1           �㏸�v�����̔�r��v��High�A���~�v��
                        ���̔�r��v��Low��OC0A��݂֏o��
 ======================================================

  1.2.COM0Bx�̏ڍ�

 ======================================================
    �\37. ��PWM�����rB�o�͑I��
 ======================================================
    COM0B1  COM0B0      �Ӗ�
    0       0           �W���߰ē��� (OC0B�ؒf)
    0       1           ��r��v��OC0B��� ĸ��(����)�o��
    1       0           ��r��v��OC0B��� Low���ُo��
    1       1           ��r��v��OC0B��� High���ُo��
 ======================================================

 ======================================================
    �\38. ����PWM�����rB�o�͑I�� (���ʒ��ӎQ��)
 ======================================================
    COM0B1  COM0B0      �Ӗ�
 ------------------------------------------------------
    0       0           �W���߰ē��� (OC0B�ؒf)
    0       1           �\��
    1       0           ��r��v��Low�ABOTTOM��High��
                        OC0B��݂֏o�� (�񔽓]����)
    1       1           ��r��v��High�ABOTTOM��Low��
                        OC0B��݂֏o�� (���]����)
 ======================================================

  1.3.WGM0x�̏ڍ�

 ===============================================================================================
    �\40. �g�`������ʑI��
 ==============================================================================================
    (WGM02) WGM01   WGM00   ���/����������                TOP�l       OCR0x�X�V�� TOV0�ݒ莞
 ----------------------------------------------------------------------------------------------
    0       0       0       �W������                        $FF         ����        MAX
    0       0       1       8�ޯĈʑ��PWM����            $FF         TOP         BOTTOM
    0       1       0       ��r��v���/���� �ر(CTC)����   OCR0A       ����        MAX
    0       1       1       8�ޯč���PWM����                $FF         BOTTOM      MAX
    1       0       0       (�\��)                          -           -           -
    1       0       1       �ʑ��PWM����                 OCR0A       TOP         BOTTOM
    1       1       0       (�\��)                          -           -           -
    1       1       1       ����PWM����                     OCR0A       BOTTOM      TOP
 ==============================================================================================

 2.���/����0����ڼ޽�B(TCCR0B)

  ================================================================
    ���/����0����ڼ޽�B �̃r�b�g����
    7       6       5       4       3       2       1       0
 ----------------------------------------------------------------
    FOC0A   FOC0B   -       -       WGM02   CS02    CS01    CS00
 ================================================================

 2.1.FOC0A�̏ڍ�

 =====================================================
    OC0A�����ύX (Force Output Compare A)
    FOC0A       �Ӗ�(��PWM������w�����鎞�����L��)
 -----------------------------------------------------
    0           �s����(disable)
    1           ����(enable)
 =====================================================

 2.2.FOC0B�̏ڍ�

 =====================================================
    OC0B�����ύX (Force Output Compare A)
    FOC0B       �Ӗ�(��PWM������w�����鎞�����L��)
 -----------------------------------------------------
    0           �s����(disable)
    1           ����(enable)
 =====================================================

 2.3.CS0x�̏ڍ�

 =====================================================
    �\41. ���/����0���͸ۯ��I��
 =====================================================
    CS02    CS01    CS00    �Ӗ�
 -----------------------------------------------------
    0       0       0       ��~ (���/����0�����~)
    0       0       1       clkI/O (�O�u�����Ȃ�)
    0       1       0       clkI/O/8 (8����)
    0       1       1       clkI/O/64 (64����)
    1       0       0       clkI/O/256 (256����)
    1       0       1       clkI/O/1024 (1024����)
    1       1       0       T0��݂̉��~�[ (�O���ۯ�)
    1       1       1       T0��݂̏㏸�[ (�O���ۯ�)
 ======================================================
*/


/****************************************
* �^�C�}�[�O�̏������@�@�@�@�@�@�@�@�@  *
****************************************/
void timer0_init(void)
{
    TCCR0A = (0<<COM0A1) | (0<<COM0A0) |            /* �W���߰ē��� (OC0A�ؒf) */
             (0<<COM0B1) | (0<<COM0B0) |            /* �W���߰ē��� (OC0B�ؒf) */
             (0<<WGM01)  | (0<<WGM00);              /* �W������ */

    TCCR0B = (0<<FOC0A) | (0<<FOC0B) |              /* �s���� */
             (0<<WGM02) |                           /* �W������ */
             (1<<CS02)  | (0<<CS01)  | (1<<CS00);   /* �v���X�P�[���� 1/1024 */

    TCNT0 = 0;              // �^�C�}0�̏����l
    TIMSK |= _BV(TOIE0);    // �^�C�}0�I�[�o�[�t���[�������荞�݋���
}


/****************************************
* �^�C�}�[�P�̏������@�@�@�@�@�@�@�@�@  *
****************************************/
void timer1_init(void)
{
    //  ----��PWM����ł̔�r�o�͑I��(��: x��A�܂���B, X��0�܂���1)----
    // =============================================================================================
    //  COM1x1  COM1x0          �Ӗ�
    // =============================================================================================
    //  0       0           �W���߰ē��� (OC1x�ؒf)
    //  0       1           ��r��v��OC1x��� ĸ��(����)�o��
    //  1       0           ��r��v��OC1x��� Low���ُo��
    //  1       1           ��r��v��OC1x��� High���ُo��
    //
    //  ----����PWM����ł̔�r�o�͑I��(��: x��A�܂���B, X��0�܂���1)----
    // =============================================================================================
    //  COM1x1  COM1x0          �Ӗ�
    // =============================================================================================
    //  0       0           �W���߰ē��� (OC1x�ؒf)
    //  0       1           WGM13�`0=111X       : ��r��v��OC1A��� ĸ��(����)�o�́AOC1B�͕W���߰ē���(OC1B�ؒf)
    //                      WGM13�`0��L�ȊO    : �W���߰ē��� (OC1x�ؒf)
    //  1       0           ��r��v��Low�ABOTTOM��High��OC1x��݂֏o�� (�񔽓]����)
    //  1       1           ��r��v��High�ABOTTOM��Low��OC1x��݂֏o�� (���]����)
    //
    //  ----45. �ʑ���܂��͈ʑ�/���g���PWM����ł̔�r�o�͑I��(��: x��A�܂���B, X��0�܂���1)----
    // =============================================================================================
    //  COM1x1  COM1x0          �Ӗ�
    // =============================================================================================
    //  0       0           �W���߰ē��� (OC1x�ؒf)
    //  0       1           WGM13�`0=10X1       : ��r��v��OC1A��� ĸ��(����)�o�́AOC1B�͕W���߰ē���(OC1B�ؒf)
    //                      WGM13�`0��L�ȊO    : �W���߰ē��� (OC1x�ؒf)
    //  1       0           �㏸�v�����̔�r��v��Low�A���~�v�����̔�r��v��High��OC1x��݂֏o��
    //  1       1           �㏸�v�����̔�r��v��High�A���~�v�����̔�r��v��Low��OC1x��݂֏o��


    // =============================================================================================
    //  �\46. �g�`������ʑI��\(WGMxx)
    // =============================================================================================
    //  WGM13   WGM12   WGM11   WGM10       ���/����������                TOP�l   OCR1x   TOV1
    //          (CTC1)  (PWM11) (PWM10)                                             �X�V��  �ݒ莞
    // =============================================================================================
    //  0       0       0       0           �W������                        $FFFF   ���l    MAX
    //  0       0       0       1           8�ޯĈʑ��PWM����            $00FF   TOP     BOTTOM
    //  0       0       1       0           9�ޯĈʑ��PWM����            $01FF   TOP     BOTTOM
    //  0       0       1       1           10�ޯĈʑ��PWM����           $03FF   TOP     BOTTOM
    //  0       1       0       0           ��r��v���/���� �ر(CTC)����   OCR1A   ���l    MAX
    //  0       1       0       1           8�ޯč���PWM����                $00FF   BOTTOM  TOP
    //  0       1       1       0           9�ޯč���PWM����                $01FF   BOTTOM  TOP
    //  0       1       1       1           10�ޯč���PWM����               $03FF   BOTTOM  TOP
    //  1       0       0       0           �ʑ�/���g���PWM����          ICR1    TOP     BOTTOM
    //  1       0       0       1           �ʑ�/���g���PWM����          OCR1A   TOP     BOTTOM
    //  1       0       1       0           �ʑ��PWM����                 ICR1    BOTTOM  TOP
    //  1       0       1       1           �ʑ��PWM����                 OCR1A   BOTTOM  TOP
    //  1       1       0       0           ��r��v���/���� �ر(CTC)����   ICR1    ���l    MAX
    //  1       1       0       1           �\��                            -       -       -
    //  1       1       1       0           ����PWM����                     ICR1    BOTTOM  TOP
    //  1       1       1       1           ����PWM����                     OCR1A   TOP     BOTTOM
    // =============================================================================================

    // =============================================================================================
    //  �ߊl�N������1�G����������
    // =============================================================================================
    //  ICNC1       �Ӗ�
    // =============================================================================================
    //  0           �s����(disable)
    //  1           ����(enable)
    // =============================================================================================

    // =============================================================================================
    //  �ߊl�N�����͒[�I��
    // ==============================================================================================
    //  ICES1       �Ӗ�
    // ==============================================================================================
    //  0           �s����(disable)
    //  1           ����(enable)
    // =============================================================================================


    // ==============================================================================================
    //  �\47. ���/����1���͸ۯ��I��
    // ==============================================================================================
    //  CS12    CS11    CS10        �Ӗ�
    //  0       0       0       ��~ (���/����1�����~)
    //  0       0       1       clkI/O (�O�u�����Ȃ�)
    //  0       1       0       clkI/O/8 (8����)
    //  0       1       1       clkI/O/64 (64����)
    //  1       0       0       clkI/O/256 (256����)
    //  1       0       1       clkI/O/1024 (1024����)
    //  1       1       0       T1��݂̉��~�[ (�O���ۯ�)
    //  1       1       1       T1��݂̏㏸�[ (�O���ۯ�)
    // ==============================================================================================


    //==========================================================//
    //   �^�C�}1 �J�E���^���W�X�^�ݒ�                           //
    //==========================================================//
    // 15.11.1 �^�C�}�^�J�E���^1���䃌�W�X�^A (�����l��0x00�Ȃ̂ŕK�v�Ȃ�)
    //         ++-------COM1A1:COM1A0 00 OC1A�ؒf
    //         ||++---- COM1B1:COM1B0 00 OC1B�ؒf
    //         ||||  ++ WGM11:WGM10   00 �g�`�������(4bit�̉���2bit)
//    TCCR1A = 0b00000000;        // CTC with interrupt


    TCCR1A =    (0<<COM1A1) | (0<<COM1A0) |
              (0<<COM1B1) | (0<<COM1B0) |
              (0<<WGM11) | (0<<WGM10) ;


    // 15.11.2 �^�C�}�^�J�E���^1���䃌�W�X�^B (P-69)
    //         +------- ICNC1          1   �ߊl�N������1�G����������
    //         |+------ ICES1          1   �ߊl�N�����͒[�I��
    //         || ++--- WGM13:WGM12    00  �g�`�������(4bit�̏��2bit) CTC top=OCR1A
    //         || ||+++ CS12:CS11:CS10 010 clkI/O (�O�u�����Ȃ�)
//    TCCR1B = 0b11000010;              // �L���v�`��: capture: LPF ON(0),1->0edge(1), CTC-A, prescale 8

    TCCR1B =    (1<<ICNC1)  | (1<<ICES1)|
              (0<<WGM13)    | (0<<WGM12)|
              (0<<CS12) | (1<<CS11) | (0<<CS10);

    // �J�E���^������
    TCNT1 = 0x0000;
    OCR1A = 0xFFFF;
    OCR1B = 0xFFFF;

    //==========================================================//
    //   �O�����荞�ݐݒ�                                       //
    //==========================================================//
    // �O�����荞�݁i���g�p)
    GIMSK = 0x00;
    MCUCR = 0x00;

    // �R���p���[�^���g�p
    ACSR=0x80;


    // �^�C�}�[1�L���v�`���[��������
    TIMSK |= (1<<ICF1);
}

/****************************************
* �ԊO�������R����M�f�[�^���M�@�@�@�@  *
****************************************/
#if IR_USE_XMIT
void xmit_remocon_send(void)
{
    xmitstr(PSTR("0x"));
    xmit_hex(DispRemData.DATA_BYTE[0]);
    xmit_hex(DispRemData.DATA_BYTE[1]);
    xmit_hex(DispRemData.DATA_BYTE[2]);
    xmit_hex(DispRemData.DATA_BYTE[3]);
    xmitstr(PSTR("\n\r"));
}
#endif


/****************************************
* ���Z�b�g                              *
****************************************/
void remocon_reset(void)
{
    cli();          // �S�̊����s����

    DispRemData.DATA_LONG = 0;
    RemData.DATA_LONG = 0;

    ReqFlag.DispCode = 1;                   // �R�[�h�\���v���Z�b�g
    BitCount = 0;                           // ��M�J�E���g�N���A
    ReqFlag.RmStatus = RM_INTIAL;
    IR_CAPT_FALL();                         // ����������쓮�ɕύX(ICP�̌��o�G�b�W��L)

    sei();          // �S�̊�������
}


/****************************************
* ���C��    �@�@�@�@�@�@�@�@�@�@�@�@�@  *
****************************************/
int main(void)
{
    WORD remo_data;

    cli(); // ���荞�݋֎~

    io_init();
    // LED�̕\���Ŏg�p
    timer0_init();
    // �ԊO�������R���M����M�����Ŏg�p
    timer1_init();

    // 2313�W��UART�̏�����
#if IR_USE_XMIT
    uart2313_init();
#endif

    /*---------------*/
    flag = REQ_DATA;
    remo_data = 0;

    IR_CAPT_FALL();     // ����������쓮�ɕύX
    IR_CAPT_CLEAR();    // �L���v�`���[�����v���N���A

    motor1_stop();
    motor2_stop();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);// �p���[�_�E�����[�h��ݒ�

    sei();          // �S�̊�������

#if IR_USE_XMIT
    xmitstr(PSTR("Ready OK\n\r"));
#endif

    while (1)
    {
        if (ReqFlag.DispCode) // �R�[�h�\���v���L��
        {
            if (DispRemData.DATA_WORD[0] != MAKER_CODE)
            {
                flag = MAKER_CODE_ERROR;
#if IR_USE_XMIT
                xmitstr(PSTR("Maker Code ERR - "));
                xmit_remocon_send();
#endif
                remocon_reset();
            }

            else if (DispRemData.DATA_WORD[1] != remo_data)
            {
                remo_data = DispRemData.DATA_WORD[1];
                flag = RECEIVE_DATA;

                switch (remo_data)
                {

                case LEFT:          //����
                    motor1_stop();
                    motor2_stop();
                    motor2_forward();
#if IR_USE_XMIT
                    xmitstr(PSTR("Left - "));
#endif
                    break;

                case LEFTTURN:      // ����]
                    motor1_stop();
                    motor2_stop();
                    motor1_reverse();
                    motor2_forward();
#if IR_USE_XMIT
                    xmitstr(PSTR("Left turn - "));
#endif
                    break;

                case LEFTBACK:      // �㍶��
                    motor1_stop();
                    motor2_stop();
                    motor1_reverse();
#if IR_USE_XMIT
                    xmitstr(PSTR("Left Back - "));
#endif
                    break;

                    // -----------------------------------
                case RIGHT:         // �E��
                    motor1_stop();
                    motor2_stop();
                    motor1_forward();
#if IR_USE_XMIT
                    xmitstr(PSTR("Right - "));
#endif
                    break;

                case RIGHTTURN:     // �E��]
                    motor1_stop();
                    motor2_stop();
                    motor1_forward();
                    motor2_reverse();
#if IR_USE_XMIT
                    xmitstr(PSTR("Right turn - "));
#endif
                    break;

                case RIGHTBACK:     // ��E��
                    motor1_stop();
                    motor2_stop();
                    motor2_reverse();
#if IR_USE_XMIT
                    xmitstr(PSTR("Right Back - "));
#endif
                    break;

                case FORWARD:       // �O�i
                    motor1_stop();
                    motor2_stop();
                    motor1_forward();
                    motor2_forward();
#if IR_USE_XMIT
                    xmitstr(PSTR("Forward - "));
#endif
                    break;

                case BACK:          // ��i
                    motor1_stop();
                    motor2_stop();
                    motor1_reverse();
                    motor2_reverse();
#if IR_USE_XMIT
                    xmitstr(PSTR("Back - "));
#endif
                    break;

                case BRAKE:
                    motor1_brake();
                    motor2_brake();
#if IR_USE_XMIT
                    xmitstr(PSTR("Brake - "));
#endif
                    remocon_reset();
                    break;

                case RESET:
                    motor1_stop();
                    motor2_stop();
#if IR_USE_XMIT
                    xmitstr(PSTR("Brake - "));
#endif
                    remocon_reset();
                    break;

                default:
#if IR_USE_XMIT
                    xmitstr(PSTR("Not Assign Code - "));
#endif
                    break;
                }
#if IR_USE_XMIT
                xmit_remocon_send();
#endif
            }
            else
            {
                flag = REPEAT_DATA;
#if IR_USE_XMIT
                xmitstr(PSTR("Repeat Data - "));
                xmit_remocon_send();
#endif
            }

            ReqFlag.DispCode = 0;
        }

        if (ReqFlag.DispFormat) // �����R���t�H�[�}�b�g�\���v��
        {
            flag = REQ_DATA;
            ReqFlag.DispFormat = 0;
#if IR_USE_XMIT
            xmitstr(PSTR("Request OK\n\r"));
#endif
        }

        if (ReqFlag.ErrDisp) // �G���[�\��
        {
            flag = RECEIVE_ERR_DATA;
            ReqFlag.ErrDisp = 0;
#if IR_USE_XMIT
            xmitstr(PSTR("Receive Data ERR\n\r"));
#endif
            remocon_reset();
        }

        // �X���[�v
        sleep_mode();


    } // �������[�v

    return 0;
}
