/*
 * keyin.c
 *
 *  Created on: 2010/10/16
 *      Author: bant
 */

//----- Include Files ---------------------------------------------------------
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "keyin.h"


/* keyin globals */
volatile static key_t key_buf;
volatile static uint8_t mode = NUMERIC_MODE;


//----- Begin Code ------------------------------------------------------------

/**
 * タイマー0設定
 */
void timer0Init(void)
{
    // start timer0
    TCNT0 = 0;
    TCCR0B |= (1<<CS01)|(0<<CS00);		// set prescaler by 8 1MHz/256/8->2msec
    TIMSK |= (1<<TOIE0);

//	sei();
}

/**
 * キーボードのポート設定&初期化
 */

void keyInit(void)
{
    // 入力設定
    DDRA &= ~(1<<PA0);
    DDRA &= ~(1<<PA1);

    DDRD &= ~(1<<PD0);
    DDRD &= ~(1<<PD1);
//	DDRD &= ~(1<<PD2);
//	DDRD &= ~(1<<PD3);
    DDRD &= ~(1<<PD4);
    DDRD &= ~(1<<PD5);
    DDRD &= ~(1<<PD6);

    // プルアップ設定
    PORTA |= (1<<PA0);
    PORTA |= (1<<PA1);

    PORTD |= (1<<PD0);
    PORTD |= (1<<PD1);
//	PORTD |= (1<<PD2);
//	PORTD |= (1<<PD3);
    PORTD |= (1<<PD4);
    PORTD |= (1<<PD5);
    PORTD |= (1<<PD6);

//	timer0Init();
}

ISR(TIMER0_OVF_vect)
{
    static uint8_t process = 0;
    uint8_t a, b;

    process ^= 1;
    if (process)  	// process evry other interrupt ???
    {
        a = (~PIND & 0x73) | ((~PINA & (1<<PA0)) << 3) | ((~PINA & (1<<PA1)) << 1);
        if (a == key_buf.filt)
        {

            b = key_buf.stat;
            key_buf.stat = a;

            // 前キーと違い、しかもキーが押された
            b = (b ^ a) & a;
            if (b)
            {
                key_buf.cmd = b;
            }
        }
        key_buf.filt = a;
    } // end of		if (process & 0x01)
}

void setKeyMode (uint8_t data)
{
    cli();

    mode = data;

    if (mode == NUMERIC_MODE)
    {
        RED_LED_ON();
        GREEN_LED_OFF();
    }
    else
    {
        RED_LED_OFF();
        GREEN_LED_ON();
    }

    sei();
}

uint8_t getKeyCmd(void)
{
    uint8_t c;

    cli();
    c = key_buf.cmd;
    key_buf.cmd = 0;
    sei();

    return c;
}

uint8_t getKeyState(void)
{
    uint8_t c;

    cli();
    switch (key_buf.stat)
    {
    case 0x09:
        c = '0';
        break;

    case 0x11:
        c = '1';
        break;

    case 0x21:
        c = '2' ;
        break;
    case 0x41:
        c = '3';
        break;

    case 0x0A:
        c = '4';
        break;

    case 0x12:
        c = '5';
        break;

    case 0x22:
        c= '6';
        break;

    case 0x42:
        c = '7';
        break;

    case 0x0C:
        c = '8';
        break;

    case 0x14:
        c = '9';
        break;

    case 0x24:
        c = '*';
        break;

    case 0x44:
        c = '#';
        break;

    default:
        c = 0x00;
        break;

    }
    sei();

    return c;
}
