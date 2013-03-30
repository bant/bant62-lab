//
//  twi_i2cMaster.c
//
//  ATmega TWI I2C(master) control subroutine
//  Copyright(C)2010-2012 Toyohiko Togashi tog001@nifty.com
//
//
//  This program is free software; you can redistribute it and/or modify it under the terms of the
//  GNU General Public License as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this program.
//  If not, see <http://www.gnu.org/licenses/>
//
//
//	Description
//		Master mode I2C interface.
//		AVR ATmega TWI(Two-wire serial interface) internal module.
//		Interrupt nonuse, simple logic.
//		I/O port is SCL=PC5 SDA=PC4, and need pull-up register.
//
//
//    Update history
//    ---------- ----- -------------------------------------------
//    2010.12.20 v0.1  Write only version
//    2010.12.21 v0.1a Enable internal pull-up register
//    2012.04.18 v0.2  Add TWI_I2CRead API, Change TWI_I2CMasterStart and TWI_I2CWrite API
//    2012.05.10 v0.2a Bug fix, Close port release
//

#include <avr/io.h>						// WinAVR library
#include <util/twi.h>					// WinAVR library 

#include "twi_i2cMaster.h"

#define RETRY 3

//
//	Open call
//
void
twi_i2c_MasterInit(unsigned char ClockConfig)
{

    // Set TWI module clock
#ifdef TWI_I2CMASTER_F_SCL

#define BR	((F_CPU / TWI_I2CMASTER_F_SCL - 16) / 2)
#if BR < 10L
    TWSR = 0;
    TWBR = 10;							// smallest TWBR value
#elif BR < 256
    TWSR = 0;							// 1/1
    TWBR = BR;
#elif BR < 1024
    TWSR = 1;							// 1/4
    TWBR = BR / 4;
#elif BR < 4096
    TWSR = 2;							// 1/16
    TWBR = BR / 16;
#elif BR < 16384
    TWSR = 3;							// 1/64
    TWBR = BR / 64;
#else
    TWSR = 0;
    TWBR = 10;							// smallest TWBR value
#endif

#else
    int	br;

    br = ((int)(F_CPU / ClockConfig / 10000) - 16) / 2;
    if (br < 10)
    {
        TWSR = 0;
        TWBR = 10;						// smallest TWBR value, see note [5] */
    }
    else if (br < 256)
    {
        TWSR = 0;
        TWBR = br;
    }
    else if (br < 1024)
    {
        TWSR = 1;						// 1/4
        TWBR = br >> 2;
    }
    else if (br < 4096)
    {
        TWSR = 2;						// 1/16
        TWBR = br >> 4;
    }
    else
    {
        TWSR = 3;						// 1/64
        TWBR = br >> 6;
    }
#endif

    TWDR = 0xff;
    TWCR = _BV(TWEN);
#ifdef TWI_I2CMASTER_PULLUP
    DDRC  &= ~(_BV(DDC4)   | _BV(DDC5));
    PORTC |=  (_BV(PORTC4) | _BV(PORTC5));	// Enable pullup
#endif

}

//
//	Alive call
//
unsigned char
twi_i2cSelect(unsigned char SlaveAddress)
{
    unsigned char	rc;
    unsigned char	twst;

    rc = 1;

    for(;;)
    {

        // Send start condition
        TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);	/* send start condition */
        while ((TWCR & _BV(TWINT)) == 0); 			/* wait for transmission */

        twst = TW_STATUS;
        if (twst == TW_MT_ARB_LOST)
        {
            rc = 11;
            break;
        }
        if ((twst != TW_REP_START) && (twst != TW_START))
        {
            rc = 12;
            break;
        }

        // Send slave address
        TWDR = (SlaveAddress << 1) | TW_WRITE;
        TWCR = _BV(TWINT) | _BV(TWEN);			/* clear interrupt to start transmission */
        while ((TWCR & _BV(TWINT)) == 0);		/* wait for transmission */

        twst = TW_STATUS;
        if (twst == TW_MT_SLA_NACK)  			/* nack during select: device busy writing */
        {
            rc = 21;
            break;
        }
        if (twst == TW_MT_ARB_LOST)  			/* re-arbitrate */
        {
            rc = 22;
            break;
        }
        if (twst != TW_MT_SLA_ACK)
        {
            rc = 23;
            break;
        }

        rc = 0;
        break;

    }

    // Send stop condition
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */
    while ((TWCR & _BV(TWSTO)));				/* wait for transmission */

    return(rc);
}

//
//	Recieve call
//
unsigned char
twi_i2cRead(unsigned char SlaveAddress, unsigned int Length, unsigned char Multi, void* RxData)
{
    char			*p;
    unsigned char	rc;
    unsigned char	i;
    unsigned char	twst;
    unsigned char	twcr;

    rc = 1;
    p = RxData;

    for(i = 0; i < RETRY; i++)
    {

        // Send start condition
        TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);	/* send start condition */
        while ((TWCR & _BV(TWINT)) == 0); 			/* wait for transmission */

        twst = TW_STATUS;
        if (twst == TW_MT_ARB_LOST)
        {
            continue;
        }
        if ((twst != TW_REP_START) && (twst != TW_START))
        {
            rc = 11;
            break;
        }

        // Send slave address
        TWDR = (SlaveAddress << 1) | TW_READ;	/* send SLA+R */
        TWCR = _BV(TWINT) | _BV(TWEN); 			/* clear interrupt to start transmission */
        while ((TWCR & _BV(TWINT)) == 0);		/* wait for transmission */

        twst = TW_STATUS;
        if (twst == TW_MR_SLA_NACK)  			/* nack during select: device busy writing */
        {
            continue;
        }
        if (twst == TW_MR_ARB_LOST)  			/* re-arbitrate */
        {
            continue;
        }
        if (twst != TW_MR_SLA_ACK)
        {
            rc = 21;
            break;
        }

        // Recieve datas
        twcr = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
        for (; Length > 0; --Length)
        {
            if (Length == 1)
            {
                twcr = _BV(TWINT) | _BV(TWEN);	/* send NAK this time */
            }
            TWCR = twcr;						/* clear int to start transmission */
            while ((TWCR & _BV(TWINT)) == 0);	/* wait for transmission */

            twst = TW_STATUS;
            if (twst == TW_MR_DATA_NACK)
            {
                Length = 1;
            }
            else if (twst != TW_MR_DATA_ACK)
            {
                break;
            }

            *p++ = TWDR;
        }
        if ((twst != TW_MR_DATA_NACK) && (twst == TW_MR_DATA_ACK))
        {
            rc = 31;
            continue;
        }

        rc = 0;
        break;

    }

    // Send stop condition
    if (!Multi)
    {
        TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */
        while ((TWCR & _BV(TWSTO)));				/* wait for transmission */
    }

    return(rc);
}

//
//	Send call
//
unsigned char
twi_i2cWrite(unsigned char SlaveAddress, unsigned int Length, unsigned char Multi,void* TxData)
{
    char			*p;
    unsigned char	rc;
    unsigned char	i;
    unsigned char	twst;

    rc = 1;
    p = TxData;

    for(i = 0; i < RETRY; i++)
    {

        // Send start condition
        TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);	/* send start condition */
        while ((TWCR & _BV(TWINT)) == 0) ;			/* wait for transmission */

        twst = TW_STATUS;
        if (twst == TW_MT_ARB_LOST)
        {
            continue;
        }
        if ((twst != TW_REP_START) && (twst != TW_START))
        {
            break;
        }

        // Send slave address
        TWDR = (SlaveAddress << 1) | TW_WRITE;
        TWCR = _BV(TWINT) | _BV(TWEN);		/* clear interrupt to start transmission */

        while ((TWCR & _BV(TWINT)) == 0);	/* wait for transmission */
        twst = TW_STATUS;
        if (twst == TW_MT_SLA_NACK)  		/* nack during select: device busy writing */
        {
            continue;
        }
        if (twst == TW_MT_ARB_LOST)  		/* re-arbitrate */
        {
            continue;
        }
        if (twst != TW_MT_SLA_ACK)
        {
            break;
        }

        // send datas
        for(; Length > 0; --Length)
        {
            TWDR = *p++;
            TWCR = _BV(TWINT) | _BV(TWEN);		/* start transmission */
            while ((TWCR & _BV(TWINT)) == 0);	/* wait for transmission */

            twst = TW_STATUS;
            if (twst == TW_MT_DATA_NACK)
            {
                break;
            }
            if (twst != TW_MT_DATA_ACK)
            {
                break;
            }
        }

        if (twst != TW_MT_DATA_ACK)
        {
            continue;
        }
        rc = 0;
        break;
    }

    // Send stop condition
    if (!Multi)
    {
        TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */
        while ((TWCR & _BV(TWSTO)));				/* wait for transmission */
    }

    return(rc);

}

//
//	Close call
//
void
twi_i2cStop(void)
{
    TWCR = 0;								// ADD 2012.05.10  Power on initial
    // ADD 2012.05.10
#ifdef TWI_I2CMASTER_PULLUP					// ADD 2012.05.10					
    PORTC &= ~(_BV(PORTC4) | _BV(PORTC5));	// ADD 2012.05.10  Disable pullup
#endif										// ADD 2012.05.10
}
