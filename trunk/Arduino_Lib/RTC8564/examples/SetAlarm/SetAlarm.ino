#include <Wire.h>
#include <RTC8564.h>

RTC8564 RTC;
ALARM_TIME alarm;
RTC_TIME rtc_time;
const char* dayofweek[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup()
{
  Serial.begin(9600);
  RTC.begin();
  //// アラーム設定:毎時25分、/INTロー割り込みあり
  //alarm.min = 25;
  //alarm.hour = 0xFF;              // 使用しないのなら0xFFに設定
  //alarm.day = 0xFF;               //           〃
  //alarm.wday = 0xFF;              //           〃
  //RTC.setAlarm( alarm, 1 );

  // アラーム設定:23時15分、/INTロー割り込みあり
  alarm.min = 40;
  alarm.hour = 23;
  alarm.day = 0xFF;                 // 使用しないのなら0xFFに設定
  alarm.wday = 0xFF;                //           〃
  RTC.setAlarm( alarm, 1 );
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
  if(RTC.checkAlarmFlag())
  {
    Serial.println("WakeUp Alarm!!");
  }

  delay(1000);
}
