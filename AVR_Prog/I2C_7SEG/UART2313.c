// --------Tiny2313 UART functions --------
//                                 1/28/08
//                             by LABO Gataro
//-----------------------------------------


#include "UART2313.h"

bool xreadOK()
{
    if (bit_is_set(UCSRA,RXC)) return true;
    else return false;
}

uint8_t xread()
{
    uint8_t ch;
    ch = UDR;
    return(ch);
}

void xmit(uint8_t ch)
{
    while (bit_is_clear(UCSRA,UDRE));
    UDR = ch;
}
/*
void xmit_hex(uint8_t chr)
{
    uint8_t wk;
//  xmitstr(PSTR("0x"));
    wk = chr /16;
    if (wk >9 ) xmit(wk + 0x37);
    else xmit(wk + 0x30);
    wk = chr % 16;
    if (wk >9) xmit(wk + 0x37);
    else xmit(wk + 0x30);
}
*/
/*
void xmit_dec(uint16_t chr)
{
    uint8_t d1;
    d1=chr/100;
    xmit(d1+0x30);
    d1=(chr % 100)/10;
    xmit(d1+0x30);
    d1=(chr % 100) % 10;
    xmit(d1+0x30);
}
*/
void xmitstr(const prog_char *s)
{
    char c;
    while ((c=pgm_read_byte(s++))!=0) xmit(c);
}

void uart2313_init()
{
    UART_PORT &= ~(1<<UART_TX);
    UART_PORT &= ~(1<<UART_RX);
    UART_DDR  |=  (1<<UART_TX);
    UART_DDR  &= ~(1<<UART_RX);
    UBRRL = UBRR;           //baud rate
    UBRRH = 0;
    UCSRB = 0b10011000;     // rcv intpt enable, rx,tx active 8bit frame
    UCSRA &= ~(1<<U2X);     // •W€‘¬“x
    UCSRC = 3<<UCSZ0 ;      //8bit frame size non parity 1stop bit
}

ISR(SIG_USART0_RECV)
{
    return;    // to wake up from sleep
}
