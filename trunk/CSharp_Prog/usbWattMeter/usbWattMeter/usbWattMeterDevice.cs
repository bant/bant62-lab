//==========================================================================
// File Name    : usbWattMeterDevice.cs
//
// Title        : USB 電力計デバイス駆動クラス
// Revision     : 0.1
// Notes        :
// Target MCU   : i386
// Tool Chain   : Visual C# 2010 Express  
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- ------------------------------------------------
// 2013/01/10   ばんと      開発開始
// 2013/01/19   ばんと      Ver0.1完了
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

using System;


namespace usbWattMeter
{
    using usbGenericHidCommunications;


    class usbWattMeterDevice : usbGenericHidCommunication
    {
        // ターゲットとの通信コマンド群を宣言
        //=================================
        enum Command : byte
        {
            CHECK = 0x30,
            LEDOUT = 0x31,
            SWOUT = 0x32,
            AIN = 0x33,
            RESET = 0xFF
        }

        enum Target : byte
        {
            ON = 0x31,
            OFF = 0x30
        }


        // 内部データ
        //================================
        private int _ch1ADCValue;
        private int _ch2ADCValue;
        private int _ch3ADCValue;

        // プロパティ設定
        //================================
        public bool isCh1Active { get; set; }
        public bool isCh2Active { get; set; }
        public bool isCh3Active { get; set; }

        public double UsbVolt { get; set; }

        // CH1関連のプロパティ
        //=================================
        public double ch1Volt
        {
            get
            {
                return (((_ch1ADCValue * UsbVolt) / 1024) / 52);
            }
        }

        public double ch1Amp
        {
            get
            {
                return (ch1Volt / 0.1);
            }
        }
        public double ch1Watt
        {
            get
            {
                return ((UsbVolt - ch1Volt) * ch1Amp);
            }
        }

        // CH2関連のプロパティ
        //=================================
        public double ch2Volt
        {
            get
            {
                return (((_ch2ADCValue * UsbVolt) / 1024) / ch2Ratio);
            }
        }

        // CH3関連のプロパティ
        //=================================
        public double ch3Volt
        {
            get
            {
                return ((_ch3ADCValue * UsbVolt) / 1024);
            }
        }

        public double ch2Ratio { get; set; }


        public usbWattMeterDevice(int vid, int pid)
            : base(vid, pid)
        {
            UsbVolt = 5.0;

            _ch1ADCValue = 0;
            _ch2ADCValue = 0;
            _ch3ADCValue = 0;
        }

        public bool Check()
        {
            bool success;

            // Declare our output buffer
            Byte[] inputBuffer = new Byte[65];
            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;
            outputBuffer[1] = (byte)Command.CHECK;     // CHECK コマンド

            // Perform the write command
            success = writeRawReportToDevice(outputBuffer);
            // ON_OFF.ONly proceed if the write was successful
            if (!success) return false;


            // Perform the read
            success = readSingleReportFromDevice(ref inputBuffer);

            // Here we expect the read to fail due to a timeout, so failure counts as a success...
            if (success && inputBuffer[3] == 'O' && inputBuffer[4] == 'K')
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool AnalogIn()
        {
            bool success;

            // Declare our output buffer
            Byte[] inputBuffer = new Byte[65];
            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;
            outputBuffer[1] = (byte)Command.AIN;     // ANALOG_IN コマンド

            if (isCh1Active)
            {
                outputBuffer[2] = (byte)Target.ON;
            }
            else
            {
                outputBuffer[2] = (byte)Target.OFF;
            }

            if (isCh2Active)
            {
                outputBuffer[3] = (byte)Target.ON;
            }
            else
            {
                outputBuffer[3] = (byte)Target.OFF;
            }

            if (isCh3Active)
            {
                outputBuffer[4] = (byte)Target.ON;
            }
            else
            {
                outputBuffer[4] = (byte)Target.OFF;
            }


            success = writeRawReportToDevice(outputBuffer);

            if (!success) return false;


            // Perform the read
            success = readSingleReportFromDevice(ref inputBuffer);

            // Here we expect the read to fail due to a timeout, so failure counts as a success...
            if (!success) return false;


            // CH1 処理
            _ch1ADCValue = (int)((inputBuffer[4] * 256) + inputBuffer[3]);

            // CH2 処理
            _ch2ADCValue = (int)((inputBuffer[6] * 256) + inputBuffer[5]);

            // CH3 処理
            _ch3ADCValue = (int)((inputBuffer[8] * 256) + inputBuffer[7]);

            return true;
        }

        public bool setLed()
        {
            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = (byte)Command.LEDOUT;

            if (isCh1Active)
            {
                outputBuffer[2] = (byte)Target.ON;
            }
            else
            {
                outputBuffer[2] = (byte)Target.OFF;
            }

            if (isCh2Active)
            {
                outputBuffer[3] = (byte)Target.ON;
            }
            else
            {
                outputBuffer[3] = (byte)Target.OFF;
            }

            if (isCh3Active)
            {
                outputBuffer[4] = (byte)Target.ON;
            }
            else
            {
                outputBuffer[4] = (byte)Target.OFF;
            }

            // Perform the write command
            return writeRawReportToDevice(outputBuffer);
        }


        public bool setSwitch(bool bSw)
        {
            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = (byte)Command.SWOUT;

            if (bSw)
            {
                outputBuffer[2] = (byte)Target.ON;
            }
            else
            {
                outputBuffer[2] = (byte)Target.OFF;
            }

            // Perform the write command
            return writeRawReportToDevice(outputBuffer);

        }


        public bool resetTarget()
        {
            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = (byte)Command.RESET;

            // Perform the write command
            return writeRawReportToDevice(outputBuffer);
        }

    }
}
