namespace XLanceDLL
{
	partial class TreeViewColumns
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

		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TreeViewColumns));
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnTreeStructure = new System.Windows.Forms.ColumnHeader();
            this.columnFileSize = new System.Windows.Forms.ColumnHeader();
            this.columnPages = new System.Windows.Forms.ColumnHeader();
            this.columnPaperSize = new System.Windows.Forms.ColumnHeader();
            this.columnOrientation = new System.Windows.Forms.ColumnHeader();
            this.columnColorDepth = new System.Windows.Forms.ColumnHeader();
            this.columnBookMarkName = new System.Windows.Forms.ColumnHeader();
            this.columnBookMarkLevel = new System.Windows.Forms.ColumnHeader();
            this.columnHidden = new System.Windows.Forms.ColumnHeader();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.imageList2 = new System.Windows.Forms.ImageList(this.components);
            this.SuspendLayout();
            // 
            // listView1
            // 
            this.listView1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnTreeStructure,
            this.columnFileSize,
            this.columnPages,
            this.columnPaperSize,
            this.columnOrientation,
            this.columnColorDepth,
            this.columnBookMarkName,
            this.columnBookMarkLevel,
            this.columnHidden});
            this.listView1.Dock = System.Windows.Forms.DockStyle.Top;
            this.listView1.FullRowSelect = true;
            this.listView1.GridLines = true;
            this.listView1.Location = new System.Drawing.Point(0, 0);
            this.listView1.Name = "listView1";
            this.listView1.Scrollable = false;
            this.listView1.Size = new System.Drawing.Size(438, 20);
            this.listView1.TabIndex = 2;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            this.listView1.ColumnWidthChanged += new System.Windows.Forms.ColumnWidthChangedEventHandler(this.listView1_ColumnWidthChanged);
            this.listView1.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.listView1_ColumnClick);
            this.listView1.ColumnWidthChanging += new System.Windows.Forms.ColumnWidthChangingEventHandler(this.listView1_ColumnWidthChanging);
            // 
            // columnTreeStructure
            // 
            this.columnTreeStructure.Text = "Tree Structure";
            this.columnTreeStructure.Width = 190;
            // 
            // columnFileSize
            // 
            this.columnFileSize.Text = "File Size";
            this.columnFileSize.Width = 129;
            // 
            // columnPages
            // 
            this.columnPages.Text = "Number of Pages";
            this.columnPages.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.columnPages.Width = 145;
            // 
            // columnPaperSize
            // 
            this.columnPaperSize.Text = "Paper Size";
            // 
            // columnOrientation
            // 
            this.columnOrientation.Text = "Orientation";
            // 
            // columnColorDepth
            // 
            this.columnColorDepth.Text = "Color depth";
            // 
            // columnBookMarkName
            // 
            this.columnBookMarkName.Text = "Bookmark Name";
            // 
            // columnBookMarkLevel
            // 
            this.columnBookMarkLevel.Text = "Bookmark Level";
            // 
            // columnHidden
            // 
            this.columnHidden.Text = "";
            this.columnHidden.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.columnHidden.Width = 0;
            // 
            // treeView1
            // 
            this.treeView1.AllowDrop = true;
            this.treeView1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.treeView1.CheckBoxes = true;
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView1.DrawMode = System.Windows.Forms.TreeViewDrawMode.OwnerDrawAll;
            this.treeView1.FullRowSelect = true;
            this.treeView1.HideSelection = false;
            this.treeView1.Location = new System.Drawing.Point(0, 20);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(438, 164);
            this.treeView1.TabIndex = 3;
            this.treeView1.DrawNode += new System.Windows.Forms.DrawTreeNodeEventHandler(this.treeView1_DrawNode);
            this.treeView1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.treeView1_MouseUp);
            this.treeView1.DragDrop += new System.Windows.Forms.DragEventHandler(this.treeView1_DragDrop);
            this.treeView1.DragEnter += new System.Windows.Forms.DragEventHandler(this.treeView1_DragEnter);
            this.treeView1.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.treeView1_ItemDrag);
            this.treeView1.Click += new System.EventHandler(this.treeView1_Click);
            // 
            // imageList2
            // 
            this.imageList2.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList2.ImageStream")));
            this.imageList2.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList2.Images.SetKeyName(0, "Blue.bmp");
            this.imageList2.Images.SetKeyName(1, "Green.bmp");
            this.imageList2.Images.SetKeyName(2, "LightBlack.bmp");
            this.imageList2.Images.SetKeyName(3, "Orange.bmp");
            this.imageList2.Images.SetKeyName(4, "Red.bmp");
            this.imageList2.Images.SetKeyName(5, "Violet.bmp");
            this.imageList2.Images.SetKeyName(6, "Yello.bmp");
            // 
            // TreeViewColumns
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.Controls.Add(this.treeView1);
            this.Controls.Add(this.listView1);
            this.Name = "TreeViewColumns";
            this.Size = new System.Drawing.Size(438, 184);
            this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ListView listView1;
		private System.Windows.Forms.ColumnHeader columnTreeStructure;
		private System.Windows.Forms.ColumnHeader columnFileSize;
		private System.Windows.Forms.ColumnHeader columnPages;
		private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.ImageList imageList2;
        private System.Windows.Forms.ColumnHeader columnPaperSize;
        private System.Windows.Forms.ColumnHeader columnOrientation;
        private System.Windows.Forms.ColumnHeader columnColorDepth;
        private System.Windows.Forms.ColumnHeader columnBookMarkName;
        private System.Windows.Forms.ColumnHeader columnBookMarkLevel;
        private System.Windows.Forms.ColumnHeader columnHidden;
	}
}
