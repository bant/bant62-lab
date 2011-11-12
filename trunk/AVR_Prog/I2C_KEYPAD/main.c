/*
 * main.c
 *
 *  Created on: 2010/10/16
 *      Author: bant
 */

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include <avr/eeprom.h>
#include <util/delay.h>
#include "global.h"		// include our global settings
#include "keyin.h"		//
#include "usiTwiSlave.h"


static const uint8_t TWI_slaveAddress = 0x31;

void ledInit(void)
{
    int i;

    DDRB |= (1<<PB0); // for RED LED output
    DDRB |= (1<<PB1); // for GREEN LED output
    for (i=0; i< 5;i++)
    {
        RED_LED_ON();
        GREEN_LED_OFF();
        _delay_ms(100);
        RED_LED_OFF();
        GREEN_LED_ON();
        _delay_ms(100);
    }
    RED_LED_OFF();
    GREEN_LED_OFF();
}

int main()
{
    uint8_t dat;

    // set I/O
    ledInit();
    // initial value
    keyInit();
    timer0Init();

    // I2C Slave initialization
    usiTwiSlaveInit( TWI_slaveAddress );

    sei(); // enable interrupts, needed for twi
    // main loop
    while (1)
    {
        //sleep_mode();

        if ( usiTwiDataInReceiveBuffer() )
        {
            switch (usiTwiReceiveByte())
            {
            case 'n':
            case 'N':
                setKeyMode(NUMERIC_MODE);
                break;

            case 'a':
            case 'A':
                setKeyMode(ALFABET_MODE);
                break;

            case 's':
            case 'S':
                dat = getKeyState();
                usiTwiTransmitByte(dat);
                break;

            case 'c':
            case 'C':
                dat = getKeyCmd();
                usiTwiTransmitByte(dat);
                break;

            }
        }
    }

    return 0;
}
