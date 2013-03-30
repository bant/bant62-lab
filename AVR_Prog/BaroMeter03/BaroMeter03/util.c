//=============================================================================
// File Name    : main.c
//
// Title        : 気圧センサメインモジュール
// Revision     : 0.1
// Notes        :
// Target MCU   : AVR ATMega328
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/02/06   ばんと      製作開始
//=============================================================================

/* Includes ------------------------------------------------------------------*/
#include <ctype.h>
#include <avr/io.h>
#include "util.h"

/* local define --------------------------------------------------------------*/
/* local typedef -------------------------------------------------------------*/
/* local macro ---------------------------------------------------------------*/
/* local variables -----------------------------------------------------------*/
/* local function prototypes -------------------------------------------------*/


/*==========================================================*/
/*   移動平均用のデータをひとつ登録							*/
/*==========================================================*/
void storeData( short data, short *buf, uint8_t bufsize )
{
    register uint8_t i;
	short *p;

	for ( i = 0, p = buf; i < bufsize - 1; i++, p++ )
	{
		*p = *(p + 1);
	}
	*p = data;
}


/*==========================================================*/
/*   移動平均用のデータの総和を取る							*/
/*==========================================================*/
long sumData( short *buf, uint8_t bufsize )
{
    register uint8_t i;
	long sum;
	short *p;

	for ( i = 0, p = buf, sum = 0; i < bufsize; i++, p++ )
	{
		sum += *p;
	}

	return sum;
}


/*==========================================================*/
/*   年、月、日を指定すると、曜日を返す。(1:日, ... 7:土)	*/
/*==========================================================*/
int getWeekday( int nYear, int nMonth, int nDay )
{
    int nWeekday, nTmp;

    if (nMonth == 1 || nMonth == 2)
    {
        nYear--;
        nMonth += 12;
    }

    nTmp = nYear/100;
    nWeekday = (nYear + (nYear >> 2) - nTmp + (nTmp >> 2) + (13 * nMonth + 8)/5 + nDay) % 7;

    return nWeekday + 1;
}


/*==========================================================*/
/*   文字列比較(大文字小文字無視)							*/
/*==========================================================*/
int strcmp_ignorecase( const char *s1, const char *s2 )
{
	int i = 0;

	/* 文字が等しい間繰り返す */
	while (toupper((unsigned char)s1[i]) == toupper((unsigned char)s2[i]))
	{
		if (s1[i] == '\0')
		{
			return 0;
		}
		i++;
	}

	return toupper((unsigned char)s1[i]) - toupper((unsigned char)s2[i]);
}
