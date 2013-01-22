/********************************************************************
*  USB 充電電力計ハードウェア宣言
********************************************************************/

#define HARDWARE_PROFILE_LOW_PIN_COUNT_USB_DEVELOPMENT_KIT_H


//#define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISAbits.TRISA2    // Input
#if defined(USE_SELF_POWER_SENSE_IO)
#define self_power          PORTAbits.RA2
#else
#define self_power          1
#endif

//#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISAbits.TRISA1    // Input
#if defined(USE_USB_BUS_SENSE_IO)
#define USB_BUS_SENSE       PORTAbits.RA1
#else
#define USB_BUS_SENSE       1
#endif

/**** ポート定義 ****/
//      #define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER    //削除

#define DEMO_BOARD LOW_PIN_COUNT_USB_DEVELOPMENT_KIT
#define LOW_PIN_COUNT_USB_DEVELOPMENT_KIT
#define CLOCK_FREQ 48000000

/** LED ************************************************************/
//    #define mInitAllLEDs()      LATC &= 0xF0; TRISC &= 0xF0;  //削除

#define mLED_1              LATCbits.LATC6			// RC6
#define mLED_2              LATCbits.LATC3			// RC3
#define mLED_3              LATCbits.LATC4			// RC4

#define mGetLED_1()         mLED_1
#define mGetLED_2()         mLED_2
#define mGetLED_3()         mLED_3

#define mLED_1_On()         mLED_1 = 1
#define mLED_2_On()         mLED_2 = 1
#define mLED_3_On()         mLED_3 = 1

#define mLED_1_Off()        mLED_1 = 0
#define mLED_2_Off()        mLED_2 = 0
#define mLED_3_Off()        mLED_3 = 0

#define mLED_1_Toggle()     mLED_1 = !mLED_1
#define mLED_2_Toggle()     mLED_2 = !mLED_2
#define mLED_3_Toggle()     mLED_3 = !mLED_3

/** **/

#define mSW					LATBbits.LATB4			// RB4
#define mGetSW()			mSW
#define mSW_On()         	mSW = 1
#define mSW_Off()         	mSW = 0
#define mSW_Toggle()		mSW = !mSW

/** SWITCH *********************************************************/
//    #define mInitSwitch2()      //TRISAbits.TRISA3=1  //削除
//    #define mInitSwitch3()      //TRISAbits.TRISA3=1  //削除
//#define sw1                 PORTCbits.RC5           // 新規追加
//#define sw2                 PORTAbits.RA3
//#define sw3                 PORTCbits.RC4           // 新規追加
//    #define mInitAllSwitches()  mInitSwitch2();       //削除


/** POT ************************************************************/　//削除
//    #define mInitPOT()          {TRISBbits.TRISB4=1;ADCON0=0x29;ADCON1=0;ADCON2=0x3E;ADCON2bits.ADFM = 1;}

/** I/O pin definitions ********************************************/
#define INPUT_PIN 1
#define OUTPUT_PIN 0

