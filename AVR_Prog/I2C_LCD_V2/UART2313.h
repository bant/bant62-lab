/*       UART2313 Header File           */
/*                       1/28/08        */
/*                       by LABO Gataro */
#ifndef UART2313_H
#define UART2313_H


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdbool.h>

// PORT & BaudRate define
#define UART_PORT  PORTD
#define UART_DDR   DDRD
#define UART_RX    PD0
#define UART_TX    PD1
#define UBRR       51   //9600bps on 8M clk

// 関数プロトタイプ
extern void xmit(uint8_t);
extern void xmitstr(const prog_char *);
extern void xmitline( char *);
extern uint8_t read();
extern uint8_t xread();
extern bool xreadOK();
//extern void xmit_hex(uint8_t);
//extern void xmit_dec(uint16_t);
extern void uart2313_init(void);
//-------------------------------

#endif
