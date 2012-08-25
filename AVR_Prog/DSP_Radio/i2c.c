//*******************************************************
//
//  I2C制御モジュール
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
//I2Cマスタ初期化
//-----------------------------------------------------------
void    I2CMsInit(void)
{
//    rs_printf_P(PSTR("I2CMsInit\r\n"));

    // CPU=16MHz,ビットレート400kHzのとき TWBR=2, TWPS=0
    TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
//  TWBR = 12;                           // TWIビットレート値設定
//  TWBR = 128;                         // TWIビットレート値設定
    TWSR = TWPS_1;                      // TWIビットレート・プリスケーラ値設定
//  TWSR = 0;                           // TWIビットレート・プリスケーラ値設定
    TWCR = _BV(TWEN);                   // TWI enable
}

/*****************************************************************************
*                               TWI開始条件送信
* 引数：void
* 戻値：uint8_t 0:開始条件送信完了
*               1:再送開始条件送信完了
*              99:NG
*****************************************************************************/
uint8_t twiSendStartCondition( void )
{
//  rs_printf_P(PSTR("twiSendStartCondition\r\n"));

    /* 開始条件送信                     */
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);

    /* 送信完了(TWINTが立つ)まで待つ    */
    while ( !( TWCR & _BV(TWINT) ) );

    /* TWIステータス(Bit7:3)検査        */
    uint8_t sts = TWSR;
    sts &= 0xf8;
    if ( sts == 0x08 )                          /* 開始条件送信     */
    {
        return ( 0 );
    }
    else if ( sts == 0x10 )                     /* 再送開始条件送信 */
    {
        return ( 1 );
    }

    return ( 99 );
}

/*****************************************************************************
*                               TWI終了条件送信
* 引数：void
* 戻値：void
*****************************************************************************/
void twiSendEndCondition( void )
{
//  rs_printf_P(PSTR("twiSendEndCondition\r\n"));

    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
}

/*****************************************************************************
*                       TWI アドレス送信
* 引数：uint8_t adr アドレス(7bit) + R/W(1bit 0:Write 1:Read)
* 戻値：uint8_t     0:OK !0:NG
*****************************************************************************/
uint8_t twiSendAddress( uint8_t adr )
{
//  rs_printf_P(PSTR("twiSendAddress\r\n"));

    /* 送信                             */
    TWDR = adr;
    TWCR = _BV(TWINT) | _BV(TWEN);

    /* 送信完了(TWINTが立つ)まで待つ    */
    while ( !( TWCR & _BV(TWINT) ) );

    /* TWIステータス(Bit7:3)検査        */
    uint8_t sts = TWSR;
    sts &= 0xf8;
    if ( sts == 0x20 )                      /* SLA+W送信 NACK受信       */
    {
        return ( 1 );
    }
    else if ( sts == 0x48 )                 /* SLA+R送信 NACK受信       */
    {
        return ( 2 );
    }
    else if ( sts == 0x18 )                 /* SLA+W送信 ACK受信        */
    {
        if ( ( adr & 0x01 ) == 0 )
        {
            return ( 0 );
        }
        return ( 3 );
    }
    else if ( sts == 0x40 )                 /* SLA+R送信 ACK受信        */
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
*                               TWI 1バイト送信
* 引数：uint8_t data 送信データ
* 戻値：uint8_t      0:ACK 1:NACK 99:NG
*****************************************************************************/
uint8_t twiSendByte( uint8_t data )
{
//  rs_printf_P(PSTR("twiSendByte\r\n"));

    /* 送信                             */
    TWDR = data;
    TWCR = _BV(TWINT) | _BV(TWEN);

    /* 送信完了(TWINTが立つ)まで待つ    */
    while ( !( TWCR & _BV(TWINT) ) );

    /* TWIステータス(Bit7:3)検査        */
    uint8_t sts = TWSR;
    sts &= 0xf8;
    if ( sts == 0x28 )                      /* データ バイト送信 ACK受信        */
    {
        return ( 0 );
    }
    else if ( sts == 0x30 )                 /* データ バイト送信 NACK受信       */
    {
        return ( 1 );
    }

    return ( 99 );
}

/*****************************************************************************
*                           TWI 1バイト受信
* 引数：uint8_t *data 受信データ格納領域へのポインタ
*       uint8_t ack   0:ACK送信 !0:NACK送信
* 戻値：uint8_t       0:OK      !0:NG
*****************************************************************************/
uint8_t twiReceiveByte( uint8_t *data ,uint8_t ack )
{
//  rs_printf_P(PSTR("twiReceiveByte\r\n"));

    /* 受信：ACK/NACK応答に応じてTWEAビットをセット */
    if ( ack == 0 )
    {
        TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
    }
    else
    {
        TWCR = _BV(TWINT) | _BV(TWEN);
    }

    /* 受信完了(TWINTが立つ)まで待つ    */
    while ( !( TWCR & _BV(TWINT) ) );

    /* 受信データ取得                   */
    (*data) = TWDR;

    /* TWIステータス(Bit7:3)検査        */
    uint8_t sts = TWSR;
    sts &= 0xf8;
    if ( sts == 0x50 )                      /* データ バイト受信 ACK受信        */
    {
        if ( ack == 0 )
        {
            return ( 0 );
        }
        return ( 1 );
    }
    else if ( sts == 0x58 )                 /* データ バイト送信 NACK受信       */
    {
        if ( ack != 0 )
        {
            return ( 0 );
        }
        return ( 2 );
    }

    return ( 99 );
}
