namespace XLanceDLL
{
    partial class ZeroPageForm
    {
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ZeroPageForm));
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            this.imageStrip = new System.Windows.Forms.ImageList(this.components);
            this.imageList2 = new System.Windows.Forms.ImageList(this.components);
            this.btnImportFiles = new System.Windows.Forms.Button();
            this.columnPaperSize = new System.Windows.Forms.ColumnHeader();
            this.columnOrientation = new System.Windows.Forms.ColumnHeader();
            this.columnColorDepth = new System.Windows.Forms.ColumnHeader();
            this.columnBookmark = new System.Windows.Forms.ColumnHeader();
            this.columnBookmarkLevel = new System.Windows.Forms.ColumnHeader();
            this.buttonSaveList = new System.Windows.Forms.Button();
            this.buttonOpenList = new System.Windows.Forms.Button();
            this.btnSave = new System.Windows.Forms.Button();
            this.buttonDelete = new System.Windows.Forms.Button();
            this.buttonBottom = new System.Windows.Forms.Button();
            this.buttonDownTen = new System.Windows.Forms.Button();
            this.buttonDownOne = new System.Windows.Forms.Button();
            this.buttonRename = new System.Windows.Forms.Button();
            this.buttonCreateFolder = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.buttonTop = new System.Windows.Forms.Button();
            this.buttonTopTen = new System.Windows.Forms.Button();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.buttonUpOne = new System.Windows.Forms.Button();
            this.panel3 = new System.Windows.Forms.Panel();
            this.tgv = new AdvancedDataGridView.TreeGridView();
            this.tcTreeStructure = new AdvancedDataGridView.TreeGridColumn();
            this.tcSelect = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.tcFileSize = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tcNumberOfPages = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tcPaperSize = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tcOrientation = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tcColorDepth = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tcBookmarkName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tcBookmarkLevel = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tcBlank = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.panel1.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tgv)).BeginInit();
            this.SuspendLayout();
            // 
            // imageStrip
            // 
            this.imageStrip.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageStrip.ImageStream")));
            this.imageStrip.TransparentColor = System.Drawing.Color.Transparent;
            this.imageStrip.Images.SetKeyName(0, "App.ico");
            this.imageStrip.Images.SetKeyName(1, "CLSDFOLD.ICO");
            this.imageStrip.Images.SetKeyName(2, "page.ico");
            this.imageStrip.Images.SetKeyName(3, "filetype_pdf.ico");
            // 
            // imageList2
            // 
            this.imageList2.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList2.ImageStream")));
            this.imageList2.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList2.Images.SetKeyName(0, "Red.bmp");
            this.imageList2.Images.SetKeyName(1, "Orange.bmp");
            this.imageList2.Images.SetKeyName(2, "Yello.bmp");
            this.imageList2.Images.SetKeyName(3, "Green.bmp");
            this.imageList2.Images.SetKeyName(4, "Blue.bmp");
            this.imageList2.Images.SetKeyName(5, "Violet.bmp");
            this.imageList2.Images.SetKeyName(6, "LightBlack.bmp");
            // 
            // btnImportFiles
            // 
            this.btnImportFiles.Dock = System.Windows.Forms.DockStyle.Left;
            this.btnImportFiles.Location = new System.Drawing.Point(0, 0);
            this.btnImportFiles.Name = "btnImportFiles";
            this.btnImportFiles.Size = new System.Drawing.Size(34, 409);
            this.btnImportFiles.TabIndex = 9;
            this.btnImportFiles.Text = ">>";
            this.btnImportFiles.UseVisualStyleBackColor = true;
            this.btnImportFiles.Click += new System.EventHandler(this.btnImportFiles_Click);
            // 
            // columnPaperSize
            // 
            this.columnPaperSize.Text = "PaperSize";
            // 
            // columnOrientation
            // 
            this.columnOrientation.Text = "Orientation";
            // 
            // columnColorDepth
            // 
            this.columnColorDepth.Text = "Color Depth";
            // 
            // columnBookmark
            // 
            this.columnBookmark.Text = "Bookmark Name";
            // 
            // columnBookmarkLevel
            // 
            this.columnBookmarkLevel.Text = "Book Mark Level";
            // 
            // buttonSaveList
            // 
            this.buttonSaveList.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonSaveList.Location = new System.Drawing.Point(727, 3);
            this.buttonSaveList.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonSaveList.Name = "buttonSaveList";
            this.buttonSaveList.Size = new System.Drawing.Size(78, 26);
            this.buttonSaveList.TabIndex = 48;
            this.buttonSaveList.Text = "&Save List";
            this.buttonSaveList.UseVisualStyleBackColor = true;
            this.buttonSaveList.Click += new System.EventHandler(this.buttonSaveList_Click);
            // 
            // buttonOpenList
            // 
            this.buttonOpenList.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonOpenList.Location = new System.Drawing.Point(0, 3);
            this.buttonOpenList.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonOpenList.Name = "buttonOpenList";
            this.buttonOpenList.Size = new System.Drawing.Size(74, 26);
            this.buttonOpenList.TabIndex = 38;
            this.buttonOpenList.Text = "&Open List";
            this.buttonOpenList.UseVisualStyleBackColor = true;
            this.buttonOpenList.Click += new System.EventHandler(this.buttonOpenList_Click);
            // 
            // btnSave
            // 
            this.btnSave.Dock = System.Windows.Forms.DockStyle.Top;
            this.btnSave.Location = new System.Drawing.Point(370, 35);
            this.btnSave.Margin = new System.Windows.Forms.Padding(0);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(135, 26);
            this.btnSave.TabIndex = 49;
            this.btnSave.Text = "&Save as PDF";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // buttonDelete
            // 
            this.buttonDelete.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonDelete.Location = new System.Drawing.Point(653, 3);
            this.buttonDelete.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Size = new System.Drawing.Size(74, 26);
            this.buttonDelete.TabIndex = 47;
            this.buttonDelete.Text = "&Delete";
            this.buttonDelete.UseVisualStyleBackColor = true;
            this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
            // 
            // buttonBottom
            // 
            this.buttonBottom.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonBottom.Location = new System.Drawing.Point(579, 3);
            this.buttonBottom.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonBottom.Name = "buttonBottom";
            this.buttonBottom.Size = new System.Drawing.Size(74, 26);
            this.buttonBottom.TabIndex = 46;
            this.buttonBottom.Text = "&Bottom";
            this.buttonBottom.UseVisualStyleBackColor = true;
            this.buttonBottom.Click += new System.EventHandler(this.button5_Click);
            // 
            // buttonDownTen
            // 
            this.buttonDownTen.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonDownTen.Location = new System.Drawing.Point(67, 3);
            this.buttonDownTen.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonDownTen.Name = "buttonDownTen";
            this.buttonDownTen.Size = new System.Drawing.Size(68, 26);
            this.buttonDownTen.TabIndex = 45;
            this.buttonDownTen.Text = "D&own 10";
            this.buttonDownTen.UseVisualStyleBackColor = true;
            this.buttonDownTen.Click += new System.EventHandler(this.buttonDownTen_Click);
            // 
            // buttonDownOne
            // 
            this.buttonDownOne.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonDownOne.Location = new System.Drawing.Point(296, 3);
            this.buttonDownOne.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonDownOne.Name = "buttonDownOne";
            this.buttonDownOne.Size = new System.Drawing.Size(74, 26);
            this.buttonDownOne.TabIndex = 44;
            this.buttonDownOne.Text = "&Down 1";
            this.buttonDownOne.UseVisualStyleBackColor = true;
            this.buttonDownOne.Click += new System.EventHandler(this.button7_Click);
            // 
            // buttonRename
            // 
            this.buttonRename.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonRename.Location = new System.Drawing.Point(148, 3);
            this.buttonRename.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonRename.Name = "buttonRename";
            this.buttonRename.Size = new System.Drawing.Size(74, 26);
            this.buttonRename.TabIndex = 41;
            this.buttonRename.Text = "&Rename";
            this.buttonRename.UseVisualStyleBackColor = true;
            this.buttonRename.Click += new System.EventHandler(this.buttonRename_Click);
            // 
            // buttonCreateFolder
            // 
            this.buttonCreateFolder.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonCreateFolder.Location = new System.Drawing.Point(74, 3);
            this.buttonCreateFolder.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonCreateFolder.Name = "buttonCreateFolder";
            this.buttonCreateFolder.Size = new System.Drawing.Size(74, 26);
            this.buttonCreateFolder.TabIndex = 39;
            this.buttonCreateFolder.Text = "&Create Folder";
            this.buttonCreateFolder.UseVisualStyleBackColor = true;
            this.buttonCreateFolder.Click += new System.EventHandler(this.buttonCreateFolder_Click);
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.Control;
            this.panel1.Controls.Add(this.tableLayoutPanel1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 409);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(805, 80);
            this.panel1.TabIndex = 51;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 10;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.80533F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 9.243853F));
            this.tableLayoutPanel1.Controls.Add(this.buttonDownOne, 4, 0);
            this.tableLayoutPanel1.Controls.Add(this.buttonOpenList, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.buttonSaveList, 9, 0);
            this.tableLayoutPanel1.Controls.Add(this.buttonCreateFolder, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.buttonDelete, 8, 0);
            this.tableLayoutPanel1.Controls.Add(this.buttonRename, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.buttonBottom, 7, 0);
            this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel2, 5, 0);
            this.tableLayoutPanel1.Controls.Add(this.btnSave, 5, 1);
            this.tableLayoutPanel1.Controls.Add(this.buttonUpOne, 3, 0);
            this.tableLayoutPanel1.Controls.Add(this.buttonTop, 6, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 44.44444F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 55.55556F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(805, 80);
            this.tableLayoutPanel1.TabIndex = 50;
            // 
            // buttonTop
            // 
            this.buttonTop.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonTop.Location = new System.Drawing.Point(505, 3);
            this.buttonTop.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonTop.Name = "buttonTop";
            this.buttonTop.Size = new System.Drawing.Size(74, 26);
            this.buttonTop.TabIndex = 40;
            this.buttonTop.Text = "&Top";
            this.buttonTop.UseVisualStyleBackColor = true;
            this.buttonTop.Click += new System.EventHandler(this.buttonTop_Click);
            // 
            // buttonTopTen
            // 
            this.buttonTopTen.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonTopTen.Location = new System.Drawing.Point(0, 3);
            this.buttonTopTen.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonTopTen.Name = "buttonTopTen";
            this.buttonTopTen.Size = new System.Drawing.Size(67, 26);
            this.buttonTopTen.TabIndex = 42;
            this.buttonTopTen.Text = "&Up 10";
            this.buttonTopTen.UseVisualStyleBackColor = true;
            this.buttonTopTen.Click += new System.EventHandler(this.buttonTopTen_Click);
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.ColumnCount = 2;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel2.Controls.Add(this.buttonTopTen, 0, 0);
            this.tableLayoutPanel2.Controls.Add(this.buttonDownTen, 1, 0);
            this.tableLayoutPanel2.Location = new System.Drawing.Point(370, 0);
            this.tableLayoutPanel2.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 1;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(135, 35);
            this.tableLayoutPanel2.TabIndex = 50;
            // 
            // buttonUpOne
            // 
            this.buttonUpOne.Dock = System.Windows.Forms.DockStyle.Top;
            this.buttonUpOne.Location = new System.Drawing.Point(222, 3);
            this.buttonUpOne.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.buttonUpOne.Name = "buttonUpOne";
            this.buttonUpOne.Size = new System.Drawing.Size(74, 26);
            this.buttonUpOne.TabIndex = 43;
            this.buttonUpOne.Text = "U&p 1";
            this.buttonUpOne.UseVisualStyleBackColor = true;
            this.buttonUpOne.Click += new System.EventHandler(this.button8_Click);
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.tgv);
            this.panel3.Controls.Add(this.btnImportFiles);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3.Location = new System.Drawing.Point(0, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(805, 409);
            this.panel3.TabIndex = 53;
            // 
            // tgv
            // 
            this.tgv.AllowUserToAddRows = false;
            this.tgv.AllowUserToDeleteRows = false;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.tgv.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.tgv.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.tcTreeStructure,
            this.tcSelect,
            this.tcFileSize,
            this.tcNumberOfPages,
            this.tcPaperSize,
            this.tcOrientation,
            this.tcColorDepth,
            this.tcBookmarkName,
            this.tcBookmarkLevel,
            this.tcBlank});
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.tgv.DefaultCellStyle = dataGridViewCellStyle2;
            this.tgv.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tgv.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnKeystroke;
            this.tgv.ImageList = null;
            this.tgv.Location = new System.Drawing.Point(34, 0);
            this.tgv.MultiSelect = false;
            this.tgv.Name = "tgv";
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.tgv.RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
            this.tgv.RowHeadersVisible = false;
            this.tgv.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.tgv.Size = new System.Drawing.Size(771, 409);
            this.tgv.TabIndex = 50;
            this.tgv.VirtualNodes = true;
            this.tgv.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.tgv_CellValueChanged);
            this.tgv.MouseDown += new System.Windows.Forms.MouseEventHandler(this.tgv_MouseDown);
            this.tgv.CurrentCellDirtyStateChanged += new System.EventHandler(this.tgv_CurrentCellDirtyStateChanged);
            // 
            // tcTreeStructure
            // 
            this.tcTreeStructure.DefaultNodeImage = null;
            this.tcTreeStructure.HeaderText = "Tree Structure";
            this.tcTreeStructure.Name = "tcTreeStructure";
            this.tcTreeStructure.ReadOnly = true;
            this.tcTreeStructure.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            this.tcTreeStructure.Width = 200;
            // 
            // tcSelect
            // 
            this.tcSelect.HeaderText = "Select";
            this.tcSelect.Name = "tcSelect";
            this.tcSelect.Visible = false;
            // 
            // tcFileSize
            // 
            this.tcFileSize.HeaderText = "File Size";
            this.tcFileSize.Name = "tcFileSize";
            this.tcFileSize.ReadOnly = true;
            this.tcFileSize.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // tcNumberOfPages
            // 
            this.tcNumberOfPages.HeaderText = "Number Of Pages";
            this.tcNumberOfPages.Name = "tcNumberOfPages";
            this.tcNumberOfPages.ReadOnly = true;
            this.tcNumberOfPages.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // tcPaperSize
            // 
            this.tcPaperSize.HeaderText = "Paper Size";
            this.tcPaperSize.Name = "tcPaperSize";
            this.tcPaperSize.ReadOnly = true;
            this.tcPaperSize.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // tcOrientation
            // 
            this.tcOrientation.HeaderText = "Orientation";
            this.tcOrientation.Name = "tcOrientation";
            this.tcOrientation.ReadOnly = true;
            this.tcOrientation.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // tcColorDepth
            // 
            this.tcColorDepth.HeaderText = "Color Depth";
            this.tcColorDepth.Name = "tcColorDepth";
            this.tcColorDepth.ReadOnly = true;
            this.tcColorDepth.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // tcBookmarkName
            // 
            this.tcBookmarkName.HeaderText = "Bookmark Name";
            this.tcBookmarkName.Name = "tcBookmarkName";
            this.tcBookmarkName.ReadOnly = true;
            this.tcBookmarkName.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // tcBookmarkLevel
            // 
            this.tcBookmarkLevel.HeaderText = "Bookmark Level";
            this.tcBookmarkLevel.Name = "tcBookmarkLevel";
            this.tcBookmarkLevel.ReadOnly = true;
            this.tcBookmarkLevel.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // tcBlank
            // 
            this.tcBlank.HeaderText = "Blank";
            this.tcBlank.Name = "tcBlank";
            this.tcBlank.ReadOnly = true;
            this.tcBlank.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // ZeroPageForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(805, 489);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Location = new System.Drawing.Point(0, 25);
            this.Name = "ZeroPageForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "ZeroPageForm";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.Resize += new System.EventHandler(this.ZeroPageForm_Resize);
            this.panel1.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel2.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.tgv)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ImageList imageStrip;
        private System.Windows.Forms.ImageList imageList2;
        private System.Windows.Forms.Button btnImportFiles;
        private System.Windows.Forms.ColumnHeader columnPaperSize;
        private System.Windows.Forms.ColumnHeader columnOrientation;
        private System.Windows.Forms.ColumnHeader columnColorDepth;
        private System.Windows.Forms.ColumnHeader columnBookmark;
        private System.Windows.Forms.ColumnHeader columnBookmarkLevel;
        private System.Windows.Forms.Button buttonSaveList;
        private System.Windows.Forms.Button buttonOpenList;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button buttonDelete;
        private System.Windows.Forms.Button buttonBottom;
        private System.Windows.Forms.Button buttonDownTen;
        private System.Windows.Forms.Button buttonDownOne;
        private System.Windows.Forms.Button buttonRename;
        private System.Windows.Forms.Button buttonCreateFolder;
        private AdvancedDataGridView.TreeGridView tgv;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel3;
        private AdvancedDataGridView.TreeGridColumn tcTreeStructure;
        private System.Windows.Forms.DataGridViewCheckBoxColumn tcSelect;
        private System.Windows.Forms.DataGridViewTextBoxColumn tcFileSize;
        private System.Windows.Forms.DataGridViewTextBoxColumn tcNumberOfPages;
        private System.Windows.Forms.DataGridViewTextBoxColumn tcPaperSize;
        private System.Windows.Forms.DataGridViewTextBoxColumn tcOrientation;
        private System.Windows.Forms.DataGridViewTextBoxColumn tcColorDepth;
        private System.Windows.Forms.DataGridViewTextBoxColumn tcBookmarkName;
        private System.Windows.Forms.DataGridViewTextBoxColumn tcBookmarkLevel;
        private System.Windows.Forms.DataGridViewTextBoxColumn tcBlank;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.Button buttonTop;
        private System.Windows.Forms.Button buttonTopTen;
        private System.Windows.Forms.Button buttonUpOne;
    }
}