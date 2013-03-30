//
//  mpl115a2.h
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
//
//
//    Update history
//    ---------- ----- -------------------------------------------
//    2012.04.18 v0.1  1st version
//

#ifndef MPL115A2_H
#define MPL115A2_H

//
//	Config
//
#define SLA_MPL115A2	0x60			// Slave address
#define RETRY_MPL115A2	3				// Error retry count
#define WAIT_MPL115A2	100				// [us] Retry wait time

//
//	API
//
unsigned char	mpl115a_open(void);
unsigned char	mpl115a_get(short *pres, short *temp);
void			mpl115a_close(void);

#endif
