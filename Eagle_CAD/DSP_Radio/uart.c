/*------------------------------------------------*/
/* UART functions                                 */
/*------------------------------------------------*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

typedef struct {
    uint8_t wi, ri, ct;
    uint8_t buff[64];
} FIFO;
static
volatile FIFO TxFifo, RxFifo;



/* Initialize UART */

void uart_init (unsigned long baud)
{
    unsigned short  val;

    PRR &= ~_BV(PRUSART0);

    val = (unsigned short)(((unsigned long)F_CPU / (16UL * baud)) - 1UL);

    UBRR0H = (unsigned char)(val >> 8); /* ポーレート設定(上位バイト) */
    UBRR0L = (unsigned char)val;            /* ポーレート設定(下位ﾊﾞｲﾄ) */
    UCSR0C = 0x06;                          /* フレーム形式設定(8ビット,ストップビット1) */
    UCSR0B = (1 << TXEN0) | 0x80;           /* 送信許可 */
}


/* Get a received character */

uint8_t uart_test (void)
{
    return RxFifo.ct;
}


uint8_t uart_get (void)
{
    uint8_t d, i;


    while (RxFifo.ct == 0) ;
    i = RxFifo.ri;
    d = RxFifo.buff[i];
    cli();
    RxFifo.ct--;
    sei();
    RxFifo.ri = (i + 1) % sizeof RxFifo.buff;

    return d;
}


/* Put a character to transmit */

void uart_put (uint8_t d)
{
    uint8_t i;


    while (TxFifo.ct >= sizeof TxFifo.buff) ;
    i = TxFifo.wi;
    TxFifo.buff[i] = d;
    cli();
    TxFifo.ct++;
    UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(TXEN0) | _BV(UDRIE0);
    sei();
    TxFifo.wi = (i + 1) % sizeof TxFifo.buff;
}


/* UART RXC interrupt */

ISR(USART_RX_vect)
{
    uint8_t d, n, i;

    d = UDR0;
    n = RxFifo.ct;
    if (n < sizeof RxFifo.buff) {
        RxFifo.ct = ++n;
        i = RxFifo.wi;
        RxFifo.buff[i] = d;
        RxFifo.wi = (i + 1) % sizeof RxFifo.buff;
    }
}


/* UART UDRE interrupt */

ISR(USART_UDRE_vect)
{
    uint8_t n, i;


    n = TxFifo.ct;
    if (n) {
        TxFifo.ct = --n;
        i = TxFifo.ri;
        UDR0 = TxFifo.buff[i];
        TxFifo.ri = (i + 1) % sizeof TxFifo.buff;
    }
    if (n == 0) UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(TXEN0);
}

