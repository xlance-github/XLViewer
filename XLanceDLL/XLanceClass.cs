using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace XLanceDLL
{
    [ComVisible(true)]
    [Guid("CBA208F2-E43B-4958-97C7-C24EA5A213DE")]
    public interface IMyClass
    {
        string helloWorld();
        int AddIntegers(int iFirst, int iSecond);
        void CreateTabPages(IntPtr pIntFirstTabHnd);
        void ShowTab(int iPageIndex);
    }

    [ClassInterface(ClassInterfaceType.None)]
    [Guid("E72110F5-E4B7-42e6-94A8-5B1BA893E9C4")]
    [ComVisible(true)]
    public class XLanceClass : IMyClass
    {
        [DllImport("user32.dll", SetLastError = true)]
        static extern IntPtr SetParent(IntPtr child, IntPtr newParent);

        private ZeroPageForm    m_pZeroPageForm;
        private FirstPageForm   m_pFirstPageForm;
        private SecondPageForm  m_pSecondPageForm;
        private ThirdPageForm   m_pThirdPageForm;        
        private IntPtr          m_pIntTabHwnd;

        public XLanceClass()
        {
            //constructor.. create object..
        }
        public string helloWorld()
        {
            string strReturn = "Hello World... I am from C# dll";
            return strReturn;
        }
        public int AddIntegers(int iFirst, int iSecond)
        {
            return iFirst + iSecond;
        }
        public void CreateTabPages(IntPtr pIntFirstTabHnd)
        {
            m_pIntTabHwnd = pIntFirstTabHnd;

            m_pZeroPageForm = new ZeroPageForm();
            SetParent(m_pZeroPageForm.Handle, pIntFirstTabHnd);
            m_pZeroPageForm.Show();
            m_pZeroPageForm.Visible = false;

            m_pFirstPageForm = new FirstPageForm();
            SetParent(m_pFirstPageForm.Handle, pIntFirstTabHnd);
            m_pFirstPageForm.Show();
            m_pFirstPageForm.Visible = false;

            m_pSecondPageForm = new SecondPageForm();
            SetParent(m_pSecondPageForm.Handle, pIntFirstTabHnd);
            m_pSecondPageForm.Show();
            m_pSecondPageForm.Visible = false;

            m_pThirdPageForm = new ThirdPageForm();
            SetParent(m_pThirdPageForm.Handle, pIntFirstTabHnd);
            m_pThirdPageForm.Show();
            m_pThirdPageForm.Visible = false;

        }
        public void ShowTab(int iPageIndex)
        {
            HideOtherPages();
            switch (iPageIndex)
            {
                case 0:
                    m_pZeroPageForm.Visible = true;
                    break;
                case 1:
                    m_pFirstPageForm.Visible = true;
                    break;
                case 2:
                    m_pSecondPageForm.Visible = true;
                    break;
                case 3:
                    m_pThirdPageForm.Visible = true;
                    break;
            }
        }
        void HideOtherPages()
        {
            m_pZeroPageForm.Visible = false;
            m_pFirstPageForm.Visible = false;
            m_pSecondPageForm.Visible = false;
            m_pThirdPageForm.Visible = false;
        }
    }
}
