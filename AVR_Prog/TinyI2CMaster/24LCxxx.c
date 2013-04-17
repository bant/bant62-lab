//========================================================================
// File Name    : 24LCxxx.c
//
// Title        : 24LCxxx �������h���C�o
// Revision     : 0.1
// Notes        :
// Target MCU   : AVR ATtiny series
// Tool Chain   : AVR toolchain Ver3.4.1.1195
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/04/13   �΂��      ����J�n
// 2013/04/14   �΂��      Ver0.1���슮��
//------------------------------------------------------------------------
// This code is distributed under Apache License 2.0 License
//		which can be found at http://www.apache.org/licenses/
//========================================================================

/* Includes -------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/delay.h>
#include "TinyI2CMaster.h"
#include "24LCxxx.h"

/* local define ---------------------------------------------------------*/
/* local typedef --------------------------------------------------------*/
/* local macro ----------------------------------------------------------*/
/* local variables ------------------------------------------------------*/
/* local function prototypes --------------------------------------------*/

/* [��������\�[�X] ==================================================== */

//========================================================================
// 1�o�C�g�������݊֐�
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : 24LCXXX�̃X���[�u�A�h���X
//       uint16_t mem_addr       : �������ރ������A�h���X
//       uint8_t data            : �������ރf�[�^
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t I2C_24LCXXX_byte_write( uint8_t slave_7bit_addr, uint16_t mem_addr, uint8_t data )
{
    uint8_t buf[3];

    /* �������ރA�h���X�̓o�^ */
    buf[0] = (0xff00 & mem_addr)>>8;        // Read Address High byte set
    buf[1] = (0x00ff & mem_addr);           // Read Address Low  byte set
    /* �������݃f�[�^�̓o�^ */
    buf[2] = data;

    return TinyI2C_write_data(slave_7bit_addr, buf, sizeof(buf), SEND_STOP);
}


//========================================================================
// n�o�C�g�������݊֐�
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : 24LCXXX�̃X���[�u�A�h���X
//       uint16_t mem_addr       : �������ރ������A�h���X
//       uint8_t *data           : �������ރf�[�^
//       int size                : �������ރf�[�^�T�C�Y(�o�C�g�P��)
// �ߒl: 0=����I���@0xff�̓f�[�^�I�[�o�[�t���[�A����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t I2C_24LCXXX_nbyte_write( uint8_t slave_7bit_addr, uint16_t mem_addr, void *data, int size )
{
    register int    i;
    uint8_t buf[3];
    uint8_t status;
    uint8_t *p;

//    status = TINYI2C_NO_ERROR;
    p = data;
    for ( i = 0; i < size; i++ )
    {
        /* �������ރA�h���X�̓o�^ */
        buf[0] = (0xff00 & mem_addr)>>8;        // Read Address High byte set
        buf[1] = (0x00ff & mem_addr);           // Read Address Low  byte set
        /* �������݃f�[�^�̓o�^ */
        buf[2] = *p++;

        status = TinyI2C_write_data(slave_7bit_addr, buf, sizeof(buf), SEND_STOP);
        if(status != TINYI2C_NO_ERROR)
        {
            return status;
        }
        _delay_ms(5);
        if( ++mem_addr >= MAXADR_24LCXXX )              // Address counter +1
        {
           return 0xff;                                 // Address range over
        }
    }

    return status;
}


//========================================================================
// �y�[�W�P�ʂ̏������݊֐�
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : 24LCXXX�̃X���[�u�A�h���X
//       uint16_t mem_addr       : �������ރ������A�h���X
//       uint8_t *data           : �������ރf�[�^
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t I2C_24LCXXX_page_write( uint8_t slave_7bit_addr, uint16_t mem_addr, uint8_t *data )
{
    register int    i;
    uint8_t buf[PAGE_SIZE_24LCXXX+2];

    /* �������ރA�h���X�̓o�^ */
    buf[0] = (0xff00 & mem_addr)>>8;        // Read Address High byte set
    buf[1] = (0x00ff & mem_addr);           // Read Address Low  byte set

    /* �������݃f�[�^�̓o�^ */
    for (i=0;i<PAGE_SIZE_24LCXXX;i++)
    {
        buf[i+2] = data[i];
    }

    return TinyI2C_write_data(slave_7bit_addr, buf, sizeof(buf), SEND_STOP);
}


//========================================================================
// n�o�C�g�Ǎ��֐�
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : 24LCXXX�̃X���[�u�A�h���X
//       uint16_t mem_addr       : �ǂݍ��ރ������A�h���X
//       uint8_t *data           : �ǂݍ��ރf�[�^
//       int size                : �ǂݍ��ރf�[�^�T�C�Y(�o�C�g�P��)
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t I2C_24LCXXX_read( uint8_t slave_7bit_addr, uint16_t mem_addr, void* data, int size )
{
    uint8_t buf[2];
    uint8_t status;

    /* �ǂݍ��ރA�h���X�̓o�^ */
    buf[0] = (0xff00 & mem_addr)>>8;            // Read Address High byte set
    buf[1] = (0x00ff & mem_addr);           // Read Address Low  byte set
    status = TinyI2C_write_data(slave_7bit_addr, buf, sizeof(buf), NO_SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    return TinyI2C_read_data(slave_7bit_addr, data, size, SEND_STOP);
}

/* =====================================================[�����܂Ń\�[�X] */
