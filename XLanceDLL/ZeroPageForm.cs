using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Windows.Forms.Design;
using System.Globalization;
using System.Threading;
using System.IO;
using System.Collections.Specialized;
using AdvancedDataGridView;

namespace XLanceDLL
{
    // enums for tree operation
    enum _ENUMPIPES
    {
        GETSELECTED = 100,
        GETSELECTEDCOUNT,
        CREATEVIRTUALFOLDER,
        DELETEVIRTUALFOLDER
    };

    // As this is the structure used for pipe communnication
    //if some change here need to update to xlvview.exe project also
    struct ValuePairs
    {
        public bool bFile;
        public bool bPage;
        public double FileSize;
        public int NoOfPages;
        public string PageSize;
        public string Orientation;
        public int ColorDepth;
        public string BookmarkName;
        public int BookMarkLevel;
    };

    //structure inside structure
    struct FileValuePairs
    {
        public string fileName;
        public ValuePairs valuePairs;
    };

    public partial class ZeroPageForm : System.Windows.Forms.Form
    {
        private Server pipeServer;
        private Client pipeClient;
        private Dictionary<string, ValuePairs> m_gridviewDetails;
        private TreeNode m_PresentNode;
        private TreeGridNode m_PresentTreeGridNode;
        private string[] m_strarray;
        public ManualResetEvent m_GetAllInformationEvent;
        int m_progressBarRange = 10;
        int m_progressBarPosition = 0;
        private bool m_bstop = false;
        Dictionary<int, Dictionary<string, string>> m_dictionaryReturn;
        ContextMenuStrip m = new ContextMenuStrip();

        int m_iGlobalTotalPages = 0;
        string m_dbGlobalTotalSize = "";
        static int iIndexCount = 0;
        bool m_isMultipleRowsSelected = false;
        

        #region Tree Grid Index
        int tcTreeStructureIndex = 0;
        int tcFileSizeIndex = 0;
        int tcNumberOfPagesIndex = 0;
        int tcPaperSizeIndex = 0;
        int tcOrientationIndex = 0;
        int tcColorDepthIndex = 0;
        int tcBookmarkNameIndex = 0;
        int tcBookmarkLevelIndex = 0;
        int tcBlankIndex = 0;
        int tcSelectIndex = 0;
        #endregion

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        public ZeroPageForm()
        {
            InitializeComponent();
            startPipeServices();
            m_GetAllInformationEvent = new ManualResetEvent(false);

            //add the color names in Items
            m.Items.Add("Red");
            m.Items.Add("Orange");
            m.Items.Add("Yellow");
            m.Items.Add("Green");
            m.Items.Add("Pink");
            m.Items.Add("Violet");
            m.Items.Add("Light Black");
            m.Items.Add("Clear");
            m.ItemClicked += new ToolStripItemClickedEventHandler(contexMenuuu_ItemClicked);
        }

        private void Form1_Load(object sender, System.EventArgs e)
        {
            m_gridviewDetails = new Dictionary<string, ValuePairs>();

            // load image strip
            this.imageStrip.ImageSize = new System.Drawing.Size(16, 16);
            this.imageStrip.TransparentColor = System.Drawing.Color.Magenta;
            this.imageStrip.ImageSize = new Size(16, 16);
            //treeViewColumns1.TreeView.ImageList = imageStrip;

            tcTreeStructureIndex = tgv.Columns["tcTreeStructure"].Index;
            tcSelectIndex = tgv.Columns["tcSelect"].Index;

            tcFileSizeIndex = tgv.Columns["tcFileSize"].Index;
            tcNumberOfPagesIndex = tgv.Columns["tcNumberOfPages"].Index;
            tcPaperSizeIndex = tgv.Columns["tcPaperSize"].Index;
            tcOrientationIndex = tgv.Columns["tcOrientation"].Index;
            tcColorDepthIndex = tgv.Columns["tcColorDepth"].Index;
            tcBookmarkNameIndex = tgv.Columns["tcBookmarkName"].Index;
            tcBookmarkLevelIndex = tgv.Columns["tcBookmarkLevel"].Index;
            tcBlankIndex = tgv.Columns["tcBlank"].Index;

            //CItextSharpExports.TestCreatePDF1();

            //this.Parent;
        }

        /// <summary>
        /// Moving the Tree node to top
        /// </summary>
        /// <param name="node"></param>
        public void MoveTop(TreeNode node)
        {
            try
            {
                TreeNode parent = node.Parent;
                TreeView view = node.TreeView;
                if (parent != null)
                {
                    int index = parent.Nodes.IndexOf(node);
                    if (index < parent.Nodes.Count)
                    {
                        parent.Nodes.RemoveAt(index);
                        parent.Nodes.Insert(0, node);
                    }
                }
                else if (view != null && view.Nodes.Contains(node)) //root node
                {
                    int index = view.Nodes.IndexOf(node);
                    if (index < view.Nodes.Count)
                    {
                        view.Nodes.RemoveAt(index);
                        view.Nodes.Insert(0, node);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// moving treeiew item to bottom
        /// </summary>
        /// <param name="node"></param>
        public void MoveBottom(TreeNode node)
        {
            try
            {
                TreeNode parent = node.Parent;
                TreeView view = node.TreeView;
                int iLastItemIndex = getLastNode(parent).Index;
                if (parent != null)
                {
                    int index = parent.Nodes.IndexOf(node);
                    if (index < parent.Nodes.Count)
                    {
                        parent.Nodes.RemoveAt(index);
                        parent.Nodes.Insert(iLastItemIndex + 1, node);
                    }
                }
                else if (view != null && view.Nodes.Contains(node)) //root node
                {
                    int index = view.Nodes.IndexOf(node);
                    if (index < view.Nodes.Count)
                    {
                        view.Nodes.RemoveAt(index);
                        view.Nodes.Insert(iLastItemIndex + 1, node);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// moving treeview item to up 1 position
        /// if there is up position is parent it will not move
        /// </summary>
        /// <param name="node"></param>
        public void MoveUp(TreeNode node)
        {
            try
            {
                TreeNode parent = node.Parent;
                TreeView view = node.TreeView;
                if (parent != null)
                {
                    int index = parent.Nodes.IndexOf(node);
                    if (index < parent.Nodes.Count)
                    {
                        parent.Nodes.RemoveAt(index);
                        parent.Nodes.Insert(index - 1, node);
                    }
                }
                else if (view != null && view.Nodes.Contains(node)) //root node
                {
                    int index = view.Nodes.IndexOf(node);
                    if (index < view.Nodes.Count)
                    {
                        view.Nodes.RemoveAt(index);
                        view.Nodes.Insert(index - 1, node);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// move treeview item 1 position down
        /// if there is item which is siblings then will not move
        /// </summary>
        /// <param name="node"></param>
        public void MoveDown(TreeNode node)
        {
            try
            {
                TreeNode parent = node.Parent;
                TreeView view = node.TreeView;
                if (parent != null)
                {
                    int index = parent.Nodes.IndexOf(node);
                    if (index < parent.Nodes.Count)
                    {
                        parent.Nodes.RemoveAt(index);
                        parent.Nodes.Insert(index + 1, node);
                    }
                }
                else if (view != null && view.Nodes.Contains(node)) //root node
                {
                    int index = view.Nodes.IndexOf(node);
                    if (index < view.Nodes.Count)
                    {
                        view.Nodes.RemoveAt(index);
                        view.Nodes.Insert(index + 1, node);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// tree view right click Menu popup
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void treeListView1_MouseClick(object sender, MouseEventArgs e)
        {
            try
            {
                if (e.Button == MouseButtons.Right)
                {
                    ContextMenuStrip m = new ContextMenuStrip();
                    m.Items.Add("Red", imageList2.Images[0]);
                    m.Items.Add("Orange", imageList2.Images[1]);
                    m.Items.Add("Yellow", imageList2.Images[2]);
                    m.Items.Add("Green", imageList2.Images[3]);
                    m.Items.Add("Pink", imageList2.Images[4]);
                    m.Items.Add("Violet", imageList2.Images[5]);
                    m.Items.Add("Light Black", imageList2.Images[6]);
                    //m.Show(treeViewColumns1, new Point(e.X, e.Y));
                    //m.Show(tgv, new Point(e.X, e.Y));

                    m.ItemClicked += new ToolStripItemClickedEventHandler(contexMenuuu_ItemClicked);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in treeListView1_MouseClick " + ex.Message);
            }

        }

        /// <summary>
        /// When used clicked on menu item
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void contexMenuuu_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            try
            {
                ToolStripItem item = e.ClickedItem;
                Color bgColor = Color.Empty;
                switch (item.Text)
                {
                    case "Red":
                        bgColor = Color.Red;
                        break;
                    case "Orange":
                        bgColor = Color.Orange;
                        break;
                    case "Yellow":
                        bgColor = Color.Yellow;
                        break;
                    case "Green":
                        bgColor = Color.GreenYellow;
                        break;
                    case "Pink":
                        bgColor = Color.Pink;
                        break;
                    case "Violet":
                        bgColor = Color.Violet;
                        break;
                    case "Light Black":
                        bgColor = Color.BlanchedAlmond;
                        break;
                    case "Clear":
                        bgColor = Color.Empty;
                        break;
                }

                //int totalRows = treeViewColumns1.TreeView.Nodes.Count;
                //int idx = treeViewColumns1.TreeView.SelectedNode.Index;
                //if (idx == totalRows - 2)
                //    return;
                //treeViewColumns1.TreeView.SelectedNode.BackColor = bgColor;
                if (tgv.SelectedRows != null &&
                    tgv.SelectedRows.Count > 0)
                {
                    for (int iCount = 0; iCount < tgv.SelectedRows.Count; iCount++)
                    {
                        int totalRows = tgv.Rows.Count;
                        int idx = tgv.SelectedRows[0].Index;
                        //if (idx == totalRows - 2)
                        //    return;
                        tgv.SelectedRows[iCount].DefaultCellStyle.BackColor = bgColor;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in contexMenuuu_ItemClicked " + ex.Message);
            }
        }

        internal class AttachmentColumnHeader : DataGridViewColumnHeaderCell
        {
            public Image _image;
            public AttachmentColumnHeader(Image img)
                : base()
            {
                this._image = img;
            }
            protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates dataGridViewElementState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
            {
                base.Paint(graphics, clipBounds, cellBounds, rowIndex, dataGridViewElementState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
                graphics.DrawImage(_image, cellBounds.X + 4, cellBounds.Y + 2);
            }
            protected override object GetValue(int rowIndex)
            {
                return null;
            }
        }

        /// <summary>
        /// Get the PDF information from selected path's
        /// </summary>
        /// <param name="strItems"></param>
        void GetAllInformationNeeded(string[] strItems)
        {
            try
            {
                for (int iCount = 0; iCount < strItems.Length; iCount++)
                {
                    string items = strItems[iCount].ToString();
                    
                    //if same item already present then no need to add


                    //ValuePairs szValuePaires = new ValuePairs();
                    //m_gridviewDetails.Add(items, szValuePaires);

                    StringCollection strFileItems = new StringCollection();

                    if (Directory.Exists(strItems[iCount].ToString()))
                    {
                        string[] strDirItems = Common.GetDirectories(strItems[iCount].ToString());

                        //for (int iIndex = 0; iIndex < strDirItems.Length; iIndex++)
                        //{
                        //    string dirItem = strDirItems[iIndex].ToString();

                        ValuePairs szValuePairesOne = new ValuePairs();
                        if (!m_gridviewDetails.ContainsKey(items))
                        {
                            m_gridviewDetails.Add(items, szValuePairesOne);
                        }
                        //}
                        strFileItems.AddRange(Common.GetFiles(items));

                        m_progressBarRange += strFileItems.Count + iCount;
                        m_progressBarPosition += 1;

                        //doing with recursion
                        GetAllInformationNeeded(strDirItems);
                    }
                    else if (File.Exists(items))
                    {
                        if (items.ToLower().Contains(".pdf"))
                        {
                            strFileItems.Add(items);
                        }

                    }
                    else
                    {
                        continue;
                    }
                    for (int iIndex = 0; iIndex < strFileItems.Count; iIndex++)
                    {
                        string fileItem = strFileItems[iIndex].ToString();

                        ValuePairs szValuePairesOne = new ValuePairs();
                        szValuePairesOne.bFile = true;

                        CItextSharpExports pdfExport = new CItextSharpExports(fileItem);
                        szValuePairesOne.FileSize = pdfExport.GetFileSize(fileItem);
                        int iPageCount = pdfExport.GetPageCount(fileItem);
                        szValuePairesOne.NoOfPages = iPageCount;
                        //Prashant
                        SendFileNames("Processing: " + fileItem);

                        if (!m_gridviewDetails.ContainsKey(fileItem))
                        {
                            m_gridviewDetails.Add(fileItem, szValuePairesOne);
                        }
                        if (iPageCount >= 1)
                        {
                            for (int iPageIndex = 1; iPageIndex <= szValuePairesOne.NoOfPages; iPageIndex++)
                            {
                                ValuePairs szPageValue = new ValuePairs();
                                szPageValue.bPage = true;

                                SendFileNames("Processing: " + fileItem + " Page " + iPageIndex.ToString());

                                double dbWidth = pdfExport.GetPageWidth(iPageIndex);
                                double dbHeight = pdfExport.GetPageHeight(iPageIndex); 

                                string paperSize = (dbWidth / 72).ToString("0.00") + " x "
                                    + (dbHeight / 72).ToString("0.00") + " inch";

                                szPageValue.PageSize = paperSize;
                                if (dbWidth > dbHeight)
                                    szPageValue.Orientation = "Landscape";
                                else
                                    szPageValue.Orientation = "Portrait";

                                szPageValue.ColorDepth = (int)pdfExport.GetColorMode();
                                string strItem = fileItem + "\\#@$" + (iPageIndex).ToString();

                                if (!m_gridviewDetails.ContainsKey(strItem))
                                {
                                    m_gridviewDetails.Add(strItem, szPageValue);
                                }
                            }
                        }
                        else if (iPageCount == -1)
                        {
                            ValuePairs szPageValue = new ValuePairs();
                            szPageValue.bPage = true;
                            string strItem = fileItem + "\\#@$" + pdfExport.m_strLastError;

                            if (!m_gridviewDetails.ContainsKey(strItem))
                            {
                                m_gridviewDetails.Add(strItem, szPageValue);
                            }
                        }
                        m_progressBarPosition += 1;
                    }
                    m_progressBarPosition += 1;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// Calculate the total number of pages, size here
        /// </summary>
        void GetTotalAll()
        {
            try
            {
                double dbTempFileSize = 0;
                m_iGlobalTotalPages = 0;
                for (int iIndex = 0; iIndex < m_gridviewDetails.Count; iIndex++)
                {
                    string items = m_gridviewDetails.ElementAt(iIndex).Key;
                    ValuePairs valuePairs = (ValuePairs)m_gridviewDetails.ElementAt(iIndex).Value;

                    //calculate total number of pages
                    m_iGlobalTotalPages += valuePairs.NoOfPages;
                    if (valuePairs.FileSize != null)
                        dbTempFileSize += Convert.ToDouble(valuePairs.FileSize.ToString());
                }

                double dbSizeinBytes = dbTempFileSize;
                dbTempFileSize /= 1024;//convert to KB
                double dbSizeinKB = dbTempFileSize;
                dbTempFileSize /= 1024;//convert to MB
                double dbSizeinMB = dbTempFileSize;

                string strTotlFileSize = "";
                if (dbSizeinKB < 1)
                {
                    strTotlFileSize = dbSizeinBytes.ToString("0.00");
                    strTotlFileSize += " " + "Bytes";
                }
                else if (dbSizeinMB < 1)
                {
                    strTotlFileSize = dbSizeinKB.ToString("0.00");
                    strTotlFileSize += " " + "KB";
                }
                else
                {
                    strTotlFileSize = dbSizeinMB.ToString("0.00");
                    strTotlFileSize += " " + "MB";
                }
                m_dbGlobalTotalSize = strTotlFileSize;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in GetTotalAll");
            }
        }

        /// <summary>
        /// Fill the gridview, by collected information
        /// </summary>
        void FillTreeGridView()
        {
            try
            {
                FillTreeGridViewNew();
                m_progressBarPosition = m_progressBarRange;
                Thread.Sleep(200);
                m_bstop = true;

                return;

                ////calculate total pages and total size here
                //GetTotalAll();

                //try
                //{
                //    int iTotalCount = m_gridviewDetails.Count;
                //    if (iTotalCount == 0)
                //    {
                //        m_bstop = true;
                //        m_progressBarPosition = m_progressBarRange;
                //        return;
                //    }

                //    double iTotalFileSize = CalCulateTotalFileSize();
                //    iTotalFileSize /= 1024;//convert to KB
                //    iTotalFileSize /= 1024;//convert to MB

                //    string strTotalFileSize = iTotalFileSize.ToString("0.00");
                //    if (iTotalFileSize == 0)
                //        strTotalFileSize = "";

                //    int iTotalPages = CalCulateTotalNumberofPages();

                //    string strTotalPages = iTotalPages.ToString();
                //    if (iTotalPages == 0)
                //        strTotalPages = "";

                //    TreeNode treeNode = new TreeNode("Untitled");
                //    treeNode.Tag = new string[] { m_dbGlobalTotalSize, 
                //        m_iGlobalTotalPages.ToString(), "", "", "", "", "", "", "" };
                //    //treeViewColumns1.TreeView.Nodes.Add(treeNode);

                //    for (int iIndex = 0; iIndex < iTotalCount; iIndex++)
                //    {
                //        string items = m_gridviewDetails.ElementAt(iIndex).Key;
                //        ValuePairs valuePairs = (ValuePairs)m_gridviewDetails.ElementAt(iIndex).Value;

                //        iTotalFileSize = 0;
                //        iTotalFileSize = valuePairs.FileSize;
                //        double dbSizeinBytes = iTotalFileSize;
                //        iTotalFileSize /= 1024;//convert to KB
                //        double dbSizeinKB = iTotalFileSize;
                //        iTotalFileSize /= 1024;//convert to MB
                //        double dbSizeinMB = iTotalFileSize;

                //        string strTotlFileSize = "";
                //        if (dbSizeinKB < 1)
                //        {
                //            strTotlFileSize = dbSizeinBytes.ToString("0.00");
                //            strTotlFileSize += " " + "Bytes";
                //        }
                //        else if (dbSizeinMB < 1)
                //        {
                //            strTotlFileSize = dbSizeinKB.ToString("0.00");
                //            strTotlFileSize += " " + "KB";
                //        }
                //        else
                //        {
                //            strTotlFileSize = dbSizeinMB.ToString("0.00");
                //            strTotlFileSize += " " + "MB";

                //        }

                //        if (iTotalFileSize == 0)
                //            strTotlFileSize = "";


                //        string strTotlPages = valuePairs.NoOfPages.ToString();
                //        if (strTotlPages == "0")
                //            strTotlPages = "";
                //        else if (valuePairs.NoOfPages == -1)
                //            strTotlPages = "0";

                //        string strLastHiddenValue = items;
                //        string strBookMarkName = items;

                //        string Orientation = "";
                //        if (valuePairs.Orientation != null)
                //            Orientation = valuePairs.Orientation;
                //        string ColorDepth = "";
                //        switch (valuePairs.ColorDepth)
                //        {
                //            case (int)PdfColorMode.Undefined:
                //                ColorDepth = "Undefined";
                //                break;
                //            case (int)PdfColorMode.Cmyk:
                //                ColorDepth = "CMYK";
                //                break;
                //            case (int)PdfColorMode.Rgb:
                //                ColorDepth = "RGB";
                //                break;
                //            default:
                //                ColorDepth = "Undefined";
                //                break;
                //        }

                //        string PageSize = "";
                //        if (valuePairs.PageSize != null)
                //        {
                //            if (valuePairs.PageSize.Length > 0)
                //                PageSize = valuePairs.PageSize;
                //        }
                //        TreeNode tempNode = treeViewColumns1.TreeView.Nodes[0];
                //        if (CheckIfISParentPresentInGrid(tempNode, items))
                //        {
                //            if (valuePairs.bPage)
                //            {
                //                int i = items.IndexOf("#@$");
                //                strBookMarkName = items.Substring(0, i - 1);
                //                items = items.Substring(i + 3);
                //                items = "Page <" + items + ">";
                //                strBookMarkName += "\\" + items;
                //            }
                //            tempNode = m_PresentNode;
                //        }

                //        string BookMarkName = "";
                //        if (valuePairs.bPage)
                //        {
                //            BookMarkName = strBookMarkName;
                //        }
                //        else if (valuePairs.bFile)
                //        {
                //            BookMarkName = items.ToString();
                //        }
                //        else
                //        {
                //            BookMarkName = items.ToString();
                //        }

                //        TreeNode node = new TreeNode(items.ToString());
                //        node.Tag = new string[] { strTotlFileSize, strTotlPages,
                //   PageSize, Orientation, ColorDepth, BookMarkName, "", "", strLastHiddenValue};

                //        tempNode.Nodes.Add(node);
                //        tempNode.Expand();

                //        if (valuePairs.bFile)
                //        {
                //            node.ImageIndex = 3;
                //            node.SelectedImageIndex = 3;
                //        }
                //        else if (valuePairs.bPage)
                //        {
                //            node.ImageIndex = 2;
                //            node.SelectedImageIndex = 2;
                //        }
                //        else
                //        {
                //            node.ImageIndex = 1;
                //            node.SelectedImageIndex = 1;
                //        }
                //        m_progressBarPosition += 1;
                //    }
                //    treeViewColumns1.TreeView.Nodes[0].Expand();

                //    m_progressBarPosition = m_progressBarRange;
                //    Thread.Sleep(200);
                //    m_bstop = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in FillTreeGridView " + ex.Message);
            }
        }

        private bool CheckIfISParentPresentInGrid(TreeNode tempNode, string items)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// fill the gridview by collected information(New function)
        /// </summary>
        void FillTreeGridViewNew()
        {
            //calculate total pages and total size here
            //tgv.Nodes.Clear();
 
            //Get Total calculation here
            GetTotalAll();

            //Check if the tree already contains the items then update the new items.
            if (tgv.Nodes.Count() > 0)
            {
                UpdateNewItemsInTreeGridView();
                UpdateTotalFileCount();
                return;
            }

            try
            {
                int iTotalCount = m_gridviewDetails.Count;
                if (iTotalCount == 0)
                {
                    m_bstop = true;
                    m_progressBarPosition = m_progressBarRange;
                    return;
                }

                double iTotalFileSize = CalCulateTotalFileSize();
                iTotalFileSize /= 1024;//convert to KB
                iTotalFileSize /= 1024;//convert to MB

                string strTotalFileSize = iTotalFileSize.ToString("0.00");
                if (iTotalFileSize == 0)
                    strTotalFileSize = "";

                int iTotalPages = CalCulateTotalNumberofPages();

                string strTotalPages = iTotalPages.ToString();
                if (iTotalPages == 0)
                    strTotalPages = "";

                tgv.ImageList = imageStrip;

                TreeGridNode treeNode = tgv.Nodes.Add("Untitled");
                treeNode.Tag = new string[] { m_dbGlobalTotalSize, 
                    m_iGlobalTotalPages.ToString(), "", "", "", "", "", "", "" };
                treeNode.Cells[tcFileSizeIndex].Value = m_dbGlobalTotalSize;
                treeNode.Cells[tcNumberOfPagesIndex].Value = m_iGlobalTotalPages;
                treeNode.Cells[tcBookmarkLevelIndex].Value = treeNode.Level - 1;
                tgv.Nodes.Add(treeNode);

                for (int iIndex = 0; iIndex < iTotalCount; iIndex++)
                {
                    string items = m_gridviewDetails.ElementAt(iIndex).Key;
                    //Prashant
                    //SendFileNames(items);

                    ValuePairs valuePairs = (ValuePairs)m_gridviewDetails.ElementAt(iIndex).Value;

                    iTotalFileSize = 0;
                    iTotalFileSize = valuePairs.FileSize;
                    double dbSizeinBytes = iTotalFileSize;
                    iTotalFileSize /= 1024;//convert to KB
                    double dbSizeinKB = iTotalFileSize;
                    iTotalFileSize /= 1024;//convert to MB
                    double dbSizeinMB = iTotalFileSize;

                    string strTotlFileSize = "";
                    if (dbSizeinKB < 1)
                    {
                        strTotlFileSize = dbSizeinBytes.ToString("0.00");
                        strTotlFileSize += " " + "Bytes";
                    }
                    else if (dbSizeinMB < 1)
                    {
                        strTotlFileSize = dbSizeinKB.ToString("0.00");
                        strTotlFileSize += " " + "KB";
                    }
                    else
                    {
                        strTotlFileSize = dbSizeinMB.ToString("0.00");
                        strTotlFileSize += " " + "MB";
                    }

                    if (iTotalFileSize == 0)
                        strTotlFileSize = "";

                    string strTotlPages = valuePairs.NoOfPages.ToString();
                    if (strTotlPages == "0")
                        strTotlPages = "";
                    else if (valuePairs.NoOfPages == -1)
                        strTotlPages = "0";

                    string strLastHiddenValue = items;
                    string strBookMarkName = items;
                    
                    string Orientation = "";
                    if (valuePairs.Orientation != null)
                        Orientation = valuePairs.Orientation;
                    string ColorDepth = "";
                    //switch (valuePairs.ColorDepth)
                    //{
                    //    case (int)PdfColorMode.Undefined:
                    //        ColorDepth = "Undefined";
                    //        break;
                    //    case (int)PdfColorMode.Cmyk:
                    //        ColorDepth = "CMYK";
                    //        break;
                    //    case (int)PdfColorMode.Rgb:
                    //        ColorDepth = "RGB";
                    //        break;
                    //    default:
                    //        ColorDepth = "Undefined";
                    //        break;
                    //}

                    string PageSize = "";
                    if (valuePairs.PageSize != null)
                    {
                        if (valuePairs.PageSize.Length > 0)
                            PageSize = valuePairs.PageSize;
                    }
                    TreeGridNode tempNode = treeNode;
                    if (CheckIfISParentPresentInGrid(treeNode, items))
                    {
                        if (valuePairs.bPage)
                        {
                            int i = items.IndexOf("#@$");
                            strBookMarkName = items.Substring(0, i - 1);
                            items = items.Substring(i + 3);
                            items = "Page <" + items + ">";
                            strBookMarkName += "\\" + items;
                        }
                        tempNode = m_PresentTreeGridNode;
                    }

                    string BookMarkName = "";
                    if (valuePairs.bPage)
                    {
                        BookMarkName = strBookMarkName;
                    }
                    else if (valuePairs.bFile)
                    {
                        BookMarkName = items.ToString();
                    }
                    else
                    {
                        BookMarkName = items.ToString();
                    }

                    TreeGridNode node = tempNode.Nodes.Add(items.ToString());
                    node.Tag = new string[] { strTotlFileSize, strTotlPages,
               PageSize, Orientation, ColorDepth, BookMarkName, "", "", strLastHiddenValue};
                    node.Cells[tcFileSizeIndex].Value = strTotlFileSize;
                    node.Cells[tcNumberOfPagesIndex].Value = strTotlPages;
                    node.Cells[tcPaperSizeIndex].Value = PageSize;
                    node.Cells[tcOrientationIndex].Value = Orientation;
                    node.Cells[tcColorDepthIndex].Value = ColorDepth;
                    node.Cells[tcBookmarkNameIndex].Value = BookMarkName;
                    node.Cells[tcBookmarkLevelIndex].Value = node.Level - 1;

                    if (valuePairs.bFile)
                    {
                        node.ImageIndex = 3;
                    }
                    else if (valuePairs.bPage)
                    {
                        node.ImageIndex = 2;
                    }
                    else
                    {
                        node.ImageIndex = 1;
                    }
                    m_progressBarPosition += 1;
                }
                ExpandNode(tgv.Nodes[0]);
                m_progressBarPosition = m_progressBarRange;
                Thread.Sleep(200);
                m_bstop = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in FillTreeGridView " + ex.Message);
            }
        }

        /// <summary>
        /// Expand the items in the treeview
        /// </summary>
        /// <param name="tgn"></param>
        private void ExpandNode(TreeGridNode tgn)
        {
            if (tgn != null)
            {
                if (tgn.Level != 2 )
                {
                    tgn.Expand();
                    for (int i = 0; i < tgn.Nodes.Count; i++)
                    {
                        ExpandNode(tgn.Nodes[i]);
                    }
                }
            }
        }

        /// <summary>
        /// Check the item is already present in the grid, will return as true or false.
        /// </summary>
        /// <param name="CheckNode"></param>
        /// <param name="strItemString"></param>
        /// <returns></returns>
        bool CheckIfISPresentInGrid(
            TreeNode CheckNode,
            string strItemString)
        {
            bool bReturn = false;
            try
            {
                foreach (TreeNode tn in CheckNode.Nodes)
                {
                    int i = strItemString.LastIndexOf('\\');

                    string tempParent = strItemString.Substring(0, i);

                    if (tn.Text == tempParent)
                    {
                        m_PresentNode = tn;
                        bReturn = true;
                        break;
                    }
                    else
                    {
                        if (CheckIfISPresentInGrid(tn, strItemString))
                        {
                            bReturn = true;
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in CheckIfISPresentInGrid " + ex.Message);
            }
            return bReturn;

            ////check if the value from textBox1 is existed in dataGridView1:
            //for (int iIndex = 0; iIndex < CheckNode.Nodes.Count; iIndex++)
            //{
            //    int i = strItemString.LastIndexOf('\\');

            //    string tempParent = strItemString.Substring(0, i);

            //    if (CheckNode.Nodes.ContainsKey(tempParent))                    
            //    {
            //        m_PresentNode = CheckNode.Nodes[iIndex];
            //        bReturn = true;
            //        break;
            //    }
            //    else if (CheckNode.Nodes[iIndex]. HasChildren)
            //    {
            //        if (CheckIfISPresentInGrid(CheckNode.Nodes[iIndex], strItemString))
            //        {
            //            bReturn = true;
            //            break;
            //        }
            //    }
            //}
            //return bReturn;
        }

        /// <summary>
        /// will check is the parent item of selcted item is present in the grid
        /// </summary>
        /// <param name="CheckNode"></param>
        /// <param name="strItemString"></param>
        /// <returns></returns>
        bool CheckIfISParentPresentInGrid(
       TreeGridNode CheckNode,
       string strItemString)
        {
            bool bReturn = false;
            try
            {
                foreach (TreeGridNode tn in CheckNode.Nodes)
                {
                    int i = strItemString.LastIndexOf('\\');

                    string tempParent = strItemString.Substring(0, i);

                    if (tn.Cells[tcTreeStructureIndex].Value.ToString() == tempParent)
                    {
                        m_PresentTreeGridNode = tn;
                        bReturn = true;
                        break;
                    }
                    else
                    {
                        if (CheckIfISParentPresentInGrid(tn, strItemString))
                        {
                            bReturn = true;
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in CheckIfISParentPresentInGrid " + ex.Message);
            }
            return bReturn;
        }

        /// <summary>
        /// will check for is the page is present in the grid.
        /// </summary>
        /// <param name="CheckNode"></param>
        /// <param name="strItemString"></param>
        /// <returns></returns>
        bool CheckIfISPresentInGrid(
       TreeGridNode CheckNode,
       string strItemString)
        {
            bool bReturn = false;
            try
            {
                foreach (TreeGridNode tn in CheckNode.Nodes)
                {
                    string itemText = tn.Cells[tcTreeStructureIndex].Value.ToString();
                    if (itemText.Contains("Page <"))
                    {
                        TreeGridNode pTgn = (TreeGridNode)tn.Parent;
                        itemText = itemText.Replace("Page <", "");
                        itemText = itemText.Replace(">", "");
                        itemText = pTgn.Cells[tcTreeStructureIndex].Value.ToString() + "\\#@$" + itemText;
                    }

                    if (itemText == strItemString)
                    {
                        bReturn = true;
                        break;
                    }
                    else
                    {
                        if (CheckIfISPresentInGrid(tn, strItemString))
                        {
                            bReturn = true;
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in CheckIfISPresentInGrid " + ex.Message);
            }
            return bReturn;
        }

        /// <summary>
        /// will start the pipe server thread
        /// </summary>
        void startPipeServices()
        {
            try
            {
                this.pipeServer = new Server();

                this.pipeServer.MessageReceived +=
                    new Server.MessageReceivedHandler(pipeServer_MessageReceived);

                    this.pipeClient = new Client();
                //this.pipeClient.MessageReceived +=
                //    new Client.MessageReceivedHandler(pipeClient_MessageReceived);

                //start the pipe server if it's not already running
                if (!this.pipeServer.Running)
                {
                    this.pipeServer.PipeName = "\\\\.\\pipe\\UIServer";
                    this.pipeServer.Start();
                }
                else
                    MessageBox.Show("Server already running.");

                if (!this.pipeClient.Connected)
                {
                    this.pipeClient.PipeName = "\\\\.\\pipe\\UIClient";
                    this.pipeClient.Connect();
                }
                else
                    MessageBox.Show("Already connected.");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in startPipeServices " + ex.Message);
            }
        }

        /// <summary>
        /// will get called when client will send the data to server
        /// </summary>
        /// <param name="client"></param>
        /// <param name="bytesReceived"></param>
        void pipeServer_MessageReceived(Server.Client client, byte[] bytesReceived)
        {
            try
            {
                this.Invoke(new Server.MessageReceivedHandler(DisplayMessageReceived),
                    new object[] { client, bytesReceived });
            }
            catch (Exception ex)
            {
                //MessageBox.Show("Exception in pipeServer_MessageReceived " + ex.Message);
            }
        }

        /// <summary>
        /// will process the client data
        /// </summary>
        /// <param name="client"></param>
        /// <param name="bytesReceived"></param>
        void DisplayMessageReceived(Server.Client client, byte[] bytesReceived)
        {
            try
            {
                XLV_PIPE_DATA pipeData = new XLV_PIPE_DATA();

                int size = Marshal.SizeOf(pipeData);
                IntPtr ptr = Marshal.AllocHGlobal(size);
                Marshal.Copy(bytesReceived, 0, ptr, size);
                pipeData = (XLV_PIPE_DATA)Marshal.PtrToStructure(ptr, pipeData.GetType());
                Marshal.FreeHGlobal(ptr);

                if (pipeData.eMessageInfo == 100)
                {
                    string strInputString = System.Text.UTF8Encoding.UTF8.GetString(pipeData.bData).ToString();
                    strInputString.Replace("\0", string.Empty);
                    m_strarray = new string[1000];
                    Common cmnCode = new Common();
                    cmnCode.Tokenize(strInputString, ref m_strarray);
                    ProcessReceiedData();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in DisplayMessageReceived " + ex.Message);
            }
        }

        /// <summary>
        /// process received data from client
        /// </summary>
        void ProcessReceiedData()
        {
            try
            {
                if (m_strarray.Count() > 0)
                {
                    if (m_strarray[0].Length > 0)
                    {
                        //RemoveAllTreeViewItems();

                        ShowProcess progressDialog = new ShowProcess();

                        m_bstop = false;

                        // Initialize the thread that will handle the background process
                        //Thread IndeterminateThread = new Thread(
                        //    new ThreadStart(() =>
                        //    {
                        //        try
                        //        {
                        //            progressDialog.SetIndeterminate(false);
                        //            while (!m_bstop)
                        //            {
                        //                Thread.Sleep(50);
                        //                if (m_progressBarRange >= m_progressBarPosition)
                        //                {
                        //                    progressDialog.SetRange(m_progressBarRange);
                        //                    progressDialog.UpdateProgress(m_progressBarPosition);
                        //                }
                        //            }

                        //            // Close the dialog if it hasn't been already
                        //            if (progressDialog.InvokeRequired)
                        //                progressDialog.BeginInvoke(new Action(() => progressDialog.Close()));
                        //        }
                        //        catch (Exception ex)
                        //        {
                        //            MessageBox.Show("Exception in IndeterminateThread 1" + ex.Message);
                        //        }
                        //    }
                        //));

                        // Initialize the thread that will handle the background process
                        Thread GetAllInformationThread = new Thread(
                            new ThreadStart(() =>
                            {
                                //Collect all information here
                                m_GetAllInformationEvent.Reset();
                                GetAllInformationNeeded(m_strarray);
                                m_GetAllInformationEvent.Set();
                            }
                        ));

                        // Initialize the thread that will handle the background process
                        Thread FillGridViewThread = new Thread(
                            new ThreadStart(() =>
                            {
                                m_GetAllInformationEvent.WaitOne();
                                this.BeginInvoke(new Action(() => FillTreeGridView()));
                            }
                        ));

                        GetAllInformationThread.Start();
                        //IndeterminateThread.Start();
                        GetAllInformationThread.Join();
                        FillGridViewThread.Start();
                        try
                        {
                            //progressDialog.ShowDialog();
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show("Exception in ProcessReceiedData 1" + ex.Message);
                        }
                    }
                    else
                    {
                        MessageBox.Show("Please Select the File(s)/Folder(s) from File Manager", "Error", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in ProcessReceiedData 2" + ex.Message);
            }
        }

        /// <summary>
        /// To calculate the total number of pages
        /// </summary>
        /// <returns></returns>
        int CalCulateTotalNumberofPages()
        {
            int iTotal = 0;
            try
            {
                for (int jIndex = 0; jIndex < m_gridviewDetails.Count; jIndex++)
                {
                    ValuePairs values = (ValuePairs)m_gridviewDetails.ElementAt(jIndex).Value;
                    iTotal += values.NoOfPages;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in CalCulateTotalNumberofPages " + ex.Message);
            }
            return iTotal;
        }

        /// <summary>
        /// To calculate total size of all files
        /// </summary>
        /// <returns></returns>
        double CalCulateTotalFileSize()
        {
            double iTotal = 0;
            try
            {
                for (int jIndex = 0; jIndex < m_gridviewDetails.Count; jIndex++)
                {
                    ValuePairs values = (ValuePairs)m_gridviewDetails.ElementAt(jIndex).Value;
                    iTotal += values.FileSize;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in CalCulateTotalFileSize " + ex.Message);
            }
            return iTotal;
        }

        
        //void pipeClient_MessageReceived(byte[] bytesReceived)
        //{
        //    try
        //    {
        //        this.Invoke(new Client.MessageReceivedHandler(DisplayReceivedMessage),
        //            new object[] { bytesReceived });
        //    }
        //    catch (Exception ex)
        //    {
        //        MessageBox.Show("Exception in pipeClient_MessageReceived " + ex.Message);
        //    }
        //}

        /// <summary>
        /// buttom click event for button up
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button8_Click(object sender, EventArgs e)
        {
            MovePosition(true);
        }

        /// <summary>
        /// move the position of treeview item 
        /// </summary>
        /// <param name="upOrDown"></param>
        private void MovePosition(Boolean upOrDown)
        {
            if (tgv.SelectedRows != null &&
                tgv.SelectedRows.Count == 1)
            {
                TreeGridNode tgn = (TreeGridNode)tgv.SelectedRows[0];
                String selectedText = tgn.Cells[tcTreeStructureIndex].Value.ToString();

                TreeGridNode pTgn = (TreeGridNode)tgn.Parent;


                MovePositionUp(tgn, upOrDown);
                if (pTgn != null)
                {
                    for (int i = 0; i < pTgn.Nodes.Count; i++)
                    {
                        if (pTgn.Nodes[i].Cells[tcTreeStructureIndex].Value.ToString() == selectedText)
                        {
                            tgv.ClearSelection();
                            pTgn.Nodes[i].Selected = true;
                            break;
                        }
                    }
                }
            }
        }
        private void button7_Click(object sender, EventArgs e)
        {
            try
            {
                //TreeNode hTreeNode = this.treeViewColumns1.TreeView.SelectedNode;
                //MoveDown(hTreeNode);
                //this.treeViewColumns1.TreeView.SelectedNode = hTreeNode;
                MovePosition(false);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in button8_Click " + ex.Message);
            }
        }

        private void btnImportFiles_Click(object sender, EventArgs e)
        {
            this.tgv.MultiSelect = true; 
            XLV_PIPE_DATA pipedata = new XLV_PIPE_DATA();
            pipedata.eMessageInfo = (int)_ENUMPIPES.GETSELECTED;

            int size = Marshal.SizeOf(pipedata);
            byte[] bytes = new byte[size];
            try
            {
                IntPtr ptr = Marshal.AllocHGlobal(size);
                Marshal.StructureToPtr(pipedata, ptr, true);
                Marshal.Copy(ptr, bytes, 0, size);
                Marshal.FreeHGlobal(ptr);
            }
            finally
            {
            }
            this.pipeClient.SendMessage(bytes);
        }

        private void SendFileNames(string FileName)
        {
            XLV_PIPE_DATA pipedata = new XLV_PIPE_DATA();
            pipedata.eMessageInfo = 104;
            pipedata.bData = new byte[4000];
            if (FileName.Contains("#@"))
            {
                FileName = FileName.Substring(0, FileName.IndexOf("#@"));
            }
            Byte[] tempbytes = System.Text.Encoding.ASCII.GetBytes(FileName + "\t\t 100%\r\n");
            tempbytes.CopyTo(pipedata.bData, 0);
            int size = Marshal.SizeOf(pipedata);
            byte[] bytes = new byte[size];
            try
            {
                IntPtr ptr = Marshal.AllocHGlobal(size);
                Marshal.StructureToPtr(pipedata, ptr, true);
                Marshal.Copy(ptr, bytes, 0, size);
                Marshal.FreeHGlobal(ptr);
            }
            finally
            {

            }
            this.pipeClient.SendMessage(bytes);
        }

        private void CheckAndSetMultipleRowsSelected()
        {
            int SelectedRows = tgv.SelectedRows.Count;
            String selectedText = string.Empty;
            m_isMultipleRowsSelected = false;
            //Dictionary<int, TreeGridNode> mSelectedNodes = new Dictionary<int, TreeGridNode>();
            //List<string> SelectedNameList = new List<string>();

            for (int iCount = 0; iCount < SelectedRows; iCount++)
            {
                selectedText = ((TreeGridNode)tgv.SelectedRows[iCount]).Cells[0].Value.ToString();
                if (selectedText == "Untitled")
                    break;
                m_isMultipleRowsSelected = true;
                //mSelectedNodes.Add(iCount + 1, (TreeGridNode)tgv.SelectedRows[iCount]);
                //SelectedNameList.Add(selectedText);
            }
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            CheckAndSetMultipleRowsSelected();
            ProcessTree();
        }
        private void ProcessTree()
        {
            // Displays a SaveFileDialog so the user can save the Image
            // assigned to Button2.

            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.Filter = "PDF Files(*.pdf)|*.pdf";
            saveFileDialog1.Title = "Save an PDF File";
            saveFileDialog1.ShowDialog();

            if (tgv.Nodes.Count() == 0)
            {
                MessageBox.Show("Tree is empty, no PDF files are present to save", "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // If the file name is not an empty string open it for saving.
            if (saveFileDialog1.FileName != "")
            {
                TreeGridNodeCollection TreeNodes = tgv.Nodes[0].Nodes;
                //TreeNodeCollection TreeNodes = treeViewColumns1.TreeView.Nodes;
                m_dictionaryReturn = new Dictionary<int, Dictionary<string, string>>();
                Traverse(TreeNodes);
                if (m_dictionaryReturn.Count > 0)
                {
                    if (CItextSharpExports.SaveTreeStructureToPDF(m_dictionaryReturn, saveFileDialog1.FileName))
                    {
                        MessageBox.Show("File created successfully", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        MessageBox.Show("Error in File creation", "Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        private void GetDataTreeToStruct(TreeGridNodeCollection nodes)
        {
            try
            {
                foreach (TreeGridNode node in nodes)
                {
                    string str = node.Cells[tcTreeStructureIndex].Value.ToString();
                    string strBookmarkName = node.Cells[tcBookmarkNameIndex].Value.ToString();

                    ValuePairs szValuePairesOne = new ValuePairs();
                    szValuePairesOne.bPage = false;
                    if (str.Contains("Page <"))
                    {
                        TreeGridNode pTgn = (TreeGridNode)node.Parent;
                        str = str.Replace("Page <", "");
                        str = str.Replace(">", "");
                        str = pTgn.Cells[tcTreeStructureIndex].Value.ToString() + "\\#@$" + str;
                        szValuePairesOne.bPage = true;
                    }


                    szValuePairesOne.bFile = false;
                    if (File.Exists(str))
                    {
                        szValuePairesOne.bFile = true;
                    }
                    string strBookmarkLevel = node.Cells[tcBookmarkLevelIndex].Value.ToString();
                    szValuePairesOne.BookMarkLevel = 0;
                    if (strBookmarkLevel != "")
                    {
                        szValuePairesOne.BookMarkLevel = Convert.ToInt16(strBookmarkLevel);
                    }
                    szValuePairesOne.BookmarkName = strBookmarkName;

                    string strColorDepth = node.Cells[tcColorDepthIndex].Value.ToString();
                    szValuePairesOne.ColorDepth = 0;
                    if (strColorDepth != "")
                    {
                        szValuePairesOne.ColorDepth = Convert.ToInt16(strColorDepth);
                    }

                    string strFileSize = node.Cells[tcFileSizeIndex].Value.ToString();
                    szValuePairesOne.FileSize = 0;
                    double dbSizeinBytes = 0;
                    if (strFileSize.Contains("MB"))
                    {
                        strFileSize = strFileSize.Substring(0, strFileSize.Length - 3);
                        dbSizeinBytes = Convert.ToDouble(strFileSize);
                        dbSizeinBytes *= 1024;
                        dbSizeinBytes *= 1024;
                    }
                    else if (strFileSize.Contains("KB"))
                    {
                        strFileSize = strFileSize.Substring(0, strFileSize.Length - 3);
                        dbSizeinBytes = Convert.ToDouble(strFileSize);
                        dbSizeinBytes *= 1024;
                    }
                    else if (strFileSize.Contains("Bytes"))
                    {
                        strFileSize = strFileSize.Substring(0, strFileSize.Length - 6);
                        dbSizeinBytes = Convert.ToDouble(strFileSize);
                    }
                    szValuePairesOne.FileSize = dbSizeinBytes;

                    string strNumberOfPages = node.Cells[tcNumberOfPagesIndex].Value.ToString();
                    szValuePairesOne.NoOfPages = 0;
                    if (strNumberOfPages != "")
                    {
                        szValuePairesOne.NoOfPages = Convert.ToInt16(strNumberOfPages);
                    }

                    szValuePairesOne.Orientation = node.Cells[tcOrientationIndex].Value.ToString(); ;
                    szValuePairesOne.PageSize = node.Cells[tcNumberOfPagesIndex].Value.ToString();

                    m_gridviewDetails.Add(str, szValuePairesOne);

                    GetDataTreeToStruct(node.Nodes);
                }
            }
            catch (Exception ex)
            {

            }
        }


        private void Traverse(TreeGridNodeCollection nodes)
        {
            foreach (TreeGridNode node in nodes)
            {
                string str = node.Cells[tcTreeStructureIndex].Value.ToString();
                string strBookmarkName = node.Cells[tcBookmarkNameIndex].Value.ToString();
                if (str.Contains("Page <"))
                {
                    TreeGridNode pTgn = (TreeGridNode)node.Parent;
                    str = str.Replace("Page <", "");
                    str = str.Replace(">", "");
                    str = pTgn.Cells[tcTreeStructureIndex].Value.ToString() + "\\#@$" + str;
                }
                
                Dictionary<string, string> innerDictionary = new Dictionary<string, string>();
                int iCount = m_dictionaryReturn.Count;
                //string[] itemString = (string[])node.Tag;
                //string strBookmarkName = itemString.ElementAt(5);
                //string str = itemString.ElementAt(8);
                if (str.Length > 0)
                {
                    if (m_isMultipleRowsSelected)
                    {
                        if (node.Selected || node.Parent.Selected)
                        {
                            innerDictionary.Add(strBookmarkName, str);
                            m_dictionaryReturn.Add(iCount, innerDictionary);
                        }
                    }
                    else
                    {
                        innerDictionary.Add(strBookmarkName, str);
                        m_dictionaryReturn.Add(iCount, innerDictionary);
                    }
                }
                Traverse(node.Nodes);
            }
        }

        private void Traverse(TreeNodeCollection nodes)
        {
            foreach (TreeNode node in nodes)
            {
                Dictionary<string, string> innerDictionary = new Dictionary<string, string>();
                int iCount = m_dictionaryReturn.Count;
                string[] itemString = (string[])node.Tag;
                string strBookmarkName = itemString.ElementAt(5);
                string str = itemString.ElementAt(8);
                if (str.Length > 0)
                {
                    innerDictionary.Add(strBookmarkName, str);
                    m_dictionaryReturn.Add(iCount, innerDictionary);
                }
                Traverse(node.Nodes);
            }
        }

        private void buttonTop_Click(object sender, EventArgs e)
        {
            //TreeNode hTreeNode = this.treeViewColumns1.TreeView.SelectedNode;
            //MoveTop(hTreeNode);
            //this.treeViewColumns1.TreeView.SelectedNode = hTreeNode;
            //TreeNode hTreeNode = (this.treeViewColumns1.TreeView.SelectedNode).Parent.Nodes[0];// this.treeViewColumns1.TreeView.Nodes[0];
            //this.treeViewColumns1.TreeView.SelectedNode = hTreeNode;
            if (tgv.SelectedRows != null &&
               tgv.SelectedRows.Count == 1)
            {
                TreeGridNode tgn = (TreeGridNode)tgv.SelectedRows[0];
                while (!tgn.IsFirstSibling)
                {
                    MovePosition(true);
                    tgn = (TreeGridNode)tgv.SelectedRows[0];
                } 
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            //if (treeViewColumns1.TreeView.Nodes.Count == 0)
            //{
            //    MessageBox.Show("Tree is empty", "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Information);
            //}
            //TreeNode hTreeNode = this.treeViewColumns1.TreeView.SelectedNode;
            //MoveBottom(hTreeNode);
            //this.treeViewColumns1.TreeView.SelectedNode = hTreeNode;

            //TreeNode lastNode = getLastNode(this.treeViewColumns1.TreeView.SelectedNode.Parent);
            //this.treeViewColumns1.TreeView.SelectedNode = lastNode;
            if (tgv.SelectedRows != null &&
               tgv.SelectedRows.Count == 1)
            {
                TreeGridNode tgn = (TreeGridNode)tgv.SelectedRows[0];

                while (!tgn.IsLastSibling)
                {
                    MovePosition(false);
                    tgn = (TreeGridNode)tgv.SelectedRows[0];
                }
            }
        }

        private TreeNode getLastNode(TreeNode subroot)
        {
            if (subroot.Nodes.Count == 0)
                return subroot;

            return getLastNode(subroot.Nodes[subroot.Nodes.Count - 1]);
        }

        private void buttonTopTen_Click(object sender, EventArgs e)
        {
            buttonTopTen.Enabled = false;
            try
            {
                for (int i = 0; i < 10; i++)
                {
                    //TreeNode hTreeNode = this.treeViewColumns1.TreeView.SelectedNode;
                    //MoveUp(hTreeNode);
                    //this.treeViewColumns1.TreeView.SelectedNode = hTreeNode;
                    MovePosition(true);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in buttonTopTen_Click " + ex.Message);
            }
            buttonTopTen.Enabled = true;
        }

        private void buttonDownTen_Click(object sender, EventArgs e)
        {
            buttonDownTen.Enabled = false;
            try
            {
                for (int i = 0; i < 10; i++)
                {
                    //TreeNode hTreeNode = this.treeViewColumns1.TreeView.SelectedNode;
                    //MoveDown(hTreeNode);
                    //this.treeViewColumns1.TreeView.SelectedNode = hTreeNode;
                    MovePosition(false);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in buttonDownTen_Click " + ex.Message);
            }
            buttonDownTen.Enabled = true;
        }

        private void buttonDelete_Click(object sender, EventArgs e)
        {
            try
            {
               // if (tgv.SelectedRows != null &&
               //tgv.SelectedRows.Count == 1)
               // {
                //int SelectedRows = tgv.SelectedRows.Count;
                //Dictionary<int, TreeGridNode> mSelectedNodes = new Dictionary<int, TreeGridNode>();
                //String selectedText = string.Empty;

                //for (int iCount = 0; iCount < SelectedRows; iCount++)
                //{
                //    mSelectedNodes.Add(iCount+1, (TreeGridNode)tgv.SelectedRows[iCount]);
                //}

                //for (int iCount = 0; iCount < SelectedRows; iCount++)
                //{
                //    //TreeGridNode tgn = (TreeGridNode)tgv.SelectedRows[0];
                //    TreeGridNode tgn = mSelectedNodes[iCount + 1];
                //    selectedText = tgn.Cells[tcTreeStructureIndex].Value.ToString();
                //    TreeGridNode pTgn = (TreeGridNode)tgn.Parent;

                //    if (selectedText == "Untitled")
                //    {
                //        MessageBox.Show("Please Select item", "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Error);
                //        return;
                //    }

                //    //if (selectedText.Contains("Page <"))
                //    //{
                //    //    selectedText = pTgn.Cells[tcTreeStructureIndex].Value.ToString() + "\\#@$" + selectedText;
                //    //}

                //    //if (File.Exists(selectedText))
                //    //{
                //    //    RemoveAllPagesFromArray(selectedText);
                //    //}
                //    //m_gridviewDetails.Remove(selectedText);

                //    List<TreeGridNode> tgnList = new List<TreeGridNode>();
                //    if (pTgn != null)
                //    {
                //        for (int i = 0; i < pTgn.Nodes.Count; i++)
                //        {
                //            tgnList.Add(pTgn.Nodes[i]);
                //        }
                //        pTgn.Nodes.Clear();

                //        tgnList.Remove(tgn);

                //        for (int i = 0; i < tgnList.Count; i++)
                //        {
                //            AddNodeRec(pTgn, tgnList[i]);
                //        }
                //        TreeGridNodeCollection TreeNodes = tgv.Nodes[0].Nodes;
                //        m_gridviewDetails.Clear();
                //        GetDataTreeToStruct(TreeNodes);
                //        tgv.Nodes.Clear();
                //        FillTreeGridViewNew();
                //        //update the total file count here
                //        UpdateTotalFileCount();
                //        XLV_PIPE_DATA pipedata = new XLV_PIPE_DATA();
                //        pipedata.eMessageInfo = (int)_ENUMPIPES.DELETEVIRTUALFOLDER;
                //        pipedata.bData = new byte[4000];

                //        System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
                //        Byte[] tempbytes = System.Text.Encoding.ASCII.GetBytes(selectedText);

                //        tempbytes.CopyTo(pipedata.bData, 0);
                //        int size = Marshal.SizeOf(pipedata);
                //        byte[] bytes = new byte[size];
                //        try
                //        {
                //            IntPtr ptr = Marshal.AllocHGlobal(size);
                //            Marshal.StructureToPtr(pipedata, ptr, true);
                //            Marshal.Copy(ptr, bytes, 0, size);
                //            Marshal.FreeHGlobal(ptr);
                //        }
                //        finally
                //        {
                //        }
                //        this.pipeClient.SendMessage(bytes);
                //    }
                //}

                int SelectedRows = tgv.SelectedRows.Count;
                Dictionary<int, TreeGridNode> mSelectedNodes = new Dictionary<int, TreeGridNode>();
                String selectedText = string.Empty;

                for (int iCount = 0; iCount < SelectedRows; iCount++)
                {
                    mSelectedNodes.Add(iCount + 1, (TreeGridNode)tgv.SelectedRows[iCount]);
                }
                for (int iCount = 0; iCount < SelectedRows; iCount++)
                {
                    TreeGridNode tgn = mSelectedNodes[iCount + 1];
                    selectedText = tgn.Cells[tcTreeStructureIndex].Value.ToString();
                    TreeGridNode pTgn = (TreeGridNode)tgn.Parent;

                    if (selectedText == "Untitled")
                    {
                        MessageBox.Show("Please Select item", "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                    pTgn.Nodes.Remove(tgn);
                    RemoveAllPagesFromArray(selectedText);
                    UpdateTotalFileCount();
                    
                    XLV_PIPE_DATA pipedata = new XLV_PIPE_DATA();
                    pipedata.eMessageInfo = (int)_ENUMPIPES.DELETEVIRTUALFOLDER;
                    pipedata.bData = new byte[4000];

                    System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
                    Byte[] tempbytes = System.Text.Encoding.ASCII.GetBytes(selectedText);

                    tempbytes.CopyTo(pipedata.bData, 0);
                    int size = Marshal.SizeOf(pipedata);
                    byte[] bytes = new byte[size];
                    try
                    {
                        IntPtr ptr = Marshal.AllocHGlobal(size);
                        Marshal.StructureToPtr(pipedata, ptr, true);
                        Marshal.Copy(ptr, bytes, 0, size);
                        Marshal.FreeHGlobal(ptr);
                    }
                    finally
                    {
                    }
                    this.pipeClient.SendMessage(bytes);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in buttonDelete_Click " + ex.Message);
            }
        }

        private void buttonCreateFolder_Click(object sender, EventArgs e)
        {
            //treeViewColumns1.TreeView.BeginUpdate();
            try
            {

                TreeGridNode tgn = tgv.CurrentNode;

                // string str = itemString.ElementAt(8);
                string str = tgn.Cells[tcTreeStructureIndex].Value.ToString();

                string MakeDirPath = "";

                string FileNameOnly = "";
                int i = str.IndexOf("#@$");
                if (str.Contains("Page <"))
                {
                    MessageBox.Show("Rename for page not allowed", "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                else
                {
                    int j = str.LastIndexOf("\\");
                    FileNameOnly = str.Substring(j + 1);
                    MakeDirPath = str.Substring(0, j);
                }
                //if (!Directory.Exists(str))
                //{
                //    MessageBox.Show("Please select proper directory", "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Information);
                //    treeViewColumns1.TreeView.EndUpdate();
                //    return;
                //}
                AddDirectory addDir = new AddDirectory();
                addDir.ShowDialog();
                if (addDir.strDirname.Length <= 0)
                {
                    //treeViewColumns1.TreeView.EndUpdate();
                    return;
                }

                string strFolderName = str;
                strFolderName += "\\" + addDir.strDirname;

                // TreeGridNode node = tempNode.Nodes.Add(items.ToString());
                // node.Tag = new string[] { strTotlFileSize, strTotlPages,
                //PageSize, Orientation, ColorDepth, BookMarkName, "", "", strLastHiddenValue};

                TreeGridNode tempNode = tgn.Nodes.Add(strFolderName); //new TreeGridNode(strFolderName);
                tempNode.Tag = new string[] { "", "",
            "", "", "", "", "", "", MakeDirPath};
                tempNode.Cells[tcFileSizeIndex].Value = "";
                tempNode.Cells[tcNumberOfPagesIndex].Value = "";
                tempNode.Cells[tcPaperSizeIndex].Value = "";
                tempNode.Cells[tcOrientationIndex].Value = "";
                tempNode.Cells[tcColorDepthIndex].Value = "";
                tempNode.Cells[tcBookmarkNameIndex].Value = strFolderName;
                tempNode.Cells[tcBookmarkLevelIndex].Value = tempNode.Level - 1;

                //node.Nodes.Add(tempNode);
                tempNode.Expand();

                tempNode.ImageIndex = 1;
                //tempNode.SelectedImageIndex = 1;

                //treeViewColumns1.TreeView.EndUpdate();

                XLV_PIPE_DATA pipedata = new XLV_PIPE_DATA();
                pipedata.eMessageInfo = (int)_ENUMPIPES.CREATEVIRTUALFOLDER;
                pipedata.bData = new byte[4000];

                System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
                Byte[] tempbytes = System.Text.Encoding.ASCII.GetBytes(strFolderName);

                tempbytes.CopyTo(pipedata.bData, 0);


                int size = Marshal.SizeOf(pipedata);
                byte[] bytes = new byte[size];
                try
                {
                    IntPtr ptr = Marshal.AllocHGlobal(size);
                    Marshal.StructureToPtr(pipedata, ptr, true);
                    Marshal.Copy(ptr, bytes, 0, size);
                    Marshal.FreeHGlobal(ptr);
                }
                finally
                {
                }
                this.pipeClient.SendMessage(bytes);

            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in buttonCreateFolder_Click " + ex.Message);
            }

            //treeViewColumns1.TreeView.EndUpdate();
        }

        //private void buttonRename_Click(object sender, EventArgs e)
        //{
        //    try
        //    {
        //        TreeNode node = this.treeViewColumns1.TreeView.SelectedNode;
        //        string[] itemString = (string[])node.Tag;

        //        TreeGridNode tgn = tgv.CurrentNode;

        //        string str = itemString.ElementAt(8);
        //        str = tgn.Cells[tcTreeStructureIndex].Value.ToString();

        //        string MakeDirPath = "";

        //        string FileNameOnly = "";
        //        int i = str.IndexOf("#@$");
        //        if (i > 0)
        //        {
        //            MessageBox.Show("Rename for page not allowed", "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Error);
        //            return;
        //        }
        //        else
        //        {
        //            int j = str.LastIndexOf("\\");
        //            FileNameOnly = str.Substring(j + 1);
        //            MakeDirPath = str.Substring(0, j);
        //        }

        //        AddDirectory addDir = new AddDirectory();
        //        addDir.Text = "Rename";
        //        addDir.m_eDialogType = DIALOGTYPE.RENAMEITEM;
        //        addDir.strDirname = FileNameOnly;

        //        if (addDir.ShowDialog() != DialogResult.OK)
        //            return;

        //        if (addDir.strDirname == FileNameOnly)
        //            return;

        //        MakeDirPath += "\\" + addDir.strDirname;

        //        if (Directory.Exists(str))
        //        {
        //            Directory.Move(str, MakeDirPath);
        //        }
        //        else if (File.Exists(str))
        //        {
        //            File.Move(str, MakeDirPath);
        //        }
        //        else
        //        {
        //            MessageBox.Show("Failed to Rename " + str, "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Information);
        //        }

        //        TreeNode nodeClone = (TreeNode)node.Clone();
        //        nodeClone.Text = MakeDirPath;
        //        nodeClone.Tag = node.Tag;
        //        nodeClone.BackColor = node.BackColor;
        //        nodeClone.ImageIndex = node.ImageIndex;
        //        nodeClone.SelectedImageIndex = node.SelectedImageIndex;

        //        node.Parent.Nodes.Insert(node.Index + 1, nodeClone);
        //        node.Parent.Nodes.RemoveAt(node.Index);
        //        nodeClone.Parent.Expand();
        //        nodeClone.Expand();

        //        tgn.Cells[tcTreeStructureIndex].Value = MakeDirPath;
        //        this.treeViewColumns1.TreeView.SelectedNode = nodeClone;
        //    }
        //    catch (Exception ex)
        //    {
        //        MessageBox.Show("Exception in buttonRename_Click " + ex.Message);
        //    }
        //}

        private void buttonRename_Click(object sender, EventArgs e)
        {
            try
            {
                //TreeNode node = this.treeViewColumns1.TreeView.SelectedNode;
                //string[] itemString = (string[])node.Tag;

                TreeGridNode tgn = tgv.CurrentNode;

                // string str = itemString.ElementAt(8);
                string str = tgn.Cells[tcTreeStructureIndex].Value.ToString();

                string MakeDirPath = "";

                string FileNameOnly = "";
                int i = str.IndexOf("#@$");
                if (str.Contains("Page <"))
                {
                    MessageBox.Show("Rename for page not allowed", "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                else
                {
                    int j = str.LastIndexOf("\\");
                    FileNameOnly = str.Substring(j + 1);
                    MakeDirPath = str.Substring(0, j);
                }

                AddDirectory addDir = new AddDirectory();
                addDir.Text = "Rename";
                addDir.m_eDialogType = DIALOGTYPE.RENAMEITEM;
                addDir.strDirname = FileNameOnly;

                if (addDir.ShowDialog() != DialogResult.OK)
                    return;

                if (addDir.strDirname == FileNameOnly)
                    return;

                MakeDirPath += "\\" + addDir.strDirname;

                if (Directory.Exists(str))
                {
                    Directory.Move(str, MakeDirPath);
                }
                else if (File.Exists(str))
                {
                    File.Move(str, MakeDirPath);
                }
                else
                {
                    MessageBox.Show("Failed to Rename " + str, "XLanceView", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }

                //TreeNode nodeClone = (TreeNode)node.Clone();
                //nodeClone.Text = MakeDirPath;
                //nodeClone.Tag = node.Tag;
                //nodeClone.BackColor = node.BackColor;
                //nodeClone.ImageIndex = node.ImageIndex;
                //nodeClone.SelectedImageIndex = node.SelectedImageIndex;

                //node.Parent.Nodes.Insert(node.Index + 1, nodeClone);
                //node.Parent.Nodes.RemoveAt(node.Index);
                //nodeClone.Parent.Expand();
                //nodeClone.Expand();

                tgn.Cells[tcTreeStructureIndex].Value = MakeDirPath;
                tgn.Cells[tcBookmarkNameIndex].Value = MakeDirPath;
                //this.treeViewColumns1.TreeView.SelectedNode = nodeClone;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in buttonRename_Click " + ex.Message);
            }
        }

        private void ZeroPageForm_Resize(object sender, EventArgs e)
        {
            int iWindowWidth = this.Width;
            int iWindowHeight = this.Height;

            int itreeviewHeight = iWindowHeight - 99;

            this.btnImportFiles.Location = new System.Drawing.Point(9, 9);
            this.btnImportFiles.Size = new System.Drawing.Size(33, itreeviewHeight);

            //this.treeViewColumns1.Location = new System.Drawing.Point(this.btnImportFiles.Location.X + btnImportFiles.Size.Width + 5, 9);
            //this.treeViewColumns1.Size = new System.Drawing.Size(iWindowWidth - 59, itreeviewHeight);

            int iButtonsTop = 0;// this.treeViewColumns1.Location.Y + this.treeViewColumns1.Size.Height + 5;
            int BtnWidth = ((iWindowWidth - 59) / 11) - 5;
            int BtnCreteFolderWidth = ((iWindowWidth - 59) / 11) + 27;

            this.buttonOpenList.Location = new System.Drawing.Point(9, iButtonsTop);
            this.buttonOpenList.Size = new System.Drawing.Size(BtnCreteFolderWidth, 26);

            this.buttonCreateFolder.Location = new System.Drawing.Point(buttonOpenList.Location.X + BtnCreteFolderWidth + 2, iButtonsTop);
            this.buttonCreateFolder.Size = new System.Drawing.Size(BtnCreteFolderWidth, 26);

            this.buttonRename.Location = new System.Drawing.Point(buttonCreateFolder.Location.X + BtnCreteFolderWidth + 2, iButtonsTop);
            this.buttonRename.Size = new System.Drawing.Size(BtnWidth, 26);

            this.buttonTop.Location = new System.Drawing.Point(buttonRename.Location.X + BtnWidth + 2, iButtonsTop);
            this.buttonTop.Size = new System.Drawing.Size(BtnWidth, 26);

            this.buttonTopTen.Location = new System.Drawing.Point(buttonTop.Location.X + BtnWidth + 2, iButtonsTop);
            this.buttonTopTen.Size = new System.Drawing.Size(BtnWidth, 26);

            this.buttonUpOne.Location = new System.Drawing.Point(buttonTopTen.Location.X + BtnWidth + 2, iButtonsTop);
            this.buttonUpOne.Size = new System.Drawing.Size(BtnWidth, 26);

            this.buttonDownOne.Location = new System.Drawing.Point(buttonUpOne.Location.X + BtnWidth + 2, iButtonsTop);
            this.buttonDownOne.Size = new System.Drawing.Size(BtnWidth, 26);

            this.buttonDownTen.Location = new System.Drawing.Point(buttonDownOne.Location.X + BtnWidth + 2, iButtonsTop);
            this.buttonDownTen.Size = new System.Drawing.Size(BtnWidth, 26);

            this.buttonBottom.Location = new System.Drawing.Point(buttonDownTen.Location.X + BtnWidth + 2, iButtonsTop);
            this.buttonBottom.Size = new System.Drawing.Size(BtnWidth, 26);

            this.buttonDelete.Location = new System.Drawing.Point(buttonBottom.Location.X + BtnWidth + 2, iButtonsTop);
            this.buttonDelete.Size = new System.Drawing.Size(BtnWidth, 26);

            this.buttonSaveList.Location = new System.Drawing.Point(buttonDelete.Location.X + BtnWidth + 2, iButtonsTop);
            this.buttonSaveList.Size = new System.Drawing.Size(BtnWidth + 10, 26);

            int iSaveListButtonTop = this.buttonSaveList.Location.Y + this.buttonSaveList.Size.Height + 5;

            int iSaveListButtonLeft = ((iWindowWidth - 59) / 2) - (BtnCreteFolderWidth / 2);
            this.btnSave.Location = new System.Drawing.Point(iSaveListButtonLeft, iSaveListButtonTop);
            this.btnSave.Size = new System.Drawing.Size(BtnCreteFolderWidth, 26);
        }

        private void buttonOpenList_Click(object sender, EventArgs e)
        {
            try
            {
                OpenFileDialog objOpenDialog = new OpenFileDialog();
                objOpenDialog.Filter = "xml files (*.xml)|*.xml";
                objOpenDialog.FilterIndex = 2;
                objOpenDialog.RestoreDirectory = true;

                if (objOpenDialog.ShowDialog() == DialogResult.OK)
                {
                    DataTable dt = new DataTable();
                    //dt.ReadXmlSchema(@"E:\RamProjects\XLV.24\Debug\TestSchema.xml");
                    dt.ReadXmlSchema("TestSchema.xml");
                    dt.ReadXml(objOpenDialog.FileName);
                    //dt.ReadXml(@"E:\RamProjects\XLV.24\Debug\Test.xml");

                    tgv.Nodes.Clear();
                    tgv.ImageList = imageStrip;
                    Dictionary<String, TreeGridNode> parentTgnDCT = new Dictionary<string, TreeGridNode>();


                    for (int i = 0; i < dt.Rows.Count; i++)
                    {
                        TreeGridNode tgn = null;
                        String level = dt.Rows[i]["parent_sr_no"].ToString();
                        int imageIndex = Convert.ToInt32(dt.Rows[i]["image_index"]);
                        String parentLevel = String.Empty;
                        if (level.Contains('.'))
                        {
                            parentLevel = level.Substring(0, level.LastIndexOf('.'));
                        }

                        if (parentTgnDCT.ContainsKey(parentLevel))
                        {
                            TreeGridNode pTgn = parentTgnDCT[parentLevel];
                            tgn = pTgn.Nodes.Add();
                            parentTgnDCT.Add(level, tgn);
                        }
                        else
                        {
                            tgn = tgv.Nodes.Add();
                            parentTgnDCT.Add(level, tgn);
                        }

                        for (int j = 0; j < tgn.Cells.Count - 1; j++)
                        {
                            tgn.Cells[j].Value = dt.Rows[i][j];
                        }
                        tgn.ImageIndex = imageIndex;
                    }
                    if (parentTgnDCT.ContainsKey("0"))
                    {
                        ExpandNode(parentTgnDCT["0"]);
                    }
                }
            }
            catch (Exception ex)
            {
            }
        }

        private void buttonSaveList_Click(object sender, EventArgs e)
        {
            SaveFileDialog objOpenDialog = new SaveFileDialog();
            objOpenDialog.Filter = "xml files (*.xml)|*.xml";
            objOpenDialog.FilterIndex = 2;
            objOpenDialog.RestoreDirectory = true;

            if (objOpenDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            CheckAndSetMultipleRowsSelected();
            string strFilePath = objOpenDialog.FileName;
            DataTable table = new DataTable("FileList");
            // copy the correct structure from datagridview to the table
            foreach (DataGridViewColumn column in tgv.Columns)
            {
                table.Columns.Add(column.Name, typeof(string));
            }
            table.Columns.Add("parent_sr_no");
            table.Columns.Add("image_index", typeof(int));
            //populate the datatable from datagridview
            for (int rowIndex = 0; rowIndex < tgv.Nodes.Count; rowIndex++)
            {
                GetDataTable(table, tgv.Nodes[rowIndex], rowIndex.ToString());
                break;
            }

            table.WriteXmlSchema("TestSchema.xml");
            table.WriteXml(strFilePath);

            MessageBox.Show("List saved successfully", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void GetDataTable(DataTable dt, TreeGridNode tgn, String parentSrNo)
        {
            if (tgn != null)
            {
                int index = dt.Rows.Count;
                dt.Rows.Add();
                for (int i = 0; i < tgn.Cells.Count; i++)
                {
                    dt.Rows[index][i] = tgn.Cells[i].Value;
                }
                dt.Rows[index]["parent_sr_no"] = parentSrNo;
                dt.Rows[index]["image_index"] = tgn.ImageIndex;
                
                for (int i = 0; i < tgn.Nodes.Count; i++)
                {
                    GetDataTable(dt, tgn.Nodes[i], parentSrNo + "." + i.ToString());
                }
            }
        }
        
        private void tgv_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                if (tgv.Rows.Count > 0)
                {
                    DataGridView.HitTestInfo hti = tgv.HitTest(e.X, e.Y);
                    if (hti.Type == DataGridViewHitTestType.Cell)
                    {
                        //if (!dgvTrialBalance.Rows[hti.RowIndex].Selected)
                        //{
                        //tgv.ClearSelection();
                        //tgv.Rows[hti.RowIndex].Cells[hti.ColumnIndex].Selected = true;
                        //}
                        TreeGridNode localTgn = (TreeGridNode)tgv.Rows[hti.RowIndex];
                        if (localTgn != null)
                        {

                            tgv.ContextMenuStrip = m;
                        }
                    }
                }
                else
                {
                    tgv.ContextMenuStrip = null;
                }

            }
        }

        private void MovePositionUp(TreeGridNode tgn, Boolean upOrDown)
        {
            if (tgn != null)
            {
                if (upOrDown)
                {
                    if (tgn.IsFirstSibling)
                    {
                        return;
                    }
                }
                else
                {
                    if (tgn.IsLastSibling)
                    {
                        return;
                    }
                }
                TreeGridNode pTgn = tgn.Parent;

                List<TreeGridNode> tgnList = new List<TreeGridNode>();

                if (pTgn != null)
                {

                    int index = tgn.Index;
                   
                    for (int i = 0; i < pTgn.Nodes.Count; i++)
                    {
                        tgnList.Add(pTgn.Nodes[i]);
                    }
                    pTgn.Nodes.Clear();

                    tgnList.Remove(tgn);
                    if (upOrDown)
                    {
                        tgnList.Insert(index - 1, tgn);
                    }
                    else
                    {
                        tgnList.Insert(index + 1, tgn);
                    }
                    for (int i = 0; i < tgnList.Count; i++)
                    {
                        //pTgn.Nodes.Add();
                        //pTgn.Nodes[i].Cells[0].Value = tgnList[i].Cells[0].Value;
                        //pTgn.Nodes[i].Cells[1].Value = tgnList[i].Cells[1].Value;
                        //pTgn.Nodes[i].DefaultCellStyle.BackColor = tgnList[i].DefaultCellStyle.BackColor;
                        

                        AddNodeRec(pTgn, tgnList[i]);
                       
                    }
                }
            }
           
            tgv.Refresh();
        }
        private void AddNodeRec(TreeGridNode pTgn, TreeGridNode node)
        {
            TreeGridNode cTgn = pTgn.Nodes.Add();
            for (int j = 0; j < cTgn.Cells.Count; j++)
            {
                cTgn.Cells[j].Value = node.Cells[j].Value;
            }
            cTgn.DefaultCellStyle.BackColor = node.DefaultCellStyle.BackColor;
            cTgn.ImageIndex  = node.ImageIndex;

            if (node.IsExpanded)
            {
                cTgn.Expand();
            }
            for (int i = 0; i < node.Nodes.Count; i++)
            {
                AddNodeRec(cTgn, node.Nodes[i]);
            }
        }

        private void tgv_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (tgv.CurrentNode!= null &&
                tcSelectIndex == e.ColumnIndex &&
                e.RowIndex ==0)
            {
                if (tgv.CurrentNode.Cells[tcTreeStructureIndex].Value.ToString() == "Untitled")
                {
                    TreeGridNode tgn = tgv.CurrentNode;
                    if (tgn.Cells[tcSelectIndex].Value != DBNull.Value &&
                        tgn.Cells[tcSelectIndex].Value != null)
                    {
                        Boolean val = Convert.ToBoolean(tgn.Cells[tcSelectIndex].Value);
                        ChangeCheckBoxColumnValueOfChild(tgn, val);
                    }
                }
            }
        }

        private void ChangeCheckBoxColumnValueOfChild(TreeGridNode tgn, bool val)
        {
            if (tgn != null)
            {
                for (int i = 0; i < tgn.Nodes.Count; i++)
                {
                    tgn.Nodes[i].Cells[tcSelectIndex].Value = val;
                    ChangeCheckBoxColumnValueOfChild(tgn.Nodes[i], val);
                }
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void tgv_CurrentCellDirtyStateChanged(object sender, EventArgs e)
        {
            if (tgv.IsCurrentCellDirty)
                tgv.CommitEdit(DataGridViewDataErrorContexts.Commit);
        }

       /// <summary>
       /// need to update the new items imported by user.
       /// </summary>
        private void UpdateNewItemsInTreeGridView()
        {
            try
            {
                int iTotalCount = m_gridviewDetails.Count;
                if (iTotalCount == 0)
                {
                    m_bstop = true;
                    m_progressBarPosition = m_progressBarRange;
                    return;
                }

                double iTotalFileSize = CalCulateTotalFileSize();
                
                double dbSizeinBytes = iTotalFileSize;
                iTotalFileSize /= 1024;//convert to KB
                double dbSizeinKB = iTotalFileSize;
                iTotalFileSize /= 1024;//convert to MB
                double dbSizeinMB = iTotalFileSize;

                string strTotlFileSize = "";
                if (dbSizeinKB < 1)
                {
                    strTotlFileSize = dbSizeinBytes.ToString("0.00");
                    strTotlFileSize += " " + "Bytes";
                }
                else if (dbSizeinMB < 1)
                {
                    strTotlFileSize = dbSizeinKB.ToString("0.00");
                    strTotlFileSize += " " + "KB";
                }
                else
                {
                    strTotlFileSize = dbSizeinMB.ToString("0.00");
                    strTotlFileSize += " " + "MB";
                }

                if (iTotalFileSize == 0)
                    strTotlFileSize = "";

                int iTotalPages = CalCulateTotalNumberofPages();

                string strTotalPages = iTotalPages.ToString();
                if (iTotalPages == 0)
                    strTotalPages = "";

                tgv.ImageList = imageStrip;

                for (int iIndex = 0; iIndex < iTotalCount; iIndex++)
                {
                    string items = m_gridviewDetails.ElementAt(iIndex).Key;
                    ValuePairs valuePairs = (ValuePairs)m_gridviewDetails.ElementAt(iIndex).Value;

                    TreeGridNode treeNode = tgv.Nodes[0];

                    treeNode.Cells[tcFileSizeIndex].Value = strTotlFileSize;
                    treeNode.Cells[tcNumberOfPagesIndex].Value = strTotalPages;

                    //no need to add the same item twice
                    if (CheckIfISPresentInGrid(treeNode, items))
                    {
                        continue;
                    }

                    iTotalFileSize = 0;
                    iTotalFileSize = valuePairs.FileSize;
                    dbSizeinBytes = iTotalFileSize;
                    iTotalFileSize /= 1024;//convert to KB
                    dbSizeinKB = iTotalFileSize;
                    iTotalFileSize /= 1024;//convert to MB
                    dbSizeinMB = iTotalFileSize;

                    strTotlFileSize = "";
                    if (dbSizeinKB < 1)
                    {
                        strTotlFileSize = dbSizeinBytes.ToString("0.00");
                        strTotlFileSize += " " + "Bytes";
                    }
                    else if (dbSizeinMB < 1)
                    {
                        strTotlFileSize = dbSizeinKB.ToString("0.00");
                        strTotlFileSize += " " + "KB";
                    }
                    else
                    {
                        strTotlFileSize = dbSizeinMB.ToString("0.00");
                        strTotlFileSize += " " + "MB";
                    }

                    if (iTotalFileSize == 0)
                        strTotlFileSize = "";


                    string strTotlPages = valuePairs.NoOfPages.ToString();
                    if (strTotlPages == "0")
                        strTotlPages = "";
                    else if (valuePairs.NoOfPages == -1)
                        strTotlPages = "0";

                    string strLastHiddenValue = items;
                    string strBookMarkName = items;

                    string Orientation = "";
                    if (valuePairs.Orientation != null)
                        Orientation = valuePairs.Orientation;
                    string ColorDepth = "";
                    //switch (valuePairs.ColorDepth)
                    //{
                    //    case (int)PdfColorMode.Undefined:
                    //        ColorDepth = "Undefined";
                    //        break;
                    //    case (int)PdfColorMode.Cmyk:
                    //        ColorDepth = "CMYK";
                    //        break;
                    //    case (int)PdfColorMode.Rgb:
                    //        ColorDepth = "RGB";
                    //        break;
                    //    default:
                    //        ColorDepth = "Undefined";
                    //        break;
                    //}

                    string PageSize = "";
                    if (valuePairs.PageSize != null)
                    {
                        if (valuePairs.PageSize.Length > 0)
                            PageSize = valuePairs.PageSize;
                    }

                    TreeGridNode tempNode = treeNode;
                    if (CheckIfISParentPresentInGrid(treeNode, items))
                    {
                        if (valuePairs.bPage)
                        {
                            int i = items.IndexOf("#@$");
                            strBookMarkName = items.Substring(0, i - 1);
                            items = items.Substring(i + 3);
                            items = "Page <" + items + ">";
                            strBookMarkName += "\\" + items;
                        }
                        tempNode = m_PresentTreeGridNode;
                    }

                    string BookMarkName = "";
                    if (valuePairs.bPage)
                    {
                        BookMarkName = strBookMarkName;
                    }
                    else if (valuePairs.bFile)
                    {
                        BookMarkName = items.ToString();
                    }
                    else
                    {
                        BookMarkName = items.ToString();
                    }

                    TreeGridNode node = tempNode.Nodes.Add(items.ToString());
                    node.Tag = new string[] { strTotlFileSize, strTotlPages,
               PageSize, Orientation, ColorDepth, BookMarkName, "", "", strLastHiddenValue};
                    node.Cells[tcFileSizeIndex].Value = strTotlFileSize;
                    node.Cells[tcNumberOfPagesIndex].Value = strTotlPages;
                    node.Cells[tcPaperSizeIndex].Value = PageSize;
                    node.Cells[tcOrientationIndex].Value = Orientation;
                    node.Cells[tcColorDepthIndex].Value = ColorDepth;
                    node.Cells[tcBookmarkNameIndex].Value = BookMarkName;
                    node.Cells[tcBookmarkLevelIndex].Value = node.Level - 1;
                    //treeNode.Nodes.Add(node);
                    //tempNode.Expand();
                    

                    if (valuePairs.bFile)
                    {
                        node.ImageIndex = 3;
                        //node.ImageIndex = 3;
                    }
                    else if (valuePairs.bPage)
                    {
                        node.ImageIndex = 2;
                        //node.SelectedImageIndex = 2;
                    }
                    else
                    {
                        node.ImageIndex = 1;
                        //node.SelectedImageIndex = 1;
                    }
                    m_progressBarPosition += 1;
                    
                }
            }
            catch (Exception ex)
            {
            }
        }

        void RemoveAllPagesFromArray(string selectedText)
        {
            try
            {
                int iTotalCount = m_gridviewDetails.Count;
                for (int iIndex = 0; iIndex < iTotalCount; )
                {
                    string items = m_gridviewDetails.ElementAt(iIndex).Key;
                    if (items.Contains(selectedText))
                    {
                        m_gridviewDetails.Remove(items);
                        iTotalCount--;
                    }
                    else
                    {
                        iIndex++;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in RemoveAllPagesFromArray " + ex.Message);
            }
        }

        void UpdateTotalFileCount()
        {
            GetTotalAll();

            //TreeGridNodeCollection TreeNodes = tgv.Nodes[0]
            TreeGridNode treeNode = tgv.Nodes[0];
            treeNode.Cells[tcFileSizeIndex].Value = m_dbGlobalTotalSize;
            treeNode.Cells[tcNumberOfPagesIndex].Value = m_iGlobalTotalPages;

        }
    }
}



