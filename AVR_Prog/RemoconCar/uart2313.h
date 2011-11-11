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

// PORT & BaudRate define
#define UART_PORT  PORTD
#define UART_DDR   DDRD
#define UART_RX    PD0
#define UART_TX    PD1
#define UBRR       51   //9600bps on 8M clk

// 関数プロトタイプ
void xmit(uint8_t);
void xmitstr(const prog_char *);
void xmitline( char *);
uint8_t read(void);
uint8_t xread();
uint8_t xreadOK();
void xmit_hex(uint8_t);
void xmit_dec(uint16_t);
void uart2313_init(void);
//-------------------------------


#endif
