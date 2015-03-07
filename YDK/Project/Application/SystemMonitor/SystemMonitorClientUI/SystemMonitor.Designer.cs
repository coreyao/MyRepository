namespace SystemMonitorClientUI
{
    partial class SystemMonitor
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mTabControl = new System.Windows.Forms.TabControl();
            this.mHardwarePage = new System.Windows.Forms.TabPage();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.mMacAddress = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.mNetworkCardInfos = new System.Windows.Forms.ComboBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.mGraphicsCardName = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.mSoundCardName = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.mMotherBoardName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.mMemorySize = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.mMemoryInfos = new System.Windows.Forms.ComboBox();
            this.mTotalMemorySize = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.mPartitionSize = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.mPartitionName = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.mPartitionInfos = new System.Windows.Forms.ComboBox();
            this.mTotalHarddiskSize = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.mCPUName = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.mOperatingSystemPage = new System.Windows.Forms.TabPage();
            this.groupBox9 = new System.Windows.Forms.GroupBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label14 = new System.Windows.Forms.Label();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.mProcessName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.mCPU = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.mMemory = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.mTabControl.SuspendLayout();
            this.mHardwarePage.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.mOperatingSystemPage.SuspendLayout();
            this.groupBox9.SuspendLayout();
            this.groupBox8.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.SuspendLayout();
            // 
            // mTabControl
            // 
            this.mTabControl.Controls.Add(this.mHardwarePage);
            this.mTabControl.Controls.Add(this.mOperatingSystemPage);
            this.mTabControl.Location = new System.Drawing.Point(12, 12);
            this.mTabControl.Name = "mTabControl";
            this.mTabControl.SelectedIndex = 0;
            this.mTabControl.Size = new System.Drawing.Size(922, 561);
            this.mTabControl.TabIndex = 0;
            // 
            // mHardwarePage
            // 
            this.mHardwarePage.Controls.Add(this.groupBox7);
            this.mHardwarePage.Controls.Add(this.groupBox5);
            this.mHardwarePage.Controls.Add(this.groupBox6);
            this.mHardwarePage.Controls.Add(this.groupBox2);
            this.mHardwarePage.Controls.Add(this.groupBox3);
            this.mHardwarePage.Controls.Add(this.groupBox4);
            this.mHardwarePage.Controls.Add(this.groupBox1);
            this.mHardwarePage.Location = new System.Drawing.Point(4, 22);
            this.mHardwarePage.Name = "mHardwarePage";
            this.mHardwarePage.Padding = new System.Windows.Forms.Padding(3);
            this.mHardwarePage.Size = new System.Drawing.Size(914, 535);
            this.mHardwarePage.TabIndex = 0;
            this.mHardwarePage.Text = "硬件";
            this.mHardwarePage.UseVisualStyleBackColor = true;
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.mMacAddress);
            this.groupBox7.Controls.Add(this.label13);
            this.groupBox7.Controls.Add(this.label12);
            this.groupBox7.Controls.Add(this.mNetworkCardInfos);
            this.groupBox7.Location = new System.Drawing.Point(498, 230);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(397, 138);
            this.groupBox7.TabIndex = 3;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "网络设备";
            // 
            // mMacAddress
            // 
            this.mMacAddress.Location = new System.Drawing.Point(90, 102);
            this.mMacAddress.Name = "mMacAddress";
            this.mMacAddress.ReadOnly = true;
            this.mMacAddress.Size = new System.Drawing.Size(255, 20);
            this.mMacAddress.TabIndex = 3;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(29, 102);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(60, 13);
            this.label13.TabIndex = 2;
            this.label13.Text = "MAC 地址:";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(29, 53);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(34, 13);
            this.label12.TabIndex = 1;
            this.label12.Text = "网卡:";
            // 
            // mNetworkCardInfos
            // 
            this.mNetworkCardInfos.FormattingEnabled = true;
            this.mNetworkCardInfos.Location = new System.Drawing.Point(90, 46);
            this.mNetworkCardInfos.Name = "mNetworkCardInfos";
            this.mNetworkCardInfos.Size = new System.Drawing.Size(255, 21);
            this.mNetworkCardInfos.TabIndex = 0;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.mGraphicsCardName);
            this.groupBox5.Controls.Add(this.label10);
            this.groupBox5.Location = new System.Drawing.Point(498, 6);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(397, 64);
            this.groupBox5.TabIndex = 2;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "显卡";
            // 
            // mGraphicsCardName
            // 
            this.mGraphicsCardName.Location = new System.Drawing.Point(90, 31);
            this.mGraphicsCardName.Name = "mGraphicsCardName";
            this.mGraphicsCardName.ReadOnly = true;
            this.mGraphicsCardName.Size = new System.Drawing.Size(255, 20);
            this.mGraphicsCardName.TabIndex = 2;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(26, 34);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(34, 13);
            this.label10.TabIndex = 2;
            this.label10.Text = "型号:";
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.mSoundCardName);
            this.groupBox6.Controls.Add(this.label11);
            this.groupBox6.Location = new System.Drawing.Point(498, 96);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(397, 100);
            this.groupBox6.TabIndex = 2;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "音频";
            // 
            // mSoundCardName
            // 
            this.mSoundCardName.Location = new System.Drawing.Point(90, 37);
            this.mSoundCardName.Name = "mSoundCardName";
            this.mSoundCardName.ReadOnly = true;
            this.mSoundCardName.Size = new System.Drawing.Size(255, 20);
            this.mSoundCardName.TabIndex = 3;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(29, 44);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(43, 13);
            this.label11.TabIndex = 0;
            this.label11.Text = "型号：";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.mMotherBoardName);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Location = new System.Drawing.Point(6, 96);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(433, 96);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "主板";
            // 
            // mMotherBoardName
            // 
            this.mMotherBoardName.Location = new System.Drawing.Point(58, 37);
            this.mMotherBoardName.Name = "mMotherBoardName";
            this.mMotherBoardName.ReadOnly = true;
            this.mMotherBoardName.Size = new System.Drawing.Size(255, 20);
            this.mMotherBoardName.TabIndex = 2;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 44);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "芯片组：";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.mMemorySize);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this.mMemoryInfos);
            this.groupBox3.Controls.Add(this.mTotalMemorySize);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Location = new System.Drawing.Point(6, 230);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(433, 138);
            this.groupBox3.TabIndex = 1;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "内存";
            // 
            // mMemorySize
            // 
            this.mMemorySize.Location = new System.Drawing.Point(82, 102);
            this.mMemorySize.Name = "mMemorySize";
            this.mMemorySize.ReadOnly = true;
            this.mMemorySize.Size = new System.Drawing.Size(100, 20);
            this.mMemorySize.TabIndex = 6;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 110);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(43, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "大小：";
            // 
            // mMemoryInfos
            // 
            this.mMemoryInfos.FormattingEnabled = true;
            this.mMemoryInfos.Location = new System.Drawing.Point(12, 65);
            this.mMemoryInfos.Name = "mMemoryInfos";
            this.mMemoryInfos.Size = new System.Drawing.Size(325, 21);
            this.mMemoryInfos.TabIndex = 4;
            // 
            // mTotalMemorySize
            // 
            this.mTotalMemorySize.Location = new System.Drawing.Point(82, 13);
            this.mTotalMemorySize.Name = "mTotalMemorySize";
            this.mTotalMemorySize.ReadOnly = true;
            this.mTotalMemorySize.Size = new System.Drawing.Size(106, 20);
            this.mTotalMemorySize.TabIndex = 3;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 20);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(67, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "内存大小：";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.mPartitionSize);
            this.groupBox4.Controls.Add(this.label9);
            this.groupBox4.Controls.Add(this.label8);
            this.groupBox4.Controls.Add(this.mPartitionName);
            this.groupBox4.Controls.Add(this.label7);
            this.groupBox4.Controls.Add(this.label6);
            this.groupBox4.Controls.Add(this.mPartitionInfos);
            this.groupBox4.Controls.Add(this.mTotalHarddiskSize);
            this.groupBox4.Controls.Add(this.label5);
            this.groupBox4.Location = new System.Drawing.Point(6, 384);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(433, 145);
            this.groupBox4.TabIndex = 1;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "硬盘";
            // 
            // mPartitionSize
            // 
            this.mPartitionSize.Location = new System.Drawing.Point(306, 86);
            this.mPartitionSize.Name = "mPartitionSize";
            this.mPartitionSize.ReadOnly = true;
            this.mPartitionSize.Size = new System.Drawing.Size(100, 20);
            this.mPartitionSize.TabIndex = 13;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(232, 86);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(67, 13);
            this.label9.TabIndex = 12;
            this.label9.Text = "分区大小：";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(229, 92);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(0, 13);
            this.label8.TabIndex = 11;
            // 
            // mPartitionName
            // 
            this.mPartitionName.Location = new System.Drawing.Point(92, 86);
            this.mPartitionName.Name = "mPartitionName";
            this.mPartitionName.ReadOnly = true;
            this.mPartitionName.Size = new System.Drawing.Size(96, 20);
            this.mPartitionName.TabIndex = 10;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(15, 86);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(67, 13);
            this.label7.TabIndex = 9;
            this.label7.Text = "分区名称：";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(15, 46);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(43, 13);
            this.label6.TabIndex = 8;
            this.label6.Text = "分区：";
            // 
            // mPartitionInfos
            // 
            this.mPartitionInfos.FormattingEnabled = true;
            this.mPartitionInfos.Location = new System.Drawing.Point(89, 46);
            this.mPartitionInfos.Name = "mPartitionInfos";
            this.mPartitionInfos.Size = new System.Drawing.Size(100, 21);
            this.mPartitionInfos.TabIndex = 7;
            // 
            // mTotalHarddiskSize
            // 
            this.mTotalHarddiskSize.Location = new System.Drawing.Point(89, 20);
            this.mTotalHarddiskSize.Name = "mTotalHarddiskSize";
            this.mTotalHarddiskSize.ReadOnly = true;
            this.mTotalHarddiskSize.Size = new System.Drawing.Size(100, 20);
            this.mTotalHarddiskSize.TabIndex = 1;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(15, 20);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(67, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "硬盘大小：";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.mCPUName);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(6, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(433, 64);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "CPU";
            // 
            // mCPUName
            // 
            this.mCPUName.Location = new System.Drawing.Point(58, 27);
            this.mCPUName.Name = "mCPUName";
            this.mCPUName.ReadOnly = true;
            this.mCPUName.Size = new System.Drawing.Size(255, 20);
            this.mCPUName.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 27);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "型号:";
            // 
            // mOperatingSystemPage
            // 
            this.mOperatingSystemPage.Controls.Add(this.groupBox9);
            this.mOperatingSystemPage.Controls.Add(this.groupBox8);
            this.mOperatingSystemPage.Location = new System.Drawing.Point(4, 22);
            this.mOperatingSystemPage.Name = "mOperatingSystemPage";
            this.mOperatingSystemPage.Padding = new System.Windows.Forms.Padding(3);
            this.mOperatingSystemPage.Size = new System.Drawing.Size(914, 535);
            this.mOperatingSystemPage.TabIndex = 1;
            this.mOperatingSystemPage.Text = "操作系统";
            this.mOperatingSystemPage.UseVisualStyleBackColor = true;
            // 
            // groupBox9
            // 
            this.groupBox9.Controls.Add(this.textBox1);
            this.groupBox9.Controls.Add(this.label14);
            this.groupBox9.Location = new System.Drawing.Point(28, 24);
            this.groupBox9.Name = "groupBox9";
            this.groupBox9.Size = new System.Drawing.Size(751, 82);
            this.groupBox9.TabIndex = 1;
            this.groupBox9.TabStop = false;
            this.groupBox9.Text = "操作系统";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(94, 36);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(213, 20);
            this.textBox1.TabIndex = 1;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(28, 36);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(43, 13);
            this.label14.TabIndex = 0;
            this.label14.Text = "名称：";
            // 
            // groupBox8
            // 
            this.groupBox8.Controls.Add(this.dataGridView1);
            this.groupBox8.Location = new System.Drawing.Point(28, 124);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(871, 391);
            this.groupBox8.TabIndex = 0;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "进程";
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowUserToOrderColumns = true;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.mProcessName,
            this.mCPU,
            this.mMemory});
            this.dataGridView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView1.Location = new System.Drawing.Point(3, 16);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(865, 372);
            this.dataGridView1.TabIndex = 0;
            // 
            // mProcessName
            // 
            this.mProcessName.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.mProcessName.HeaderText = " 进程名";
            this.mProcessName.Name = "mProcessName";
            // 
            // mCPU
            // 
            this.mCPU.HeaderText = "CPU";
            this.mCPU.Name = "mCPU";
            // 
            // mMemory
            // 
            this.mMemory.HeaderText = "内存";
            this.mMemory.Name = "mMemory";
            // 
            // SystemMonitor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(938, 578);
            this.Controls.Add(this.mTabControl);
            this.Name = "SystemMonitor";
            this.Text = "Monitor";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.SystemMonitor_FormClosed);
            this.mTabControl.ResumeLayout(false);
            this.mHardwarePage.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.mOperatingSystemPage.ResumeLayout(false);
            this.groupBox9.ResumeLayout(false);
            this.groupBox9.PerformLayout();
            this.groupBox8.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.ResumeLayout(false);

        }
 
        #endregion

        private System.Windows.Forms.TabControl mTabControl;
        private System.Windows.Forms.TabPage mHardwarePage;
        private System.Windows.Forms.TabPage mOperatingSystemPage;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox mMotherBoardName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox mCPUName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox mMemorySize;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox mMemoryInfos;
        private System.Windows.Forms.TextBox mTotalMemorySize;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox mPartitionName;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox mPartitionInfos;
        private System.Windows.Forms.TextBox mTotalHarddiskSize;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.TextBox mMacAddress;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox mNetworkCardInfos;
        private System.Windows.Forms.TextBox mGraphicsCardName;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox mSoundCardName;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox mPartitionSize;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.GroupBox groupBox9;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.DataGridViewTextBoxColumn mProcessName;
        private System.Windows.Forms.DataGridViewTextBoxColumn mCPU;
        private System.Windows.Forms.DataGridViewTextBoxColumn mMemory;

    }
}