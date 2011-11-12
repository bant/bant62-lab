/*
 * keyin.h
 *
 *  Created on: 2010/10/16
 *      Author: bant
 */

#ifndef KEYIN_H_
#define KEYIN_H_

//#include "keyin.h"
//#define FALSE 0
//#define TRUE 1
#define NUMERIC_MODE 0
#define ALFABET_MODE 1

#define ESC (0x1B)

#define RED_LED_ON()  (PORTB |=  (1<<PB0))
#define RED_LED_OFF() (PORTB &= ~(1<<PB0))
#define GREEN_LED_ON()  (PORTB |=  (1<<PB1))
#define GREEN_LED_OFF() (PORTB &= ~(1<<PB1))

typedef struct key_t_tag {
	uint8_t cmd;
	uint8_t stat;
	uint8_t filt;
} key_t;


/* prototypes */
extern void keyInit(void);
extern uint8_t getKeyCmd(void);
extern uint8_t getKeyState(void);
extern void setKeyMode (uint8_t data);
extern void timer0Init(void);

#endif /* KEYIN_H_ */
