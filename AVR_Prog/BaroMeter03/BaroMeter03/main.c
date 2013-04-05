//=============================================================================
// File Name    : main.c
//
// Title        : 気圧センサメイン
// Revision     : 0.1
// Notes        :
// Target MCU   : AVR ATMega328
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/02/06   ばんと      製作開始
//=============================================================================

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>                     // WinAVR library
#include <avr/interrupt.h>              // WinAVR library
#include <avr/sleep.h>                  // WinAVR library
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "delay.h"
#include "integer.h"
#include "twi_i2cMaster.h"
#include "ST7032i.h"
#include "DS3231S.h"
#include "mpl115a2.h"
#include "ff.h"
#include "util.h"
#include "xitoa.h"

/* local define --------------------------------------------------------------*/
#define BUFSIZE     32          /*バッファサイズ*/
#define TREND_BUFFSIZE 13
#define FONT_SIZE   8
#define IVT_SENSOR  10          /* センサの読み取り間隔(10分) */

#define ADC_VREF        3.286                   // [V] 内蔵VREF(1.1V)
#define ADC_DIV_RATE        ( 96600.0/194400.0)
#define ADC_DIV_RATE2       ((194400.0/96600.0)*1000.0)

#define ADC_RESOLUTION  1024                // [count]
#define R_AMB           10000               // [ohm] NJL7502L shunt register
#define K_AMB           2.0                 // 1[uA]=2[lx] -> NJL7502L datasheet

#define BATTERY_LEVEL       5           //
#define BATTERY_LEVEL4      3800
#define BATTERY_LEVEL3      3725
#define BATTERY_LEVEL2      3650
#define BATTERY_LEVEL1      3575
#define BATTERY_LEVEL0      3500
#define BATTERY_WARNING     0           // Beep and blink

//#define AMB_INIT()        { DDRC &= ~_BV(DDC0); PORTC &= ~_BV(PORTC0);}   // Input + pull-ups disable
#define AMB_ADCH        1
#define BATTERY_ADCH    0           // ADC0

/* local typedef -------------------------------------------------------------*/
typedef struct
{
    uint8_t font[FONT_SIZE];
} custom_char_t;

enum { S_INIT = 0, S_OPEN, S_CONFIG, S_UART, S_MEASUREMENT, S_USER_SHUTDOWN, S_BATT_LOW_SHUTDOWN, S_POWERDOWN, S_ERROR };       // メインステータス
enum { CS_START=0, CS_YEAR, CS_MONTH, CS_DAY, CS_WDAY, CS_HOUR, CS_MIN, CS_SEC, CS_INTERVAL, CS_ALTITUDE, CS_SET }; // コンフィグステータス

/* local macro ---------------------------------------------------------------*/
#define BUTTON1         !(PIND & _BV(PIND5))
#define BUTTON2         !(PIND & _BV(PIND7))
#define Disable_analog_comp()   {ACSR = _BV(ACD);}      /* Disable analog comp */

#define LED_ON()        { PORTD |= _BV(PORTD4); }
#define LED_OFF()       { PORTD &= ~_BV(PORTD4);}

#define SENSOR_ON()     { DDRB |= _BV(DDB0); PORTB |= _BV(PORTB0); }    /* Output,ON */
#define SENSOR_OFF()    { DDRB &= ~_BV(DDB0);PORTB &= ~_BV(PORTB0);}    /* Hi-Z */

#define LCD_ON()        { DDRB |= _BV(DDB1); PORTB |= _BV(PORTB1); }    /* Output,ON */
#define LCD_OFF()       { DDRB &= ~_BV(DDB1);PORTB &= ~_BV(PORTB1);}    /* Hi-Z */

#define BEEP_ON()       { TCCR0A &= ~_BV(COM0A1); TCCR0A |= _BV(COM0A0); }  /* OC0A = 4kHz */
#define BEEP_OFF()      { TCCR0A |= _BV(COM0A1);  TCCR0A &= ~_BV(COM0A0);}  /* OC0A Low */

#define POWER_ON()      { PORTD |= _BV(PORTD3); }   /* これは使わない。*/
#define POWER_OFF()     { PORTD &= ~_BV(PORTD3);}

#define SD_DETECT_INIT()    (DDRD &= ~_BV(DDD2), PORTD &= ~_BV(PORTD2)) // input, no-pullup
#define SD_DETECT       !(PIND & _BV(PIND2))

#define alarm_beep()    beep(300, 3)    /* Three long beep (stopped due to disk error or low battery) */
#define stop_beep()     beep(500, 1)    /* One long beep (stopped by button) */
#define click_beep()    beep(50, 1)     /* Single beep. */
#define start_beep()    beep(50, 2)     /* Two beeps. Start logging. */

#define abs(a) ((a) < 0 ? - (a) : (a))

/* local variables -----------------------------------------------------------*/
FATFS Fatfs;                /* File system object       */
FIL File1;                  /* File object              */
FILINFO fno;
char Fname[16];             /* File name buffer         */

volatile BYTE MmcTmr[2];    /* 100Hz decrement timers   */
volatile BYTE Timer;        /* 100Hz decrement timer    */
RTC_TIME rtc_time;

short atom_data[BUFSIZE]={0};
short temperature_data[BUFSIZE]={0};
short volt_data[BUFSIZE]={0};
short ilum_data[BUFSIZE]={0};
short trend_data[TREND_BUFFSIZE];
short trend;              /* */

volatile uint8_t state; /*メインステート*/
volatile bool isSensor;
volatile bool isSync;
volatile bool isTrend;
volatile bool isLog;
const char week_name[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

short battery_level[BATTERY_LEVEL] = {  // Voltage check table
                    BATTERY_LEVEL0,
                    BATTERY_LEVEL1,
                    BATTERY_LEVEL2,
                    BATTERY_LEVEL3,
                    BATTERY_LEVEL4
                };

// 気圧データ保存エリア
const short save_trend_data[TREND_BUFFSIZE] EEMEM = {-1};

// EEPROM カスタムフォントの初期データ
// O-Familyさんのフォントをいただきました。
const custom_char_t eAtomFont[] EEMEM =
{
{ 0x10 , 0x18 , 0x14 , 0x14 , 0x02 , 0x05 , 0x06 , 0x04},     //カスタム文字[hp] : 0
{ 0x08 , 0x14 , 0x08 , 0x06 , 0x09 , 0x08 , 0x09 , 0x06},     //カスタム文字[℃] : 1
{	//平 : 2
	0b11111,
	0b00100,
	0b10101,
	0b00100,
	0b11111,
	0b00100,
	0b00100,
	0b00000
},
{	//成 : 3
	0b00101,
	0b11111,
	0b10100,
	0b10101,
	0b11010,
	0b11010,
	0b10101,
	0b00000
},
{	//年 : 4
	0b10000,
	0b11111,
	0b00100,
	0b11111,
	0b10100,
	0b11111,
	0b00100,
	0b00000
},
{	//日 : 5
	0b11111,
	0b10001,
	0b10001,
	0b11111,
	0b10001,
	0b10001,
	0b11111,
	0b00000
},
{	//月 : 6
	0b01111,
	0b01001,
	0b01111,
	0b01001,
	0b01111,
	0b01001,
	0b10001,
	0b00000
},
{	//火 : 7
	0b00100,
	0b10101,
	0b10101,
	0b00100,
	0b01010,
	0b01010,
	0b10001,
	0b00000
},
{	//水 : 8
	0b00100,
	0b00101,
	0b11110,
	0b00110,
	0b01101,
	0b10100,
	0b00100,
	0b00000
},
{	//木 : 9
	0b00100,
	0b00100,
	0b11111,
	0b00100,
	0b01110,
	0b10101,
	0b00100,
	0b00000
},
{	//金 : 10
	0b01110,
	0b10001,
	0b01110,
	0b11111,
	0b00100,
	0b10101,
	0b11111,
	0b00000
},
{	//土 : 11
	0b00100,
	0b00100,
	0b11111,
	0b00100,
	0b00100,
	0b00100,
	0b11111,
	0b00000
},
{	//AM : 12
	0b00100,
	0b01010,
	0b11111,
	0b10001,
	0b00000,
	0b11011,
	0b10101,
	0b10001
},
{	//PM : 13
	0b11110,
	0b10001,
	0b11110,
	0b10000,
	0b00000,
	0b11011,
	0b10101,
	0b10001
}	
};

#if 0
const custom_char_t eWeekFont[] EEMEM =
{

};
#endif

/* local function prototypes -------------------------------------------------*/
void IO_Init( void );
void PCINIT_Init( void );
void AS2_Init( void );
void Timer1_Init( void );
void Timer0_Init( void );
void SPI_Init(void);
void SPI_Stop(void);
short getadc(unsigned char ch);
//uint16_t getadc(uint8_t ch);
void beep (uint16_t len, uint8_t cnt);
DWORD get_fattime ( void );
bool setMpl115a2Data( void );
bool initMpl115a2Data( void );
bool averageMpl115a2Data( int *atmo_tmp1, int *atmo_tmp2, int *temp_tmp1, int *temp_tmp2 );
void config( void );
void mapCustomChar( void );
void initTrendData( void );
void setTrendData( int atmo_tmp1, int atmo_tmp2 );
short getTrendDiff(void);
void restoreTrendData( void );
void backupTrendData( void );

/* [ここから割り込みハンドラ] =============================================== */

/*======================================*/
/*  key interrupt                       */
/*======================================*/
ISR( PCINT2_vect )
{
    PCIFR &= ~_BV(PCIF2);
}

/*======================================*/
/*  RTC用1秒割り込み(Timer2)            */
/*======================================*/
ISR( TIMER2_OVF_vect )
{
    TIFR2 &= ~_BV(TOV2);

    /* Sensor Sense Request Timing */
    {
        static uint8_t pre_sensor_min = 99;

        if ( ((rtc_time.min % IVT_SENSOR) == 0) && (pre_sensor_min != rtc_time.min))
        {
            pre_sensor_min = rtc_time.min;
            isSensor = true;
        }
    }


    /* Sync Request Timing(1時間間隔) */
    {
        static uint8_t pre_sync_hour = 99;

        if (pre_sync_hour != rtc_time.hour)
        {
            pre_sync_hour = rtc_time.hour;
            isSync = true;
            isTrend = true;
        }
    }
}

/*======================================================================*/
/*  MMC用100Hz割り込み(Timer2) 100Hz timer interrupt generated by OC1A  */
/*======================================================================*/
ISR( TIMER1_COMPA_vect )
{
    BYTE n;
    n = Timer;
    if (n) Timer = --n;;
    n = MmcTmr[0];
    if (n) MmcTmr[0] = --n;;
    n = MmcTmr[1];
    if (n) MmcTmr[1] = --n;;
}

/* =============================================== [ここまで割り込みハンドラ] */

/* [ここから初期化関数] ===================================================== */

/*======================================*/
/*  IOポート設定                        */
/*======================================*/
void IO_Init( void )
{
    /*=============================================================================================
    【ピン配置】
                                           +---- ----+
              NC ...    (PCINT14/RESET)PC6 |1   U  28| PC5(ADC5/SCL/PCINT13)... I2C(SCL)
              NC ...      (PCINT16/RXD)PD0 |2      27| PC4(ADC4/SDA/PCINT12)... I2C(SDA)
              NC ...      (PCINT17/TXD)PD1 |3      26| PC3(ADC3/PCINT11)    ... NC
       SD Detect ...     (PCINT18/INT0)PD2 |4      25| PC2(ADC2/PCINT10)    ... NC
      Main Power ...(PCINT19/OC2B/INT1)PD3 |5      24| PC1(ADC1/PCINT9)     ... NJL7502L(ADC IN)
             LED ...   (PCINT20/XCK/T0)PD4 |6      23| PC0(ADC0/PCINT8)     ... LOW BATTERY(ADC IN)
             VCC ...                   VCC |7      22| GND                  ... GND
             GND ...                   GND |8      21| AREF                 ... VCC
         Ext Clk ..(PCINT6/XTAL1/TOSC1)PB6 |9      20| AVCC                 ... VCC
              NC ..(PCINT7/XTAL2/TOSC2)PB7 |10     19| PB5(SCK/PCINT5)      ... SPI(SCK)
         BUTTON1 ...  (PCINT21/OC0B/T1)PD5 |11     18| PB4(MISO/PCINT4)     ... SPI(MISO)
          BUZZER ...(PCINT22/OC0A/AIN0)PD6 |12     17| PB3(MOSI/OC2A/PCINT3)... SPI(MOSI)
         BUTTON2 ...     (PCINT23/AIN1)PD7 |13     16| PB2(SS/OC1B/PCINT2)  ... SPI(SS): SD CRAD
    SENSOR POWER ... (PCINT0/CLKO/ICP1)PB0 |14     15| PB1(OC1A/PCINT1)     ... LCD POWER
                                           +---------+
                                           ATMega328P
    ===============================================================================================*/

    //========================================
    // ポート初期化
    //========================================

    /* PortB  +--------7: Pullup        : non use               */
    /*        |+-------6: Pullup        : non use               */
    /*        ||+------5: HIZ           : SD SCK                */
    /*        |||+-----4: HIZ           : SD MISO               */
    /*        ||||+----3: HIZ           : SD MOSI               */
    /*        |||||+---2: Output(Low)   : SD #CS                */
    /*        ||||||+--1: Output(High)  : LCD POWER             */
    /*        |||||||+-0: Output(High)  : SENSOR POWER          */
    DDRB  = 0b00101111;
    PORTB = 0b11000011;

    /* PortC  +--------7: Pullup        : non use               */
    /*        |+-------6: Pullup        : non use               */
    /*        ||+------5: Pullup        : I2C(SCL)              */
    /*        |||+-----4: Pullup        : I2C(SDA)              */
    /*        ||||+----3: Pullup        : non use               */
    /*        |||||+---2: Pullup        : non use               */
    /*        ||||||+--1: Input(non-Pullup) : NJL7502L(ADC IN)  */
    /*        |||||||+-0: Input(non-Pullup) : LOW BATTERY(ADC IN)*/
    DDRC  = 0b00100000;
    PORTC = 0b11011100;

    /* PortD  +--------7: Input(Pullup) : BUTTON2               */
    /*        |+-------6: Output(Low)   : BUZZER                */
    /*        ||+------5: Input(PullUp) : BUTTON1               */
    /*        |||+-----4: Output(Low)   : LED                   */
    /*        ||||+----3: Output(High)  : Main Power            */
    /*        |||||+---2: Input(non-Pullup) : SD Detect         */
    /*        ||||||+--1: Pullup        : non use               */
    /*        |||||||+-0: Pullup        : non use               */
    DDRD  = 0b01010000;
    PORTD = 0b10101011;

    // ADC0とADC1をアナログ専用ポートにする
    DIDR0 = _BV(ADC1D) | _BV(ADC0D) |  0;           // Digital Input Disable Register 0

}

/*==============================================*/
/*  ピン変化割り込み許可(スイッチボタン入力)    */
/*==============================================*/
void PCINIT_Init( void )
{
    PCMSK2 = _BV(PCINT21)       // PD5(BUTTON1)
            | _BV(PCINT23)      // PD7(BUTTON2)
            ;
    PCIFR  = 0;                 //
    PCICR  = _BV(PCIE2);        // Enable PCINT16-23
}

/*======================================================*/
/*  非同期Timer2割り込み許可(32.768kHzで1秒の割り込み)  */
/*======================================================*/
void AS2_Init( void )
{
    //Disable timer2 interrupts
    TIMSK2  = 0;
    //Enable asynchronous mode
    ASSR  = _BV(EXCLK) | _BV(AS2);
    //set initial counter value
    TCNT2=0;
    //set prescaller 128
    TCCR2B |= _BV(CS22)|_BV(CS00);
    //wait for registers update
    while (!(ASSR & (_BV(TCN2UB)|_BV(TCR2BUB))));
    //clear interrupt flags
    TIFR2  = _BV(TOV2);
    //enable TOV2 interrupt
    TIMSK2  = _BV(TOIE2);
}

/*==================================================*/
/*  Timer1割り込み許可(比較一致で100Hzの割り込み)       */
/*==================================================*/
void Timer1_Init( void )
{
    OCR1A = F_CPU/8/100-1;      // Timer1: 100Hz interval (OC1A)
    TCCR1B = 0b00001010;        // 比較一致ﾀｲﾏ/ｶｳﾝﾀ解除(CTC)動作, 8分周

    TIMSK1 |= _BV(OCIE1A);
}

#define Timer1_Start()          { TIMSK1 |= _BV(OCIE1A); }  /* Enable TC1.oca interrupt */
#define Timer1_Stop()           { TIMSK1 &= ~_BV(OCIE1A); } /* Disable TC1.oca interrupt */

/*==========================================*/
/*  Timer0 PWM設定(4KHzのPWM 圧電ブザー用)  */
/*==========================================*/
void Timer0_Init( void )
{
    OCR0A = F_CPU/64/4000/2-1;
    TCCR0A = _BV(COM0A1) | _BV(WGM01);
    TCCR0B = _BV(CS01) | _BV(CS00);
}

/*======================================*/
/*  SPI設定(SDカード用)                 */
/*======================================*/
void SPI_Init(void)
{
    PORTB |= _BV(PORTB2);       /* MMC CS = H */
    SPCR = 0b01010000;          /* Initialize SPI port (Mode 0) */
    SPSR = 0b00000001;
}

void SPI_Stop(void)
{
    SPCR = 0;                   /* Disable SPI */
    PORTB &= ~_BV(PORTB2);      /* MMC CS = L */
}

#define SPI_Start()     SPI_Init();

/* ================================================= [ここまで初期化関数宣言] */

/* [ここから各種ハードウェア駆動関数] ======================================== */

/*======================================*/
/*  ADCデータ読み関数                   */
/*======================================*/
short getadc(unsigned char ch)
{
    short   a;

    //  DIDR0 =                             // Digital Input Disable Register 0
    //      _BV(ADC0D) |                    //    ADC0
    //      _BV(ADC1D) |                    //    ADC1
    //      _BV(ADC2D) |                    //    ADC2
    //      _BV(ADC3D) |                    //    ADC3
    //      _BV(ADC4D) |                    //    ADC4
    //      _BV(ADC5D) |                    //    ADC5
    //      0;
    DIDR0 |= (1 << (ADC0D+ch));
    ADMUX =                             // ADC Multiplexer Selection Register
    // Analog Channel Selection Bits
            (ch << MUX0) |                  //    Variable
    //      _BV(MUX0) |                     //    0  1  0  1  0  1  0  1  0  0  1
    //      _BV(MUX1) |                     //    0  0  1  1  0  0  1  1  0  1  1
    //      _BV(MUX2) |                     //    0  0  0  0  1  1  1  1  0  1  1
    //      _BV(MUX3) |                     //    0  0  0  0  0  0  0  0  1  1  1
    //                                            ADC0
    //                                               ADC1
    //                                                  ADC2
    //                                                     ADC3
    //                                                        ADC4
    //                                                           ADC5
    //                                                              ADC6
    //                                                                 ADC7
    //                                                                    TempSenser
    //                                                                       VBG=1.1V
    //                                                                         GND=0V
    //      _BV(ADLAR) |                    // ADC Left Adjust Result
    // Reference Selection Bits
            _BV(REFS0) |                    //    0  1  0  1
    //      _BV(REFS1) |                    //    0  0  1  1
    //                                            AREF, Internal Vref turned off
    //                                               AVcc with external capacitor at AREF pin
    //                                                  Reserved
    //                                                     Internal 1.1V Voltage Reference with external capacitor at AREF pin
    0;
    ADCSRA =                            // ADC Control and Status Register A
    // ADC Prescaler Select Bits
    //      _BV(ADPS0) |                    //    0   1   0   1   0    1    0    1
            _BV(ADPS1) |                    //    0   0   1   1   0    0    1    1
            _BV(ADPS2) |                    //    0   0   0   0   1    1    1    1
    //   1/2 1/2 1/4 1/8 1/16 1/32 1/64 1/128
    //      _BV(ADIE)  |                    // ADC Interrupt Enable
    //      _BV(ADIF)  |                    // ADC Interrupt Flag
    //      _BV(ADATE) |                    // ADC Auto Trigger Enable
            _BV(ADSC)  |                    // ADC Start Conversion
            _BV(ADEN)  |                    // ADC Enable
            0;

    while((ADCSRA & _BV(ADSC))) ;   // 1/1MHz*16*26cycle=416us
    a = ADCL;
    a |= (ADCH << 8);
    ADCSRA = 0;
    DIDR0 &= ~(1 << (ADC0D+ch));        // Resume I/O port

    return(a);
}

/*======================================*/
/*  ブザー鳴動関数                      */
/*======================================*/
void beep (uint16_t len, uint8_t cnt)
{
    while (cnt--)
    {
        BEEP_ON();
        wait_ms(len);
        BEEP_OFF();
        wait_ms(len);
    }
}

/*======================================*/
/*  LEDブリンク関数                     */
/*======================================*/
void led_blink(uint16_t len, uint8_t cnt)
{
    while (cnt--)
    {
        LED_ON();
        wait_ms(len);
        LED_OFF();
        wait_ms(len);
    }
}

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
DWORD get_fattime ( void )
{
    return    ((DWORD)(rtc_time.year - 1980) << 25)
    | ((DWORD)rtc_time.month << 21)
    | ((DWORD)rtc_time.day << 16)
    | ((DWORD)rtc_time.hour << 11)
    | ((DWORD)rtc_time.min << 5)
    | ((DWORD)rtc_time.sec >> 1);
}

/*======================================*/
/*  LCD電源ON                           */
/*======================================*/
void lcdpower_on( void )
{
    LCD_ON();           /* 電源ON */
    ST7032i_Init();
    ST7032i_Clear();

    mapCustomChar();    //カスタム文字をLCDへ書き込む。

    ST7032i_setCursor(0 , 0);
}

/*======================================*/
/*  LCD電源ON                           */
/*======================================*/
void lcdpower_off( void )
{
    LCD_OFF();          /* 電源OFF */
}

/* ====================================== [ここまで各種ハードウェア駆動関数]  */

/* [ここからユーティリティ関数] ============================================= */

/*======================================*/
/*  移動平均用のデータを登録            */
/*======================================*/
bool setMpl115a2Data( void )
{
    short atom, temperature;

    /* 初期データを収集する。*/
    if ( mpl115a_get( &atom, &temperature ) != 0 )
    {
        return false;
    }
    else
    {
        /* データ格納 */
        storeData( atom, atom_data, BUFSIZE );
        storeData( temperature, temperature_data, BUFSIZE );
        return true;
    }
}

/*======================================*/
/*  移動平均用の初期データを収集            */
/*======================================*/
bool initMpl115a2Data( void )
{
    register uint8_t i;

    /* 初期データを収集する。*/
    for ( i = 0; i < BUFSIZE; i++ )
    {
        if (setMpl115a2Data())
            wait_ms(100);
        else
            return false;
    }
    return true;
}

/*======================================*/
/*  移動平均用の初期データを収集        */
/*======================================*/
bool averageMpl115a2Data( int *atmo_tmp1, int *atmo_tmp2, int *temp_tmp1, int *temp_tmp2 )
{
    long sum;

    /* 最新データをセット */
    if ( setMpl115a2Data() )
    {
        sum = sumData( atom_data, BUFSIZE );            //気圧の総和
        *atmo_tmp1 = sum / BUFSIZE;
        *atmo_tmp2 = (sum % BUFSIZE) / 10;
        sum = sumData( temperature_data, BUFSIZE );     //温度の総和
        *temp_tmp1 = sum/(BUFSIZE * 10);
        *temp_tmp2 = (sum % (BUFSIZE * 10)) / 100;

        return true;
    }
    else
    {
        return false;
    }
}

/*======================================*/
/*  電圧データを登録                        */
/*======================================*/
void setVoltData( void )
{
    short adc_data;

    adc_data = getadc(BATTERY_ADCH);
    storeData( adc_data, volt_data, BUFSIZE );
}

/*======================================*/
/*  移動平均用の電圧データを収集            */
/*======================================*/
void initVoltData( void )
{
    register uint8_t i;

    /* 初期データを収集する。*/
    for ( i = 0; i < BUFSIZE; i++ )
    {
        setVoltData();
    }
}

/*======================================*/
/*  電圧の平均値を取得                  */
/*======================================*/
short averageVoltData( void )
{
    long sum;
    short avr, volt;

    /* 最新データをセット */
    setVoltData();

    sum = sumData( volt_data, BUFSIZE );
    avr = sum / BUFSIZE;

    volt = (avr*ADC_DIV_RATE2*ADC_VREF)/ADC_RESOLUTION;

    return volt;
}

/*======================================*/
/*  電圧データを登録                        */
/*======================================*/
void setIlumData( void )
{
    short adc_data;

    adc_data = getadc(AMB_ADCH);
    storeData( adc_data, ilum_data, BUFSIZE );
}

/*======================================*/
/*  移動平均用の電圧データを収集            */
/*======================================*/
void initIlumData( void )
{
    register uint8_t i;

    /* 初期データを収集する。*/
    for ( i = 0; i < BUFSIZE; i++ )
    {
        setIlumData();
    }
}

/*======================================*/
/*  電圧の平均値を取得                  */
/*======================================*/
short averageIlumData( void )
{
    long sum;
    short avr,ilum;

    /* 最新データをセット */
    setIlumData();

    sum = sumData( ilum_data, BUFSIZE );
    avr = sum / BUFSIZE;

    ilum = avr * (long)(ADC_VREF * 1000000 * K_AMB / R_AMB) / ADC_RESOLUTION;

    return ilum;
}

/*======================================*/
/*  気圧傾向データ初期化                */
/*======================================*/
void initTrendData( void )
{
    register uint8_t i;

	// データの復元
	restoreTrendData( );
	// 最新のデータが-1なら初期化
	if (trend_data[TREND_BUFFSIZE-1] == -1)
	{
	    for ( i = 0; i < TREND_BUFFSIZE; i++ )
		{
		    storeData( 0, trend_data, TREND_BUFFSIZE );
		}
	}
}


/*======================================*/
/*  気圧傾向データを設定                */
/*======================================*/
void setTrendData( int atmo_tmp1, int atmo_tmp2 )
{
    short data;

    /* 10倍したデータが入る */
    data = atmo_tmp1 * 10 + atmo_tmp2;
    storeData( data, trend_data, TREND_BUFFSIZE );
}


/*======================================*/
/*  天気予報データ設定                   */
/*======================================*/
short getTrendDiff(void)
{
    short filter1, filter2, diff;

	// 12時間前のデータがセットされてるか?
    if (trend_data[0] ==0)
    {
        return 0;
    }

    //          現在              3時間前             6時間前         9時間前
    filter1 = 4*trend_data[12] + 3*trend_data[9] + 2*trend_data[6] + trend_data[3];
    //          3時間前           6時間前            9時間前         12時間前
    filter2 = 4*trend_data[9] + 3*trend_data[6] + 2*trend_data[3] + trend_data[0];

    diff = filter1 - filter2;

	return diff;
}

/*======================================*/
/*  天気予報データ-保存                  */
/*======================================*/
void backupTrendData( void )
{
    eeprom_busy_wait();
    eeprom_write_block (trend_data, save_trend_data, sizeof(short)*TREND_BUFFSIZE);
}

/*======================================*/
/*  天気予報データ-読込                  */
/*======================================*/
void restoreTrendData( void )
{
	eeprom_busy_wait();
    eeprom_read_block (trend_data, save_trend_data, sizeof(short)*TREND_BUFFSIZE);
}


/*======================================*/
/*  LCDへ特殊文字登録                    */
/*======================================*/
void mapCustomChar( void )
{
    register uint8_t i;
    uint8_t rCustom[FONT_SIZE];

    for( i = 0 ; i < 5; i++ )
    {
        // EEPROM よりデータを取り出す。
        eeprom_busy_wait();
        eeprom_read_block(rCustom, (void *)(sizeof(custom_char_t)*i),sizeof(custom_char_t));
        eeprom_busy_wait();

        ST7032i_createChar(i, rCustom);
    }
}

/*======================================*/
/*  時刻の表示                          */
/*======================================*/
void dispTime( void )
{
    char buf[17];

    xsprintf_p(buf,PSTR("%02d-%02d-%02d"), rtc_time.year - 2000, rtc_time.month, rtc_time.day);
    ST7032i_setCursor(0, 0);
    ST7032i_puts(buf);

    xsprintf_p(buf,PSTR("%02d:%02d:%02d"), rtc_time.hour,  rtc_time.min,  rtc_time.sec);
    ST7032i_setCursor(0, 1);
    ST7032i_puts(buf);
}

/*======================================*/
/*  気圧・温度の表示                    */
/*======================================*/
void dispAtom( int atmo_tmp1, int atmo_tmp2, int temp_tmp1, int temp_tmp2 )
{
    char buf[17];

    xsprintf_p(buf,PSTR("%d.%d"), atmo_tmp1, atmo_tmp2);
    ST7032i_setCursor(9, 0);
    ST7032i_puts(buf);
    ST7032i_putc(0);

    xsprintf_p(buf,PSTR("%4d.%d"), temp_tmp1, temp_tmp2);
    ST7032i_setCursor(9, 1);
    ST7032i_puts(buf);
    ST7032i_putc(1);

    if (trend_data[0] !=0)
	{
		if (trend >= 150)
		{
			ST7032i_Up_Icon(true);
			ST7032i_Down_Icon(false);
//			ST7032i_putc(2);
		}
		else if (trend <= -150)
		{
			ST7032i_Up_Icon(false);
			ST7032i_Down_Icon(true);
//			ST7032i_putc(4);
		}
		else
		{
			ST7032i_Up_Icon(true);
			ST7032i_Down_Icon(true);
//			ST7032i_putc(3);
		}
	}	
}

/*======================================*/
/*  エラー表示(無限ループ)              */
/*======================================*/
void dispMessage(const char* title, const char *msg)
{
    ST7032i_Clear();
    ST7032i_setCursor(0 , 0);
    ST7032i_puts_p(title);
    ST7032i_setCursor(0 , 1);
    ST7032i_puts_p(msg);
}

/*======================================*/
/*  エラー表示(無限ループ)              */
/*======================================*/
void dispMessage2(const char* title, const char* msg)
{
    ST7032i_Clear();
    ST7032i_setCursor(0 , 0);
    ST7032i_puts_p(title);
    ST7032i_setCursor(0 , 1);
    ST7032i_puts(msg);
}

/* ============================================ [ここまでユーティリティ関数]  */

/* [ここから各種処理関数] =================================================== */

/*======================================*/
/*  Setup処理                           */
/*======================================*/
void setConfig( bool isLog )
{
    RTC_TIME _time;
    uint8_t times;
    uint8_t cfg_state;
    char buf[32];

    while(BUTTON2 || BUTTON1);      // Wait at button release
    cfg_state = CS_START;

    while (1)
    {
        switch(cfg_state)
        {
            case CS_START:
                DS3231S_now(&_time);

                // 表示
                ST7032i_Clear();
                ST7032i_onBlink();
                ST7032i_setCursor(0, 0);
                xsprintf_p(buf, PSTR("%04d-%02d-%02d %s"), _time.year, _time.month, _time.day, week_name[_time.wday-1]);
                ST7032i_puts(buf);
                ST7032i_setCursor(0, 1);
                xsprintf_p(buf, PSTR("%02d:%02d:%02d%"), _time.hour, _time.min, _time.sec);
                ST7032i_puts(buf);
                cfg_state = CS_YEAR;        // 無条件で次のステートへ
                break;

            case CS_YEAR:
                ST7032i_setCursor(0, 0);
                if ( BUTTON1 )  // ボタン１が押された時
                {
                    click_beep();
                    if (++_time.year > 2099)
                    {
                        _time.year = 2013;
                    }

                    times = 0;
                    // スイッチが放されるまで待つ
                    while( BUTTON1 )
                    {
                        click_beep();
                        wait_ms(100);
                        ++times;
                        if (times >= 3 && times < 10)   // 長押しされてたら初期値へ
                        {
                            _time.year = 2013;
                        }
                        else if (times >= 10)
                        {
                            ST7032i_offBlink();
                            ST7032i_Clear();
                            return;
                        }
                    }
                    xsprintf_p(buf, PSTR("%02d"), _time.year);
                    ST7032i_puts(buf);
                }
                else if ( BUTTON2 )
                {
                    click_beep();
                    // スイッチが放されるまで待つ
                    while( BUTTON2 )
                    {
                        wait_ms(100);
                    }
                    cfg_state = CS_MONTH;       /* 次のステートへ*/
                }
                break;

            case CS_MONTH:
                ST7032i_setCursor(5, 0);
                if ( BUTTON1 )  // ボタン１が押された時
                {
                    click_beep();
                    if(++_time.month > 12)
                    {
                        _time.month = 1;
                    }

                    times = 0;
                    // スイッチが放されるまで待つ
                    while( BUTTON1 )
                    {
                        click_beep();
                        wait_ms(100);
                        ++times;
                        if (times >= 3 && times < 10)   // 長押しされてたら初期値へ
                        {
                            _time.month = 1;
                        }
                        else if (times >= 10)
                        {
                            ST7032i_offBlink();
                            ST7032i_Clear();
                            return;
                        }
                    }
                    xsprintf_p(buf, PSTR("%02d"), _time.month);
                    ST7032i_puts(buf);
                }
                else if ( BUTTON2 )
                {
                    click_beep();
                    // スイッチが放されるまで待つ
                    while( BUTTON2 )
                    {
                        wait_ms(100);
                    }
                    cfg_state = CS_DAY;     /* 次のステートへ*/
                }
                break;

        case CS_DAY:
                ST7032i_setCursor(8, 0);
                if ( BUTTON1 )  // ボタン１が押された時
                {
                    click_beep();
                    ++_time.day;
                    switch (_time.month)
                    {
                        case 1:
                        case 3:
                        case 5:
                        case 7:
                        case 8:
                        case 10:
                        case 12:
                            if(_time.day>=31)
                            {
                                _time.day =31;
                            }
                            break;
                        case 4:
                        case 6:
                        case 9:
                        case 11:
                            if(_time.day >=30)
                            {
                                _time.day = 30;
                            }
                            break;
                        case 2:
                            if (_time.year % 400 == 0 || (_time.year % 4 == 0 && _time.year % 100 != 0))
                            {
                                if(_time.day >=29)
                                _time.day = 29;
                            }
                            else
                            {
                                if(_time.day>=28)
                                _time.day = 28;
                            }
                            break;
                        default:
                            break;
                    }

                    times = 0;
                    // スイッチが放されるまで待つ
                    while( BUTTON1 )
                    {
                        click_beep();
                        wait_ms(100);
                        ++times;
                        if (times >= 3 && times < 10)   // 長押しされてたら初期値へ
                        {
                            _time.day = 1;
                        }
                        else if (times >= 10)
                        {
                            ST7032i_offBlink();
                            ST7032i_Clear();
                            return;
                        }
                    }
                    _time.wday = getWeekday(_time.year, _time.month, _time.day);
                    xsprintf_p(buf, PSTR("%02d %s"), _time.day, week_name[_time.wday-1]);
                    ST7032i_puts(buf);
                }
                else if ( BUTTON2 )
                {
                    click_beep();
                    // スイッチが放されるまで待つ
                    while( BUTTON2 )
                    {
                        wait_ms(100);
                    }
                    cfg_state = CS_HOUR;        /* 次のステートへ*/
                }
                break;

            case CS_HOUR:
                ST7032i_setCursor(0, 1);
                if ( BUTTON1 )  // ボタン１が押された時
                {
                    click_beep();
                    if(++_time.hour > 23)
                    {
                        _time.hour = 0;
                    }

                    times = 0;
                    // スイッチが放されるまで待つ
                    while( BUTTON1 )
                    {
                        click_beep();
                        wait_ms(100);
                        ++times;
                        if (times >= 3 && times < 10)   // 長押しされてたら初期値へ
                        {
                            _time.hour = 0;
                        }
                        else if (times >= 10)
                        {
                            ST7032i_offBlink();
                            ST7032i_Clear();
                            return;
                        }
                    }
                    xsprintf_p(buf, PSTR("%02d"), _time.hour);
                    ST7032i_puts(buf);
                }
                else if ( BUTTON2 )
                {
                    click_beep();
                    // スイッチが放されるまで待つ
                    while( BUTTON2 )
                    {
                        wait_ms(100);
                    }
                    cfg_state = CS_MIN;     /* 次のステートへ*/
                }
                break;

            case CS_MIN:
                ST7032i_setCursor(3, 1);
                if ( BUTTON1 )  // ボタン１が押された時
                {
                    click_beep();
                    if(++_time.min > 59)
                    {
                        _time.min = 0;
                    }

                    times = 0;
                    // スイッチが放されるまで待つ
                    while( BUTTON1 )
                    {
                        click_beep();
                        wait_ms(100);
                        ++times;
                        if (times >= 3 && times < 10)   // 長押しされてたら初期値へ
                        {
                            _time.min = 0;
                        }
                        else if (times >= 10)
                        {
                            ST7032i_offBlink();
                            ST7032i_Clear();
                            return;
                        }
                    }
                    xsprintf_p(buf, PSTR("%02d"), _time.min);
                    ST7032i_puts(buf);
                }
                else if ( BUTTON2 )
                {
                    click_beep();
                    // スイッチが放されるまで待つ
                    while( BUTTON2 )
                    {
                        wait_ms(100);
                    }
                    cfg_state = CS_SEC;     /* 次のステートへ*/
                }
                break;

            case CS_SEC:
                ST7032i_setCursor(6, 1);
                //  ボタン1とボタン2の同時でループルを抜ける
                //===============================================
                if (BUTTON2 && BUTTON1)
                {
                    click_beep();
                    while(BUTTON2 || BUTTON1);

                    ST7032i_offBlink();
                    ST7032i_Clear();
                    return;
                }
                if ( BUTTON1 )  // ボタン１が押された時
                {
                    click_beep();
                    if(++_time.sec > 59)
                    {
                        _time.sec = 0;
                    }

                    times = 0;
                    // スイッチが放されるまで待つ
                    while( BUTTON1 )
                    {
                        click_beep();
                        wait_ms(100);
                        ++times;
                        if (times >= 3 && times < 10)   // 長押しされてたら初期値へ
                        {
                            _time.sec = 0;
                        }
                        else if (times >= 10)
                        {
                            ST7032i_offBlink();
                            ST7032i_Clear();
                            return;
                        }
                    }
                    xsprintf_p(buf, PSTR("%02d"), _time.sec);
                    ST7032i_puts(buf);
                }
                else if ( BUTTON2 )
                {
                    click_beep();
                    // スイッチが放されるまで待つ
                    while( BUTTON2 )
                    {
                        wait_ms(100);
                    }
                    cfg_state = CS_SET; /* 次のステートへ*/
                }
                break;

        case CS_SET:
            DS3231S_adjust(&_time);
            ST7032i_Clear();

            ST7032i_offBlink();
            state = S_INIT;

            if (isLog)
            {
                if (f_sync(&File1))
                {
                    dispMessage(PSTR("[Config]"), PSTR("File Save Error."));
                    alarm_beep();
                    state = S_ERROR;
                }

                f_close(&File1);
            }
            return;

        default:
            break;
        }
    }
}

/* ================================================== [ここまで各種処理関数]  */

/* [ここからメイン関数] =================================================== */
int main(void)
{
    int     atmo_tmp1, atmo_tmp2, temp_tmp1, temp_tmp2;
    uint8_t rc;
    uint8_t pre_month;
    char    buf[64];
    static short    volt, ilum;
    uint8_t batt;

    /* IOポート初期化 */
    IO_Init();

    /* 割り込み処理設定 */
    PCINIT_Init();              // ボタン割り込み処理許可
    Timer0_Init();              // ブザー鳴動設定
    Timer1_Init();              // SD MMC用割り込み設定
    AS2_Init();                 // RTC割り込み設定
    SPI_Init();                 // SD 用 SPI設定
    Disable_analog_comp()       // Disable analog comp
    twi_i2c_MasterInit(40);     // I2Cマスタ初期化
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);    // Deep sleep
    sei();                      // Start ISR

    pre_month = 13;
    volt = 3900;
    ilum = 10;
    trend = -1;
    isLog = false;
    isSensor = false;
    isSync = false;
    isTrend = false;
    SENSOR_ON();                /*センサ電源ON*/
    start_beep();
    DS3231S_now(&rtc_time);
    state = S_INIT;

    while(1)
    {
        if ( BUTTON1 )      /* ボタン1が押された*/
        {
            uint8_t bt1_times;

            ST7032i_Clear();

            bt1_times = 0;
            // スイッチが放されるまで待つ
            while( BUTTON1 )
            {
                click_beep();
                wait_ms(100);
                if (++bt1_times > 10)   // 長押しされてシャットダウンへ
                {
                    state = S_USER_SHUTDOWN;
                    break;
                }

                ST7032i_setCursor(0 , 0);
                xsprintf_p(buf, PSTR("Shutdown...%d"),10 - bt1_times);
                ST7032i_puts(buf);
            }
            ST7032i_Clear();
        }

        if ( BUTTON2 )      /* ボタン1が押された*/
        {
            uint8_t bt2_times;

            ST7032i_Clear();

            ST7032i_setCursor(0 , 0);
            //                    ﾃﾞﾝﾁ => x.xxxV
            xsprintf_p(buf, PSTR("\xC3\xDE\xDD\xC1 => %u.%03uV"),volt/1000,volt%1000);
            ST7032i_puts(buf);

            ST7032i_setCursor(0 , 1);
            //                    ｱｶﾙｻ => xx
            xsprintf_p(buf, PSTR("\xB1\xB6\xD9\xBB => %d"),ilum);
            ST7032i_puts(buf);

            bt2_times = 0;
            // スイッチが放されるまで待つ
            while( BUTTON2 )
            {
                click_beep();
                wait_ms(100);
                if (++bt2_times > 10)   // 長押しされて設定画面へ
                {
                    state = S_CONFIG;
                    break;
                }
            }
            ST7032i_Clear();
        }

        if ( SD_DETECT )    /* SDが挿入されてないときはSPIを停止する。*/
        {
            SPI_Start();
            isLog = true;
        }
        else
        {
            SPI_Stop();
            isLog = false;
        }

        switch(state)
        {
            case S_INIT:
                lcdpower_on();
                // スタートアップメッセージ出力
                dispMessage(PSTR("AVR Barometer"), PSTR("v0.3  2013/03/24"));

                if((rc = mpl115a_open()) != 0)
                {
                    //              1234567890123456        1234567890123456
                    dispMessage(PSTR("[MPL115A2]"),     PSTR("Init Error!!"));
                    alarm_beep();
                    state = S_ERROR;
                }
                // 初期データ収集
                if ( !initMpl115a2Data())
                {
                    //              1234567890123456        1234567890123456
                    dispMessage(PSTR("[MPL115A2]"),     PSTR("Init Data Error!"));
                    alarm_beep();
                    state = S_ERROR;
                }

                initVoltData( );            // 電圧データ収集
                initIlumData( );            // 照度データ収集
                initTrendData( );           // 天気予報データ初期化

                state = S_MEASUREMENT;      // 次のステート
                if ( SD_DETECT )
                {
                    SPI_Start();
                    wait_ms(100);

                    if (f_mount(0,&Fatfs) != FR_OK)
                    {
                        alarm_beep();
                        isLog = false;
                    }
                    else
                    {
                        state = S_OPEN;     // ファイルオープン処理へ
                        isLog = true;
                    }
                }
                else
                {
                    SPI_Stop();
                    isLog = false;
                }
                ST7032i_Clear();
                break;

            case S_CONFIG:
                setConfig(isLog);
                if (isLog)
                {
                    state = S_OPEN;
                }
                else
                {
                    state = S_MEASUREMENT;
                }
                break;

            case S_OPEN:
                pre_month = rtc_time.month;
                // 日単位から月単位のファイルに変更
                xsprintf_p(Fname, PSTR("%04u%02u.LOG"), rtc_time.year, rtc_time.month);
                // ファイルをオープン
                if (f_open(&File1, Fname, FA_WRITE | FA_OPEN_ALWAYS)    /* Open log file*/
                   || f_lseek(&File1, f_size(&File1)))      /* Append mode  */
                {
                    isLog = false;
                    //                 1234567890123456        1234567890123456
                    dispMessage2(PSTR("[Open ERROR]"),Fname);
                    alarm_beep();
                    ST7032i_Mute_Icon(true);
                    ST7032i_KeyLock_Icon(false);
                    wait_sec(1);
                }
                else
                {
                    isLog = true;

                    dispMessage2(PSTR("[File Opened!]"),Fname);
                    start_beep();
                    ST7032i_Mute_Icon(false);
                    ST7032i_KeyLock_Icon(true);
                    wait_sec(1);
                }
                ST7032i_Clear();
                state = S_MEASUREMENT;
                break;

            case S_MEASUREMENT:
                dispTime();

                /* 月が変わったらファイルを閉じて新規ファイルを開く*/
                if((pre_month != rtc_time.month) && isLog)
                {
                    pre_month = rtc_time.month;

                    if (f_sync(&File1))
                    {
                        //              1234567890123456        1234567890123456
                        dispMessage(PSTR("[FILE]"),         PSTR("Sync Error!!"));
                        alarm_beep();
                        state = S_ERROR;
                    }
                    else
                    {
                        led_blink(50, 2);
                    }

                    f_close(&File1);
                    state = S_OPEN;
                    continue;
                }

                /* 電源電圧を計測する*/
                volt = averageVoltData();   /* */

                for (batt =0; batt < (BATTERY_LEVEL-1); batt++)
                {
                    if (volt < battery_level[batt])
                        break;
                }

                if (batt == 0)
                {
                    //              1234567890123456        1234567890123456
                    dispMessage(PSTR("[Low Power]"),    PSTR("goto ShutDown."));
                    alarm_beep();
                    wait_sec(1);
                    state = S_POWERDOWN;
                    continue;
                }

                ST7032i_Power_Icon(batt-1, true);

                /*照度を測る*/
                ilum = averageIlumData();

                if ( averageMpl115a2Data(&atmo_tmp1, &atmo_tmp2, &temp_tmp1, &temp_tmp2) )
                {

                    // 天気予報データ計算
                    if (isTrend)
                    {
	                    setTrendData( atmo_tmp1, atmo_tmp2 );
	                    trend = getTrendDiff();
	                    cli(); isTrend = false; sei();
                    }

                    dispAtom(atmo_tmp1, atmo_tmp2, temp_tmp1, temp_tmp2);

                    /* 1分毎の記録を書き込み */
                    if (isLog)
                    {
                        if (isSensor)
                        {
                            f_printf(&File1, "%04u-%02u-%02u,%02u:%02u:%02u,%u.%u,%u.%u,%u,%u.%03u,%d\n",
                                        rtc_time.year,rtc_time.month,rtc_time.day,
                                        rtc_time.hour,rtc_time.min,rtc_time.sec,
                                        atmo_tmp1, atmo_tmp2,
                                        temp_tmp1, temp_tmp2,
                                        ilum, volt/1000, volt%1000, trend);
                            led_blink(50, 2);
                            cli(); isSensor = false; sei();
                        }
                        if(isSync)
                        {
                            if (f_sync(&File1))
                            {
                                //              1234567890123456        1234567890123456
                                dispMessage(PSTR("[FILE]"),         PSTR("Sync Error!!"));
                                alarm_beep();
                                state = S_ERROR;
                            }
                            else
                            {
                                click_beep();
                            }
                            cli(); isSync = false; sei();
                        }
                    }
                }
                break;

            case S_USER_SHUTDOWN:
				backupTrendData();	/* 天気予報データセーブ*/
                if (isLog)			/* SDへデータ書き込み&クローズ*/
                {
                    f_sync(&File1);
                    f_close(&File1);
                }
                ST7032i_Clear();
                ST7032i_setCursor(0 , 0);
                ST7032i_puts_p(PSTR("Shut Down"));
                state = S_POWERDOWN;
                break;

            case S_POWERDOWN:
                POWER_OFF();
                break;
        }

        // RTCデータの更新
        DS3231S_now(&rtc_time);
        SENSOR_OFF();   /*センサ電源OFF*/
        sleep_mode();
        SENSOR_ON();    /*センサ電源ON*/
    }
}
/* ================================================== [ここまでメイン関数]  */
