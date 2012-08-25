//*******************************************************
//
//  I2C���䃂�W���[��
//
//*******************************************************

#include    <stdlib.h>
#include    <string.h>
#include    <avr/io.h>
#include    <stdint.h>
#include    <stddef.h>
#include    <avr/pgmspace.h>

#include "i2c.h"

//void    rs_printf_P(const char *format);

//-----------------------------------------------------------
//I2C�}�X�^������
//-----------------------------------------------------------
void    I2CMsInit(void)
{
//    rs_printf_P(PSTR("I2CMsInit\r\n"));

    // CPU=16MHz,�r�b�g���[�g400kHz�̂Ƃ� TWBR=2, TWPS=0
    TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
//  TWBR = 12;                           // TWI�r�b�g���[�g�l�ݒ�
//  TWBR = 128;                         // TWI�r�b�g���[�g�l�ݒ�
    TWSR = TWPS_1;                      // TWI�r�b�g���[�g�E�v���X�P�[���l�ݒ�
//  TWSR = 0;                           // TWI�r�b�g���[�g�E�v���X�P�[���l�ݒ�
    TWCR = _BV(TWEN);                   // TWI enable
}

/*****************************************************************************
*                               TWI�J�n�������M
* �����Fvoid
* �ߒl�Fuint8_t 0:�J�n�������M����
*               1:�đ��J�n�������M����
*              99:NG
*****************************************************************************/
uint8_t twiSendStartCondition( void )
{
//  rs_printf_P(PSTR("twiSendStartCondition\r\n"));

    /* �J�n�������M                     */
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);

    /* ���M����(TWINT������)�܂ő҂�    */
    while ( !( TWCR & _BV(TWINT) ) );

    /* TWI�X�e�[�^�X(Bit7:3)����        */
    uint8_t sts = TWSR;
    sts &= 0xf8;
    if ( sts == 0x08 )                          /* �J�n�������M     */
    {
        return ( 0 );
    }
    else if ( sts == 0x10 )                     /* �đ��J�n�������M */
    {
        return ( 1 );
    }

    return ( 99 );
}

/*****************************************************************************
*                               TWI�I���������M
* �����Fvoid
* �ߒl�Fvoid
*****************************************************************************/
void twiSendEndCondition( void )
{
//  rs_printf_P(PSTR("twiSendEndCondition\r\n"));

    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
}

/*****************************************************************************
*                       TWI �A�h���X���M
* �����Fuint8_t adr �A�h���X(7bit) + R/W(1bit 0:Write 1:Read)
* �ߒl�Fuint8_t     0:OK !0:NG
*****************************************************************************/
uint8_t twiSendAddress( uint8_t adr )
{
//  rs_printf_P(PSTR("twiSendAddress\r\n"));

    /* ���M                             */
    TWDR = adr;
    TWCR = _BV(TWINT) | _BV(TWEN);

    /* ���M����(TWINT������)�܂ő҂�    */
    while ( !( TWCR & _BV(TWINT) ) );

    /* TWI�X�e�[�^�X(Bit7:3)����        */
    uint8_t sts = TWSR;
    sts &= 0xf8;
    if ( sts == 0x20 )                      /* SLA+W���M NACK��M       */
    {
        return ( 1 );
    }
    else if ( sts == 0x48 )                 /* SLA+R���M NACK��M       */
    {
        return ( 2 );
    }
    else if ( sts == 0x18 )                 /* SLA+W���M ACK��M        */
    {
        if ( ( adr & 0x01 ) == 0 )
        {
            return ( 0 );
        }
        return ( 3 );
    }
    else if ( sts == 0x40 )                 /* SLA+R���M ACK��M        */
    {
        if ( ( adr & 0x01 ) != 0 )
        {
            return ( 0 );
        }
        return ( 4 );
    }

    return ( 99 );
}

/*****************************************************************************
*                               TWI 1�o�C�g���M
* �����Fuint8_t data ���M�f�[�^
* �ߒl�Fuint8_t      0:ACK 1:NACK 99:NG
*****************************************************************************/
uint8_t twiSendByte( uint8_t data )
{
//  rs_printf_P(PSTR("twiSendByte\r\n"));

    /* ���M                             */
    TWDR = data;
    TWCR = _BV(TWINT) | _BV(TWEN);

    /* ���M����(TWINT������)�܂ő҂�    */
    while ( !( TWCR & _BV(TWINT) ) );

    /* TWI�X�e�[�^�X(Bit7:3)����        */
    uint8_t sts = TWSR;
    sts &= 0xf8;
    if ( sts == 0x28 )                      /* �f�[�^ �o�C�g���M ACK��M        */
    {
        return ( 0 );
    }
    else if ( sts == 0x30 )                 /* �f�[�^ �o�C�g���M NACK��M       */
    {
        return ( 1 );
    }

    return ( 99 );
}

/*****************************************************************************
*                           TWI 1�o�C�g��M
* �����Fuint8_t *data ��M�f�[�^�i�[�̈�ւ̃|�C���^
*       uint8_t ack   0:ACK���M !0:NACK���M
* �ߒl�Fuint8_t       0:OK      !0:NG
*****************************************************************************/
uint8_t twiReceiveByte( uint8_t *data ,uint8_t ack )
{
//  rs_printf_P(PSTR("twiReceiveByte\r\n"));

    /* ��M�FACK/NACK�����ɉ�����TWEA�r�b�g���Z�b�g */
    if ( ack == 0 )
    {
        TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
    }
    else
    {
        TWCR = _BV(TWINT) | _BV(TWEN);
    }

    /* ��M����(TWINT������)�܂ő҂�    */
    while ( !( TWCR & _BV(TWINT) ) );

    /* ��M�f�[�^�擾                   */
    (*data) = TWDR;

    /* TWI�X�e�[�^�X(Bit7:3)����        */
    uint8_t sts = TWSR;
    sts &= 0xf8;
    if ( sts == 0x50 )                      /* �f�[�^ �o�C�g��M ACK��M        */
    {
        if ( ack == 0 )
        {
            return ( 0 );
        }
        return ( 1 );
    }
    else if ( sts == 0x58 )                 /* �f�[�^ �o�C�g���M NACK��M       */
    {
        if ( ack != 0 )
        {
            return ( 0 );
        }
        return ( 2 );
    }

    return ( 99 );
}
