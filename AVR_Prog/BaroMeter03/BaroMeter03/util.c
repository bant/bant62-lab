//=============================================================================
// File Name    : main.c
//
// Title        : �C���Z���T���C�����W���[��
// Revision     : 0.1
// Notes        :
// Target MCU   : AVR ATMega328
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/02/06   �΂��      ����J�n
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
/*   �ړ����ϗp�̃f�[�^���ЂƂo�^							*/
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
/*   �ړ����ϗp�̃f�[�^�̑��a�����							*/
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
/*   �N�A���A�����w�肷��ƁA�j����Ԃ��B(1:��, ... 7:�y)	*/
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
/*   �������r(�啶������������)							*/
/*==========================================================*/
int strcmp_ignorecase( const char *s1, const char *s2 )
{
	int i = 0;

	/* �������������ԌJ��Ԃ� */
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
