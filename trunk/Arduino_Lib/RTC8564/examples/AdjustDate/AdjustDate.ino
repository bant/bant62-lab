#include <Wire.h>
#include <RTC8564.h>

RTC8564 RTC;
RTC_TIME rtc_time;
const char* dayofweek[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup()
{
  Serial.begin(9600);
  RTC.begin();

  /* 2013年4月27日 0時0分0秒に時刻合わせ */
  rtc_time.year = 2013;
  rtc_time.month = 4;
  rtc_time.day = 27;
  rtc_time.hour = 0;
  rtc_time.min = 0;
  rtc_time.sec = 0;
  RTC.adjust( rtc_time );

//  これでもOK
//  RTC.adjust( 2013, 4, 24, 0, 0, 0 );
}

void loop()
{
  char buf[32];

  if(RTC.now(&rtc_time))
  {
    snprintf(buf,32,"%04u-%02u-%02u(%s) %02u:%02u:%02u\n",
    rtc_time.year,
    rtc_time.month,
    rtc_time.day,
    dayofweek[rtc_time.wday],
    rtc_time.hour,
    rtc_time.min,
    rtc_time.sec   );

    Serial.print(buf);
  }
  delay(1000);
}