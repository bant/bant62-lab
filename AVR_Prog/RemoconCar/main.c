/********************************************************
 *                                                      *
 *　   Tiny2313による赤外線リモコン受信モジュール       *
 *                                                      *
 *                                                      *
 *                      Created on: 2010/10/16          *
 *                          Author: bant                *
 *                                                      *
 ********************************************************

【ピン配置】　　　　  　___ ___
                   RST# |  U  |Vcc
             UART....TX |     |PB7 ...
              "  ....RX |     |PB6 ...
               　  Xtl2 |  　 |PB5 ...
                   Xtl1 |     |PB4 ...モータ2のIN2
                ....PD2 |     |PB3 ...モータ2のIN1
    モータ1のIN1....PD3 |     |PB2 ...OC0A(モータ1PWM)
    モータ1のIN2....PD4 |     |PB1 ...通常LED
OC0B(モータ2PWM)....PD5 |     |PB0 ...Switch
                    GND |     |PD6 ...ICP(input capture)
                        +-----+
                       ATTiny2313

【機能】
　　　・タイマーのキャプチャー機能を使って、赤外線モジュール
 　　　　からのPulse Position Modulation(パルス位置変調）
 　　　　信号をデコードする。
      ・信号は繰り返されるので、最後の回のみ有効とする。
 　　 ・デコードした信号をLCDに表示する。

【改訂履歴】
2011/2/7 V0.1: NECフォーマット信号のデコードルーチン完成
--------------------------------------------------------*/


//----- Include Files ---------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "remocon2313.h"
#include "uart2313.h"

/****************************************
            グローバル変数定義          *
****************************************/

volatile BYTE BitCount;             // リモコン受信ビットカウンタ
volatile FLAGS ReqFlag;             // 要求フラグ
volatile L4BYTE RemData;
volatile L4BYTE DispRemData;        // リモコン受信データ,表示バッファ
volatile L4BYTE PreRemData;

volatile BYTE timer0_count = 32;

volatile BYTE flag;


/****************************************
*                                       *
* タイマー０オーバーフロー割込          *
*                                       *
*  内部発振8MHz動作　かつ1024分周で     *
*  255回のカウントオーバーフローで      *
*  割り込みが入るとすると、割り込み     *
*  時間は、                             *
*  (1/8000000)*1024*255 = 0.03264(s)    *
*                        => 3.264(ms)   *
*                                       *
*  32倍すると約１秒になる               *
*                                       *
*****************************************/
ISR( TIMER0_OVF_vect )
{
    BYTE set_data;

    switch (flag)
    {
    case RECEIVE_DATA:
        set_data = 32;
        break;

    case REPEAT_DATA:
        set_data = 24;
        break;

    case RECEIVE_ERR_DATA:
        set_data = 16;
        break;

    case MAKER_CODE_ERROR:
        set_data = 8;
        break;

    default:
        set_data = 0;
        break;
    }

    if (set_data == 0)
    {
        LED_ON();
    }
    else
    {
        if (--timer0_count == 0)
        {
            timer0_count = set_data;
            PORTB ^= (1<<PB1);
        }
    }
}



/****************************************
* タイマー１割込(カウントキャプチャー)  *
****************************************/
ISR( TIMER1_CAPT_vect )
{
    WORD PulseWidth;

    PulseWidth = ICR1;  // カウント値読み取り
    TCNT1=0x0000;   // カウント値クリア

    switch (ReqFlag.RmStatus)
    {
    case RM_INTIAL :    // 初期状態
        if (bit_is_clear(PIND, PIND6))
        { // (負論理)立ち下がりエッジ検出（リーダーのキャリア開始）
            IR_CAPT_RISE();                     // 立ち上がり駆動に変更(ICPの検出エッジをH)
            ReqFlag.RmStatus = RM_READER_L;
        }
        break;
    case RM_READER_L :  // リーダーのキャリアON終了(9mS)
        if (bit_is_set(PIND, PIND6))
        { // (負論理)立ち上がりエッジ検出（リーダーのキャリア停止）
            IR_CAPT_FALL();                 // 立ち下がり駆動に変更(ICPの検出エッジをL)
            if ((PulseWidth >= 7200)  && (PulseWidth <= 10800))
            { // リーダー部キャリアON時間チェック(9mS)
                ReqFlag.RmStatus = RM_READER_H;
            }
            else
            {// 未定義フォーマット検出
                ReqFlag.ErrDisp = 1;
                BitCount = 0;
                ReqFlag.RmStatus = RM_INTIAL;
            }
        }
        break;
    case RM_READER_H :  // リーダーのキャリアOFF終了(4.5mS)
        if (bit_is_clear(PIND, PIND6))
        {// (負論理)立ち下がりエッジ検出（データ又はストップビットのキャリア）
            if ((PulseWidth >= 3600)  && (PulseWidth <= 5400))
            {// リーダー部キャリアOFF時間チェック(4.5mS)
                ReqFlag.RmStatus = RM_DATA;         // データの開始
                IR_CAPT_FALL();             // 立ち下がり駆動に変更(ICPの検出エッジをL)
            }
            else if ((PulseWidth >= 1800)  && (PulseWidth <= 2700))
            {// リピートリーダー部キャリアOFF時間チェック(2.25mS)
                ReqFlag.RmStatus = RM_STOP;         // ストップビット
                IR_CAPT_RISE();                     // 立ち上がり駆動に変更(ICPの検出エッジをH)
            }
            else
            {// 未定義フォーマット検出
                ReqFlag.ErrDisp = 1;
                BitCount = 0;
                IR_CAPT_FALL();                     // 立ち下がり駆動に変更(ICPの検出エッジをL)
                ReqFlag.RmStatus = RM_INTIAL;
            }
        }
        break;
    case RM_DATA :  // データ１ビット受信の終了
        if (bit_is_clear(PIND, PIND6))
        {// (負論理)立ち下がりエッジ検出（データ又はストップビットのキャリア）
            if ((PulseWidth >= 896)  && (PulseWidth <= 1344))
            {// データ0検出(1.12mS)
                RemData.DATA_LONG >>= 1;            // データ右シフト
                BitCount++;                         // 受信カウント更新
                if (BitCount == NEC_BIT_COUNT)
                {
                    ReqFlag.RmStatus = RM_STOP;     // ストップビット
                    IR_CAPT_RISE();                 // 立ち上がり駆動に変更(ICPの検出エッジをH)
                }
            }
            else if ((PulseWidth >= 1800)  && (PulseWidth <= 2700))
            {// データ1検出(1.68mS)
                RemData.DATA_LONG >>= 1;            // データ右シフト
                RemData.DATA_LONG |= 0x80000000;
                BitCount++;                         // 受信カウント更新
                if (BitCount == NEC_BIT_COUNT)
                {
                    ReqFlag.RmStatus = RM_STOP;     // ストップビット
                    IR_CAPT_RISE();                 // 立ち上がり駆動に変更(ICPの検出エッジをH)
                }
            }
            else
            {// 未定義フォーマット検出
                ReqFlag.ErrDisp = 1;
                BitCount = 0;
                IR_CAPT_FALL();                     // 立ち下がり駆動に変更(ICPの検出エッジをL)
                ReqFlag.RmStatus = RM_INTIAL;
            }
        }
        break;
    case RM_STOP :  // ストップビットの終了
        if (bit_is_set(PIND, PIND6))
        {// (負論理)立ち上がりエッジ検出（ストップビットのキャリア停止）
            DispRemData = RemData;
            ReqFlag.DispCode = 1;                   // コード表示要求セット
            BitCount = 0;                           // 受信カウントクリア
            ReqFlag.RmStatus = RM_INTIAL;
            IR_CAPT_FALL();                         // 立ち下がり駆動に変更(ICPの検出エッジをL)
        }
        break;
    default:
        break;
    }
}

/****************************************
* IOの初期化　　　　　　　　　　　　　  *
****************************************/
void io_init(void)
{
    //==================//
    //   IOの設定       //
    //==================//

    // LED の設定
    // ポート B0 B1を出力に設定
    DDRB |= (1<<PB1);          // ポート B1　を出力設定

    // 赤外線センサ設定
    // ポート D6を入力に設定
    DDRD &= ~(1 << PD6);

    // モータ駆動IC制御ピン(出力)
    DDRB |= (1<<PB3) | (1<<PB4);
    DDRD |= (1<<PD3) | (1<<PD4);
}

/*

【タイマ０関連レジスタ】

 1.ﾀｲﾏ/ｶｳﾝﾀ0制御ﾚｼﾞｽﾀA(TCCR0A)

 =================================================================
    ﾀｲﾏ/ｶｳﾝﾀ0制御ﾚｼﾞｽﾀA(TCCR0A)のビット並び
    7       6       5       4       3       2       2       0
 ----------------------------------------------------------------
    COM0A1  COM0A0  COM0B1  COM0B0  -       -       WGM01   WGM00
 ================================================================

 1.1.COM0Axの詳細

 ======================================================
    表34. 非PWM動作比較A出力選択
 ======================================================
    COM0A1  COM0A0      意味
    0       0           標準ﾎﾟｰﾄ動作 (OC0A切断)
    0       1           比較一致でOC0Aﾋﾟﾝ ﾄｸﾞﾙ(交互)出力
    1       0           比較一致でOC0Aﾋﾟﾝ Lowﾚﾍﾞﾙ出力
    1       1           比較一致でOC0Aﾋﾟﾝ Highﾚﾍﾞﾙ出力
 ======================================================

 ======================================================
    表35. 高速PWM動作比較A出力選択 (共通注意参照)
 ======================================================
    COM0A1  COM0A0      意味
    0       0           標準ﾎﾟｰﾄ動作 (OC0A切断)
    0       1           WGM02=0 :   標準ﾎﾟｰﾄ動作 (OC0A切断)
                        WGM02=1 :   比較一致でOC0Aﾋﾟﾝ ﾄｸﾞﾙ
    1       0           比較一致でLow、BOTTOMでHighを
                        OC0Aﾋﾟﾝへ出力 (非反転動作)
    1       1           比較一致でHigh、BOTTOMでLowを
                        OC0Aﾋﾟﾝへ出力 (反転動作)
 ======================================================

 ======================================================
    表36. 位相基準PWM動作比較A出力選択 (共通注意参照)
 ======================================================
    COM0A1  COM0A0      意味
    0       0           標準ﾎﾟｰﾄ動作 (OC0A切断)
    0       1           WGM02=0 :   標準ﾎﾟｰﾄ動作 (OC0A切断)
                        WGM02=1 :   比較一致でOC0Aﾋﾟﾝ ﾄｸﾞﾙ
                                    (交互)出力
    1       0           上昇計数時の比較一致でLow、下降計数
                        時の比較一致でHighをOC0Aﾋﾟﾝへ出力
    1       1           上昇計数時の比較一致でHigh、下降計数
                        時の比較一致でLowをOC0Aﾋﾟﾝへ出力
 ======================================================

  1.2.COM0Bxの詳細

 ======================================================
    表37. 非PWM動作比較B出力選択
 ======================================================
    COM0B1  COM0B0      意味
    0       0           標準ﾎﾟｰﾄ動作 (OC0B切断)
    0       1           比較一致でOC0Bﾋﾟﾝ ﾄｸﾞﾙ(交互)出力
    1       0           比較一致でOC0Bﾋﾟﾝ Lowﾚﾍﾞﾙ出力
    1       1           比較一致でOC0Bﾋﾟﾝ Highﾚﾍﾞﾙ出力
 ======================================================

 ======================================================
    表38. 高速PWM動作比較B出力選択 (共通注意参照)
 ======================================================
    COM0B1  COM0B0      意味
 ------------------------------------------------------
    0       0           標準ﾎﾟｰﾄ動作 (OC0B切断)
    0       1           予約
    1       0           比較一致でLow、BOTTOMでHighを
                        OC0Bﾋﾟﾝへ出力 (非反転動作)
    1       1           比較一致でHigh、BOTTOMでLowを
                        OC0Bﾋﾟﾝへ出力 (反転動作)
 ======================================================

  1.3.WGM0xの詳細

 ===============================================================================================
    表40. 波形生成種別選択
 ==============================================================================================
    (WGM02) WGM01   WGM00   ﾀｲﾏ/ｶｳﾝﾀ動作種別                TOP値       OCR0x更新時 TOV0設定時
 ----------------------------------------------------------------------------------------------
    0       0       0       標準動作                        $FF         即時        MAX
    0       0       1       8ﾋﾞｯﾄ位相基準PWM動作            $FF         TOP         BOTTOM
    0       1       0       比較一致ﾀｲﾏ/ｶｳﾝﾀ ｸﾘｱ(CTC)動作   OCR0A       即時        MAX
    0       1       1       8ﾋﾞｯﾄ高速PWM動作                $FF         BOTTOM      MAX
    1       0       0       (予約)                          -           -           -
    1       0       1       位相基準PWM動作                 OCR0A       TOP         BOTTOM
    1       1       0       (予約)                          -           -           -
    1       1       1       高速PWM動作                     OCR0A       BOTTOM      TOP
 ==============================================================================================

 2.ﾀｲﾏ/ｶｳﾝﾀ0制御ﾚｼﾞｽﾀB(TCCR0B)

  ================================================================
    ﾀｲﾏ/ｶｳﾝﾀ0制御ﾚｼﾞｽﾀB のビット並び
    7       6       5       4       3       2       1       0
 ----------------------------------------------------------------
    FOC0A   FOC0B   -       -       WGM02   CS02    CS01    CS00
 ================================================================

 2.1.FOC0Aの詳細

 =====================================================
    OC0A強制変更 (Force Output Compare A)
    FOC0A       意味(非PWM動作を指示する時だけ有効)
 -----------------------------------------------------
    0           不許可(disable)
    1           許可(enable)
 =====================================================

 2.2.FOC0Bの詳細

 =====================================================
    OC0B強制変更 (Force Output Compare A)
    FOC0B       意味(非PWM動作を指示する時だけ有効)
 -----------------------------------------------------
    0           不許可(disable)
    1           許可(enable)
 =====================================================

 2.3.CS0xの詳細

 =====================================================
    表41. ﾀｲﾏ/ｶｳﾝﾀ0入力ｸﾛｯｸ選択
 =====================================================
    CS02    CS01    CS00    意味
 -----------------------------------------------------
    0       0       0       停止 (ﾀｲﾏ/ｶｳﾝﾀ0動作停止)
    0       0       1       clkI/O (前置分周なし)
    0       1       0       clkI/O/8 (8分周)
    0       1       1       clkI/O/64 (64分周)
    1       0       0       clkI/O/256 (256分周)
    1       0       1       clkI/O/1024 (1024分周)
    1       1       0       T0ﾋﾟﾝの下降端 (外部ｸﾛｯｸ)
    1       1       1       T0ﾋﾟﾝの上昇端 (外部ｸﾛｯｸ)
 ======================================================
*/


/****************************************
* タイマー０の初期化　　　　　　　　　  *
****************************************/
void timer0_init(void)
{
    TCCR0A = (0<<COM0A1) | (0<<COM0A0) |            /* 標準ﾎﾟｰﾄ動作 (OC0A切断) */
             (0<<COM0B1) | (0<<COM0B0) |            /* 標準ﾎﾟｰﾄ動作 (OC0B切断) */
             (0<<WGM01)  | (0<<WGM00);              /* 標準動作 */

    TCCR0B = (0<<FOC0A) | (0<<FOC0B) |              /* 不許可 */
             (0<<WGM02) |                           /* 標準動作 */
             (1<<CS02)  | (0<<CS01)  | (1<<CS00);   /* プリスケーラは 1/1024 */

    TCNT0 = 0;              // タイマ0の初期値
    TIMSK |= _BV(TOIE0);    // タイマ0オーバーフローだけ割り込み許可
}


/****************************************
* タイマー１の初期化　　　　　　　　　  *
****************************************/
void timer1_init(void)
{
    //  ----非PWM動作での比較出力選択(注: xはAまたはB, Xは0または1)----
    // =============================================================================================
    //  COM1x1  COM1x0          意味
    // =============================================================================================
    //  0       0           標準ﾎﾟｰﾄ動作 (OC1x切断)
    //  0       1           比較一致でOC1xﾋﾟﾝ ﾄｸﾞﾙ(交互)出力
    //  1       0           比較一致でOC1xﾋﾟﾝ Lowﾚﾍﾞﾙ出力
    //  1       1           比較一致でOC1xﾋﾟﾝ Highﾚﾍﾞﾙ出力
    //
    //  ----高速PWM動作での比較出力選択(注: xはAまたはB, Xは0または1)----
    // =============================================================================================
    //  COM1x1  COM1x0          意味
    // =============================================================================================
    //  0       0           標準ﾎﾟｰﾄ動作 (OC1x切断)
    //  0       1           WGM13～0=111X       : 比較一致でOC1Aﾋﾟﾝ ﾄｸﾞﾙ(交互)出力、OC1Bは標準ﾎﾟｰﾄ動作(OC1B切断)
    //                      WGM13～0上記以外    : 標準ﾎﾟｰﾄ動作 (OC1x切断)
    //  1       0           比較一致でLow、BOTTOMでHighをOC1xﾋﾟﾝへ出力 (非反転動作)
    //  1       1           比較一致でHigh、BOTTOMでLowをOC1xﾋﾟﾝへ出力 (反転動作)
    //
    //  ----45. 位相基準または位相/周波数基準PWM動作での比較出力選択(注: xはAまたはB, Xは0または1)----
    // =============================================================================================
    //  COM1x1  COM1x0          意味
    // =============================================================================================
    //  0       0           標準ﾎﾟｰﾄ動作 (OC1x切断)
    //  0       1           WGM13～0=10X1       : 比較一致でOC1Aﾋﾟﾝ ﾄｸﾞﾙ(交互)出力、OC1Bは標準ﾎﾟｰﾄ動作(OC1B切断)
    //                      WGM13～0上記以外    : 標準ﾎﾟｰﾄ動作 (OC1x切断)
    //  1       0           上昇計数時の比較一致でLow、下降計数時の比較一致でHighをOC1xﾋﾟﾝへ出力
    //  1       1           上昇計数時の比較一致でHigh、下降計数時の比較一致でLowをOC1xﾋﾟﾝへ出力


    // =============================================================================================
    //  表46. 波形生成種別選択表(WGMxx)
    // =============================================================================================
    //  WGM13   WGM12   WGM11   WGM10       ﾀｲﾏ/ｶｳﾝﾀ動作種別                TOP値   OCR1x   TOV1
    //          (CTC1)  (PWM11) (PWM10)                                             更新時  設定時
    // =============================================================================================
    //  0       0       0       0           標準動作                        $FFFF   即値    MAX
    //  0       0       0       1           8ﾋﾞｯﾄ位相基準PWM動作            $00FF   TOP     BOTTOM
    //  0       0       1       0           9ﾋﾞｯﾄ位相基準PWM動作            $01FF   TOP     BOTTOM
    //  0       0       1       1           10ﾋﾞｯﾄ位相基準PWM動作           $03FF   TOP     BOTTOM
    //  0       1       0       0           比較一致ﾀｲﾏ/ｶｳﾝﾀ ｸﾘｱ(CTC)動作   OCR1A   即値    MAX
    //  0       1       0       1           8ﾋﾞｯﾄ高速PWM動作                $00FF   BOTTOM  TOP
    //  0       1       1       0           9ﾋﾞｯﾄ高速PWM動作                $01FF   BOTTOM  TOP
    //  0       1       1       1           10ﾋﾞｯﾄ高速PWM動作               $03FF   BOTTOM  TOP
    //  1       0       0       0           位相/周波数基準PWM動作          ICR1    TOP     BOTTOM
    //  1       0       0       1           位相/周波数基準PWM動作          OCR1A   TOP     BOTTOM
    //  1       0       1       0           位相基準PWM動作                 ICR1    BOTTOM  TOP
    //  1       0       1       1           位相基準PWM動作                 OCR1A   BOTTOM  TOP
    //  1       1       0       0           比較一致ﾀｲﾏ/ｶｳﾝﾀ ｸﾘｱ(CTC)動作   ICR1    即値    MAX
    //  1       1       0       1           予約                            -       -       -
    //  1       1       1       0           高速PWM動作                     ICR1    BOTTOM  TOP
    //  1       1       1       1           高速PWM動作                     OCR1A   TOP     BOTTOM
    // =============================================================================================

    // =============================================================================================
    //  捕獲起動入力1雑音消去許可
    // =============================================================================================
    //  ICNC1       意味
    // =============================================================================================
    //  0           不許可(disable)
    //  1           許可(enable)
    // =============================================================================================

    // =============================================================================================
    //  捕獲起動入力端選択
    // ==============================================================================================
    //  ICES1       意味
    // ==============================================================================================
    //  0           不許可(disable)
    //  1           許可(enable)
    // =============================================================================================


    // ==============================================================================================
    //  表47. ﾀｲﾏ/ｶｳﾝﾀ1入力ｸﾛｯｸ選択
    // ==============================================================================================
    //  CS12    CS11    CS10        意味
    //  0       0       0       停止 (ﾀｲﾏ/ｶｳﾝﾀ1動作停止)
    //  0       0       1       clkI/O (前置分周なし)
    //  0       1       0       clkI/O/8 (8分周)
    //  0       1       1       clkI/O/64 (64分周)
    //  1       0       0       clkI/O/256 (256分周)
    //  1       0       1       clkI/O/1024 (1024分周)
    //  1       1       0       T1ﾋﾟﾝの下降端 (外部ｸﾛｯｸ)
    //  1       1       1       T1ﾋﾟﾝの上昇端 (外部ｸﾛｯｸ)
    // ==============================================================================================


    //==========================================================//
    //   タイマ1 カウンタレジスタ設定                           //
    //==========================================================//
    // 15.11.1 タイマ／カウンタ1制御レジスタA (初期値は0x00なので必要ない)
    //         ++-------COM1A1:COM1A0 00 OC1A切断
    //         ||++---- COM1B1:COM1B0 00 OC1B切断
    //         ||||  ++ WGM11:WGM10   00 波形生成種別(4bitの下位2bit)
//    TCCR1A = 0b00000000;        // CTC with interrupt


    TCCR1A =    (0<<COM1A1) | (0<<COM1A0) |
              (0<<COM1B1) | (0<<COM1B0) |
              (0<<WGM11) | (0<<WGM10) ;


    // 15.11.2 タイマ／カウンタ1制御レジスタB (P-69)
    //         +------- ICNC1          1   捕獲起動入力1雑音消去許可
    //         |+------ ICES1          1   捕獲起動入力端選択
    //         || ++--- WGM13:WGM12    00  波形生成種別(4bitの上位2bit) CTC top=OCR1A
    //         || ||+++ CS12:CS11:CS10 010 clkI/O (前置分周なし)
//    TCCR1B = 0b11000010;              // キャプチャ: capture: LPF ON(0),1->0edge(1), CTC-A, prescale 8

    TCCR1B =    (1<<ICNC1)  | (1<<ICES1)|
              (0<<WGM13)    | (0<<WGM12)|
              (0<<CS12) | (1<<CS11) | (0<<CS10);

    // カウンタ初期化
    TCNT1 = 0x0000;
    OCR1A = 0xFFFF;
    OCR1B = 0xFFFF;

    //==========================================================//
    //   外部割り込み設定                                       //
    //==========================================================//
    // 外部割り込み（未使用)
    GIMSK = 0x00;
    MCUCR = 0x00;

    // コンパレータ未使用
    ACSR=0x80;


    // タイマー1キャプチャー割込許可
    TIMSK |= (1<<ICF1);
}

/****************************************
* 赤外線リモコン受信データ送信　　　　  *
****************************************/
#if IR_USE_XMIT
void xmit_remocon_send(void)
{
    xmitstr(PSTR("0x"));
    xmit_hex(DispRemData.DATA_BYTE[0]);
    xmit_hex(DispRemData.DATA_BYTE[1]);
    xmit_hex(DispRemData.DATA_BYTE[2]);
    xmit_hex(DispRemData.DATA_BYTE[3]);
    xmitstr(PSTR("\n\r"));
}
#endif


/****************************************
* リセット                              *
****************************************/
void remocon_reset(void)
{
    cli();          // 全体割込不許可

    DispRemData.DATA_LONG = 0;
    RemData.DATA_LONG = 0;

    ReqFlag.DispCode = 1;                   // コード表示要求セット
    BitCount = 0;                           // 受信カウントクリア
    ReqFlag.RmStatus = RM_INTIAL;
    IR_CAPT_FALL();                         // 立ち下がり駆動に変更(ICPの検出エッジをL)

    sei();          // 全体割込許可
}


/****************************************
* メイン    　　　　　　　　　　　　　  *
****************************************/
int main(void)
{
    WORD remo_data;

    cli(); // 割り込み禁止

    io_init();
    // LEDの表示で使用
    timer0_init();
    // 赤外線リモコン信号受信処理で使用
    timer1_init();

    // 2313標準UARTの初期化
#if IR_USE_XMIT
    uart2313_init();
#endif

    /*---------------*/
    flag = REQ_DATA;
    remo_data = 0;

    IR_CAPT_FALL();     // 立ち下がり駆動に変更
    IR_CAPT_CLEAR();    // キャプチャー割込要求クリア

    motor1_stop();
    motor2_stop();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);// パワーダウンモードを設定

    sei();          // 全体割込許可

#if IR_USE_XMIT
    xmitstr(PSTR("Ready OK\n\r"));
#endif

    while (1)
    {
        if (ReqFlag.DispCode) // コード表示要求有り
        {
            if (DispRemData.DATA_WORD[0] != MAKER_CODE)
            {
                flag = MAKER_CODE_ERROR;
#if IR_USE_XMIT
                xmitstr(PSTR("Maker Code ERR - "));
                xmit_remocon_send();
#endif
                remocon_reset();
            }

            else if (DispRemData.DATA_WORD[1] != remo_data)
            {
                remo_data = DispRemData.DATA_WORD[1];
                flag = RECEIVE_DATA;

                switch (remo_data)
                {

                case LEFT:          //左折
                    motor1_stop();
                    motor2_stop();
                    motor2_forward();
#if IR_USE_XMIT
                    xmitstr(PSTR("Left - "));
#endif
                    break;

                case LEFTTURN:      // 左回転
                    motor1_stop();
                    motor2_stop();
                    motor1_reverse();
                    motor2_forward();
#if IR_USE_XMIT
                    xmitstr(PSTR("Left turn - "));
#endif
                    break;

                case LEFTBACK:      // 後左折
                    motor1_stop();
                    motor2_stop();
                    motor1_reverse();
#if IR_USE_XMIT
                    xmitstr(PSTR("Left Back - "));
#endif
                    break;

                    // -----------------------------------
                case RIGHT:         // 右折
                    motor1_stop();
                    motor2_stop();
                    motor1_forward();
#if IR_USE_XMIT
                    xmitstr(PSTR("Right - "));
#endif
                    break;

                case RIGHTTURN:     // 右回転
                    motor1_stop();
                    motor2_stop();
                    motor1_forward();
                    motor2_reverse();
#if IR_USE_XMIT
                    xmitstr(PSTR("Right turn - "));
#endif
                    break;

                case RIGHTBACK:     // 後右折
                    motor1_stop();
                    motor2_stop();
                    motor2_reverse();
#if IR_USE_XMIT
                    xmitstr(PSTR("Right Back - "));
#endif
                    break;

                case FORWARD:       // 前進
                    motor1_stop();
                    motor2_stop();
                    motor1_forward();
                    motor2_forward();
#if IR_USE_XMIT
                    xmitstr(PSTR("Forward - "));
#endif
                    break;

                case BACK:          // 後進
                    motor1_stop();
                    motor2_stop();
                    motor1_reverse();
                    motor2_reverse();
#if IR_USE_XMIT
                    xmitstr(PSTR("Back - "));
#endif
                    break;

                case BRAKE:
                    motor1_brake();
                    motor2_brake();
#if IR_USE_XMIT
                    xmitstr(PSTR("Brake - "));
#endif
                    remocon_reset();
                    break;

                case RESET:
                    motor1_stop();
                    motor2_stop();
#if IR_USE_XMIT
                    xmitstr(PSTR("Brake - "));
#endif
                    remocon_reset();
                    break;

                default:
#if IR_USE_XMIT
                    xmitstr(PSTR("Not Assign Code - "));
#endif
                    break;
                }
#if IR_USE_XMIT
                xmit_remocon_send();
#endif
            }
            else
            {
                flag = REPEAT_DATA;
#if IR_USE_XMIT
                xmitstr(PSTR("Repeat Data - "));
                xmit_remocon_send();
#endif
            }

            ReqFlag.DispCode = 0;
        }

        if (ReqFlag.DispFormat) // リモコンフォーマット表示要求
        {
            flag = REQ_DATA;
            ReqFlag.DispFormat = 0;
#if IR_USE_XMIT
            xmitstr(PSTR("Request OK\n\r"));
#endif
        }

        if (ReqFlag.ErrDisp) // エラー表示
        {
            flag = RECEIVE_ERR_DATA;
            ReqFlag.ErrDisp = 0;
#if IR_USE_XMIT
            xmitstr(PSTR("Receive Data ERR\n\r"));
#endif
            remocon_reset();
        }

        // スリープ
        sleep_mode();


    } // 無限ループ

    return 0;
}
