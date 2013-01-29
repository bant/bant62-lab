//==========================================================================
// File Name    : main.c
//
// Title        : USB �[�d�d�͌v�t�@�[���E�F�A
// Revision     : 0.1
// Notes        :
// Target MCU   : PIC 18F14K50
// Tool Chain   : MPLAB C18 Ver 3.43 with MPLAB IDE v8.88
// Frame Work   : Microchip Application Libraries v2012-07-18
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2013/01/01   �΂��      �J���J�n
// 2013/01/19   �΂��      Ver0.1����
//==========================================================================
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//==========================================================================

/** �t�@�C���C���N���[�h **************************/
#include "USB/usb.h"
#include "HardwareProfile.h"
#include "USB/usb_function_hid.h"

/** �R���t�B�M�����[�V���� ***********************/
#pragma config CPUDIV = NOCLKDIV ,USBDIV = OFF, PCLKEN = ON
#pragma config FOSC = HS, PLLEN = ON, HFOFST = OFF
#pragma config PWRTEN = ON, BOREN = OFF, MCLRE = OFF, BORV = 30
#pragma config WDTEN = OFF, LVP = OFF, FCMEN = OFF, IESO = OFF
#pragma config CP0 = OFF, XINST = OFF

/******** USB�֘A�o�b�t�@�A�ϐ���` ****/
#pragma udata usbram2
unsigned char ReceivedDataBuffer[64];
unsigned char SendBuf[64];
#pragma udata
USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid = TRUE;

/**** �O���[�o���ϐ���` ***/
BYTE  counter;
BOOL  blinkStatusValid;

/*** �R�}���h�萔��` ***/
typedef enum
{
    CHECK   = 0x30,
    LEDOUT  = 0x31,
    SWOUT   = 0x32,
    AIN     = 0x33,
    RESET   = 0xFF
} TYPE_CMD;

#define mLED_All_Off()          {mLED_1_Off();mLED_2_Off();mLED_3_Off();}
#define mLED_All_On()           {mLED_1_On();mLED_2_On();mLED_3_On();}
#define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();mLED_3_Off();}
#define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();mLED_3_Off();}
#define mLED_Only_3_On()        {mLED_1_Off();mLED_2_Off();mLED_3_On();}

/** �֐��v���g�^�C�s���O ****************************/
void BlinkUSBStatus(void);
void ProcessIO(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
void USBCBSendResume(void);

/************ ���荞�݃x�N�^��` ***********/
#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = 0x08
void Remapped_High_ISR (void)
{
    _asm goto YourHighPriorityISRCode _endasm
}
#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = 0x18
void Remapped_Low_ISR (void)
{
    _asm goto YourLowPriorityISRCode _endasm
}

/************* ���荞�ݏ����֐� ************/
#pragma code
#pragma interrupt YourHighPriorityISRCode
void YourHighPriorityISRCode()
{
    USBDeviceTasks();
}
#pragma interruptlow YourLowPriorityISRCode
void YourLowPriorityISRCode()
{
}

/***********  ���C���֐� ***************************/
#pragma code
void main(void)
{
    /* IO�s�������ݒ� */
    ANSEL  =0x60;                   // AN5,6�̂݃A�i���O���̓f�W�^���ɐݒ�
    ANSELH= 0x08;                   // AN11�̂݃A�i���O���̓f�W�^���ɐݒ�

    TRISA = 0xFF;                   // RA0-5����

    LATB = 0;                       // �o�͏�����
    TRISB = 0x2F;                   // RB5�A�i���O���́A����ȊO�o��

    LATC = 0;                       // �o�͏�����
    TRISC = 0x06;                   // RC1,2�A�i���O���́A����ȊO�o��

    /* ADC������ */
    ADCON0 = 0;                     // ��~
    ADCON1 = 0;                     // VDD-Vss
    ADCON2 = 0xBE;                  // �E�l��,20Tad,Fosc/64

    /* USB�֘A **/
    USBDeviceInit();                // USB������
    USBInHandle = 0;
    USBOutHandle = 0;
    blinkStatusValid = TRUE;        // USB�ڈ�LED�L����
    USBDeviceAttach();              // USB���荞�ݗL����

    /*********** ���C�����[�v ***************/
    while (1)
    {
        /** USB�ڈ�LED�_�� **/
        if (blinkStatusValid)
        {
            BlinkUSBStatus();       // LED�_�Ŏ��s
        }
        /*** USB�ڑ����Ȃ瑗��M���s ***/
        if ((USBDeviceState >= CONFIGURED_STATE)&&(USBSuspendControl!=1))
        {
            ProcessIO();                // �R�}���h���s
        }
    }
}
/***************************************************
 * ���[�U�[�A�v���̓��o�͏����֐�
 * USB����̃R�}���h�ɂ��@�\���s
 ***************************************************/
void ProcessIO(void)
{
    /***** �f�[�^��M���� ******/
    if (!HIDRxHandleBusy(USBOutHandle))
    {
        blinkStatusValid = FALSE;
        SendBuf[0] = ReceivedDataBuffer[0];
        SendBuf[1] = ReceivedDataBuffer[1];
        /******** �R�}���h�̏��� ********/
        switch (ReceivedDataBuffer[0])      // �R�}���h�R�[�h�`�F�b�N
        {
        /** �ڑ��m�F���� **/
        case CHECK:
            SendBuf[2] = 'O';
            SendBuf[3] = 'K';
            if (!HIDTxHandleBusy(USBInHandle))
            {
                USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&SendBuf[0],64);
            }
            break;
        /** LED�̓_�ŗv���̉��� ***/
        case LEDOUT:
            if (ReceivedDataBuffer[1] == 0x31)
            {
                mLED_1_On();
            }
            else
            {
                mLED_1_Off();
            }
            if (ReceivedDataBuffer[2] == 0x31)
            {
                mLED_2_On();
            }
            else
            {
                mLED_2_Off();
            }
            if (ReceivedDataBuffer[3] == 0x31)
            {
                mLED_3_On();
            }
            else
            {
                mLED_3_Off();
            }
            SendBuf[2] = mLED_1 + 0x30;
            SendBuf[3] = mLED_2 + 0x30;
            SendBuf[4] = mLED_3 + 0x30;

            if (!HIDTxHandleBusy(USBInHandle))
            {
                USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&SendBuf[0],64);
            }
            break;

        /** SW��ON OFF�v���̉��� ***/
        case SWOUT:
            if (ReceivedDataBuffer[1] == 0x31)
            {
                mSW_On();
            }
            else if (ReceivedDataBuffer[1] == 0x30)
            {
                mSW_Off();
            }
            SendBuf[2] = mSW + 0x30;

            if (!HIDTxHandleBusy(USBInHandle))
            {
                USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&SendBuf[0],64);
            }
            break;

        /***** �A�i���O���͗v���Ɖ��� *****/
        case AIN:
            if (ReceivedDataBuffer[1] == 0x31)      // CH1 �擾
            {
                ADCON0 = 0x05 << 2;                 // AN5 �I��
                ADCON0bits.ADON = 1;                // ADC �C�l�[�u��
                ADCON0bits.GO = 1;                  // A/D�ϊ��J�n
                while (ADCON0bits.NOT_DONE);        // �ϊ������҂�
                SendBuf[2] = ADRESL;                // ���M�o�b�t�@�ɃZ�b�g
                SendBuf[3] = ADRESH;
            }
            else
            {
                SendBuf[2] = 0;                     // ���M�o�b�t�@��0�Z�b�g
                SendBuf[3] = 0;
            }

            if (ReceivedDataBuffer[2] == 0x31)      // CH2 �擾
            {
                ADCON0 = 0x06 << 2;                 // AN6 �I��
                ADCON0bits.ADON = 1;                // ADC �C�l�[�u��
                ADCON0bits.GO = 1;                  // A/D�ϊ��J�n
                while (ADCON0bits.NOT_DONE);        // �ϊ������҂�
                SendBuf[4] = ADRESL;                // ���M�o�b�t�@�ɃZ�b�g
                SendBuf[5] = ADRESH;
            }
            else
            {
                SendBuf[4] = 0;                     // ���M�o�b�t�@��0�Z�b�g
                SendBuf[5] = 0;
            }

            if (ReceivedDataBuffer[3] == 0x31)      // CH3 �擾
            {
                ADCON0 = 0x0B << 2;                 // AN11 �I��
                ADCON0bits.ADON = 1;                // ADC �C�l�[�u��
                ADCON0bits.GO = 1;                  // A/D�ϊ��J�n
                while (ADCON0bits.NOT_DONE);        // �ϊ������҂�
                SendBuf[6] = ADRESL;                // ���M�o�b�t�@�ɃZ�b�g
                SendBuf[7] = ADRESH;
            }
            else
            {
                SendBuf[6] = 0;                     // ���M�o�b�t�@��0�Z�b�g
                SendBuf[7] = 0;
            }

            if (!HIDTxHandleBusy(USBInHandle))
            {
                USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&SendBuf[0],64);
            }
            break;

        /** ���Z�b�g�f�o�C�X ***/
        case RESET:
            Reset();
            break;

        /*** �s�� ****/
        default:
            break;
        }
        /* ���̎�M���s */
        USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);
    }
}

/****************************************************
 * USB��ԕ\���pLED�u�����N�����֐�
 *  USB�A�^�b�`:�����u�����N
 *  SUB�ڑ�����:�ᑬ�u�����N
 ***************************************************/
void BlinkUSBStatus(void)
{
    static WORD led_count=0;
    static WORD led_number=0;

    if (led_count == 0)
    {
        led_count = 20000U;
    }
    led_count--;

    if (USBSuspendControl == 1)
    {
        if (led_count==0)
        {
            led_number++;
            if (led_number==1)
            {
                mLED_Only_1_On();
            }
            else if (led_number==2)
            {
                mLED_Only_2_On();
            }
            else
            {
                mLED_Only_3_On();
                led_number=0;
            }
        }
    }
    else
    {
        if (USBDeviceState == DETACHED_STATE)
        {
            mLED_All_Off();
        }
        else if (USBDeviceState == ATTACHED_STATE)
        {
            mLED_All_On();
        }
        else if (USBDeviceState == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if (USBDeviceState == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if (USBDeviceState == ADDRESS_STATE)
        {
            if (led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }
        }
        else if (USBDeviceState == CONFIGURED_STATE)
        {
            if (led_count==0)
            {
                mLED_1_Toggle();
                if (mGetLED_1())
                {
                    mLED_2_Off();
                }
                else
                {
                    mLED_2_On();
                }
            }
        }
    }
}

/******************************************************************
************** USB Callback Functions *****************************
*******************************************************************/
/******************************************************************
 * Function:        void USBCBSuspend(void)
 ******************************************************************/
void USBCBSuspend(void)
{
}
/*******************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *******************************************************************/
void USBCBWakeFromSuspend(void)
{
}
/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
}
/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *******************************************************************/
void USBCBErrorHandler(void)
{
}
/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();
}//end

/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
}//end

/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *******************************************************************/
void USBCBInitEP(void)
{
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);
}

/*******************************************************************
 * Function:        void USBCBSendResume(void)
 ******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;

    if (USBGetRemoteWakeupStatus() == TRUE)
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if (USBIsBusSuspended() == TRUE)
        {
            USBMaskInterrupts();

            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0;
            USBBusIsSuspended = FALSE;  //So we don't execute this code again,

            delay_count = 3600U;
            do
            {
                delay_count--;
            }
            while (delay_count);

            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;       // Start RESUME signaling
            delay_count = 1800U;        // Set RESUME line for 1-13 ms
            do
            {
                delay_count--;
            }
            while (delay_count);
            USBResumeControl = 0;       //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch (event)
    {
    case EVENT_TRANSFER:
        break;
    case EVENT_SOF:
        USBCB_SOF_Handler();
        break;
    case EVENT_SUSPEND:
        USBCBSuspend();
        break;
    case EVENT_RESUME:
        USBCBWakeFromSuspend();
        break;
    case EVENT_CONFIGURED:
        USBCBInitEP();
        break;
    case EVENT_SET_DESCRIPTOR:
        USBCBStdSetDscHandler();
        break;
    case EVENT_EP0_REQUEST:
        USBCBCheckOtherReq();
        break;
    case EVENT_BUS_ERROR:
        USBCBErrorHandler();
        break;
    case EVENT_TRANSFER_TERMINATED:
        break;
    default:
        break;
    }
    return TRUE;
}
