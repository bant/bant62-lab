#ifndef __I2C__H__
#define __I2C__H__

#define TWI_FREQ 400000L

#define     TWPS_1      0       // TWIプリスケーラ分周値 1
#define     TWPS_4      1       // TWIプリスケーラ分周値 4
#define     TWPS_16     2       // TWIプリスケーラ分周値 16
#define     TWPS_64     3       // TWIプリスケーラ分周値 64

#define     rgSCond _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) // スタート・コンディション発行
#define     rgPCond _BV(TWINT) | _BV(TWSTO) | _BV(TWEN) // ストップ・コンディション発行
#define     rgClrTWInt _BV(TWINT) | _BV(TWEN)           // TWINT割り込み要因フラグのクリア

void    I2CMsInit(void);
uint8_t twiSendStartCondition( void );
void twiSendEndCondition( void );
uint8_t twiSendAddress( uint8_t adr );
uint8_t twiSendByte( uint8_t data );
uint8_t twiReceiveByte( uint8_t *data ,uint8_t ack );

#endif
