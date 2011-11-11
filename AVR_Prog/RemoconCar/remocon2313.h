/****************************************
									*
****************************************/

#ifndef REMOCON2313_H
#define REMOCON2313_H

// globa defines include
#include "global.h"

#define IR_USE_XMIT 1


/*==================================*/
/* �e��萔�錾						*/
/*==================================*/

/* �����R�����[�J�[�錾				*/
/*----------------------------------*/
#define NEC_BIT_COUNT	32


/* ��M�J�ڏ�Ԑ錾					*/
/*----------------------------------*/
enum { RM_INTIAL=0, RM_READER_L, RM_READER_H, RM_DATA, RM_STOP };

enum { POWER_DOWN=0, REQ_DATA, RECEIVE_DATA, REPEAT_DATA, RECEIVE_ERR_DATA, MAKER_CODE_ERROR };

/*==================================*/
/* �f�[�^�^�錾						*/
/*==================================*/

/* �����R���M���J�ڃf�[�^�^�錾		*/
/*----------------------------------*/
typedef struct {
	BYTE b0:1;
	BYTE b1:1;
	BYTE RmStatus:3;	// �����R����M���
	BYTE ErrDisp:1;		// ����`�f�[�^��M
	BYTE DispFormat:1;	// �����R���t�H�[�}�b�g�\���v��
	BYTE DispCode:1;	// �R�[�h�\���v��
} FLAGS;

/* �����R���M���f�[�^�^�錾			*/
/*----------------------------------*/
typedef struct {
	BYTE L4:4;
	BYTE U4:4;
} NIBBLE;

typedef union {
	ULONG 		DATA_LONG;
	WORD		DATA_WORD[2];
	BYTE		DATA_BYTE[4];
	NIBBLE		DATA_NIBBLE[4];
} L4BYTE;

/*==================================*/
/* �}�N���֐�						*/
/*==================================*/
#define IR_CAPT_TEST()	TCCR1B & _BV(ICES1)		/* Rx: Check which edge generated the capture interrupt */
#define IR_CAPT_RISE()	TCCR1B |= _BV(ICES1)	/* Rx: Set captureing is triggered on rising edge */
#define IR_CAPT_FALL()	TCCR1B &= ~_BV(ICES1)	/* Rx: Set captureing is triggered on falling edge */
#define IR_CAPT_CLEAR()	TIFR |= _BV(ICF1)		/* Rx: Enable captureing interrupt */
#define IR_CAPT_ENA()	TIMSK |= _BV(ICIE1)		/* Rx: Enable captureing interrupt */
#define	IR_CAPT_REG()	ICR1					/* Rx: Returns the value in capture register */
#define IR_CAPT_DIS()	TIMSK &= ~_BV(ICIE1)	/* Tx && Rx: Disable captureing interrupt */

/* no PWM motor control */
#define motor1_stop()		PORTB &= ~_BV(PB3); PORTB &= ~_BV(PB4)
#define motor1_forward()	PORTB |= _BV(PB3); PORTB &= ~_BV(PB4)
#define motor1_reverse()	PORTB &= ~_BV(PB3); PORTB |= _BV(PB4)
#define motor1_brake()		PORTB |= _BV(PB3); PORTB |= _BV(PB4)

#define motor2_stop()		PORTD &= ~_BV(PD3); PORTD &= ~_BV(PD4)
#define motor2_forward()	PORTD |= _BV(PD3); PORTD &= ~_BV(PD4)
#define motor2_reverse()	PORTD &= ~_BV(PD3); PORTD |= _BV(PD4)
#define motor2_brake()		PORTD |= _BV(PD3); PORTD |= _BV(PD4)

/*==================================*/
/* LED�̕\���}�N���֐�				*/
/*==================================*/
#define LED_ON()	PORTB |= _BV(PB1)
#define LED_OFF()	PORTB &= ~_BV(PB1)

/*======================================*/
/*	�f�W�b�g�ōw�����������R���̏��	*/
/*======================================*/

#define DIGIT
//#define TOSHIBA
//#define REMOCON

#ifdef DIGIT 
// �f�W�b�g�̃����R��
#define	MAKER_CODE		0xFF00		// �@��R�[�h:0x00

#define	NO0_BUTTON		0xFF00		// �R�}���h:0x00
#define	NO1_BUTTON		0xFE01		// �R�}���h:0x01
#define	NO2_BUTTON		0xFD02		// �R�}���h:0x02
#define	NO3_BUTTON		0xFC03		// �R�}���h:0x03
#define	NO4_BUTTON		0xFB04		// �R�}���h:0x04
#define	NO5_BUTTON		0xFA05		// �R�}���h:0x05
#define	NO6_BUTTON		0xF906		// �R�}���h:0x06
#define	NO7_BUTTON		0xF807		// �R�}���h:0x07
#define	NO8_BUTTON		0xF708		// �R�}���h:0x08
#define	NO9_BUTTON		0xF609		// �R�}���h:0x09

#define	NO10_BUTTON		0xF50A		// �R�}���h:0x0A
#define	NO11_BUTTON		0xF40B		// �R�}���h:0x0B
#define	NO12_BUTTON		0xF30C		// �R�}���h:0x0C

#define	DOT_BUTTON		0xF40B		// �R�}���h:0x0B
#define	START_BUTTON	0xF30C		// �R�}���h:0x0C
#define	ESC_BUTTON		0xF20D		// �R�}���h:0x0D
#define	ENTER_BUTTON	0xF00F		// �R�}���h:0x0F

#define POWER_BUTTON	0xEF10		// �R�}���h:0x10
#define FLASH_BUTTON 	0xEE11		// �R�}���h:0x11
#define MENU_BUTTON		0xED12		// �R�}���h:0x12
#define SET_BUTTON 		0xEC13		// �R�}���h:0x13
#define OSD_BUTTON 		0xEB14		// �R�}���h:0x14
#define TEXT_BUTTON 	0xEA15		// �R�}���h:0x15
#define L1_BUTTON 		0xE916		// �R�}���h:0x16
#define L2_BUTTON 		0xE817		// �R�}���h:0x17
#define L3_BUTTON 		0xE718		// �R�}���h:0x18
#define L4_BUTTON 		0xE619		// �R�}���h:0x19

//==== �{�^���ݒ�
#define	LEFT			MENU_BUTTON
#define	LEFTTURN		L1_BUTTON
#define	LEFTBACK		NO1_BUTTON

#define	FORWARD			SET_BUTTON
#define	BRAKE			L2_BUTTON 
#define	BACK			NO2_BUTTON

#define	RIGHT			OSD_BUTTON
#define	RIGHTTURN		L3_BUTTON
#define	RIGHTBACK		NO3_BUTTON

#define	RESET			POWER_BUTTON


#else
// ���Ńe���r�����R��
#define MAKER_CODE		0xBF40 		// �@��R�[�h  0x40BF

#define	NO0_BUTTON		0xFF00		// �R�}���h:0x00
#define	NO1_BUTTON		0xFE01		// �R�}���h:0x01
#define	NO2_BUTTON		0xFD02		// �R�}���h:0x02
#define	NO3_BUTTON		0xFC03		// �R�}���h:0x03
#define	NO4_BUTTON		0xFB04		// �R�}���h:0x04
#define	NO5_BUTTON		0xFA05		// �R�}���h:0x05
#define	NO6_BUTTON		0xF906		// �R�}���h:0x06
#define	NO7_BUTTON		0xF807		// �R�}���h:0x07
#define	NO8_BUTTON		0xF708		// �R�}���h:0x08
#define	NO9_BUTTON		0xF609		// �R�}���h:0x09
#define	NO10_BUTTON		0xF50A		// �R�}���h:0x0A
#define	NO11_BUTTON		0xF40B		// �R�}���h:0x0B
#define	NO12_BUTTON		0xF30C		// �R�}���h:0x0C

#define	TV_POWER_BUTTON	0xED12		// �R�}���h:0x12

//==== �{�^���ݒ�
#define	LEFT			NO1_BUTTON
#define	LEFTTURN		NO4_BUTTON
#define	LEFTBACK		NO7_BUTTON

#define	FORWARD			NO2_BUTTON
#define	BRAKE			NO5_BUTTON 
#define	BACK			NO8_BUTTON

#define	RIGHT			NO3_BUTTON
#define	RIGHTTURN		NO6_BUTTON
#define	RIGHTBACK		NO9_BUTTON

#define	RESET			TV_POWER_BUTTON


#endif

#endif
