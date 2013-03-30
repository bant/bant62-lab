#ifndef __MY_UTIL__H__
#define __MY_UTIL__H__

/*======================================*/
/*  ä÷êîíËã`								*/
/*======================================*/
extern void storeData( short data, short *buf, uint8_t bufsize );
extern long sumData( short *buf, uint8_t bufsize );
extern int getWeekday( int nYear, int nMonth, int nDay );
extern int strcmp_ignorecase( const char *s1, const char *s2 );


#endif
