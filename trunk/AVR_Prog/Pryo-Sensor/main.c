//=============================================================================
// File Name    : main.c
//
// Title        : 焦電型赤外線センサ AKE-1(RE-210)テストプログラム
// Revision     : 0.1
// Notes        :
// Target MCU   : Atmel AVR ATtiny2313
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2012/09/29   ばんと      開発開始
//=============================================================================
//
//【FUSE】
//   Low Fuse       0xE4
//   High Fuse      0xDF
//   Extended Fuse  0xFF
//
//==============================================================================

/*
【ピン配置】            ___ ___
                   RST# |  U  |Vcc
                 ....TX |     |PB7 ...
                 ....RX |     |PB6 ...
                   Xtl2 |     |PB5 ...
                   Xtl1 |     |PB4 ...
                ....PD2 |     |PB3 ...
                ....PD3 |     |PB2 ...
                ....PD4 |     |PB1 ...確認LED
                ....PD5 |     |PB0 ...
                    GND |     |PD6 ...AKE-1(RE-210)
                        +-----+
                       ATTiny2313
*/


/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

/* typedef ------------------------------------------------------------------- */
/* define -------------------------------------------------------------------- */
enum { WAVE_FALL=0, WAVE_RISE };

#define debug 0
/* macro --------------------------------------------------------------------- */
/*==================================*/
/* キャプチャー関係マクロ関数       */
/*==================================*/
#define IR_CAPT_TEST()  TCCR1B & _BV(ICES1)     /* Rx: Check which edge generated the capture interrupt */
#define IR_CAPT_RISE()  TCCR1B |= _BV(ICES1)    /* Rx: Set captureing is triggered on rising edge */
#define IR_CAPT_FALL()  TCCR1B &= ~_BV(ICES1)   /* Rx: Set captureing is triggered on falling edge */
#define IR_CAPT_CLEAR() TIFR |= _BV(ICF1)       /* Rx: Enable captureing interrupt */
#define IR_CAPT_ENA()   TIMSK |= _BV(ICIE1)     /* Rx: Enable captureing interrupt */
#define IR_CAPT_REG()   ICR1                    /* Rx: Returns the value in capture register */
#define IR_CAPT_DIS()   TIMSK &= ~_BV(ICIE1)    /* Tx && Rx: Disable captureing interrupt */

/*==================================*/
/* LEDの表示マクロ関数              */
/*==================================*/
#define LED_OFF()   PORTB |= _BV(PB1)
#define LED_ON()    PORTB &= ~_BV(PB1)


/* variables ----------------------------------------------------------------- */
volatile uint16_t pryo_count;
volatile uint8_t PryoState;

/* functions ----------------------------------------------------------------- */

/****************************************
* IOの初期化                            *
****************************************/
void io_init(void)
{
    // LED の設定
    // ポート B0 B1を出力に設定
    DDRB |= (1<<PB1);          // ポート B1  を出力設定

    // 赤外線センサ設定
    // ポート D6を入力に設定
    DDRD &= ~(1 << PD6);
}


/****************************************
* タイマー0オーバーフロー割込           *
*****************************************/
ISR( TIMER0_OVF_vect )
{
    if ( pryo_count != 0)
    {
        pryo_count--;
    }

    if (pryo_count == 0)
    {
        LED_OFF();

    }
    else
    {
        LED_ON();
    }

}

/****************************************
* タイマー1割込(カウントキャプチャー)   *
****************************************/
ISR( TIMER1_CAPT_vect )
{
    uint16_t PulseWidth;

    PulseWidth = ICR1;          // カウント値の読み取り
    TCNT1 = 0x0000;             // カウンタ値のクリア

    switch ( PryoState )
    {
    case WAVE_FALL:
        if (bit_is_clear(PIND,PIND6))       // (負論理)立ち下がりエッジ検出
        {
            IR_CAPT_RISE();                 // 立ち上がり駆動に変更
            PryoState = WAVE_RISE;
        }
        break;

    case WAVE_RISE:
        if ( bit_is_set(PIND, PIND6) )      // (負論理)立ち上がりエッジ検出(リーダーのキャリア停止)
        {
            IR_CAPT_FALL();                 // 立ち下がり駆動に変更(ICPの検出エッジをL)
            PryoState = WAVE_FALL;          // はじめに戻す
            if (PulseWidth > 1520)          // 約50mS以上の信号
            {
                pryo_count += 32*10;
                if (pryo_count > 32*60)
                {
                    pryo_count = 32*60;
                }
            }
        }
        break;

    default:
        IR_CAPT_FALL();                     // 立ち下がり駆動に変更(ICPの検出エッジをL)
        PryoState = WAVE_FALL;
        break;
    }
}


/****************************************
* タイマー0の初期化                    *
****************************************/
void timer0_init(void)
{
    TCCR0A =
        (0<<COM0A1) |
        (0<<COM0A0) |           /* 標準ポート動作 (OC0A切断) */
        (0<<COM0B1) |
        (0<<COM0B0) |           /* 標準ポート動作 (OC0B切断) */
        (0<<WGM01)  |
        (0<<WGM00);             /* 標準動作 */

    TCCR0B =
        (0<<FOC0A) |
        (0<<FOC0B) |            /* 不許可 */
        (0<<WGM02) |            /* 標準動作 */
        (1<<CS02)  |            /* プリスケーラは 1/1024 */
        (0<<CS01)  |
        (1<<CS00);

    TCNT0 = 0;              // タイマ0の初期値
    TIMSK |= _BV(TOIE0);    // タイマ0オーバーフローだけ割り込み許可
}


/****************************************
* タイマー1の初期化                     *
* キャプチャー割り込みとして設定        *
*****************************************/
void timer1_init(void)
{
    //======================
    // TCCR1設定
    //======================
    TCCR1A =            //
        (0<<COM1A1) |   // COM1A1:COM1A0
        (0<<COM1A0) |   //  00  OC1A切断(使わないということ)
        (0<<COM1B1) |   // COM1B1:COM1B0
        (0<<COM1B0) |   //  00  OC1A切断(使わないということ)
        (0<<WGM11)  |   // WGM11:WGM10 波形生成種別(4bitの下位2bit)
        (0<<WGM10);     //  00  使わない

    //======================
    // TCCR1B 設定
    //======================
    TCCR1B =            //
        (1<<ICNC1)  |   // ICNC1 : 捕獲起動入力1雑音消去許可    (0:不許可 1:許可)
        (1<<ICES1)  |   // ICES1 : 捕獲起動入力端選択           (0:不選択 1:選択)
        (0<<WGM13)  |   // WGM13 : 波形生成種別(4bitの上位2bit)
        (0<<WGM12)  |   // WGM12 : 波形生成種別(4bitの上位2bit)
        (1<<CS12)   |   // CS12:CS11:CS10 => プリスケール(256分周)
        (0<<CS11)   |   //
        (1<<CS10);      //

    //======================
    // カウンタ初期化
    //======================
    TCNT1 = 0x0000;
    OCR1A = 0xFFFF;
    OCR1B = 0xFFFF;

    //======================
    //   外部割り込み設定
    //======================
    // 外部割り込み(未使用)
    GIMSK = 0x00;
    MCUCR = 0x00;

    // コンパレータ未使用
    ACSR=0x80;

    // タイマー1キャプチャー割込許可
    TIMSK |= (1<<ICF1);
}

/*======================================*/
/* メイン                               */
/*======================================*/
int main(void)
{

    uint8_t i;

    cli(); // 割り込み禁止

    io_init();

    // センサの起動を待つ
    for(i=0; i < 90; i++)
    {
        LED_ON();
        _delay_ms(500);
        LED_OFF();
        _delay_ms(500);
    }

    // LEDの表示で使用
    timer0_init();

    // 焦電赤外線センサで使用
    timer1_init();

    IR_CAPT_FALL();     // 立ち下がり駆動に変更
    IR_CAPT_CLEAR();    // キャプチャー割込要求クリア
    PryoState = WAVE_FALL;
    pryo_count = 0;     // 0秒

    set_sleep_mode(SLEEP_MODE_IDLE);// パワーダウンモードを設定

    sei();          // 全体割込許可

    while (1)
    {
        sleep_mode();
    }

    return 0;
}
