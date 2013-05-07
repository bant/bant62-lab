#include <Wire.h>
#include <RTC8564.h>

RTC8564 RTC;

void setup()
{
  RTC.begin();

  /* ====================
   * CLKOUT端子出力周波数
   * --------------------
   * FREQ_32768 : 32768Hz
   * FREQ_1024  : 1024Hz
   * FREQ_32    : 32hz
   * FREQ_1     : 1Hz
   * FREQ_0     : 停止
   * --------------------
   */
  RTC.setClkOut( RTC.FREQ_32768 );
}

void loop()
{
}