/* ‹¤’ÊŠÖ”’è‹` */

#ifndef _COMMON_H_
#define _COMMON_H_
#include <avr/io.h>

/* NOPŠÖ” */
#define asmNOP()	__asm__ __volatile__ ("nop")

void wait_ms(uint16_t time);
void wait_us(uint16_t time);

#endif
