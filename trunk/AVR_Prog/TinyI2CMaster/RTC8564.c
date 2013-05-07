//========================================================================
// File Name    : RTC8564.c
//
// Title        : Seiko Epson RTC-8564 �h���C�o
// Revision     : 0.2
// Notes        :
// Target MCU   : AVR ATtiny series
// Tool Chain   : AVR toolchain Ver3.4.1.1195
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/04/13   �΂��      ����J�n
// 2013/04/14   �΂��      Ver0.1���슮��
// 2013/04/26   �΂��      ���W�X�^����֐��ǉ�&�ύX
// 2013/05/07   �΂��      TIMER & ALARM�̃o�N�C�� Ver0.2
//------------------------------------------------------------------------
// This code is distributed under Apache License 2.0 License
//		which can be found at http://www.apache.org/licenses/
//========================================================================

/* Includes ------------------------------------------------------------*/
#include <avr/io.h>
#include "delay.h"
#include "TinyI2CMaster.h"
#include "rtc8564.h"

/* local define --------------------------------------------------------*/
/* local typedef -------------------------------------------------------*/
/* local macro ---------------------------------------------------------*/
/* local variables -----------------------------------------------------*/
/* local function prototypes -------------------------------------------*/
static uint8_t dec2bcd(uint8_t d);
static uint8_t bcd2dec(uint8_t b);

/* [��������\�[�X] ==================================================== */

//========================================================================
//  convert bin to BCD
//------------------------------------------------------------------------
// ����:
// �ߒl:
//========================================================================
static uint8_t dec2bcd(uint8_t d)
{
    return ((d/10 * 16) + (d % 10));
}

//========================================================================
//  convert BCD to bin
//------------------------------------------------------------------------
// ����:
// �ߒl:
//========================================================================
static uint8_t bcd2dec(uint8_t b)
{
    return ((b/16 * 10) + (b % 16));
}

//========================================================================
//  �j������
//------------------------------------------------------------------------
// ����: int nYear  : �N
//       int nMonth : ��
//       int nDay   : ��
// �ߒl: 0:��, ... 6:�y
//========================================================================
int getWeekday( int nYear, int nMonth, int nDay )
{
    int nWeekday, nTmp;

    if (nMonth == 1 || nMonth == 2)
    {
        nYear--;
        nMonth += 12;
    }

    nTmp = nYear/100;
    nWeekday = (nYear + (nYear >> 2) - nTmp + (nTmp >> 2) + (13 * nMonth + 8)/5 + nDay) % 7;

    return nWeekday;
}

//========================================================================
// ������(�A�v���P�[�V�����}�j���A�� P-29)
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_init( void )
{
    uint8_t data[18];

    data[0] = 0x00;          // write reg addr 00
    data[1] = 0x20;          // 00 Control 1, STOP=1
    data[2] = 0x00;          // 01 Control 2
    data[3] = 0x00;          // 02 Seconds
    data[4] = 0x00;          // 03 Minutes
    data[5] = 0x00;          // 04 Hours
    data[6] = 0x01;          // 05 Days
    data[7] = 0x01;          // 06 Weekdays
    data[8] = 0x01;          // 07 Months
    data[9] = 0x01;          // 08 Years
    data[10] = 0x80;         // 09 Minutes Alarm
    data[11] = 0x80;         // 0A Hours Alarm
    data[12] = 0x80;         // 0B Days Alarm
    data[13] = 0x80;         // 0C Weekdays Alarm
    data[14] = 0x00;         // 0D CLKOUT
    data[15] = 0x00;         // 0E Timer control
    data[16] = 0x00;         // 0F Timer
    data[17] = 0x00;         // 00 Control 1, STOP=0(START)

    return TinyI2C_write_data(I2C_ADDR_RTC8564, data, sizeof(data), SEND_STOP);
}

//========================================================================
// �d���������̏���(�A�v���P�[�V�����}�j���A�� P-29)
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_power_on( void )
{
    wait_sec(1);

    return RTC8564_init();

}

//========================================================================
// �o�b�N�A�b�v���A����(�A�v���P�[�V�����}�j���A�� P-30)
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_backup_return( void )
{
    uint8_t data;
    uint8_t status;

    // Seconds ���W�X�^
    status = TinyI2C_readReg( I2C_ADDR_RTC8564, 0x02, &data );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    if( data & _BV(7) ) /* VL�`�F�b�N: �d���~��?*/
    {
        return RTC8564_power_on();
    }

    return status;
}

//========================================================================
// ���v�E�J�����_�̐ݒ�(�A�v���P�[�V�����}�j���A�� P-30)
//------------------------------------------------------------------------
// ����: RTC_TIME *time: �ݒ肷������f�[�^
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_adjust( const RTC_TIME *time )
{
    uint8_t data[8];
    uint8_t status;

    // RTC8564 ��~
    status = RTC8564_stop();
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }
    //
    data[0] = 0x02;          // ���W�X�^�e�[�u�� �A�h���X(�b)
    data[1] = dec2bcd(time->sec);    // �b
    data[2] = dec2bcd(time->min);    // ��
    data[3] = dec2bcd(time->hour);   // ��
    data[4] = dec2bcd(time->day);    // ��
    data[5] = dec2bcd(time->wday);   // �j��
    data[6] = dec2bcd(time->month);  // ��

    if (time->year >= 2100)
    {
        data[7] = dec2bcd(time->year - 2100);   // �N
		data[6] |= 0x80;						// ���I�t���b�O�Z�b�g
    }
    else
    {
        data[7] = dec2bcd(time->year - 2000);   // �N
    }

    status = TinyI2C_write_data(I2C_ADDR_RTC8564, data, sizeof(data), SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    // RTC8564 �X�^�[�g
    return RTC8564_start();

}

//========================================================================
// ���v�E�J�����_�̓ǂݏo��(�A�v���P�[�V�����}�j���A�� P-30)
//------------------------------------------------------------------------
// ����: RTC_TIME *time: �擾��������̃f�[�^
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_now( RTC_TIME *time )
{
    uint8_t data[7];
    uint8_t status;

    /* �ǂݍ��ރA�h���X�̓o�^ */
    data[0] = 0x02;          // ���W�X�^�e�[�u�� �A�h���X(�b)
    status = TinyI2C_write_data(I2C_ADDR_RTC8564, data, 1, NO_SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }
    status = TinyI2C_read_data(I2C_ADDR_RTC8564, data, 7, SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    time->sec  = bcd2dec( data[0] & 0x7F );
    time->min  = bcd2dec( data[1] & 0x7F );
    time->hour = bcd2dec( data[2] & 0x3F );
    time->day  = bcd2dec( data[3] & 0x3F );
    time->wday = bcd2dec( data[4] & 0x07 );
    time->month =bcd2dec( data[5] & 0x1F );

    if ( data[5] & 0x80 ) /* ���I�t���O */
    {
        time->year = bcd2dec( data[6] ) + 2100;
    }
    else
    {
        time->year = bcd2dec( data[6] ) + 2000;
    }

    return status;
}

//========================================================================
//  �^�C�}���荞�݋@�\�ݒ�(�A�v���P�[�V�����}�j���A�� P-31)
//------------------------------------------------------------------------
// ����: uint8_t cycle   : 0�Ȃ��x����̊��荞�݁@��0�Ȃ�J��Ԃ����荞��
//       uint8_t int_out : 0�Ȃ�/INT "LOW"���x�����荞�ݏo�͕s����
//                         ��0�Ȃ�/INT "LOW"���x�����荞�ݏo�͋���
//       enum RTC_TIMER_TIMING sclk
//          : �\�[�X�N���b�N�̎w��(244.14us[0] 15.625ms[1] 1sec[2] 1min[3])
//       uint8_t count : �J�E���g�l�̎w��(1-255)
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_setTimer( enum RTC_TIMER_TIMING sclk, uint8_t count, uint8_t cycle, uint8_t int_out )
{
    uint8_t data[2];
    uint8_t status;

    // �^�C�}���荞�ݒ�~(TE = 0)
    status = TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x0E, _BV(7) );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    // ���荞�݉���( TIE=0, TF=0 )
    status = TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x01, _BV(2) | _BV(0) );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    if ( cycle )
    {
        // �J��Ԃ����荞��( TI/TP = 1 )
        status = TinyI2C_setRegBit( I2C_ADDR_RTC8564, 0x01, _BV(4) );
    }
    else
    {
        // ��x����̊��荞��( TI/TP = 0 )
        status = TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x01, _BV(4) );
    }

    if ( int_out )
    {
        // /INT "LOW"���x�����荞�ݏo�͋���( TIE = 1 )
        status = TinyI2C_setRegBit( I2C_ADDR_RTC8564, 0x01, _BV(0) );
    }
    else
    {
        // /INT "LOW"���x�����荞�ݏo�͕s����( TIE = 0 )
        status = TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x01, _BV(0) );
    }
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    // �^�C�}�J�E���g�_�E�������ݒ�
    status = TinyI2C_masksetRegBit( I2C_ADDR_RTC8564, 0x0E, _BV(1) | _BV(0), sclk );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    // �^�C�}�J�E���^�l�ݒ�
    data[0] = 0x0F;          // �^�C�}�[�̃��W�X�^�E�A�h���X
    data[1] = count;         // �J�E���g�_�E���^�C�}�l�ݒ�
    status = TinyI2C_write_data(I2C_ADDR_RTC8564, data, 2, SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    return TinyI2C_setRegBit( I2C_ADDR_RTC8564, 0x0E, _BV(7) );    // �^�C�}���荞�݋���(TE = 1)
}

//========================================================================
//  �^�C�}�X�g�b�v
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_stopTimer( void )
{
    uint8_t status;

	// �^�C�}���荞�ݒ�~(TE = 0)
	status = TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x0E, _BV(7) );
    if(status != TINYI2C_NO_ERROR)
    {
		return status;
	}

	// ���荞�݉�������уt���b�O�N���A( TIE=0, TF=0 )
	return TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x01, _BV(2) | _BV(0) );

}

//========================================================================
//  �^�C�}�N���A
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_clearTimer( void )
{
	return TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x01, _BV(2) );
}

//========================================================================
//  �A���[���ݒ�E�J�n
//------------------------------------------------------------------------
// ����: ALARM_TIME *alarm : �A���[���̐ݒ�f�[�^
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_setAlarm( ALARM_TIME *alarm )
{
    uint8_t data[5];
    uint8_t status;

    // �A���[�����荞�ݒ�~(AE = 1)
    data[0] = 0x09;         // Minute Alarm���W�X�^�E�A�h���X
    data[1] = 0x80;         // Minute Alarm (AE=1)
    data[2] = 0x80;         // Hour Alarm (AE=1)
    data[3] = 0x80;         // Day Alarm (AE=1)
    data[4] = 0x80;         // Week Day Alarm (AE=1)
    status = TinyI2C_write_data(I2C_ADDR_RTC8564, data, 5, SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    // ���荞�݉���( AIE=0, AF=0 )
    status = TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x01, _BV(3) | _BV(1) );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    data[0] = 0x09;              // ���A���[�����W�X�^�E�A�h���X
    // �����ݒ�
    data[1] = dec2bcd(alarm->min & 0x7F);
    if(alarm->min & 0x80)
    {
        data[1] |= 0x80;
    }

    // �����ݒ�
    data[2] = dec2bcd(alarm->hour & 0x7F);
    if(alarm->hour & 0x80)
    {
        data[2] |= 0x80;
    }

    // �����ݒ�
    data[3] = dec2bcd(alarm->day & 0x7F);
    if(alarm->day & 0x80)
    {
        data[3] |= 0x80;
    }

    // ���j���ݒ�
    data[4] = dec2bcd(alarm->wday & 0x7F);
    if(alarm->wday & 0x80)
    {
        data[4] |= 0x80;
    }

    status = TinyI2C_write_data(I2C_ADDR_RTC8564, data, 5, SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    // ���荞�݋���(AIE=1)
    return TinyI2C_setRegBit( I2C_ADDR_RTC8564, 0x01, _BV(1) );
}

//========================================================================
// �A���[���f�[�^�̓ǂݏo��(�A�v���P�[�V�����}�j���A�� P-30)
//------------------------------------------------------------------------
// ����: ALARM_TIME *alarm: �擾����A���[���̃f�[�^
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_getAlarm( ALARM_TIME *alarm )
{
    uint8_t data[4];
    uint8_t status;

    /* �ǂݍ��ރA�h���X�̓o�^ */
    data[0] = 0x09;          // ���W�X�^�e�[�u�� �A�h���X(�b)
    status = TinyI2C_write_data(I2C_ADDR_RTC8564, data, 1, NO_SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }
    status = TinyI2C_read_data(I2C_ADDR_RTC8564, data, 4, SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    alarm->min = bcd2dec( data[0] & 0x7F );
    if ( data[0] & 0x80 )
    {
        alarm->min |= 0x80;
    }

    alarm->hour = bcd2dec( data[1] & 0x3F );
    if ( data[1] & 0x80 )
    {
        alarm->hour |= 0x80;
    }

    alarm->day = bcd2dec( data[2] & 0x3F );
    if ( data[2] & 0x80 )
    {
        alarm->day |= 0x80;
    }

    alarm->wday = bcd2dec( data[3] & 0x07 );
    if ( data[3] & 0x80 )
    {
        alarm->wday |= 0x80;
    }

    return status;
}

//========================================================================
//  �A���[����~
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_stopAlarm( void )
{
    uint8_t data[5];
    uint8_t status;
    ALARM_TIME alarm;

	// ���荞�݉���( AIE=0, AF=0 )
	status = TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x01, _BV(3) | _BV(1) );
    if(status != TINYI2C_NO_ERROR)
    {
		return status;
	}

    status = RTC8564_getAlarm( &alarm );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    // �A���[�����荞�ݒ�~(AE = 1)
    data[0] = 0x09;         // Minute Alarm���W�X�^�E�A�h���X
    data[1] = dec2bcd(alarm.min  & 0x7F) | 0x80;         // Minute Alarm (AE=1)
    data[2] = dec2bcd(alarm.hour & 0x7F) | 0x80;         // Hour Alarm (AE=1)
    data[3] = dec2bcd(alarm.day  & 0x7F) | 0x80;         // Day Alarm (AE=1)
    data[4] = dec2bcd(alarm.wday & 0x7F) | 0x80;         // Week Day Alarm (AE=1)
    return TinyI2C_write_data(I2C_ADDR_RTC8564, data, 5, SEND_STOP);
}

//========================================================================
//  �A���[���N���A
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_clearAlarm( void )
{
	return TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x01, _BV(3) );
}

//========================================================================
//  CLKOUT�̐ݒ�
//------------------------------------------------------------------------
// ����: enum  RTC_CLKOUT_FREQ clkout: CLKOUT�̎��g��
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t RTC8564_setClkOut( enum  RTC_CLKOUT_FREQ clkout )
{
    if( clkout == FREQ_0 )
    {
        return TinyI2C_clearRegBit( I2C_ADDR_RTC8564, 0x0D, _BV(7) );
    }
    else
    {
        return TinyI2C_masksetRegBit( I2C_ADDR_RTC8564, 0x0D, _BV(7) | _BV(1) | _BV(0), _BV(7) | clkout );
    }
}

/* =====================================================[�����܂Ń\�[�X] */
