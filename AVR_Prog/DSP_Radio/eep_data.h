#ifndef _EEP_DATA_H_
#define _EEP_DATA_H_

#include "NS9542.h"

#define NAME_SIZE 16 

typedef struct
{
    BandType    band;               //
    uint16_t    freq;               // 周波数
    char        name[NAME_SIZE];    // 局名
} CHANNEL;                          // 局データ

#define EEP_MAGIC       0xCC    // シグネチャー
#define EEP_MAGIC_PTR   0       // シグネチャー
#define EEP_COUNTER_PTR 2       // カウンタポインタ
#define EEP_START_PTR   4       // スタートポインタ
#define EEP_END         1022    // ATMega328Pでは1024バイト
#define EEP_SIZE        1024    // ATMega328Pでは1024バイト
#define CHANNEL_SIZE    sizeof(CHANNEL)
#define MAX_CHANNEL     (EEP_END/sizeof(CHANNEL))


// 関数プロトタイプ
extern void eep_init(void);
extern uint16_t eep_save(CHANNEL *channel);
extern uint16_t eep_count(void);
extern bool eep_read(uint16_t ch_no, CHANNEL *channel);
extern void eep_clear(void);

#endif
