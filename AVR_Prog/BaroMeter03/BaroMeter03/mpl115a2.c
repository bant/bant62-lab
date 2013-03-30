//
//  mpl115a2.c
//
//  Miniature I2C digital barometer "Freescale MPL115A2" control subroutine
//  Copyright(C)2012 Toyohiko Togashi tog001@nifty.com
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
//		This is a device access handler and coefficient calculation.
//		AVR ATmega TWI(Two-wire serial interface) internal module.
//		Communication speed is 400kHz.
//		Using freescale semiconductor application note AN3785 sample code
//
//
//    Update history
//    ---------- ----- -------------------------------------------
//    2012.04.18 v0.1  1st version
//    2012.05.10 v0.2  ReOpen speed up (Deleted a history of update)
//

#include <avr/io.h>						// WinAVR library
#include <util/delay.h>					// WinAVR library
#include "twi_i2cMaster.h"				// Private
#include "mpl115a2.h"					// Private

// Byte order convert
#define BO_SET2(d,s)	(*((unsigned char *)&d + 1) = *((unsigned char *)&s), *((unsigned char *)&d) = *((unsigned char *)&s + 1))

short	a0  = 0;
short	b1  = 0;
short	b2  = 0;
short	c12 = 0;
short	c11 = 1;
short	c22 = 1;

//
//	Open
//
unsigned char
mpl115a_open(void)
{
    unsigned char	buf[12];
    unsigned char	rc;
    unsigned char	i;

    // Get coefficient data
    if (c11 == 0)
    {
        rc = 0;							// Already loaded
    }
    else
    {
        rc = 1;							// Retry out
        for(i = 0; i < RETRY_MPL115A2; i++)
        {

            // Prewait
            _delay_us(WAIT_MPL115A2);

            // I2C communication
            buf[0] = 0x04;				// Command
            if ((rc = twi_i2cWrite(SLA_MPL115A2, 1, 1, buf)) != 0)
            {
//				printf("i2cW rc=%d\n", rc);
                continue;
            }
            if ((rc = twi_i2cRead(SLA_MPL115A2, 12, 0, buf)) != 0)
            {
//				printf("i2cR rc=%d\n", rc);
                rc += 100;
                continue;
            }

            // Data set
            BO_SET2(a0,  buf[0]);
            BO_SET2(b1,  buf[2]);
            BO_SET2(b2,  buf[4]);
            BO_SET2(c12, buf[6]);
            BO_SET2(c11, buf[8]);
            BO_SET2(c22, buf[10]);

            // Data check
            if ((c11 == 0) && (c22 == 0))
            {
                rc = 0;					// Normal
                break;
            }

            rc = 100;					// Data error
        }
    }

    return(rc);
}

//
//	Get data
//
unsigned char
mpl115a_get(short *pres, short *temp)
{
    unsigned char	buf[4];
    unsigned char	i;
    unsigned char	rc;
    unsigned short	padc;
    unsigned short	tadc;
    long	lt1;
    long	lt2;
    long	lt3;
    long	c11x1;
    long	a11;
    long	c12x2;
    long	a1;
    long	c22x2;
    long	a2;
    long	a1x1;
    long	y1;
    long	a2x2;
    short	pcomp;

    // Retry loop
    rc = 1;
    for(i = 0; i < RETRY_MPL115A2; i++)
    {

        // Get P and T data
        buf[0] = 0x12;					// Start convert
        buf[1] = 0x01;
        if ((rc = twi_i2cWrite(SLA_MPL115A2, 2, 0, buf)) != 0)
        {
//			printf("i2cW=%d %02x%02x\n", rc, buf[0], buf[1]);
            continue;
        }

        _delay_ms(1);					// Conversion time Max

        buf[0] = 0x00;					// Get data
        if ((rc = twi_i2cWrite(SLA_MPL115A2, 1, 1, buf)) != 0)
        {
//			printf("i2cW=%d %02x\n", rc, buf[0]);
            rc += 100;
            continue;
        }
        if ((rc = twi_i2cRead(SLA_MPL115A2, 4, 0, buf)) != 0)
        {
//			printf("i2cR=%d %02x%02x%02x%02x\n", rc, buf[0], buf[1], buf[2], buf[3]);
            rc += 200;
            continue;
        }

        BO_SET2(padc, buf[0]);
        BO_SET2(tadc, buf[2]);

        //******* Freescale semiconductor application note AN3785 sample code
        padc >>= 6;						//Note that the PressCntdec is the raw value from the MPL115A data address.  Its shifted >>6 since its 10 bit.
        tadc >>= 6;						//Note that the TempCntdec is the raw value from the MPL115A data address.  Its shifted >>6 since its 10 bit.
//		tadc -= 30;
//		printf("%d %d\n", padc, tadc);

        //******* STEP 1 c11x1= c11 * Padc
        lt1 = (long)c11;				// s(16,27)    s(N,F+zeropad) goes from s(11,10)+11ZeroPad = s(11,22) => Left Justified = s(16,27)
        lt2 = (long)padc;				// u(10,0)     s(N,F)
        lt3 = lt1 * lt2;				// s(26,27)           /c11*Padc
        c11x1 = (long)lt3;				// s(26,27)- EQ 1     =c11x1   /checked
        //divide this hex number by 2^30 to get the correct decimal value.

        //b1 =s(14,11) => s(16,13) Left justified
        //******* STEP 2 a11= b1 + c11x1
        lt1 = (long)b1 << 14;			// s(30,27)        b1=s(16,13)   Shift b1 so that the F matches c11x1(shift by 14)
        lt2 = (long)c11x1;				// s(26,27)  //ensure fractional bits are compatible
        lt3 = lt1 + lt2;				// s(30,27)           /b1+c11x1
        a11 = (long)lt3 >> 14;			// s(16,13) - EQ 2     =a11     Convert this block back to s(16,X)

        //******* STEP 3 c12x2= c12 * Tadc
        // sic12 is s(14,13)+9zero pad = s(16,15)+9 => s(16,24) left justified
        lt1 = (long)c12;				// s(16,24)
        lt2 = (long)tadc;				// u(10,0)
        lt3 = lt1 * lt2;				// s(26,24)
        c12x2 = (long)lt3;       		// s(26,24) - EQ 3     =c12x2   /checked

        //******* STEP 4 a1= a11 + c12x2
        lt1 = (long)a11 << 11;			// s(27,24) This is done by s(16,13) <<11 goes to s(27,24) to match c12x2's F part
        lt2 = (long)c12x2;				// s(26,24)
        lt3 = lt1 + lt2;				// s(27,24)            /a11+c12x2
        a1 =(long)lt3 >> 11;			// s(16,13) - EQ 4     =a1   /check

        //******* STEP 5 c22x2= c22 * Tadc
        // c22 is s(11,10)+9zero pad = s(11,19) => s(16,24) left justified
        lt1 = (long)c22;				// s(16,30)  This is done by s(11,10) + 15 zero pad goes to s(16,15)+15, to s(16,30)
        lt2 = (long)tadc;				// u(10,0)
        lt3 = lt1 * lt2;				// s(26,30)            /c22*Tadc
        c22x2 = (long)lt3;				// s(26,30) - EQ 5     /=c22x2

        //******* STEP 6 a2= b2 + c22x2
        //WORKS and loses the least in data. One extra execution.  Note how the 31 is really a 32 due to possible overflow.

        // b2 is s(16,14) User shifted left to  => s(31,29) to match c22x2 F value
        lt1 = (long)b2 << 15;			//s(31,29)
        lt2 = (long)c22x2 >> 1;			//s(25,29)  s(26,30) goes to >>16 s(10,14) to match F from sib2
        lt3 = lt1 + lt2;				//s(32,29) but really is a s(31,29) due to overflow the 31 becomes a 32.
        a2 = (long)lt3 >> 16;			//s(16,13)

        //******* STEP 7  a1x1= a1 * Padc

        lt1 = (long)a1;					// s(16,13)
        lt2 = (long)padc;				// u(10,0)
        lt3 = lt1 * lt2;				// s(26,13)            /a1*Padc
        a1x1 = (long)lt3;				// s(26,13)  - EQ 7    /=a1x1  /check

        //******* STEP 8  y1= a0 + a1x1
        // a0 = s(16,3)
        lt1 = (long)a0 << 10;			// s(26,13)  This is done since has to match a1x1 F value to add.  So S(16,3) <<10 = S(26,13)
        lt2 = (long)a1x1;				// s(26,13)
        lt3 = lt1 + lt2;				// s(26,13)            /a0+a1x1
        y1 = (long)lt3 >> 10;			// s(16,3)  - EQ 8     /=y1    /check

        //******* STEP 9  a2x2= a2 *Tadc
        lt1 = (long)a2;					// s(16,13)
        lt2 = (long)tadc;				// u(10,0)
        lt3 = lt1 * lt2;				// s(26,13)            /a2*Tadc
        a2x2 = (long)lt3;				// s(26,13)  - EQ 9     /=a2x2

        //******* STEP 10 pComp = y1 +a2x2
        // y1= s(16,3)
        lt1 = (long)y1 << 10;			// s(26,13)  This is done to match a2x2 F value so addition can match.  s(16,3) <<10
        lt2 = (long)a2x2;				// s(26,13)
        lt3 = lt1 + lt2;				// s(26,13)             /y1+a2x2

        // FIXED POINT RESULT WITH ROUNDING:
        pcomp = lt3 >> 13;				// goes to no fractional parts since this is an ADC count.
//		printf("pcomp=%d.\n", pcomp);

        //decPcomp is defined as  a floating point number.
        //Conversion to Decimal value from 1023 ADC count value.  ADC counts are 0 to 1023.  Pressure is 50 to 115kPa correspondingly.
        //decPcomp = ((65.0/1023.0)*siPcomp)+50;
        *pres = 650L * pcomp / 1023 + 500;	// [hPa] Float->Integer expression (reduce rom)
//		printf("P=%d[hPa]\n", *pres);

//		*temp = (tadc / -5.35 + 113.22) * 10.0;
//		*temp = -187L * tadc / 100  + 1132;	// [x10^C] Float->Integer expression (reduce rom)
        *temp = -187L * tadc / 100  + 1190;	// [x10^C] Float->Integer expression (reduce rom) Individual offset
//		printf("T=%d.%d[^C]\n", *temp/10, *temp%10);

        rc = 0;
        break;
    }

    return(rc);
}

//
//	Close
//
void
mpl115a_close(void)
{
    twi_i2cStop();
}
