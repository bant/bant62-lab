#ifndef __NS9542__H__
#define __NS9542__H__

#include <avr/io.h>
#include "def.h"

#define I2C_ADDR_NS9542 0xC8

typedef enum { JAPAN, EUROPE,USA, ASIA, RUSSIAN } RegionType;
typedef enum { AM, OIRT, FM } BandType;
typedef enum { CLEAR_BIT, SET_BIT } SetType;

extern void NS9542_WriteCmd( uint8_t addr, uint8_t data );
extern uint8_t NS9542_ReadReg( uint8_t addr );
extern void NS9542_RegBitMaskSet( uint8_t addr, uint8_t mask, uint8_t set_bit );
extern void NS9542_RegBitSet( uint8_t addr, uint8_t set_bit, SetType set_clear );

extern void NS9542_set_reqion( RegionType region );
extern void NS9542_init( void );
extern void NS9542_Starting( BandType band, uint16_t freq, uint8_t clk_sel );
extern void NS9542_power_on_seq( void );
extern void NS9542_power_off_seq( void );
extern void NS9542_dsp_alignment( void );
extern void NS9542_imf_adjust( void );
extern void NS9542_dsp_align_body( void );
extern void NS9542_best_iml( uint8_t iml );
extern void NS9542_find_pg( uint8_t ialgn, uint8_t *fine_phase, uint8_t *fine_gain, uint8_t *result_pg );
extern void NS9542_table_write( uint8_t *fine_p, uint8_t *fine_g );
extern void NS9542_band_freq_change( BandType band, uint16_t freq,  uint8_t clk_sel );
extern void NS9542_band_setting( BandType band );
extern void NS9542_freq_setting(uint16_t psy, uint8_t clk_sel );
extern void NS9542_mute( bool mute );

extern uint16_t NS9542_fm_pluse_seek( uint16_t start_freq, uint8_t clk_sel );
extern uint16_t NS9542_am_pluse_seek( uint16_t start_freq, uint8_t clk_sel );

extern int8_t NS9542_signal_strength( BandType band );
extern bool NS9542_is_stereo( void );

extern uint16_t fm_low_limit_freq;
extern uint16_t fm_high_limit_freq;
extern uint16_t fm_step;
extern BandType fm_band_type;
extern uint16_t am_low_limit_freq;
extern uint16_t am_high_limit_freq;
extern uint16_t am_step;

#endif
