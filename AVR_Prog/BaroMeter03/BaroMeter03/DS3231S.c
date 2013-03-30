//=============================================================================
// File Name    : DS3231S.c
//
// Title        : DS3231S RTCモジュールドライバ
// Revision     : 0.1
// Notes        :
// Target MCU   : AVR ATMega328
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/02/06   ばんと      修正完了
//=============================================================================

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "DS3231S.h"
#include "twi_i2cMaster.h"

/* local typedef -------------------------------------------------------------*/

/* local define --------------------------------------------------------------*/
/* local macro ---------------------------------------------------------------*/
/* local variables -----------------------------------------------------------*/
/* local function prototypes -------------------------------------------------*/
static uint8_t dec2bcd(uint8_t d);
static uint8_t bcd2dec(uint8_t b);
static uint8_t DS3231S_read_byte(uint8_t offset);
static uint8_t DS3231S_read_byte(uint8_t offset);

static uint8_t dec2bcd(uint8_t d)
{
    return ((d/10 * 16) + (d % 10));
}

static uint8_t bcd2dec(uint8_t b)
{
    return ((b/16 * 10) + (b % 16));
}

static uint8_t DS3231S_write_byte(uint8_t offset, uint8_t data)
{
    uint8_t rc, i;
    uint8_t buf[2];

    rc = 1;							// Retry out

    buf[0] = offset;
    buf[1] = data;
    for(i = 0; i < RETRY_DS3231S; i++)
    {
        if ((rc = twi_i2cWrite(DS3231S_ADDR, 2, 0, buf)) != 0)
        {
#ifdef DEGUG
            xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
            continue;
        }
        else
        {
            break;
        }
    }

    return rc;
}

static uint8_t DS3231S_read_byte(uint8_t offset)
{
    uint8_t rc, i;
    uint8_t buf;

    rc = 1;							// Retry out
    buf = offset;					// set offset address
    for(i = 0; i < RETRY_DS3231S; i++)
    {
        if ((rc = twi_i2cWrite(DS3231S_ADDR, 1, 1, &buf)) != 0)
        {
#ifdef DEGUG
            xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
            continue;
        }
        if ((rc = twi_i2cRead(DS3231S_ADDR, 1, 0, &buf)) != 0)
        {
#ifdef DEGUG
            xprintf_p(PSTR("i2cR rc=%d\n"), rc);
#endif
            rc += 100;
            continue;
        }
        else
        {
            break;
        }
    }

    if (rc != 0)
    {
        return buf;
    }
    else
    {
        return 0;
    }
}

/*======================================*/
/*  現在時刻取得関数					*/
/*======================================*/
uint8_t DS3231S_now(RTC_TIME *time)
{
    uint8_t rc, i;
    uint8_t buf[7];

    rc = 1;							// Retry out
    for(i = 0; i < RETRY_DS3231S; i++)
    {
        buf[0] = 0x00;				// Command
        if ((rc = twi_i2cWrite(DS3231S_ADDR, 1, 1, buf)) != 0)
        {
#ifdef DEGUG
			xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
            continue;
        }
        if ((rc = twi_i2cRead(DS3231S_ADDR, 7, 0, buf)) != 0)
        {
#ifdef DEGUG
			xprintf_p(PSTR("i2cR rc=%d\n"), rc);
#endif
            rc += 100;
            continue;
        }
        else
        {
            time->sec = bcd2dec(buf[0] & 0x7F);
            time->min = bcd2dec(buf[1] & 0x7F);

            if (buf[2] & _BV(6))	// 6ビット目がONの時は12時間制
            {
                time->hour = bcd2dec(buf[2] & 0x1F);
                time->isTwelve = true;

                if (buf[2] & _BV(5))
                {
                    time->isAM = false;
                }
                else
                {
                    time->isAM = true;
                }
            }
            else	// 24時間制
            {
                time->hour = bcd2dec(buf[2] & 0x3F);
                time->isTwelve = false;
            }

            time->wday = bcd2dec(buf[3] & 0x07);
            time->day = bcd2dec(buf[4] & 0x3F);
            time->month = bcd2dec(buf[5]  & 0x1F );
            time->year = bcd2dec(buf[6])+2000;
            if(buf[5] & 0x80)	// 世紀溢れビットチェック
            {
                time->year += 100;
            }
            break;
        }
    }

    return rc;
}

/*======================================*/
/*  アジャスト関数						*/
/*======================================*/
uint8_t DS3231S_adjust(RTC_TIME *time)
{
    uint8_t rc, i;
    uint8_t buf[8];

    // 時刻設定コマンド製作
    buf[0] = 0x00;						// Command ??
    buf[1] = dec2bcd(time->sec);		// 秒
    buf[2] = dec2bcd(time->min);		// 分
    buf[3] = dec2bcd(time->hour);		// 時
    if (time->isTwelve)
    {
        buf[3] |= _BV(6);				// BIT6を立てる

        if (!time->isAM)
        {
            buf[3] |= _BV(5);
        }
    }

    buf[4] = dec2bcd(time->wday);		// 曜日
    buf[5] = dec2bcd(time->day);		// 日
    buf[6] = dec2bcd(time->month);		// 月
    buf[7] = dec2bcd(time->year - 2000);// 年
//	buf[8] = 0x00;						// Command ??

    rc = 1;								// Retry out
    for(i = 0; i < RETRY_DS3231S; i++)
    {
        if ((rc = twi_i2cWrite(DS3231S_ADDR, 8, 0, buf)) != 0)
        {
#ifdef DEGUG
			xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
            rc += 100;
            continue;
        }
        else
        {
            break;
        }
    }

    return rc;
}

/*======================================*/
/*  SQW許可関数							*/
/*======================================*/
uint8_t DS3231S_SQW_enable(bool enable)
{
    uint8_t rc, i;
    uint8_t buf[2];

    rc = 1;							// Retry out
    for(i = 0; i < RETRY_DS3231S; i++)
    {
        buf[0] = 0x0E;				// address set
        if ((rc = twi_i2cWrite(DS3231S_ADDR, 1, 1, buf)) != 0)
        {
#ifdef DEGUG
			xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
            continue;
        }
        if ((rc = twi_i2cRead(DS3231S_ADDR, 1, 0, buf)) != 0)
        {
#ifdef DEGUG
			xprintf_p(PSTR("i2cR rc=%d\n"), rc);
#endif
            rc += 100;
            continue;
        }
        else
        {
            break;
        }
    }

    if (rc == 0)
    {
        buf[1] = buf[0];
        if (enable)
        {
            buf[1] |=  0b01000000; // set BBSQW to 1
            buf[1] &= ~0b00000100; // set INTCN to 0
        }
        else
        {
            buf[1] &= ~0b01000000; // set BBSQW to 0
        }
        buf[0] = 0x0E;

        for(i = 0; i < RETRY_DS3231S; i++)
        {
            if ((rc = twi_i2cWrite(DS3231S_ADDR, 2, 0, buf)) != 0)
            {
#ifdef DEGUG
				xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
                rc += 100;
                continue;
            }
            else
            {
                break;
            }
        }
    }
    return rc;
}

/*======================================*/
/*  SQW周期設定関数						*/
/*======================================*/
uint8_t DS3231S_SQW_set_freq(enum RTC_SQW_FREQ freq)
{
    uint8_t rc, i;
    uint8_t buf[2];

    rc = 1;							// Retry out
    for(i = 0; i < RETRY_DS3231S; i++)
    {
        buf[0] = 0x0E;				// address set
        if ((rc = twi_i2cWrite(DS3231S_ADDR, 1, 1, buf)) != 0)
        {
#ifdef DEGUG
			xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
            continue;
        }
        if ((rc = twi_i2cRead(DS3231S_ADDR, 1, 0, buf)) != 0)
        {
#ifdef DEGUG
			xprintf_p(PSTR("i2cR rc=%d\n"), rc);
#endif
            rc += 100;
            continue;
        }
        else
        {
            break;
        }
    }

    if (rc == 0)
    {
        buf[1] = buf[0];
        buf[1] &= ~0b00011000; // Set to 0
        buf[1] |= (freq << 4); // Set freq bitmask
        buf[0] = 0x0E;

        for(i = 0; i < RETRY_DS3231S; i++)
        {
            if ((rc = twi_i2cWrite(DS3231S_ADDR, 2, 0, buf)) != 0)
            {
#ifdef DEGUG
				xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
                rc += 100;
                continue;
            }
            else
            {
                break;
            }
        }
    }
    return rc;
}

/* ■↓以下は今回使用しないので未テスト■===============================  */

/*======================================*/
/*  32kHz信号許可関数					*/
/*        		電源投入時は許可状態	*/
/*======================================*/
uint8_t DS3231S_osc32kHz_enable(bool enable)
{
    uint8_t rc, i;
    uint8_t buf[2];

    rc = 1;							// Retry out
    for(i = 0; i < RETRY_DS3231S; i++)
    {
        buf[0] = 0x0F;				// address set
        if ((rc = twi_i2cWrite(DS3231S_ADDR, 1, 1, buf)) != 0)
        {
#ifdef DEGUG
			xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
            continue;
        }
        if ((rc = twi_i2cRead(DS3231S_ADDR, 1, 0, buf)) != 0)
        {
#ifdef DEGUG
            xprintf_p(PSTR("i2cR rc=%d\n"), rc);
#endif
            rc += 100;
            continue;
        }
        else
        {
            break;
        }
    }

    if (rc == 0)
    {
        buf[1] = buf[0];
        if (enable)
        {
            buf[1] |= 0b00001000; // set to 1
        }
        else
        {
            buf[1] &= ~0b00001000; // Set to 0
        }
        buf[0] = 0x0F;

        for(i = 0; i < RETRY_DS3231S; i++)
        {
            if ((rc = twi_i2cWrite(DS3231S_ADDR, 2, 0, buf)) != 0)
            {
#ifdef DEGUG
                xprintf_p(PSTR("i2cW rc=%d\n"), rc);
#endif
                rc += 100;
                continue;
            }
            else
            {
                break;
            }
        }
    }
    return rc;
}

/*======================================*/
/*  アラーム・リセット関数				*/
/*======================================*/
// Alarm functionality
// fixme: should decide if "alarm disabled" mode should be available, or if alarm should always be enabled
// at 00:00:00. Currently, "alarm disabled" only works for ds3231
void DS3231S_reset_alarm(void)
{
    // writing 0 to bit 7 of all four alarm 1 registers disables alarm
    DS3231S_write_byte(0x07,0); // second
    DS3231S_write_byte(0x08,0); // minute
    DS3231S_write_byte(0x09,0); // hour
    DS3231S_write_byte(0x0a,0); // day
}

/*======================================*/
/*  アラームセット関数１				*/
/*======================================*/
// fixme: add an option to set whether or not the INTCN and Interrupt Enable flag is set when setting the alarm
void DS3231S_set_alarm_s(uint8_t hour, uint8_t min, uint8_t sec)
{
    if (hour > 23) return;
    if (min > 59) return;
    if (sec > 59) return;

    /*
     *  07h: A1M1:0  Alarm 1 seconds
     *  08h: A1M2:0  Alarm 1 minutes
     *  09h: A1M3:0  Alarm 1 hour (bit6 is am/pm flag in 12h mode)
     *  0ah: A1M4:1  Alarm 1 day/date (bit6: 1 for day, 0 for date)
     *  Sets alarm to fire when hour, minute and second matches
     */
    DS3231S_write_byte(0x07, dec2bcd(sec)); // second
    DS3231S_write_byte(0x08, dec2bcd(min)); // minute
    DS3231S_write_byte(0x09, dec2bcd(hour)); // hour
    DS3231S_write_byte(0x0a, 0b10000001); // day (upper bit must be set)

    // clear alarm flag
    uint8_t val = DS3231S_read_byte(0x0f);
    DS3231S_write_byte(val & ~0b00000001, 0x0f);
}

/*======================================*/
/*  アラームセット関数関数２			*/
/*======================================*/
void DS3231S_set_alarm(RTC_TIME* tm_)
{
    if (!tm_) return;
    DS3231S_set_alarm_s(tm_->hour, tm_->min, tm_->sec);
}
/*======================================*/
/*  アラーム時間取得関数１				*/
/*======================================*/
void DS3231S_get_alarm_s(uint8_t* hour, uint8_t* min, uint8_t* sec)
{
    *sec  = bcd2dec(DS3231S_read_byte(0x07) & ~0b10000000);
    *min  = bcd2dec(DS3231S_read_byte(0x08) & ~0b10000000);
    *hour = bcd2dec(DS3231S_read_byte(0x09) & ~0b10000000);
}

/*======================================*/
/*  アラーム時間取得関数２				*/
/*======================================*/
void DS3231S_get_alarm(RTC_TIME* _tm)
{
    uint8_t hour, min, sec;

    DS3231S_get_alarm_s(&hour, &min, &sec);
    _tm->hour = hour;
    _tm->min = min;
    _tm->sec = sec;
//    return &_tm;
}

/*======================================*/
/*  アラーム設定確認関数					*/
/*======================================*/
bool DS3231S_check_alarm(void)
{
    // Alarm 1 flag (A1F) in bit 0
    uint8_t val = DS3231S_read_byte(0x0f);

    // clear flag when set
    if (val & 1)
        DS3231S_write_byte(0x0f , val & ~0b00000001);

    return val & 1 ? true : false;
}
