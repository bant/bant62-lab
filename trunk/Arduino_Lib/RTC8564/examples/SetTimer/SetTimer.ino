#include <Wire.h>
#include <RTC8564.h>

RTC8564 RTC;
RTC_TIME rtc_time;

const char* dayofweek[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup()
{
  Serial.begin(9600);
  RTC.begin();

  // タイマー設定:10分後 繰り返し、/INTロー割り込みあり
  RTC.setTimer( RTC.TIMING_1_MIN, 10, 1, 1 );
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

  /* /INTを接続してないのならフラグを毎回読む*/
  if(RTC.checkTimerFlag())
  {
    Serial.println("WakeUp Timer!!");
  }

  delay(1000);
}
