using System;
using System.Drawing;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;

using System.ComponentModel;

namespace TreeViewColumnsProject
{
	public partial class TreeViewColumns : UserControl
	{
        private TreeNode tn;

		public TreeViewColumns()
		{
			InitializeComponent();

			this.BackColor = VisualStyleInformation.TextControlBorder;
			this.Padding = new Padding(1);
		}

		[Description("TreeView associated with the control"), Category("Behavior")]
		public TreeView TreeView
		{
			get
			{
				return this.treeView1;
			}
		}

		[Description("Columns associated with the control"), Category("Behavior")]
		public ListView.ColumnHeaderCollection Columns
		{
			get
			{
				return this.listView1.Columns;
			}
		}

		private void listView1_ColumnClick(object sender, ColumnClickEventArgs e)
		{
			this.treeView1.Focus();
		}

		private void treeView1_Click(object sender, EventArgs e)
		{

		}

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
                }
                this.treeView1.SelectedNode.BackColor = bgColor;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
		private void listView1_ColumnWidthChanged(object sender, ColumnWidthChangedEventArgs e)
		{
			this.treeView1.Focus();
			this.treeView1.Invalidate();
		}

		private void listView1_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
		{
			this.treeView1.Focus();
			this.treeView1.Invalidate();
		}

		private void treeView1_DrawNode(object sender, DrawTreeNodeEventArgs e)
		{
            e.DrawDefault = true;

            Rectangle rect = e.Bounds;

            if ((e.State & TreeNodeStates.Selected) != 0)
            {
                if ((e.State & TreeNodeStates.Focused) != 0)
                    e.Graphics.FillRectangle(SystemBrushes.Highlight, rect);
                else
                    e.Graphics.FillRectangle(SystemBrushes.Control, rect);
            }
            else
                e.Graphics.FillRectangle(Brushes.White, rect);

            e.Graphics.DrawRectangle(SystemPens.Control, rect);

            for (int intColumn = 1; intColumn < this.listView1.Columns.Count; intColumn++)
            {
                rect.Offset(this.listView1.Columns[intColumn - 1].Width, 0);
                rect.Width = this.listView1.Columns[intColumn].Width;

                e.Graphics.DrawRectangle(SystemPens.Control, rect);

                string strColumnText;
                string[] list = e.Node.Tag as string[];
                if (list != null && intColumn <= list.Length)
                    strColumnText = list[intColumn - 1];
                else
                    strColumnText = intColumn + " " + e.Node.Text; // dummy

                TextFormatFlags flags = TextFormatFlags.EndEllipsis;
                switch (this.listView1.Columns[intColumn].TextAlign)
                {
                    case HorizontalAlignment.Center:
                        flags |= TextFormatFlags.HorizontalCenter;
                        break;
                    case HorizontalAlignment.Left:
                        flags |= TextFormatFlags.Left;
                        break;
                    case HorizontalAlignment.Right:
                        flags |= TextFormatFlags.Right;
                        break;
                    default:
                        break;
                }

                rect.Y++;
                if ((e.State & TreeNodeStates.Selected) != 0 &&
                    (e.State & TreeNodeStates.Focused) != 0)
                    TextRenderer.DrawText(e.Graphics, strColumnText, e.Node.NodeFont, rect, SystemColors.HighlightText, flags);
                else
                    TextRenderer.DrawText(e.Graphics, strColumnText, e.Node.NodeFont, rect, e.Node.ForeColor, e.Node.BackColor, flags);
                rect.Y--;
            }
		}
        private void treeView1_ItemDrag(object sender, System.Windows.Forms.ItemDragEventArgs e)
        {
            tn = e.Item as TreeNode;
            DoDragDrop(e.Item.ToString(), DragDropEffects.Move);
        }

        private void treeView1_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
        {
            Point pt = new Point(e.X, e.Y);
            pt = treeView1.PointToClient(pt);
            TreeNode ParentNode = treeView1.GetNodeAt(pt);
            ParentNode.Nodes.Add(tn.Text); // this copies the node
            tn.Remove(); // need to remove the original version of the node
        }

        private void treeView1_DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
        {
            e.Effect = DragDropEffects.Move;
        }
        private void treeView1_MouseUp(object sender, MouseEventArgs e)
        {
            Point p = this.treeView1.PointToClient(Control.MousePosition);
            TreeNode tn = this.treeView1.GetNodeAt(p);
            if (tn != null)
                this.treeView1.SelectedNode = tn;
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
                    m.Show(treeView1, new Point(e.X, e.Y));
                    m.ItemClicked += new ToolStripItemClickedEventHandler(contexMenuuu_ItemClicked);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

	}
}
