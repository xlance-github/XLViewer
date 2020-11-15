using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace XLanceDLL
{
    public partial class ShowProcess : Form
    {
        public ManualResetEvent m_TreeViewUpdateFinishedEvent;
        public ShowProcess()
        {
            InitializeComponent();
            m_TreeViewUpdateFinishedEvent = new ManualResetEvent(false);
        }

        public void SetRange(int iMax)
        {
            if (prgBarLoading.InvokeRequired)
            {
                prgBarLoading.BeginInvoke(new Action(() =>
                   {
                       SetIndeterminate(false);
                       prgBarLoading.Minimum = 0;
                       prgBarLoading.Maximum = iMax;
                   }));
            }
            else
            {
                SetIndeterminate(false);
                prgBarLoading.Minimum = 0;
                prgBarLoading.Maximum = iMax;
            }
        }

        public void SetIndeterminate(bool isIndeterminate)
        {
            if (prgBarLoading.InvokeRequired)
            {
                prgBarLoading.BeginInvoke(new Action(() =>
                {
                    if (isIndeterminate)
                        prgBarLoading.Style = ProgressBarStyle.Marquee;
                    else
                        prgBarLoading.Style = ProgressBarStyle.Blocks;
                }
                ));
            }
            else
            {
                if (isIndeterminate)
                    prgBarLoading.Style = ProgressBarStyle.Marquee;
                else
                    prgBarLoading.Style = ProgressBarStyle.Blocks;
            }
        }

        public void UpdateProgress(int progress)
        {
            if (prgBarLoading.InvokeRequired)
                prgBarLoading.BeginInvoke(new Action(() => prgBarLoading.Value = progress));
            else
                prgBarLoading.Value = progress;

        }
    }
}
