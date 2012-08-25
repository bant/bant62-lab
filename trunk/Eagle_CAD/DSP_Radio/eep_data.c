/* ****** EEPROM Stream Funcitons *********** */
/*     12/8/2007 V1.2 EOF fixed               */
/*               V1.3 for Tiny861             */
/*     12/20     V1.4 bug fixed(addr 2bytes)  */
/*     5/11/2008 V2.0 eep_writestr added      */
#include <avr/io.h>
#include <avr/eeprom.h>
#include "def.h"
#include "eep_data.h"

void eep_init(void)
{
    uint8_t magic;                 //先頭バイトのMagic Nunberが頼り

    magic = eeprom_read_byte((uint8_t *)EEP_MAGIC_PTR);
    eeprom_busy_wait();
    if (magic != EEP_MAGIC)       //出来ていれば先頭テーブルを読む
    {
        eeprom_write_byte((uint8_t *)EEP_MAGIC_PTR,EEP_MAGIC);      //マジックナンバの書き込み
        eeprom_busy_wait();
        eeprom_write_word((uint16_t *)EEP_COUNTER_PTR,0);           //登録個数の初期化
        eeprom_busy_wait();
    }
}


uint16_t eep_save(CHANNEL *channel)
{

    uint16_t pos;
    uint16_t count;                 //先頭WORDに登録済み数が入ってる

    count = eep_count();
    if (count >= MAX_CHANNEL)
    {
        return -1;
    }

    pos = sizeof(CHANNEL) * count + EEP_START_PTR;

    eeprom_write_block ((const void *)channel, (void *)pos, sizeof(CHANNEL));
    eeprom_busy_wait();
    count++;
    eeprom_write_word((uint16_t *)EEP_COUNTER_PTR,count);                       //登録局数を更新
    eeprom_busy_wait();

    return count;
}

uint16_t eep_count(void)
{

    uint16_t count;                 //先頭WORDに登録済み数が入ってる

    count = eeprom_read_word((uint16_t *)EEP_COUNTER_PTR);
    eeprom_busy_wait();

    return count;
}


bool eep_read(uint16_t ch_no, CHANNEL *channel)
{
    uint16_t count;
    uint16_t pos;

    count = eep_count();
    if (ch_no > count)
        return false;

    pos = sizeof(CHANNEL) * (ch_no -1) + EEP_START_PTR;
    eeprom_read_block ((void *)channel, (const void *)pos, sizeof(CHANNEL));

    return true;
}

void eep_clear(void)
{
    eeprom_write_word((uint16_t *)EEP_COUNTER_PTR,0);           //登録個数の初期化
    eeprom_busy_wait();
}
