#ifndef __I2C__H__
#define __I2C__H__

#define TWI_FREQ 400000L

#define     TWPS_1      0       // TWI�v���X�P�[�������l 1
#define     TWPS_4      1       // TWI�v���X�P�[�������l 4
#define     TWPS_16     2       // TWI�v���X�P�[�������l 16
#define     TWPS_64     3       // TWI�v���X�P�[�������l 64

#define     rgSCond _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) // �X�^�[�g�E�R���f�B�V�������s
#define     rgPCond _BV(TWINT) | _BV(TWSTO) | _BV(TWEN) // �X�g�b�v�E�R���f�B�V�������s
#define     rgClrTWInt _BV(TWINT) | _BV(TWEN)           // TWINT���荞�ݗv���t���O�̃N���A

void    I2CMsInit(void);
uint8_t twiSendStartCondition( void );
void twiSendEndCondition( void );
uint8_t twiSendAddress( uint8_t adr );
uint8_t twiSendByte( uint8_t data );
uint8_t twiReceiveByte( uint8_t *data ,uint8_t ack );

#endif
