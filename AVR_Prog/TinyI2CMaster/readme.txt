本書庫には、AVR ATtiny用 I2Cマスタライブラリィが含まれてます。

構成は以下の通り

TinyI2CMaster.c … AVR ATtiny用 I2Cマスタライブラリィソース
TinyI2CMaster.h … AVR ATtiny用 I2Cマスタライブラリィヘッダ
24LCxxx.c       … マイクロチップ製 I2C EEPROM 24LCxxx用ライブラリィソース
24LCxxx.h       … マイクロチップ製 I2C EEPROM 24LCxxx用ライブラリィヘッダ
RTC8564.c       … エプソントヨコム製 I2C RTC-8564NB用ライブラリィソース
RTC8564.h       … エプソントヨコム製 I2C RTC-8564NB用ライブラリィソヘッダ
readme.txt      … このファイル

AVR ATtiny用 I2Cマスタライブラリィ TinyI2CMaster.cは、がた老さんのSoftI2C
のソースを元にばんとが改良(改悪?)したものです。
がた老さんは、SoftI2Cと名付けられてたのですが、AVR USI機能を利用しての実装
なのでTinyI2CMasterと改名させていただきました。
ATtiny85,ATtiny2313,ATtiny861での動作確認をしました。

TinyI2CMasterライブラリィの使い方は、24LCxxxおよびRTC8564のライブラリィを
参照してください。

                                      2013/4/17   ばんと bant62@gmail.com
