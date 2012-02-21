#include <avr/io.h>
#include "common.h"
#include "lcd_lib.h"

int main(void)
{
    lcd_init();

	lcd_clear();

    while(1)
    {
	    lcd_pos(1,1);
		lcd_puts("TEST1");
		wait_ms(500);
		lcd_clear();
	    lcd_pos(2,1);
    	lcd_puts("TEST2");
		wait_ms(500);
		lcd_clear();
    }

    return 0;
}
