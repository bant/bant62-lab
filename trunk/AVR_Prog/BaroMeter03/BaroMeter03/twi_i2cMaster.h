//
//  twi_i2cMaster.h
//
//  TWI I2C control subroutine
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
//    2012.04.18 v0.2  Add API TWI_I2CRead, Change API TWI_I2CMasterStart and TWI_I2CWrite
//

#ifndef TWI_I2CMASTER_H
#define TWI_I2CMASTER_H

//
//    Configure
//
//#define F_CPU 	1000000UL				// See AVRstudio project option
#define TWI_I2CMASTER_F_SCL		100000L		// Enable Fixed SCL frequency[Hz] <-> ClockConfig
//#define TWI_I2CMASTER_PULLUP				// Enable ATmega internal pull-ups (20..50kohm/ATmega329P)

//
// API
//
extern void twi_i2c_MasterInit(unsigned char ClockConfig);
extern unsigned char twi_i2cSelect(unsigned char SlaveAddress);
extern unsigned char twi_i2cRead(unsigned char SlaveAddress, unsigned int Length, unsigned char Multi, void* RxData);
extern unsigned char twi_i2cWrite(unsigned char SlaveAddress, unsigned int Length, unsigned char Multi,void* TxData);
extern void twi_i2cStop(void);
// ClockConfig: SCL frequency[Hz]/10000 (1=10kHz,10=100kHz...40=400kHz) <-> TWI_I2CMASTER_F_SCL
// StatusCallback: 0(Not use reserve)
// Multi: 0=StopMessage 1=ContinueMessage

#endif
