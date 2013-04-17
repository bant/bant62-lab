//========================================================================
// File Name    : RTC8564.h
//
// Title        : Seiko Epson RTC-8564 �h���C�o�E�w�b�_�t�@�C��
// Revision     : 0.1
// Notes        :
// Target MCU   : AVR ATtiny series
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/04/13   �΂��      ����J�n
// 2013/04/14   �΂��      Ver0.1���슮��
//------------------------------------------------------------------------
// This code is distributed under Apache License 2.0 License
//		which can be found at http://www.apache.org/licenses/
//========================================================================

#ifndef __RTC8564_H_
#define __RTC8564_H_

/* define --------------------------------------------------------------*/
#define	I2C_ADDR_RTC8564	0x51	//Slave address=1010001

/* typedef -------------------------------------------------------------*/
enum RTC_CLKOUT_FREQ { FREQ_32768=0, FREQ_1024=1, FREQ_32=2, FREQ_1=3, FREQ_0=4 };
enum RTC_TIMER_TIMING { TIMING_244_14_MS=0, TIMING_15_625_MS=1, TIMING_1_SEC=2, TIMING_1_MIN=3 };
typedef struct
{
	uint8_t sec;		// 0 to 59
	uint8_t min;		// 0 to 59
	uint8_t hour;		// 12���Ԑ��Ȃ�(0����11) 24���Ԑ��Ȃ�(0����23)
	uint8_t day;		// 1 to 31
	uint8_t	month;		// 1 to 12
	uint16_t year;		// 2000 to
	uint8_t wday;		// 1 to 7
} RTC_TIME;

typedef struct
{
	uint8_t min;		// 0 to 59
	uint8_t hour;		// 12���Ԑ��Ȃ�(0����11) 24���Ԑ��Ȃ�(0����23)
	uint8_t day;		// 1 to 31
	uint8_t wday;		// 1 to 7
} ALARM_TIME;

/* macro ---------------------------------------------------------------*/
/* variables -----------------------------------------------------------*/
/* function prototypes -------------------------------------------------*/
int getWeekday( int nYear, int nMonth, int nDay );
uint8_t RTC8564_init( void );
uint8_t RTC8564_power_on( void );
uint8_t RTC8564_backup_return( void );
uint8_t RTC8564_adjust( RTC_TIME *time );
uint8_t RTC8564_now( RTC_TIME *time );
uint8_t RTC8564_setTimer( uint8_t cycle,  uint8_t int_out, enum RTC_TIMER_TIMING sclk, uint8_t count );
uint8_t RTC8564_setClkOut( enum  RTC_CLKOUT_FREQ clkout );
uint8_t RTC8564_setAlarm( ALARM_TIME *alarm );
uint8_t RTC8564_getAlarm( ALARM_TIME *alarm );
uint8_t RTC8564_stopAlarm( void );


#define RTC8564_start() TinyI2C_setRegBit( I2C_ADDR_RTC8564, 0x00, _BV(7) | _BV(5) | _BV(3), CLEAR_BIT )
#define RTC8564_stop() TinyI2C_masksetRegBit( I2C_ADDR_RTC8564, 0x00, _BV(7) | _BV(5) | _BV(3), _BV(5) )

#define RTC8564_stopTimer()	TinyI2C_setRegBit( I2C_ADDR_RTC8564, 0x0E, _BV(7), CLEAR_BIT )

#endif	/*  #ifndef */