//========================================================================
// File Name    : 24LCxxx.h
//
// Title        : 24LCxxx メモリドライバ・ヘッダファイル
// Revision     : 0.1
// Notes        :
// Target MCU   : AVR ATtiny series
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/04/13   ばんと      製作開始
// 2013/04/14   ばんと      Ver0.1製作完了
//------------------------------------------------------------------------
// This code is distributed under Apache License 2.0 License
//		which can be found at http://www.apache.org/licenses/
//========================================================================
#ifndef __24LCXXX_H_
#define __24LCXXX_H_

/* define --------------------------------------------------------------*/
#define		I2C_ADDR_24LCXXX		0x50
#define		__24LC256__ 


#if defined(__24LC64__)
#define		MAXADR_24LCXXX			8192
#define		PAGE_SIZE_24LCXXX		32
#endif

#if defined(__24LC128__)
#define		MAXADR_24LCXXX			16384
#define		PAGE_SIZE_24LCXXX		64
#endif

#if defined(__24LC256__)
#define		MAXADR_24LCXXX			32768
#define		PAGE_SIZE_24LCXXX		64
#endif

/* macro ---------------------------------------------------------------*/
/* variables -----------------------------------------------------------*/
/* function prototypes -------------------------------------------------*/
uint8_t I2C_24LCXXX_byte_write( uint8_t slave_7bit_addr, uint16_t mem_addr, uint8_t data );
uint8_t I2C_24LCXXX_read( uint8_t slave_7bit_addr, uint16_t mem_addr, void* data, int size );
uint8_t I2C_24LCXXX_nbyte_write( uint8_t slave_7bit_addr, uint16_t mem_addr, void *data, int size );
uint8_t I2C_24LCXXX_page_write( uint8_t slave_7bit_addr, uint16_t mem_addr, uint8_t *data );

#endif /* __24LCXXX_H_ */
