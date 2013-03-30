#ifndef __DS3231S__H__
#define __DS3231S__H__

#include <stdbool.h>

#define DS3231S_ADDR	0x68
#define RETRY_DS3231S	3

typedef struct
{
    uint8_t sec;		// 0 to 59
    uint8_t min;		// 0 to 59
    uint8_t hour;		// 12���Ԑ��Ȃ�(0����11) 24���Ԑ��Ȃ�(0����23)
    uint8_t day;		// 1 to 31
    uint8_t	month;		// 1 to 12
    uint16_t year;		// 2000 to
    uint8_t wday;		// 1 to 7

    bool	isTwelve;	// TRUE �Ȃ� 12���Ԑ�
    bool	isAM;		// TRUE for AM, FALSE for PM
} RTC_TIME;

// Auxillary functions
enum RTC_SQW_FREQ { FREQ_1 = 0, FREQ_1024, FREQ_4096, FREQ_8192 };

/*======================================*/
/*  �֐���`						    */
/*======================================*/
extern uint8_t DS3231S_adjust(RTC_TIME *time);
extern uint8_t DS3231S_now(RTC_TIME *time);
extern uint8_t DS3231S_SQW_enable(bool enable);
extern uint8_t DS3231S_SQW_set_freq(enum RTC_SQW_FREQ freq);
extern uint8_t DS3231S_osc32kHz_enable(bool enable);
extern void DS3231S_reset_alarm(void);
extern void DS3231S_set_alarm_s(uint8_t hour, uint8_t min, uint8_t sec);
extern void DS3231S_set_alarm(RTC_TIME *tm_);
extern void DS3231S_get_alarm_s(uint8_t* hour, uint8_t* min, uint8_t* sec);
extern void DS3231S_get_alarm(RTC_TIME* _tm);
extern bool DS3231S_check_alarm(void);

#endif
