namespace usbWattMeter
{
    partial class MainForm
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.LapsedTimeLabel = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.usbVoltTextBox = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.ch1SumAmpLabel = new System.Windows.Forms.Label();
            this.ch1PowerLabel = new System.Windows.Forms.Label();
            this.ch1ChargeLabel = new System.Windows.Forms.Label();
            this.ch1AmpLabel = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.ch1CsvCheckBox = new System.Windows.Forms.CheckBox();
            this.ch1CheckBox = new System.Windows.Forms.CheckBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.ch2VoltLabel = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.ch2RatioTextBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.ch2CsvCheckBox = new System.Windows.Forms.CheckBox();
            this.ch2CheckBox = new System.Windows.Forms.CheckBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.ch3VoltLabel = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.ch3CsvCheckBox = new System.Windows.Forms.CheckBox();
            this.ch3CheckBox = new System.Windows.Forms.CheckBox();
            this.mesureStartButton = new System.Windows.Forms.Button();
            this.configButton = new System.Windows.Forms.Button();
            this.closeButton = new System.Windows.Forms.Button();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.usbToolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // chart1
            // 
            chartArea1.Name = "ChartArea1";
            chartArea2.Name = "ChartArea2";
            this.chart1.ChartAreas.Add(chartArea1);
            this.chart1.ChartAreas.Add(chartArea2);
            legend1.Name = "Legend1";
            this.chart1.Legends.Add(legend1);
            this.chart1.Location = new System.Drawing.Point(13, 13);
            this.chart1.Name = "chart1";
            series1.ChartArea = "ChartArea1";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series1.Legend = "Legend1";
            series1.Name = "Series1";
            series2.ChartArea = "ChartArea2";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series2.Legend = "Legend1";
            series2.Name = "Series2";
            series3.ChartArea = "ChartArea2";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series3.Legend = "Legend1";
            series3.Name = "Series3";
            this.chart1.Series.Add(series1);
            this.chart1.Series.Add(series2);
            this.chart1.Series.Add(series3);
            this.chart1.Size = new System.Drawing.Size(503, 404);
            this.chart1.TabIndex = 0;
            this.chart1.Text = "chart1";
            // 
            // LapsedTimeLabel
            // 
            this.LapsedTimeLabel.AutoSize = true;
            this.LapsedTimeLabel.Font = new System.Drawing.Font("MS UI Gothic", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.LapsedTimeLabel.Location = new System.Drawing.Point(522, 13);
            this.LapsedTimeLabel.Name = "LapsedTimeLabel";
            this.LapsedTimeLabel.Size = new System.Drawing.Size(92, 19);
            this.LapsedTimeLabel.TabIndex = 2;
            this.LapsedTimeLabel.Text = "時間: 0秒";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.usbVoltTextBox);
            this.groupBox1.Location = new System.Drawing.Point(526, 35);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(158, 42);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "[USB電圧(V)]";
            // 
            // usbVoltTextBox
            // 
            this.usbVoltTextBox.Location = new System.Drawing.Point(12, 16);
            this.usbVoltTextBox.Name = "usbVoltTextBox";
            this.usbVoltTextBox.Size = new System.Drawing.Size(134, 19);
            this.usbVoltTextBox.TabIndex = 0;
            this.usbVoltTextBox.TextChanged += new System.EventHandler(this.usbVoltTextBox_TextChanged);
            this.usbVoltTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.usbVoltTextBox_KeyPress);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.ch1SumAmpLabel);
            this.groupBox2.Controls.Add(this.ch1PowerLabel);
            this.groupBox2.Controls.Add(this.ch1ChargeLabel);
            this.groupBox2.Controls.Add(this.ch1AmpLabel);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.ch1CsvCheckBox);
            this.groupBox2.Controls.Add(this.ch1CheckBox);
            this.groupBox2.Location = new System.Drawing.Point(526, 83);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(158, 99);
            this.groupBox2.TabIndex = 4;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "[USB電力計(CH1)]";
            // 
            // ch1SumAmpLabel
            // 
            this.ch1SumAmpLabel.AutoSize = true;
            this.ch1SumAmpLabel.Location = new System.Drawing.Point(75, 80);
            this.ch1SumAmpLabel.Name = "ch1SumAmpLabel";
            this.ch1SumAmpLabel.Size = new System.Drawing.Size(11, 12);
            this.ch1SumAmpLabel.TabIndex = 9;
            this.ch1SumAmpLabel.Text = "-";
            // 
            // ch1PowerLabel
            // 
            this.ch1PowerLabel.AutoSize = true;
            this.ch1PowerLabel.Location = new System.Drawing.Point(75, 65);
            this.ch1PowerLabel.Name = "ch1PowerLabel";
            this.ch1PowerLabel.Size = new System.Drawing.Size(11, 12);
            this.ch1PowerLabel.TabIndex = 8;
            this.ch1PowerLabel.Text = "-";
            // 
            // ch1ChargeLabel
            // 
            this.ch1ChargeLabel.AutoSize = true;
            this.ch1ChargeLabel.Location = new System.Drawing.Point(75, 50);
            this.ch1ChargeLabel.Name = "ch1ChargeLabel";
            this.ch1ChargeLabel.Size = new System.Drawing.Size(11, 12);
            this.ch1ChargeLabel.TabIndex = 7;
            this.ch1ChargeLabel.Text = "-";
            // 
            // ch1AmpLabel
            // 
            this.ch1AmpLabel.AutoSize = true;
            this.ch1AmpLabel.Location = new System.Drawing.Point(75, 35);
            this.ch1AmpLabel.Name = "ch1AmpLabel";
            this.ch1AmpLabel.Size = new System.Drawing.Size(11, 12);
            this.ch1AmpLabel.TabIndex = 6;
            this.ch1AmpLabel.Text = "-";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(10, 80);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(55, 12);
            this.label4.TabIndex = 5;
            this.label4.Text = "累計電流:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(10, 65);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(31, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "電力:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "充電容量:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(31, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "電流:";
            // 
            // ch1CsvCheckBox
            // 
            this.ch1CsvCheckBox.AutoSize = true;
            this.ch1CsvCheckBox.Location = new System.Drawing.Point(75, 15);
            this.ch1CsvCheckBox.Name = "ch1CsvCheckBox";
            this.ch1CsvCheckBox.Size = new System.Drawing.Size(71, 16);
            this.ch1CsvCheckBox.TabIndex = 1;
            this.ch1CsvCheckBox.Text = "CSV有効";
            this.ch1CsvCheckBox.UseVisualStyleBackColor = true;
            // 
            // ch1CheckBox
            // 
            this.ch1CheckBox.AutoSize = true;
            this.ch1CheckBox.Location = new System.Drawing.Point(10, 15);
            this.ch1CheckBox.Name = "ch1CheckBox";
            this.ch1CheckBox.Size = new System.Drawing.Size(48, 16);
            this.ch1CheckBox.TabIndex = 0;
            this.ch1CheckBox.Text = "有効";
            this.ch1CheckBox.UseVisualStyleBackColor = true;
            this.ch1CheckBox.CheckedChanged += new System.EventHandler(this.ch1CheckBox_CheckedChanged);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.ch2VoltLabel);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.ch2RatioTextBox);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Controls.Add(this.ch2CsvCheckBox);
            this.groupBox3.Controls.Add(this.ch2CheckBox);
            this.groupBox3.Location = new System.Drawing.Point(526, 188);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(158, 76);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "[CH2]";
            // 
            // ch2VoltLabel
            // 
            this.ch2VoltLabel.AutoSize = true;
            this.ch2VoltLabel.Location = new System.Drawing.Point(75, 55);
            this.ch2VoltLabel.Name = "ch2VoltLabel";
            this.ch2VoltLabel.Size = new System.Drawing.Size(11, 12);
            this.ch2VoltLabel.TabIndex = 13;
            this.ch2VoltLabel.Text = "-";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(10, 55);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(31, 12);
            this.label6.TabIndex = 12;
            this.label6.Text = "電圧:";
            // 
            // ch2RatioTextBox
            // 
            this.ch2RatioTextBox.Location = new System.Drawing.Point(75, 31);
            this.ch2RatioTextBox.Name = "ch2RatioTextBox";
            this.ch2RatioTextBox.Size = new System.Drawing.Size(71, 19);
            this.ch2RatioTextBox.TabIndex = 11;
            this.ch2RatioTextBox.TextChanged += new System.EventHandler(this.ch2RatioTextBox_TextChanged);
            this.ch2RatioTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.ch2RatioTextBox_KeyPress);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(10, 35);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(31, 12);
            this.label5.TabIndex = 10;
            this.label5.Text = "倍率:";
            // 
            // ch2CsvCheckBox
            // 
            this.ch2CsvCheckBox.AutoSize = true;
            this.ch2CsvCheckBox.Location = new System.Drawing.Point(75, 15);
            this.ch2CsvCheckBox.Name = "ch2CsvCheckBox";
            this.ch2CsvCheckBox.Size = new System.Drawing.Size(71, 16);
            this.ch2CsvCheckBox.TabIndex = 1;
            this.ch2CsvCheckBox.Text = "CSV有効";
            this.ch2CsvCheckBox.UseVisualStyleBackColor = true;
            // 
            // ch2CheckBox
            // 
            this.ch2CheckBox.AutoSize = true;
            this.ch2CheckBox.Location = new System.Drawing.Point(10, 15);
            this.ch2CheckBox.Name = "ch2CheckBox";
            this.ch2CheckBox.Size = new System.Drawing.Size(48, 16);
            this.ch2CheckBox.TabIndex = 0;
            this.ch2CheckBox.Text = "有効";
            this.ch2CheckBox.UseVisualStyleBackColor = true;
            this.ch2CheckBox.CheckedChanged += new System.EventHandler(this.ch2CheckBox_CheckedChanged);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.ch3VoltLabel);
            this.groupBox4.Controls.Add(this.label7);
            this.groupBox4.Controls.Add(this.ch3CsvCheckBox);
            this.groupBox4.Controls.Add(this.ch3CheckBox);
            this.groupBox4.Location = new System.Drawing.Point(526, 270);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(158, 60);
            this.groupBox4.TabIndex = 6;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "[CH3]";
            // 
            // ch3VoltLabel
            // 
            this.ch3VoltLabel.AutoSize = true;
            this.ch3VoltLabel.Location = new System.Drawing.Point(75, 35);
            this.ch3VoltLabel.Name = "ch3VoltLabel";
            this.ch3VoltLabel.Size = new System.Drawing.Size(11, 12);
            this.ch3VoltLabel.TabIndex = 14;
            this.ch3VoltLabel.Text = "-";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(10, 35);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(31, 12);
            this.label7.TabIndex = 13;
            this.label7.Text = "電圧:";
            // 
            // ch3CsvCheckBox
            // 
            this.ch3CsvCheckBox.AutoSize = true;
            this.ch3CsvCheckBox.Location = new System.Drawing.Point(75, 15);
            this.ch3CsvCheckBox.Name = "ch3CsvCheckBox";
            this.ch3CsvCheckBox.Size = new System.Drawing.Size(71, 16);
            this.ch3CsvCheckBox.TabIndex = 1;
            this.ch3CsvCheckBox.Text = "CSV有効";
            this.ch3CsvCheckBox.UseVisualStyleBackColor = true;
            // 
            // ch3CheckBox
            // 
            this.ch3CheckBox.AutoSize = true;
            this.ch3CheckBox.Location = new System.Drawing.Point(10, 15);
            this.ch3CheckBox.Name = "ch3CheckBox";
            this.ch3CheckBox.Size = new System.Drawing.Size(48, 16);
            this.ch3CheckBox.TabIndex = 0;
            this.ch3CheckBox.Text = "有効";
            this.ch3CheckBox.UseVisualStyleBackColor = true;
            this.ch3CheckBox.CheckedChanged += new System.EventHandler(this.ch3CheckBox_CheckedChanged);
            // 
            // mesureStartButton
            // 
            this.mesureStartButton.Enabled = false;
            this.mesureStartButton.Location = new System.Drawing.Point(526, 336);
            this.mesureStartButton.Name = "mesureStartButton";
            this.mesureStartButton.Size = new System.Drawing.Size(157, 23);
            this.mesureStartButton.TabIndex = 7;
            this.mesureStartButton.Text = "測定開始";
            this.mesureStartButton.UseVisualStyleBackColor = true;
            this.mesureStartButton.Click += new System.EventHandler(this.mesureStartButton_Click);
            // 
            // configButton
            // 
            this.configButton.Location = new System.Drawing.Point(526, 365);
            this.configButton.Name = "configButton";
            this.configButton.Size = new System.Drawing.Size(158, 23);
            this.configButton.TabIndex = 8;
            this.configButton.Text = "その他設定";
            this.configButton.UseVisualStyleBackColor = true;
            this.configButton.Click += new System.EventHandler(this.configButton_Click);
            // 
            // closeButton
            // 
            this.closeButton.Location = new System.Drawing.Point(526, 394);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(157, 23);
            this.closeButton.TabIndex = 9;
            this.closeButton.Text = "終了";
            this.closeButton.UseVisualStyleBackColor = true;
            this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.usbToolStripStatusLabel});
            this.statusStrip1.Location = new System.Drawing.Point(0, 434);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(696, 22);
            this.statusStrip1.TabIndex = 10;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // usbToolStripStatusLabel
            // 
            this.usbToolStripStatusLabel.Name = "usbToolStripStatusLabel";
            this.usbToolStripStatusLabel.Size = new System.Drawing.Size(108, 17);
            this.usbToolStripStatusLabel.Text = "ターゲット確認中(Not Ready)";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(696, 456);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.closeButton);
            this.Controls.Add(this.configButton);
            this.Controls.Add(this.mesureStartButton);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.LapsedTimeLabel);
            this.Controls.Add(this.chart1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MainForm";
            this.Text = "USB電力計";
            this.Load += new System.EventHandler(this.MainForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.Label LapsedTimeLabel;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox usbVoltTextBox;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label ch1SumAmpLabel;
        private System.Windows.Forms.Label ch1PowerLabel;
        private System.Windows.Forms.Label ch1ChargeLabel;
        private System.Windows.Forms.Label ch1AmpLabel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox ch1CsvCheckBox;
        private System.Windows.Forms.CheckBox ch1CheckBox;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label ch2VoltLabel;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox ch2RatioTextBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckBox ch2CsvCheckBox;
        private System.Windows.Forms.CheckBox ch2CheckBox;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label ch3VoltLabel;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.CheckBox ch3CsvCheckBox;
        private System.Windows.Forms.CheckBox ch3CheckBox;
        private System.Windows.Forms.Button mesureStartButton;
        private System.Windows.Forms.Button configButton;
        private System.Windows.Forms.Button closeButton;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel usbToolStripStatusLabel;
    }
}

