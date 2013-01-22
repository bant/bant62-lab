namespace usbWattMeter
{
    partial class ConfigForm
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.conditionOffRadioButton = new System.Windows.Forms.RadioButton();
            this.conditionOnRadioButton = new System.Windows.Forms.RadioButton();
            this.constOffRadioButton = new System.Windows.Forms.RadioButton();
            this.constOnRadioButton = new System.Windows.Forms.RadioButton();
            this.ch2UpperRadioButton = new System.Windows.Forms.RadioButton();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.conditionTextBox = new System.Windows.Forms.TextBox();
            this.timeRadioButton = new System.Windows.Forms.RadioButton();
            this.ch1UpperRadioButton = new System.Windows.Forms.RadioButton();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.ch3LowerRadioButton = new System.Windows.Forms.RadioButton();
            this.ch3UpperRadioButton = new System.Windows.Forms.RadioButton();
            this.ch2LowerRadioButton = new System.Windows.Forms.RadioButton();
            this.ch1LowerRadioButton = new System.Windows.Forms.RadioButton();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.dirButton = new System.Windows.Forms.Button();
            this.csvDirTextBox = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.conditionOffRadioButton);
            this.groupBox1.Controls.Add(this.conditionOnRadioButton);
            this.groupBox1.Controls.Add(this.constOffRadioButton);
            this.groupBox1.Controls.Add(this.constOnRadioButton);
            this.groupBox1.Location = new System.Drawing.Point(28, 17);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(165, 118);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "[状態]";
            // 
            // conditionOffRadioButton
            // 
            this.conditionOffRadioButton.AutoSize = true;
            this.conditionOffRadioButton.Location = new System.Drawing.Point(18, 91);
            this.conditionOffRadioButton.Name = "conditionOffRadioButton";
            this.conditionOffRadioButton.Size = new System.Drawing.Size(125, 16);
            this.conditionOffRadioButton.TabIndex = 7;
            this.conditionOffRadioButton.TabStop = true;
            this.conditionOffRadioButton.Text = "条件を満たしたらOFF";
            this.conditionOffRadioButton.UseVisualStyleBackColor = true;
            // 
            // conditionOnRadioButton
            // 
            this.conditionOnRadioButton.AutoSize = true;
            this.conditionOnRadioButton.Location = new System.Drawing.Point(18, 69);
            this.conditionOnRadioButton.Name = "conditionOnRadioButton";
            this.conditionOnRadioButton.Size = new System.Drawing.Size(119, 16);
            this.conditionOnRadioButton.TabIndex = 6;
            this.conditionOnRadioButton.TabStop = true;
            this.conditionOnRadioButton.Text = "条件を満たしたらON";
            this.conditionOnRadioButton.UseVisualStyleBackColor = true;
            // 
            // constOffRadioButton
            // 
            this.constOffRadioButton.AutoSize = true;
            this.constOffRadioButton.Location = new System.Drawing.Point(18, 47);
            this.constOffRadioButton.Name = "constOffRadioButton";
            this.constOffRadioButton.Size = new System.Drawing.Size(66, 16);
            this.constOffRadioButton.TabIndex = 5;
            this.constOffRadioButton.TabStop = true;
            this.constOffRadioButton.Text = "常にOFF";
            this.constOffRadioButton.UseVisualStyleBackColor = true;
            // 
            // constOnRadioButton
            // 
            this.constOnRadioButton.AutoSize = true;
            this.constOnRadioButton.Location = new System.Drawing.Point(18, 25);
            this.constOnRadioButton.Name = "constOnRadioButton";
            this.constOnRadioButton.Size = new System.Drawing.Size(60, 16);
            this.constOnRadioButton.TabIndex = 4;
            this.constOnRadioButton.TabStop = true;
            this.constOnRadioButton.Text = "常にON";
            this.constOnRadioButton.UseVisualStyleBackColor = true;
            // 
            // ch2UpperRadioButton
            // 
            this.ch2UpperRadioButton.AutoSize = true;
            this.ch2UpperRadioButton.Location = new System.Drawing.Point(18, 95);
            this.ch2UpperRadioButton.Name = "ch2UpperRadioButton";
            this.ch2UpperRadioButton.Size = new System.Drawing.Size(176, 16);
            this.ch2UpperRadioButton.TabIndex = 2;
            this.ch2UpperRadioButton.TabStop = true;
            this.ch2UpperRadioButton.Text = "CH2の電圧が設定値以上のとき";
            this.ch2UpperRadioButton.UseVisualStyleBackColor = true;
            // 
            // okButton
            // 
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(308, 285);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 1;
            this.okButton.Text = "OK";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(412, 285);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 2;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // conditionTextBox
            // 
            this.conditionTextBox.Location = new System.Drawing.Point(18, 31);
            this.conditionTextBox.Name = "conditionTextBox";
            this.conditionTextBox.Size = new System.Drawing.Size(129, 19);
            this.conditionTextBox.TabIndex = 8;
            this.conditionTextBox.TextChanged += new System.EventHandler(this.conditionTextBox_TextChanged);
            this.conditionTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.conditionTextBox_KeyPress);
            // 
            // timeRadioButton
            // 
            this.timeRadioButton.AutoSize = true;
            this.timeRadioButton.Location = new System.Drawing.Point(18, 29);
            this.timeRadioButton.Name = "timeRadioButton";
            this.timeRadioButton.Size = new System.Drawing.Size(182, 16);
            this.timeRadioButton.TabIndex = 6;
            this.timeRadioButton.TabStop = true;
            this.timeRadioButton.Text = "経過時間が設定時間になったとき";
            this.timeRadioButton.UseVisualStyleBackColor = true;
            // 
            // ch1UpperRadioButton
            // 
            this.ch1UpperRadioButton.AutoSize = true;
            this.ch1UpperRadioButton.Location = new System.Drawing.Point(18, 51);
            this.ch1UpperRadioButton.Name = "ch1UpperRadioButton";
            this.ch1UpperRadioButton.Size = new System.Drawing.Size(207, 16);
            this.ch1UpperRadioButton.TabIndex = 7;
            this.ch1UpperRadioButton.TabStop = true;
            this.ch1UpperRadioButton.Text = "USB(CH1)の電流が設定値以上のとき";
            this.ch1UpperRadioButton.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.ch3LowerRadioButton);
            this.groupBox2.Controls.Add(this.ch3UpperRadioButton);
            this.groupBox2.Controls.Add(this.ch2LowerRadioButton);
            this.groupBox2.Controls.Add(this.ch1LowerRadioButton);
            this.groupBox2.Controls.Add(this.timeRadioButton);
            this.groupBox2.Controls.Add(this.ch1UpperRadioButton);
            this.groupBox2.Controls.Add(this.ch2UpperRadioButton);
            this.groupBox2.Location = new System.Drawing.Point(213, 17);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(241, 193);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "[条件]";
            // 
            // ch3LowerRadioButton
            // 
            this.ch3LowerRadioButton.AutoSize = true;
            this.ch3LowerRadioButton.Location = new System.Drawing.Point(18, 158);
            this.ch3LowerRadioButton.Name = "ch3LowerRadioButton";
            this.ch3LowerRadioButton.Size = new System.Drawing.Size(176, 16);
            this.ch3LowerRadioButton.TabIndex = 11;
            this.ch3LowerRadioButton.TabStop = true;
            this.ch3LowerRadioButton.Text = "CH3の電圧が設定値以下のとき";
            this.ch3LowerRadioButton.UseVisualStyleBackColor = true;
            // 
            // ch3UpperRadioButton
            // 
            this.ch3UpperRadioButton.AutoSize = true;
            this.ch3UpperRadioButton.Location = new System.Drawing.Point(18, 139);
            this.ch3UpperRadioButton.Name = "ch3UpperRadioButton";
            this.ch3UpperRadioButton.Size = new System.Drawing.Size(176, 16);
            this.ch3UpperRadioButton.TabIndex = 10;
            this.ch3UpperRadioButton.TabStop = true;
            this.ch3UpperRadioButton.Text = "CH3の電圧が設定値以上のとき";
            this.ch3UpperRadioButton.UseVisualStyleBackColor = true;
            // 
            // ch2LowerRadioButton
            // 
            this.ch2LowerRadioButton.AutoSize = true;
            this.ch2LowerRadioButton.Location = new System.Drawing.Point(18, 117);
            this.ch2LowerRadioButton.Name = "ch2LowerRadioButton";
            this.ch2LowerRadioButton.Size = new System.Drawing.Size(176, 16);
            this.ch2LowerRadioButton.TabIndex = 9;
            this.ch2LowerRadioButton.TabStop = true;
            this.ch2LowerRadioButton.Text = "CH2の電圧が設定値以下のとき";
            this.ch2LowerRadioButton.UseVisualStyleBackColor = true;
            // 
            // ch1LowerRadioButton
            // 
            this.ch1LowerRadioButton.AutoSize = true;
            this.ch1LowerRadioButton.Location = new System.Drawing.Point(18, 73);
            this.ch1LowerRadioButton.Name = "ch1LowerRadioButton";
            this.ch1LowerRadioButton.Size = new System.Drawing.Size(207, 16);
            this.ch1LowerRadioButton.TabIndex = 8;
            this.ch1LowerRadioButton.TabStop = true;
            this.ch1LowerRadioButton.Text = "USB(CH1)の電流が設定値以下のとき";
            this.ch1LowerRadioButton.UseVisualStyleBackColor = true;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(479, 257);
            this.tabControl1.TabIndex = 10;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.groupBox3);
            this.tabPage1.Controls.Add(this.groupBox2);
            this.tabPage1.Controls.Add(this.groupBox1);
            this.tabPage1.Location = new System.Drawing.Point(4, 21);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(471, 232);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "SW 設定";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.conditionTextBox);
            this.groupBox3.Location = new System.Drawing.Point(28, 141);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(165, 69);
            this.groupBox3.TabIndex = 8;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "[設定値]";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.groupBox4);
            this.tabPage2.Location = new System.Drawing.Point(4, 21);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(471, 232);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "CSV設定";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.dirButton);
            this.groupBox4.Controls.Add(this.csvDirTextBox);
            this.groupBox4.Location = new System.Drawing.Point(21, 20);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(430, 69);
            this.groupBox4.TabIndex = 9;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "[CSV保存ディレクトリィ]";
            // 
            // dirButton
            // 
            this.dirButton.Location = new System.Drawing.Point(334, 28);
            this.dirButton.Name = "dirButton";
            this.dirButton.Size = new System.Drawing.Size(75, 23);
            this.dirButton.TabIndex = 9;
            this.dirButton.Text = "参照...";
            this.dirButton.UseVisualStyleBackColor = true;
            this.dirButton.Click += new System.EventHandler(this.dirButton_Click);
            // 
            // csvDirTextBox
            // 
            this.csvDirTextBox.Location = new System.Drawing.Point(20, 30);
            this.csvDirTextBox.Name = "csvDirTextBox";
            this.csvDirTextBox.Size = new System.Drawing.Size(297, 19);
            this.csvDirTextBox.TabIndex = 8;
            // 
            // ConfigForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(502, 320);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ConfigForm";
            this.Text = "その他設定";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton ch2UpperRadioButton;
        private System.Windows.Forms.RadioButton constOnRadioButton;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.TextBox conditionTextBox;
        private System.Windows.Forms.RadioButton timeRadioButton;
        private System.Windows.Forms.RadioButton constOffRadioButton;
        private System.Windows.Forms.RadioButton conditionOffRadioButton;
        private System.Windows.Forms.RadioButton conditionOnRadioButton;
        private System.Windows.Forms.RadioButton ch1UpperRadioButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton ch3LowerRadioButton;
        private System.Windows.Forms.RadioButton ch3UpperRadioButton;
        private System.Windows.Forms.RadioButton ch2LowerRadioButton;
        private System.Windows.Forms.RadioButton ch1LowerRadioButton;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Button dirButton;
        private System.Windows.Forms.TextBox csvDirTextBox;
    }
}