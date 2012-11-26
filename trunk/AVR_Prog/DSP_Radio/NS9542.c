//=============================================================================
// File Name    : NS9542.c
//
// Title        : NS9542 ラジオモジュールドライバ
// Revision     : 0.1
// Notes        :
// Target MCU   : AVR ATMega328
// Tool Chain   : AVR toolchain Ver3.4.1.1195
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// ????/??/??   そらみみさん
// 2012/08/14   ばんと      修正完了
//=============================================================================

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include "def.h"
#include "NS9542.h"
#include "i2c.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* macro ---------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
uint16_t fm_low_limit_freq;
uint16_t fm_high_limit_freq;
uint16_t fm_step;
BandType fm_band_type;
uint16_t am_low_limit_freq;
uint16_t am_high_limit_freq;
uint16_t am_step = 9;

/* function prototypes -------------------------------------------------------*/
/* functions -----------------------------------------------------------------*/


void NS9542_WriteCmd( uint8_t addr, uint8_t data )
{
    twiSendStartCondition( );
    twiSendAddress( I2C_ADDR_NS9542 );
    twiSendByte( addr );
    twiSendByte( data );
    twiSendEndCondition( );
}

uint8_t NS9542_ReadReg( uint8_t addr )
{
    uint8_t data;

    twiSendStartCondition( );
    twiSendAddress( I2C_ADDR_NS9542 );
    twiSendByte( addr );
    twiSendStartCondition( );
    twiSendAddress( I2C_ADDR_NS9542 | 0x01 );
    twiReceiveByte( &data, 1 );
    twiSendEndCondition( );

    return data;
}

void NS9542_RegBitMaskSet( uint8_t addr, uint8_t mask, uint8_t set_bit )
{
    uint8_t data;

    data = NS9542_ReadReg( addr );
    data &= ~mask;
    data |= set_bit;
    NS9542_WriteCmd( addr, data );
}

void NS9542_RegBitSet( uint8_t addr, uint8_t set_bit, SetType set_clear )
{
    uint8_t data;

    data = NS9542_ReadReg( addr );

    if (set_clear == SET_BIT)
    {
        data |= set_bit;
    }
    else
    {
        data &= ~set_bit;
    }

    NS9542_WriteCmd( addr, data );
}

void NS9542_init( void )
{
    twiSendStartCondition( );
    twiSendByte( 0xff );
    twiSendEndCondition( );
}

void NS9542_set_reqion( RegionType region )
{
    switch ( region )
    {
    case JAPAN:
        fm_low_limit_freq = 7600;
        fm_high_limit_freq = 10800;
        fm_step = 10;
        fm_band_type = FM;
        am_low_limit_freq = 522;
        am_high_limit_freq = 1629;
        am_step = 9;
        break;

    case EUROPE:
        fm_low_limit_freq = 8750;
        fm_high_limit_freq = 10800;
        fm_step = 50;
        fm_band_type = FM;
        am_low_limit_freq = 522;
        am_high_limit_freq = 1620;
        am_step = 9;
        break;

    case USA:
        fm_low_limit_freq = 8750;
        fm_high_limit_freq = 10800;
        fm_step = 20;
        fm_band_type = FM;
        am_low_limit_freq = 520;
        am_high_limit_freq = 1720;
        am_step = 10;
        break;

    case ASIA:
        fm_low_limit_freq = 8700;
        fm_high_limit_freq = 10800;
        fm_step = 10;
        fm_band_type = FM;
        am_low_limit_freq = 522;
        am_high_limit_freq = 1611;
        am_step = 9;
        break;

    case RUSSIAN:
        fm_low_limit_freq = 6500;
        fm_high_limit_freq = 7400;
        fm_step = 50;
        fm_band_type = OIRT;
        am_low_limit_freq = 522;
        am_high_limit_freq = 1620;
        am_step = 9;
        break;
    }
}


void NS9542_Starting( BandType band, uint16_t freq, uint8_t clk_sel )
{
    uint16_t psy;

    if ( band == AM )
    {
        psy = freq;
    }
    else
    {
        psy = freq / 5;
    }

    NS9542_power_on_seq( );
    NS9542_dsp_alignment( );

//  NS9542_Audio_Output_setting();   ドキュメントに記載なし、どう実装していいのか分からない??

    NS9542_band_setting( band );
    NS9542_freq_setting( psy, clk_sel );

    NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );    // MUTE Off
}

void NS9542_power_on_seq( void )
{
    NS9542_init();                                  // I2C Bus Reset

    NS9542_WriteCmd( 0xFE, 0xAA );                  // Resister Reset

    NS9542_WriteCmd( 0x01, 0x30 );
    NS9542_WriteCmd( 0x0c, 0x80 );
    NS9542_WriteCmd( 0x0e, 0x34 );
    NS9542_WriteCmd( 0x15, 0xc4 );
    NS9542_WriteCmd( 0x20, 0x3c );
    NS9542_WriteCmd( 0x21, 0x03 );
    NS9542_WriteCmd( 0x22, 0x0a );
    NS9542_WriteCmd( 0x23, 0x0a );
    NS9542_WriteCmd( 0x30, 0xff );
    NS9542_WriteCmd( 0x3d, 0x07 );
    NS9542_WriteCmd( 0x40, 0x1a );
    NS9542_WriteCmd( 0x41, 0x9a );
    NS9542_WriteCmd( 0x50, 0xe1 );
    NS9542_WriteCmd( 0x54, 0xb0 );
    NS9542_WriteCmd( 0x55, 0x36 );
    NS9542_WriteCmd( 0x5c, 0xc8 );
    NS9542_WriteCmd( 0x5d, 0x61 );
    NS9542_WriteCmd( 0x5e, 0x88 );
    NS9542_WriteCmd( 0x5f, 0xa5 );
    NS9542_WriteCmd( 0x71, 0x2c );
    NS9542_WriteCmd( 0x72, 0x06 );

    NS9542_RegBitSet( 0x00, _BV(0), SET_BIT );      // Chip電源 ON
    NS9542_RegBitSet( 0x00, _BV(1), SET_BIT );      // MUTE ON
}

void NS9542_power_off_seq( void )
{
    NS9542_RegBitSet( 0x00, _BV(1), SET_BIT );     // MUTE ON

    _delay_ms( 20 );

    NS9542_WriteCmd( 0x30, 0xEF );

    NS9542_RegBitSet( 0x00, _BV(0), CLEAR_BIT );   // Chip電源 OFF
}

void NS9542_dsp_alignment( void )
{
    NS9542_RegBitMaskSet( 0x0E, _BV(5) | _BV(6), _BV(6) );

    NS9542_WriteCmd( 0x01, 0x08 );
    NS9542_WriteCmd( 0x15, 0x0c );
    NS9542_WriteCmd( 0x16, 0x17 );
    NS9542_WriteCmd( 0x37, 0x82 );
    NS9542_WriteCmd( 0x3d, 0x37 );

    _delay_ms( 50 );

    NS9542_imf_adjust( );
    NS9542_dsp_align_body( );

    NS9542_WriteCmd( 0x01, 0x38 );

    NS9542_RegBitMaskSet( 0x0E, _BV(5) | _BV(6), _BV(5) );

    NS9542_WriteCmd( 0x15, 0xc0 );
    NS9542_WriteCmd( 0x17, 0x20 );
    NS9542_WriteCmd( 0x32, 0x00 );
    NS9542_WriteCmd( 0x37, 0x01 );
}

void NS9542_imf_adjust( void )
{
    uint8_t bF, imf, fhm, g_fhm;

    bF = 0;
    g_fhm = 0xf0;

    NS9542_WriteCmd( 0x15, 0x0e );
    NS9542_WriteCmd( 0x3d, 0x27 );

    for ( fhm = 0; fhm < 4; fhm++ )
    {
        bF = 0;
        for ( imf = 0; imf < 3; imf++ )
        {
            NS9542_WriteCmd( 0x37, fhm );
            NS9542_WriteCmd( 0x16, 22 + imf );
            _delay_ms( 10 );

            if ( ( NS9542_ReadReg( 0x70 ) & 0x0c ) == 0x0c )
            {
                bF++;
                if ( imf == 1 && g_fhm == 0xf0 )
                {
                    g_fhm = fhm;
                }
            }
        }

        if ( bF == 3 )
        {
            g_fhm = fhm;
            break;
        }
    }

    NS9542_WriteCmd( 0x37, 0x80 | g_fhm );
    NS9542_WriteCmd( 0x16, 23 );
    NS9542_WriteCmd( 0x3d, 0x37 );
    _delay_ms( 50 );
}

void NS9542_dsp_align_body( void )
{
    uint8_t iml, ialgn, cnt, fp, fg;
    uint8_t fine_p[5] = { 0, 0, 0, 0, 0 };
    uint8_t fine_g[5] = { 0, 0, 0, 0, 0 };

    iml = 5;

    for ( ialgn = 0; ialgn < 4; ialgn++ )
    {
        NS9542_WriteCmd( 0x15, 0x0a | ( ialgn << 4 ) );
        _delay_ms( 140 );

        NS9542_best_iml( iml );

        cnt = 0;
        fp = 0;
        fg = 0;
        for ( cnt = 0; cnt < 5; cnt++ )
        {
            uint8_t fine_phase, fine_gain, result_pg;

            NS9542_find_pg( ialgn, &fine_phase, &fine_gain, &result_pg );
            if ( result_pg == 0 )
            {
                return;
            }
            fp = fp + fine_phase;
            fg = fg + fine_gain;
            if ( cnt == 2 && ialgn < 2 )
            {
                cnt++;
                break;
            }
        }
        fine_p[ialgn] = fp / cnt;
        fine_g[ialgn] = fg / cnt;
    }

    NS9542_table_write( fine_p, fine_g );
}

void NS9542_best_iml( uint8_t iml )
{
    NS9542_WriteCmd( 0x32, 0x00 );

    while ( iml < 16 )
    {
        NS9542_WriteCmd( 0x17, 0xc0 | iml );

        _delay_ms( 5 );

        if ( !( NS9542_ReadReg( 0x70 ) & 0x08 ) )
        {
            break;
        }
        iml++;
    }

    iml--;
    NS9542_WriteCmd( 0x17, 0xc0 | iml );
    NS9542_WriteCmd( 0x32, 0x80 );

    _delay_ms( 5 );

    NS9542_WriteCmd( 0xfe, 0x0a );

    _delay_ms( 20 );
}

void NS9542_find_pg( uint8_t ialgn, uint8_t *fine_phase, uint8_t *fine_gain, uint8_t *result_pg )
{
    uint8_t i, j;

    for ( i = 0; i < 16; i++ )
    {
        NS9542_WriteCmd( 0x15, 0x0a | ( ialgn << 4 ) );
        NS9542_WriteCmd( 0x15, 0x0b | ( ialgn << 4 ) );
        if ( NS9542_ReadReg( 0x05 ) & 0x08 )
        {
            for ( j = 0; j < 20; j++ )
            {
                if ( !( NS9542_ReadReg( 0x05 ) & 0x08 ) )
                {
                    uint8_t g = NS9542_ReadReg( 0x65 );
                    uint8_t p = NS9542_ReadReg( 0x66 );

                    if ( g >= 103 && g <= 138 && 2 >= p && p <= 14 )
                    {
                        *fine_gain = g;
                        *fine_phase = p;
                        *result_pg = 1;
                        return;
                    }
                }
                _delay_ms( 10 );
            }
        }
    }

    *result_pg = 0;
}

void NS9542_table_write( uint8_t *fine_p, uint8_t *fine_g )
{
    uint8_t i, j, k, result;

    result = 0;

    for ( i = 0; i < 4; i++ )
    {
        NS9542_WriteCmd( 0x38, fine_g[i] );
        NS9542_WriteCmd( 0x39, fine_p[i] << 4 );
        for ( j = 0; j < 10; j++ )
        {
            NS9542_WriteCmd( 0x15, 0x0e | ( i << 4 ) );
            NS9542_WriteCmd( 0x15, 0x03 | ( i << 4 ) );
            if ( NS9542_ReadReg( 0x05 ) & 0x08 )
            {
                _delay_ms( 100 );

                for ( k = 0; k < 10; k++ )
                {
                    if ( !( NS9542_ReadReg( 0x05 ) & 0x08 ) )
                    {
                        result++;
                        goto L1;
                    }
                    _delay_ms( 10 );
                }
                break;
            }
        }

L1:
        if ( result != i + 1 )
        {
            break;
        }
    }

}

void NS9542_band_freq_change( BandType band, uint16_t freq, uint8_t clk_sel )
{
    uint16_t psy;

    if (band == AM)
    {
        psy = freq;
    }
    else
    {
        psy = freq / 5;
    }

    // ※注意:ドキュメントがおかしい
    NS9542_RegBitSet( 0x00, _BV(1), SET_BIT );     // MUTE ON

    _delay_ms( 20 );

    NS9542_band_setting( band );

    NS9542_freq_setting( psy, clk_sel );

    // ※注意:ドキュメントがおかしい
    NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );   // MUTE OFF
}

void NS9542_band_setting( BandType band )
{
    switch ( band )
    {
    case AM:
        NS9542_RegBitMaskSet( 0x00, _BV(4) | _BV(5) | _BV(6), _BV(5) );         // BAND
        NS9542_RegBitMaskSet( 0x0C, _BV(0) | _BV(1) | _BV(2) | _BV(3), 0 );     // AMFSEL

        if ( band == AM )
        {
            if ( am_step == 9 )
            {
                NS9542_RegBitSet( 0x04, _BV(7), SET_BIT );         // 9KHz
                NS9542_RegBitSet( 0x0C, _BV(7), SET_BIT );         // AAUTO
                NS9542_RegBitMaskSet( 0x0C, _BV(5) | _BV(6), 0 );  // CM
                NS9542_RegBitSet( 0x0C, _BV(4), CLEAR_BIT );       // CME
            }
            else if ( am_step == 10 )
            {
                NS9542_RegBitSet( 0x04, _BV(7), CLEAR_BIT );       // 10KHz
                NS9542_RegBitSet( 0x0C, _BV(7), SET_BIT );         // AAUTO
                NS9542_RegBitMaskSet( 0x0C, _BV(5) | _BV(6), 0 );  // CM
                NS9542_RegBitSet( 0x0C, _BV(4), CLEAR_BIT );       // CME
            }
        }
        break;

    case OIRT:
        NS9542_RegBitMaskSet( 0x00, _BV(4) | _BV(5) | _BV(6), _BV(4) );     // OIRT BAND
        break;

    case FM:
    default:
        NS9542_RegBitMaskSet( 0x00, _BV(4) | _BV(5) | _BV(6), 0 );          // FM BAND
        break;
    }
}

void NS9542_freq_setting( uint16_t psy, uint8_t clk_sel )
{
    if ( clk_sel == 0 )
    {
        NS9542_RegBitSet( 0x10, _BV(4), SET_BIT );                                  // CLK_SEL_AUTO
        NS9542_RegBitMaskSet( 0x10, _BV(0) | _BV(1) | _BV(2) | _BV(3), 0 );         // CLK_SEL
    }
    else
    {
        NS9542_RegBitSet( 0x10, _BV(4), CLEAR_BIT );                                // CLK_SEL_AUTO
        NS9542_RegBitMaskSet( 0x10, _BV(0) | _BV(1) | _BV(2) | _BV(3), clk_sel );   // CLK_SEL
    }

    NS9542_WriteCmd( 0x02, psy & 0xff );
    NS9542_WriteCmd( 0x03, psy >> 8 );
}

void NS9542_mute( bool mute )
{
    if ( mute )
    {
        NS9542_RegBitSet( 0x00, _BV(1), SET_BIT );              // MUTE ON
    }
    else
    {
        NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );            // MUTE Off
    }

}

int8_t NS9542_signal_strength( BandType band )
{
    if ( band == AM )
    {
        NS9542_RegBitSet( 0x01, _BV(3), CLEAR_BIT );            // STATION_EN Off
        NS9542_RegBitSet( 0x01, _BV(3), SET_BIT );              // STATION_EN On
        _delay_ms( 12 );

        return ( (int8_t)NS9542_ReadReg( 0x06 ) & 0x7f );
    }
    else
    {
        return (int8_t)NS9542_ReadReg( 0x6C );
    }

}

// FM Only
bool NS9542_is_stereo( void )
{
    return  (( NS9542_ReadReg( 0x05 ) & 0x01 ) == 0x01 );
}

uint16_t NS9542_fm_pluse_seek( uint16_t start_freq,  uint8_t clk_sel )
{
    uint16_t psy, freq;
    uint8_t loop;
    int8_t sensitivity;

    freq = start_freq;

    NS9542_RegBitSet( 0x00, _BV(1), SET_BIT );                      // MUTE ON

FM_SEEK_LABEL:
    psy = freq / 5;
    NS9542_freq_setting( psy, clk_sel );

    NS9542_WriteCmd( 0xFE, 0x0A );                                  // DSP Reset

    _delay_ms( 30 );

    for ( loop=0; loop < 2; loop++ )
    {
        if ( NS9542_ReadReg( 0x05 ) & _BV(2) )
        {
            freq += fm_step;
            if ( freq > fm_high_limit_freq )
            {
                NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );        // MUTE Off
                return 0;
            }
            else
            {
                goto FM_SEEK_LABEL;
            }
        }

        sensitivity = (int8_t)NS9542_ReadReg( 0x6C );
        if (sensitivity < 30 )                                      // デフォルトの感度は30
        {
            freq += fm_step;
            if ( freq > fm_high_limit_freq )
            {
                NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );        // MUTE Off
                return 0;
            }
            else
            {
                goto FM_SEEK_LABEL;
            }
        }
        _delay_ms( 10 );
    }

    NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );                    // MUTE Off

    return freq;
}

uint16_t NS9542_am_pluse_seek( uint16_t start_freq, uint8_t clk_sel )
{
    uint16_t psy, freq;
    uint8_t loop;
    int8_t sensitivity;

    freq = start_freq;

    NS9542_RegBitSet( 0x00, _BV(1), SET_BIT );                      // MUTE ON

AM_SEEK_LABEL:
    psy = freq;
    NS9542_freq_setting( psy, clk_sel );

    NS9542_WriteCmd( 0xFE, 0x0A );                                  // DSP Reset

    _delay_ms( 30 );

    for (loop=0; loop < 2; loop++)
    {
        NS9542_RegBitSet( 0x01, _BV(3), CLEAR_BIT );                // STATION_EN Off
        NS9542_RegBitSet( 0x01, _BV(3), SET_BIT );                  // STATION_EN On

        _delay_ms( 12 );

        if ( !(NS9542_ReadReg( 0x05 ) & _BV(1)) )                   // STATION ?
        {
            freq += am_step;
            if ( freq > am_high_limit_freq )
            {
                NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );        // MUTE Off
                return 0;
            }
            else
            {
                goto AM_SEEK_LABEL;
            }
        }

        sensitivity = (int8_t)NS9542_ReadReg( 0x06 ) & 0x7F;
        if (sensitivity < 40 )                                      // デフォルトの感度は40
        {
            freq += am_step;
            if ( freq > am_high_limit_freq )
            {
                NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );        // MUTE Off
                return 0;
            }
            else
            {
                goto AM_SEEK_LABEL;
            }
        }
    }

    NS9542_RegBitSet( 0x00, _BV(1), CLEAR_BIT );      // MUTE Off

    return freq;
}
