using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;

namespace SystemMonitorClientUI
{
    public partial class SystemMonitor : Form
    {
        public struct HardwareInfo
        {
            public string mCompanyName;
            public string mProductName;
        }

        public struct CPUInfo
        {
            public string mCompanyName;
            public string mProductName;
        }

        public struct GraphicsCardInfo
        {
            public string mCompanyName;
            public string mProductName;
        }

        public struct HardDiskInfo
        {
            public class PartitionInfo
            {
                public string mPartitionName;
                public string mPartitionVolumneNamel;
                public float mPartitionSize;
                public float mPartitionFreespace;
            }

            public string mCompanyName;
            public string mProductName;

            public float mTotalSize;
            public float mTotalFreespace;
            public uint mPartitionCount;
            public IntPtr mPartitionInfos;
        }

        public struct MemoryInfo
        {
            public string mCompanyName;
            public string mProductName;

            public uint mTotalSize;
        }

        public struct MotherBoardInfo
        {
            public string mCompanyName;
            public string mProductName;
        }

        public struct NetworkCardInfo
        {
            public string mCompanyName;
            public string mProductName;

            public string mMacAddress;
        }

        public struct SoundCardInfo
        {
            public string mCompanyName;
            public string mProductName;
        }

        public struct ProcessInfo
        {
            public string mProcessName;
            public uint mCurrentCPUUsage;
            public uint mCurrentMemoryUsage;
            public uint mPID;
        }

        public struct OperatingSystemInfo
        {
            public string mOSName;
            public List<ProcessInfo> mProcessInfos;
        }

        public struct ComputerInfo
        {
            CPUInfo                     mCPUInfo;
            HardDiskInfo                mHardDiskInfo;
            MotherBoardInfo             mMotherBoardInfo;
            GraphicsCardInfo            mGraphicsCardInfo;
            List<MemoryInfo>            mMemoryInfos;
            List<SoundCardInfo>         mSoundCardInfos;
            List<NetworkCardInfo>       mNetworkCardInfos;
            OperatingSystemInfo         mOperatingSystemInfo;
        }

        static public class SystemMonitorClientDLL
        {
            [DllImport("SystemMonitorClient.dll")]
            public static extern int ProcessComputerInfo();

            [DllImport("SystemMonitorClient.dll")]
            public static extern ComputerInfo GetComputerInfo();
        }

        public void Listen()
        {
            SystemMonitorClientDLL.ProcessComputerInfo();
        }

        public SystemMonitor()
        {
            InitializeComponent();
            //mListenThread = new Thread(new ThreadStart(Listen));
            //mListenThread.Start();
            HardDiskInfo h = new HardDiskInfo();
            int a = Marshal.SizeOf(h);
            MessageBox.Show(a.ToString());
        }

        private void SystemMonitor_FormClosed(object sender, FormClosedEventArgs e)
        {
            //mListenThread.Abort();
        }

        public Thread mListenThread;
        public ComputerInfo mComputerInfo;
    }
}
