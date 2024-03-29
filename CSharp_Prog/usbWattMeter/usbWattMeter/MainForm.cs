//==========================================================================
// File Name    : MainForm.cs
//
// Title        : メイン・フォーム・クラス
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
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;


namespace usbWattMeter
{
    public partial class MainForm : Form
    {
        const int CONST_ON = 1;
        const int CONST_OFF = 2;
        const int CONDITION_ON = 3;
        const int CONDITION_OFF = 4;

        const int TIME = 1;
        const int CH1_UPPER = 2;
        const int CH1_LOWER = 3;
        const int CH2_UPPER = 4;
        const int CH2_LOWER = 5;
        const int CH3_UPPER = 6;
        const int CH3_LOWER = 7;
        const int MAX_HISTORY = 30;

        int LapsedTime = 0;
        int chart1_max_history = MAX_HISTORY;
        int chart2_max_history = MAX_HISTORY;
        double sumAmp = 0.0;

        int sw_state;
        int sw_timing;
        double sw_condition_limit;
        string csv_directory;

        private bool isOnce = true;
        private bool isStartButtuon = true;

        Queue<double> UsbAmpHistory = new Queue<double>();    // 単位はA
        Queue<double> ch2VoltHistory = new Queue<double>();   // 単位はV
        Queue<double> ch3VoltHistory = new Queue<double>();   // 単位はV

        // Create an instance of the USB reference device
        private usbWattMeterDevice theWattMeterUsbDevice;
        ConfigForm cForm = new ConfigForm();

        // private functions
        //========================


        /// ------------------------------------------------------------------------
        /// <summary>
        ///     指定した精度の数値に四捨五入します。</summary>
        /// <param name="dValue">
        ///     丸め対象の倍精度浮動小数点数。</param>
        /// <param name="iDigits">
        ///     戻り値の有効桁数の精度。</param>
        /// <returns>
        ///     iDigits に等しい精度の数値に四捨五入された数値。</returns>
        /// ------------------------------------------------------------------------
        private double ToHalfAdjust(double dValue, int iDigits)
        {
            double dCoef = System.Math.Pow(10, iDigits);
            return dValue > 0 ? System.Math.Floor((dValue * dCoef) + 0.5) / dCoef :
                                System.Math.Ceiling((dValue * dCoef) - 0.5) / dCoef;
        }

        // CSVファイルにデータ書き込み(結構適当)
        //=====================================
        private void saveCSV()
        {
            int count;
            string ss;
            if (ch1CsvCheckBox.Checked && ch1CheckBox.Checked)
            {
                string file = csv_directory + "\\usbAmp.csv";
                StreamWriter swUSB = new StreamWriter(@file);
                count = 1;
                foreach (double value in UsbAmpHistory)
                {
                    ss = count.ToString() + "," + ToHalfAdjust(value, 2).ToString();
                    // データをチャートに追加
                    count++;
                    swUSB.WriteLine(ss);
                }
                swUSB.Close();
            }
            if (ch2CsvCheckBox.Checked && ch2CheckBox.Checked)
            {
                string file = csv_directory + "\\ch2Volt.csv";
                StreamWriter swCH2 = new StreamWriter(@file);
                count = 1;
                foreach (double value in ch2VoltHistory)
                {
                    ss = count.ToString() + "," + ToHalfAdjust(value, 2).ToString();
                    // データをチャートに追加
                    count++;
                    swCH2.WriteLine(ss);
                }
                swCH2.Close();
            }
            if (ch3CsvCheckBox.Checked && ch3CheckBox.Checked)
            {
                string file = csv_directory + "\\ch3Volt.csv";
                StreamWriter swCH3 = new StreamWriter(@file);
                count = 1;
                foreach (double value in ch3VoltHistory)
                {
                    ss = count.ToString() + "," + ToHalfAdjust(value, 2).ToString();
                    // データをチャートに追加
                    count++;
                    swCH3.WriteLine(ss);
                }
                swCH3.Close();
            }
        }

        // Listener for USB events
        private void usbEvent_receiver(object o, EventArgs e)
        {
            // Check the status of the USB device and update the form accordingly
            if (theWattMeterUsbDevice.isDeviceAttached)
            {
                theWattMeterUsbDevice.setLed();
                // Update the status label
                this.usbToolStripStatusLabel.Text = "ターゲット接続中(Ready)";
                mesureStartButton.Enabled = true;
            }
            else
            {
                // Update the status label
                this.usbToolStripStatusLabel.Text = "ターゲット切断(Not Ready)";
                mesureStartButton.Enabled = false;


                // =======================================
                timer1.Stop();  // タイマ OFF

                ch1CheckBox.Enabled = true;
                ch2CheckBox.Enabled = true;
                ch3CheckBox.Enabled = true;
                ch1CsvCheckBox.Enabled = true;
                ch2CsvCheckBox.Enabled = true;
                ch3CsvCheckBox.Enabled = true;
                configButton.Enabled = true;
                usbVoltTextBox.Enabled = true;
                ch2RatioTextBox.Enabled = true;

                saveCSV();

                UsbAmpHistory.Clear();    // 単位はA
                ch2VoltHistory.Clear();   // 単位はV
                ch3VoltHistory.Clear();   // 単位はV

                mesureStartButton.Text = "計測開始";
                isStartButtuon = true;
            }
        }

        private void initChart()
        {
            //
            // 枠描画
            //================================================
            chart1.BackColor = System.Drawing.Color.WhiteSmoke;
            chart1.BackGradientStyle = System.Windows.Forms.DataVisualization.Charting.GradientStyle.TopBottom;
            chart1.BackSecondaryColor = System.Drawing.Color.White;
            chart1.BorderlineColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(59)))), ((int)(((byte)(105)))));
            chart1.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chart1.BorderlineWidth = 2;
            chart1.BorderSkin.SkinStyle = System.Windows.Forms.DataVisualization.Charting.BorderSkinStyle.Emboss;
            chart1.ChartAreas[0].Area3DStyle.Inclination = 15;
            chart1.ChartAreas[0].Area3DStyle.IsClustered = true;
            chart1.ChartAreas[0].Area3DStyle.IsRightAngleAxes = false;
            chart1.ChartAreas[0].Area3DStyle.Perspective = 10;
            chart1.ChartAreas[0].Area3DStyle.PointGapDepth = 0;
            chart1.ChartAreas[0].Area3DStyle.Rotation = 10;
            chart1.ChartAreas[0].Area3DStyle.WallWidth = 0;
            chart1.ChartAreas[0].AxisX.LabelAutoFitStyle = ((System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles)((((System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.DecreaseFont | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.StaggeredLabels)
                        | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.LabelsAngleStep30)
                        | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.WordWrap)));
            chart1.ChartAreas[0].AxisX.LabelStyle.Font = new System.Drawing.Font("Trebuchet MS", 8.25F, System.Drawing.FontStyle.Bold);
            chart1.ChartAreas[0].AxisX.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[0].AxisX.MajorGrid.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[0].AxisY.LabelAutoFitStyle = ((System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles)((((System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.DecreaseFont | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.StaggeredLabels)
                        | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.LabelsAngleStep30)
                        | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.WordWrap)));
            chart1.ChartAreas[0].AxisX.Title = "経過時間(S)";
            chart1.ChartAreas[0].AxisY.LabelStyle.Font = new System.Drawing.Font("Trebuchet MS", 8.25F, System.Drawing.FontStyle.Bold);
            chart1.ChartAreas[0].AxisY.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[0].AxisY.MajorGrid.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[0].AxisY.Title = "電流(mA)";
            chart1.ChartAreas[0].BackColor = System.Drawing.Color.WhiteSmoke;
            chart1.ChartAreas[0].BackSecondaryColor = System.Drawing.Color.White;
            chart1.ChartAreas[0].BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[0].BorderDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chart1.ChartAreas[0].Name = "Default";
            chart1.ChartAreas[0].ShadowColor = System.Drawing.Color.Transparent;
            chart1.Legends[0].BackColor = System.Drawing.Color.Transparent;
            chart1.Legends[0].Enabled = true;
            chart1.Legends[0].Font = new System.Drawing.Font("Trebuchet MS", 8.25F, System.Drawing.FontStyle.Bold);
            chart1.Legends[0].IsTextAutoFit = false;
            chart1.Legends[0].Name = "Default";
            chart1.Series[0].BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(26)))), ((int)(((byte)(59)))), ((int)(((byte)(105)))));
            //            chart1.Series[0].ChartArea = "ChartArea1";
            chart1.Series[0].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            chart1.Series[0].Color = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(59)))), ((int)(((byte)(105)))));
            //            chart1.Series[0].Legend = "Default";
            chart1.Series[0].Name = "CH1(USB)電流";
            chart1.Series[0].XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            chart1.ChartAreas[1].Area3DStyle.Inclination = 15;
            chart1.ChartAreas[1].Area3DStyle.IsClustered = true;
            chart1.ChartAreas[1].Area3DStyle.IsRightAngleAxes = false;
            chart1.ChartAreas[1].Area3DStyle.Perspective = 10;
            chart1.ChartAreas[1].Area3DStyle.PointGapDepth = 0;
            chart1.ChartAreas[1].Area3DStyle.Rotation = 10;
            chart1.ChartAreas[1].Area3DStyle.WallWidth = 0;
            chart1.ChartAreas[1].AxisX.LabelAutoFitStyle = ((System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles)((((System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.DecreaseFont | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.StaggeredLabels)
                        | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.LabelsAngleStep30)
                        | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.WordWrap)));
            chart1.ChartAreas[1].AxisX.LabelStyle.Font = new System.Drawing.Font("Trebuchet MS", 8.25F, System.Drawing.FontStyle.Bold);
            chart1.ChartAreas[1].AxisX.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[1].AxisX.MajorGrid.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[1].AxisY.LabelAutoFitStyle = ((System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles)((((System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.DecreaseFont | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.StaggeredLabels)
                        | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.LabelsAngleStep30)
                        | System.Windows.Forms.DataVisualization.Charting.LabelAutoFitStyles.WordWrap)));
            chart1.ChartAreas[1].AxisX.Title = "経過時間(S)";
            chart1.ChartAreas[1].AxisY.LabelStyle.Font = new System.Drawing.Font("Trebuchet MS", 8.25F, System.Drawing.FontStyle.Bold);
            chart1.ChartAreas[1].AxisY.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[1].AxisY.MajorGrid.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[1].AxisY.Title = "電圧(V)";
            chart1.ChartAreas[1].BackColor = System.Drawing.Color.WhiteSmoke;
            chart1.ChartAreas[1].BackSecondaryColor = System.Drawing.Color.White;
            chart1.ChartAreas[1].BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            chart1.ChartAreas[1].BorderDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            //            chart1.ChartAreas[1].Name = "Default2";
            chart1.ChartAreas[1].ShadowColor = System.Drawing.Color.Transparent;
            //            chart1.Legends[1].BackColor = System.Drawing.Color.Transparent;
            //            chart1.Legends[1].Enabled = true;
            //            chart1.Legends[1].Font = new System.Drawing.Font("Trebuchet MS", 8.25F, System.Drawing.FontStyle.Bold);
            //            chart1.Legends[1].IsTextAutoFit = false;
            //            chart1.Legends[1].Name = "Default";
            chart1.Series[1].BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(26)))), ((int)(((byte)(59)))), ((int)(((byte)(105)))));
            //            chart1.Series[1].ChartArea = "ChartArea2";
            chart1.Series[1].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            chart1.Series[1].Color = System.Drawing.Color.Tomato;
            //            chart1.Series[1].Legend = "Default";
            chart1.Series[1].Name = "CH2電圧";
            chart1.Series[1].XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            chart1.Series[2].BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(26)))), ((int)(((byte)(59)))), ((int)(((byte)(105)))));
            //           chart1.Series[2].ChartArea = "ChartArea2";
            chart1.Series[2].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            chart1.Series[2].Color = System.Drawing.Color.Teal;
            //            chart1.Series[2].Legend = "Default";
            chart1.Series[2].Name = "CH3電圧";
            chart1.Series[2].XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            //            chart1.Legends[1].BackColor = System.Drawing.Color.Transparent;
            //            chart1.Legends[1].Enabled = false;
            //            chart1.Legends[1].Font = new System.Drawing.Font("Trebuchet MS", 8.25F, System.Drawing.FontStyle.Bold);
            //            chart1.Legends[1].IsTextAutoFit = false;
            //            chart1.Legends[1].Name = "Legend1";
        }


        //***************************************************************************
        /// <summary> チャートを描画する
        /// </summary>
        /// <param name="chart"></param>
        //***************************************************************************
        private void showCh2Chart()
        {
            //-----------------------
            // チャートに値をセット
            //-----------------------
            chart1.Series[1].Points.Clear();
            foreach (double value in ch2VoltHistory)
            {
                // データをチャートに追加
                chart1.Series[1].Points.Add(new DataPoint(0, value));
            }
            if (ch2VoltHistory.Count >= chart2_max_history)
            {
                chart2_max_history += MAX_HISTORY;
            }
            for (int i = ch2VoltHistory.Count; i < chart2_max_history; i++)
            {
                chart1.Series[1].Points.Add(new DataPoint(0, 0));
            }
            for (int i = ch2VoltHistory.Count; i < chart2_max_history; i++)
            {
                chart1.Series[1].Points[i].IsEmpty = true;
            }
        }

        //***************************************************************************
        /// <summary> チャートを描画する
        /// </summary>
        /// <param name="chart"></param>
        //***************************************************************************
        private void showCh3Chart()
        {
            //-----------------------
            // チャートに値をセット
            //-----------------------
            chart1.Series[2].Points.Clear();
            foreach (double value in ch3VoltHistory)
            {
                // データをチャートに追加
                chart1.Series[2].Points.Add(new DataPoint(0, value));
            }
            if (ch3VoltHistory.Count >= chart2_max_history)
            {
                chart2_max_history += MAX_HISTORY;
            }
            for (int i = ch3VoltHistory.Count; i < chart2_max_history; i++)
            {
                chart1.Series[2].Points.Add(new DataPoint(0, 0));
            }
            for (int i = ch3VoltHistory.Count; i < chart2_max_history; i++)
            {
                chart1.Series[2].Points[i].IsEmpty = true;
            }
        }

        //***************************************************************************
        /// <summary> チャートを描画する
        /// </summary>
        /// <param name="chart"></param>
        //***************************************************************************
        private void showCh1Chart()
        {
            //-----------------------
            // チャートに値をセット
            //-----------------------
            chart1.Series[0].Points.Clear();
            foreach (double value in UsbAmpHistory)
            {
                // データをチャートに追加
                chart1.Series[0].Points.Add(new DataPoint(0, value));
            }
            if (UsbAmpHistory.Count >= chart1_max_history)
            {
                chart1_max_history += MAX_HISTORY;
            }
            for (int i = UsbAmpHistory.Count; i < chart1_max_history; i++)
            {
                chart1.Series[0].Points.Add(new DataPoint(0, 0));
            }
            for (int i = UsbAmpHistory.Count; i < chart1_max_history; i++)
            {
                chart1.Series[0].Points[i].IsEmpty = true;
            }
        }



        //
        //============================
        public MainForm()
        {
            InitializeComponent();

            // Create the USB reference device object (passing VID and PID)
            theWattMeterUsbDevice = new usbWattMeterDevice(0x04D8, 0x003F);
            // Add a listener for usb events
            theWattMeterUsbDevice.usbEvent += new usbWattMeterDevice.usbEventsHandler(usbEvent_receiver);
            // Perform an initial search for the target device
            theWattMeterUsbDevice.findTargetDevice();

            // チャートの表示を初期化
            initChart();

            // チャートの表示を初期化
            // グラフを再描画する
            //------------------------------------------------
            showCh1Chart();
            showCh2Chart();
            showCh3Chart();

            timer1.Interval = 1000; //1000ミリ秒
        }

        private void MainForm_Load(object sender, EventArgs e)
        {

            Properties.Settings.Default.Reload();
            theWattMeterUsbDevice.isCh1Active = Properties.Settings.Default.ch1Active;
            ch1CheckBox.Checked = Properties.Settings.Default.ch1Active;
            theWattMeterUsbDevice.isCh2Active = Properties.Settings.Default.ch2Active;
            ch2CheckBox.Checked = Properties.Settings.Default.ch2Active;
            theWattMeterUsbDevice.isCh3Active = Properties.Settings.Default.ch3Active;
            ch3CheckBox.Checked = Properties.Settings.Default.ch3Active;
            ch1CsvCheckBox.Checked = Properties.Settings.Default.ch1Csv;
            ch2CsvCheckBox.Checked = Properties.Settings.Default.ch2Csv;
            ch3CsvCheckBox.Checked = Properties.Settings.Default.ch3Csv;
            theWattMeterUsbDevice.ch2Ratio = Properties.Settings.Default.ch2Ratio;
            ch2RatioTextBox.Text = Properties.Settings.Default.ch2Ratio.ToString();
            theWattMeterUsbDevice.UsbVolt = Properties.Settings.Default.UsbVolt;
            usbVoltTextBox.Text = Properties.Settings.Default.UsbVolt.ToString();

            sw_state = Properties.Settings.Default.sw_state;
            sw_condition_limit = Properties.Settings.Default.sw_condition_limit;
            sw_timing = Properties.Settings.Default.sw_timing;
            csv_directory = Properties.Settings.Default.csv_direcory;
        }

        private void closeButton_Click(object sender, EventArgs e)
        {
            Properties.Settings.Default.ch1Active = ch1CheckBox.Checked;
            Properties.Settings.Default.ch2Active = ch2CheckBox.Checked;
            Properties.Settings.Default.ch3Active = ch3CheckBox.Checked;
            Properties.Settings.Default.ch1Csv = ch1CsvCheckBox.Checked;
            Properties.Settings.Default.ch2Csv = ch2CsvCheckBox.Checked;
            Properties.Settings.Default.ch3Csv = ch3CsvCheckBox.Checked;
            Properties.Settings.Default.ch2Ratio = theWattMeterUsbDevice.ch2Ratio;
            Properties.Settings.Default.UsbVolt = theWattMeterUsbDevice.UsbVolt;
            Properties.Settings.Default.sw_condition_limit = sw_condition_limit;
            Properties.Settings.Default.sw_state = sw_state;
            Properties.Settings.Default.sw_timing = sw_timing;
            Properties.Settings.Default.csv_direcory = csv_directory;

            Properties.Settings.Default.Save();
            Close();
        }

        private void configButton_Click(object sender, EventArgs e)
        {
            cForm.sw_state = sw_state;
            cForm.sw_timing = sw_timing;
            cForm.sw_condition_limit = sw_condition_limit;
            cForm.csv_directory = csv_directory;

            DialogResult dr = cForm.ShowDialog();

            if (dr == DialogResult.OK)
            {
                sw_state = cForm.sw_state;
                sw_condition_limit = cForm.sw_condition_limit;
                sw_timing = cForm.sw_timing;
                csv_directory = cForm.csv_directory;
            }
         }

        private void mesureStartButton_Click(object sender, EventArgs e)
        {
            if (isStartButtuon)
            {
                LapsedTime = 0;
                sumAmp = 0.0;

                isOnce = true;

                ch1CheckBox.Enabled = false;
                ch2CheckBox.Enabled = false;
                ch3CheckBox.Enabled = false;
                ch1CsvCheckBox.Enabled = false;
                ch2CsvCheckBox.Enabled = false;
                ch3CsvCheckBox.Enabled = false;
                configButton.Enabled = false;
                usbVoltTextBox.Enabled = false;
                ch2RatioTextBox.Enabled = false;

                chart1_max_history = MAX_HISTORY;
                chart2_max_history = MAX_HISTORY;

                // チャートの表示を初期化
                // グラフを再描画する
                //------------------------------------------------
                showCh1Chart();
                showCh2Chart();
                showCh3Chart();

                mesureStartButton.Text = "計測終了";
                isStartButtuon = false;


                switch (sw_state)
                {
                    case CONST_ON:
                        theWattMeterUsbDevice.setSwitch(true);
                        break;
                    case CONST_OFF:
                        theWattMeterUsbDevice.setSwitch(false);
                        break;
                    case CONDITION_ON:
                        theWattMeterUsbDevice.setSwitch(false);
                        break;
                    case CONDITION_OFF:
                        theWattMeterUsbDevice.setSwitch(true);
                        break;
                }
                timer1.Start();  // タイマ ON
            }
            else
            {
                timer1.Stop();  // タイマ OFF

                ch1CheckBox.Enabled = true;
                ch2CheckBox.Enabled = true;
                ch3CheckBox.Enabled = true;
                ch1CsvCheckBox.Enabled = true;
                ch2CsvCheckBox.Enabled = true;
                ch3CsvCheckBox.Enabled = true;
                configButton.Enabled = true;
                usbVoltTextBox.Enabled = true;
                ch2RatioTextBox.Enabled = true;

                saveCSV();

                UsbAmpHistory.Clear();    // 単位はA
                ch2VoltHistory.Clear();   // 単位はV
                ch3VoltHistory.Clear();   // 単位はV

                mesureStartButton.Text = "計測開始";
                isStartButtuon = true;
            }
        }

        private void ch1CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (ch1CheckBox.Checked)
            {
                theWattMeterUsbDevice.isCh1Active = true;
            }
            else
            {
                theWattMeterUsbDevice.isCh1Active = false;
            }
            theWattMeterUsbDevice.setLed();
        }


        private void usbVoltTextBox_TextChanged(object sender, EventArgs e)
        {
            try
            {
                theWattMeterUsbDevice.UsbVolt = double.Parse(usbVoltTextBox.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        private void ch2CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (ch2CheckBox.Checked)
            {
                theWattMeterUsbDevice.isCh2Active = true;
            }
            else
            {
                theWattMeterUsbDevice.isCh2Active = false;
            }
            theWattMeterUsbDevice.setLed();
        }


        private void ch3CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (ch3CheckBox.Checked)
            {
                theWattMeterUsbDevice.isCh3Active = true;
            }
            else
            {
                theWattMeterUsbDevice.isCh3Active = false;
            }
            theWattMeterUsbDevice.setLed();
        }

        private void ch2RatioTextBox_TextChanged(object sender, EventArgs e)
        {
            try
            {
                theWattMeterUsbDevice.ch2Ratio = double.Parse(ch2RatioTextBox.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }




        private void timer1_Tick(object sender, EventArgs e)
        {
            LapsedTime++;
            LapsedTimeLabel.Text = "時間: " + LapsedTime.ToString() + "秒";

            if (isOnce && sw_timing == TIME && LapsedTime >= (int)sw_condition_limit)
            {
                isOnce = false;
                if (sw_state == CONDITION_OFF)
                {
                    theWattMeterUsbDevice.setSwitch(false);
                }
                else if (sw_state == CONDITION_ON)
                {
                    theWattMeterUsbDevice.setSwitch(true);
                }
            }


            if (theWattMeterUsbDevice.AnalogIn())
            {
                if (theWattMeterUsbDevice.isCh1Active)
                {
                    if (theWattMeterUsbDevice.ch1Amp > 1) return;
                    ch1AmpLabel.Text = ToHalfAdjust(theWattMeterUsbDevice.ch1Amp * 1000, 2).ToString() + "mA";
                    double Power = (theWattMeterUsbDevice.UsbVolt - theWattMeterUsbDevice.ch1Volt) * theWattMeterUsbDevice.ch1Amp;
                    ch1PowerLabel.Text = ToHalfAdjust(Power, 2).ToString() + "W";
                    sumAmp += theWattMeterUsbDevice.ch1Amp;
                    ch1SumAmpLabel.Text = ToHalfAdjust(sumAmp, 2).ToString() + "A";
                    ch1ChargeLabel.Text = ToHalfAdjust(sumAmp * 1000 / 3600, 2).ToString() + "mAh";
                    UsbAmpHistory.Enqueue(theWattMeterUsbDevice.ch1Amp * 1000);
                    showCh1Chart();

                    if (isOnce && sw_timing == CH1_LOWER && (theWattMeterUsbDevice.ch1Amp * 1000) <= sw_condition_limit)
                    {
                        isOnce = false;
                        if (sw_state == CONDITION_OFF)
                        {
                            theWattMeterUsbDevice.setSwitch(false);
                        }
                        else if (sw_state == CONDITION_ON)
                        {
                            theWattMeterUsbDevice.setSwitch(true);
                        }
                    }
                    else if (isOnce && sw_timing == CH1_UPPER && (theWattMeterUsbDevice.ch1Amp * 1000) >= sw_condition_limit)
                    {
                        isOnce = false;
                        if (sw_state == CONDITION_OFF)
                        {
                            theWattMeterUsbDevice.setSwitch(false);
                        }
                        else if (sw_state == CONDITION_ON)
                        {
                            theWattMeterUsbDevice.setSwitch(true);
                        }
                    }
                }
                if (theWattMeterUsbDevice.isCh2Active)
                {
                    if (theWattMeterUsbDevice.ch2Volt > 5) return;
                    ch2VoltHistory.Enqueue(theWattMeterUsbDevice.ch2Volt);
                    ch2VoltLabel.Text = ToHalfAdjust(theWattMeterUsbDevice.ch2Volt, 2).ToString() + "V";
                    showCh2Chart();

                    if (isOnce && sw_timing == CH2_LOWER && theWattMeterUsbDevice.ch2Volt <= sw_condition_limit)
                    {
                        isOnce = false;
                        if (sw_state == CONDITION_OFF)
                        {
                            theWattMeterUsbDevice.setSwitch(false);
                        }
                        else if (sw_state == CONDITION_ON)
                        {
                            theWattMeterUsbDevice.setSwitch(true);
                        }
                    }
                    else if (isOnce && sw_timing == CH2_UPPER && theWattMeterUsbDevice.ch2Volt >= sw_condition_limit)
                    {
                        isOnce = false;
                        if (sw_state == CONDITION_OFF)
                        {
                            theWattMeterUsbDevice.setSwitch(false);
                        }
                        else if (sw_state == CONDITION_ON)
                        {
                            theWattMeterUsbDevice.setSwitch(true);
                        }
                    }
                }
                if (theWattMeterUsbDevice.isCh3Active)
                {
                    if (theWattMeterUsbDevice.ch3Volt > 5) return;
                    ch3VoltHistory.Enqueue(theWattMeterUsbDevice.ch3Volt);
                    ch3VoltLabel.Text = ToHalfAdjust(theWattMeterUsbDevice.ch3Volt, 2).ToString() + "V";
                    showCh3Chart();

                    if (isOnce && sw_timing == CH3_LOWER && theWattMeterUsbDevice.ch3Volt <= sw_condition_limit)
                    {
                        isOnce = false;
                        if (sw_state == CONDITION_OFF)
                        {
                            theWattMeterUsbDevice.setSwitch(false);
                        }
                        else if (sw_state == CONDITION_ON)
                        {
                            theWattMeterUsbDevice.setSwitch(true);
                        }
                    }
                    else if (isOnce && sw_timing == CH3_UPPER && theWattMeterUsbDevice.ch3Volt >= sw_condition_limit)
                    {
                        isOnce = false;
                        if (sw_state == CONDITION_OFF)
                        {
                            theWattMeterUsbDevice.setSwitch(false);
                        }
                        else if (sw_state == CONDITION_ON)
                        {
                            theWattMeterUsbDevice.setSwitch(true);
                        }
                    }
                }
            }
        }

        private void usbVoltTextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            // 制御文字は入力可
            if (char.IsControl(e.KeyChar))
            {
                e.Handled = false;
                return;
            }

            // 数字(0-9)は入力可
            if (char.IsDigit(e.KeyChar))
            {
                e.Handled = false;
                return;
            }

            // 小数点は１つだけ入力可
            if (e.KeyChar == '.')
            {
                TextBox target = sender as TextBox;
                if (target.Text.IndexOf('.') < 0)
                {
                    // 複数のピリオド入力はNG
                    e.Handled = false;
                    return;
                }
            }

            // 上記以外は入力不可
            e.Handled = true;
        }

        private void ch2RatioTextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            // 制御文字は入力可
            if (char.IsControl(e.KeyChar))
            {
                e.Handled = false;
                return;
            }

            // 数字(0-9)は入力可
            if (char.IsDigit(e.KeyChar))
            {
                e.Handled = false;
                return;
            }

            // 小数点は１つだけ入力可
            if (e.KeyChar == '.')
            {
                TextBox target = sender as TextBox;
                if (target.Text.IndexOf('.') < 0)
                {
                    // 複数のピリオド入力はNG
                    e.Handled = false;
                    return;
                }
            }

            // 上記以外は入力不可
            e.Handled = true;
        }
    }
}
