using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace XLanceDLL
{
    public enum DIALOGTYPE
    {
        CREATEFOLDER = 0,
        RENAMEITEM = 1
    };
    public partial class AddDirectory : Form
    {
        public DIALOGTYPE m_eDialogType = DIALOGTYPE.CREATEFOLDER;
        public string strDirname = "";
        public AddDirectory()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (textBoxDirName.Text == "")
            {
                if (m_eDialogType == DIALOGTYPE.RENAMEITEM)
                {
                    MessageBox.Show("Please enter new name");
                    return; 
                }
                MessageBox.Show("Please enter the directory name");
                return;
            }
            strDirname = textBoxDirName.Text;
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void AddDirectory_Load(object sender, EventArgs e)
        {
            if (m_eDialogType == DIALOGTYPE.RENAMEITEM)
            {
                label1.Text = "Please enter new name";
                button1.Text = "&Rename";
                textBoxDirName.Text = strDirname;
            }
        }
    }
}
