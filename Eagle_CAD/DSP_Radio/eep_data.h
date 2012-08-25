#ifndef _EEP_DATA_H_
#define _EEP_DATA_H_

#include "NS9542.h"

#define NAME_SIZE 16 

typedef struct
{
    BandType    band;               //
    uint16_t    freq;               // ���g��
    char        name[NAME_SIZE];    // �ǖ�
} CHANNEL;                          // �ǃf�[�^

#define EEP_MAGIC       0xCC    // �V�O�l�`���[
#define EEP_MAGIC_PTR   0       // �V�O�l�`���[
#define EEP_COUNTER_PTR 2       // �J�E���^�|�C���^
#define EEP_START_PTR   4       // �X�^�[�g�|�C���^
#define EEP_END         1022    // ATMega328P�ł�1024�o�C�g
#define EEP_SIZE        1024    // ATMega328P�ł�1024�o�C�g
#define CHANNEL_SIZE    sizeof(CHANNEL)
#define MAX_CHANNEL     (EEP_END/sizeof(CHANNEL))


// �֐��v���g�^�C�v
extern void eep_init(void);
extern uint16_t eep_save(CHANNEL *channel);
extern uint16_t eep_count(void);
extern bool eep_read(uint16_t ch_no, CHANNEL *channel);
extern void eep_clear(void);

#endif
