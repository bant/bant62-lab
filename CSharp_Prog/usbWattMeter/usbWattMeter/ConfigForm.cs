//==========================================================================
// File Name    : ConfigForm.cs
//
// Title        : 設定ダイアログ・フォーム・クラス
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
using System.Windows.Forms;

namespace usbWattMeter
{
    public partial class ConfigForm : Form
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

        // プロパティ設定
        //================================
        public int sw_state
        {
            get
            {
                if (constOnRadioButton.Checked)
                {
                    return CONST_ON;
                }
                else if (constOffRadioButton.Checked)
                {
                    return CONST_OFF;
                }
                else if (conditionOnRadioButton.Checked)
                {
                    return CONDITION_ON;
                }
                else
                {
                    return CONDITION_OFF;
                }
            }
            set
            {
                switch (value)
                {
                    case CONST_ON:
                        constOnRadioButton.Checked = true;
                        break;
                    case CONST_OFF:
                        constOffRadioButton.Checked = true;
                        break;
                    case CONDITION_ON:
                        conditionOnRadioButton.Checked = true;
                        break;
                    default:
                    case CONDITION_OFF:
                        conditionOffRadioButton.Checked = true;
                        break;
                }
            }
        }

        public int sw_timing
        {
            get
            {
                if (timeRadioButton.Checked)
                {
                    return TIME;
                }
                else if (ch1UpperRadioButton.Checked)
                {
                    return CH1_UPPER;
                }
                else if (ch1LowerRadioButton.Checked)
                {
                    return CH1_LOWER;
                }
                else if (ch2UpperRadioButton.Checked)
                {
                    return CH2_UPPER;
                }
                else if (ch2LowerRadioButton.Checked)
                {
                    return CH2_LOWER;
                }
                else if (ch3UpperRadioButton.Checked)
                {
                    return CH3_UPPER;
                }
                else
                {
                    return CH3_LOWER;
                }
            }
            set
            {
                switch (value)
                {
                    case TIME:
                        timeRadioButton.Checked = true;
                        break;
                    case CH1_UPPER:
                        ch1UpperRadioButton.Checked = true;
                        break;
                    case CH1_LOWER:
                        ch1LowerRadioButton.Checked = true;
                        break;
                    case CH2_UPPER:
                        ch2UpperRadioButton.Checked = true;
                        break;
                    case CH2_LOWER:
                        ch2LowerRadioButton.Checked = true;
                        break;
                    case CH3_UPPER:
                        ch3UpperRadioButton.Checked = true;
                        break;
                    default:
                    case CH3_LOWER:
                        ch3LowerRadioButton.Checked = true;
                        break;
                }
            }
        }


        private double _sw_condition_limit;
        public double sw_condition_limit 
        {
            get
            {
                return _sw_condition_limit;
            }
            set
            {
                _sw_condition_limit = value;
                conditionTextBox.Text = _sw_condition_limit.ToString();
            }
        }
        
        public string csv_directory
        {
            get
            {
                return csvDirTextBox.Text;
            }
            set
            {
                csvDirTextBox.Text = value;
            }
        }

        public ConfigForm()
        {
            InitializeComponent();
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void dirButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();

            fbd.Description = "CSVを保存するディレクトリィを選択してください。";
            DialogResult dr = fbd.ShowDialog();

            if (dr == DialogResult.OK)
            {
                csvDirTextBox.Text = fbd.SelectedPath;
            }
        }

        private void conditionTextBox_KeyPress(object sender, KeyPressEventArgs e)
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

        private void conditionTextBox_TextChanged(object sender, EventArgs e)
        {
            try
            {
                sw_condition_limit = double.Parse(conditionTextBox.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}
