using System;
using System.Collections;
using System.Windows.Forms;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Collections.Specialized;
using AdvancedDataGridView;

namespace XLanceDLL
{
	/// <summary>
	/// jhTreeViewTools is a namespace for classes around the TreeView control
	/// This (only) one is a class for saving and loading the TreeView 
	/// </summary>
	public class LoadAndSave
    {
        #region Traverse(TreeNodeCollection nodes, ArrayList al)
        private static void Traverse(TreeNodeCollection nodes)
        {
            foreach (TreeNode node in nodes)
            {
                string[] itemString = (string[])node.Tag;
                string str = itemString[8];
                if (str.Length > 0)
                {
                    if (str.Contains("###") == true)
                    {
                        node.ImageIndex = 2;
                        node.SelectedImageIndex = 2;
                    }
                    else if (str == "Untitled")
                    {
                        node.ImageIndex = 1;
                        node.SelectedImageIndex = 1;
                    }
                    else 
                    {
                        node.ImageIndex = 3;
                        node.SelectedImageIndex = 3;
                    }
                }
                Traverse(node.Nodes);
                node.ExpandAll();
            }
        }
        #endregion

        #region Save (saveTree, saveNode)
        /// <summary>
		/// Save the TreeView content
		/// </summary>
		/// <param name="tree"></param>
		/// <param name="filename"></param>
		/// <returns>Errorcode as int</returns>
		public static int saveTree(TreeView tree, string filename)
		{
			// Neues Array anlegen
			ArrayList al = new ArrayList();
           // Traverse(tree.Nodes, ref al);
            foreach (TreeNode tn in tree.Nodes)
            {
                al.Add(tn);
            }

			// Datei anlegen
			Stream file = File.Open(filename, FileMode.Create);
			// Binär-Formatierer init.
			BinaryFormatter bf = new BinaryFormatter();
			try
			{
				// Serialisieren des Arrays
				bf.Serialize(file, al);
			}
			catch (System.Runtime.Serialization.SerializationException e)
			{
				MessageBox.Show("Serialization failed : {0}", e.Message); 
				return -1; // ERROR
			}

			// Datei schliessen
			file.Close();

			return 1; // OKAY
		}

        public static int SaveTreeGridView(TreeGridView tree, string filename)
        {
            // Neues Array anlegen
            ArrayList al = new ArrayList();
            // Traverse(tree.Nodes, ref al);
            foreach (TreeGridNode tn in tree.Nodes)
            {
                al.Add(tn);
            }

            // Datei anlegen
            Stream file = File.Open(filename, FileMode.Create);
            // Binär-Formatierer init.
            BinaryFormatter bf = new BinaryFormatter();
            try
            {
                // Serialisieren des Arrays
                bf.Serialize(file, al);
            }
            catch (System.Runtime.Serialization.SerializationException e)
            {
                MessageBox.Show("Serialization failed : {0}", e.Message);
                return -1; // ERROR
            }

            // Datei schliessen
            file.Close();

            return 1; // OKAY
        }
		#endregion

	
		#region Load (loadTree, searchNode)
		/// <summary>
		/// Load the TreeView content
		/// </summary>
		/// <param name="tree"></param>
		/// <param name="filename"></param>
		/// <returns>Errorcode as int</returns>
		public static int loadTree(TreeView tree, string filename)
		{
			if (File.Exists(filename))
            {
                // Datei öffnen
                Stream file = File.Open(filename, FileMode.Open);
                // Binär-Formatierer init.
                BinaryFormatter bf = new BinaryFormatter();
                // Object var. init.
                object obj = null;
                try
                {
                    // Daten aus der Datei deserialisieren
                    obj = bf.Deserialize(file);
                }
                catch (System.Runtime.Serialization.SerializationException e)
                {
                    MessageBox.Show("De-Serialization failed : {0}", e.Message);
                    return -1;
                }
                // Datei schliessen
                file.Close();

                // Neues Array erstellen
                ArrayList nodeList = obj as ArrayList;
                try
                {
                    // load Root-Nodes
                    foreach (TreeNode node in nodeList)
                    {
                        tree.Nodes.Add(node);
                    }
                    TreeNodeCollection Nodes = tree.Nodes;
                    Traverse(Nodes);
                }
                catch (Exception ex)
                {
                }
                return 0;
            }
			else return -2; // File existiert nicht
		}
    	#endregion
	}
}