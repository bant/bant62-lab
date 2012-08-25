#ifndef __MAIN__H__
#define __MAIN__H__

#define		TIMER2_COUNT	64L					// 64us (タイマー2の1カウント時間)
#define		TIMER2_CYCLE	10000UL				// 10000us (タイマ2周期)
#define		TIMER2_CMPA		((TIMER2_CYCLE / TIMER2_COUNT) - 1)

#define IR_DDR		DDRB
#define IR_PORT     PORTB
#define IR_SENSOR	PB0

#define SW_DDR		DDRD
#define SW_PORT     PORTD
#define SW_IN     	PIND

#define LED_DDR		DDRB
#define	LED_PORT	PORTB
#define	LED			PB2

#define SW1			PD4
#define SW2			PD5
#define SW3			PD6
#define SW4			PD7

#define SW_1		0x01
#define SW_2		0x02
#define SW_3		0x04
#define SW_4		0x08

#define MAX_LCD_NO	3
#define MAX_LEN 10

typedef enum { NO_IR, SELECT_CH, TUNE_CH } IrMode;

#endif
