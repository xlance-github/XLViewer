using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
namespace XLanceDLL
{
    class Common
    {
        /// <summary>
        /// To get the (*.PDF) files present in the given folder
        /// </summary>
        public static string[] GetFiles(string strFolderPath)
        {
            ArrayList list = new ArrayList();
            if (strFolderPath.IndexOf("System Volume Information") == -1)
            {
                DirectoryInfo dirInfo = new DirectoryInfo(strFolderPath);
                FileInfo[] fileInfos = dirInfo.GetFiles("*.pdf");
                foreach (FileInfo info in fileInfos)
                {
                    // HACK: Just skip the protected samples file...
                    if (info.Name.IndexOf("protected") == -1)
                        list.Add(info.FullName);
                }
            }
            return (string[])list.ToArray(typeof(string));
        }

        public void Tokenize(string stInput, ref string[] stringarray)
        {
            int i = stInput.LastIndexOf(';');

            if (i == -1)
            {
                stringarray = stInput.Split(new string[] { "" }, StringSplitOptions.RemoveEmptyEntries);
                stringarray[0] = "";
                return;
            }
            string tempParent = stInput.Substring(0, i);            
            stInput.ToLower();
            stringarray = tempParent.Split(new string[] { ";" }, StringSplitOptions.RemoveEmptyEntries);
        }
        /// <summary>
        /// Get number of folders in given directory.
        /// </summary>
        public static string[] GetDirectories(string strFolderPath)
        {
            ArrayList list = new ArrayList();

            if (strFolderPath.IndexOf("System Volume Information") == -1)
            {
                DirectoryInfo dirInfo = new DirectoryInfo(strFolderPath);
                DirectoryInfo[] dirInfos = dirInfo.GetDirectories();
                foreach (DirectoryInfo info in dirInfos)
                {
                    // HACK: Just skip the protected samples file...
                    if (info.Name.IndexOf("protected") == -1)
                        list.Add(info.FullName);
                }
            }
            return (string[])list.ToArray(typeof(string));
        }

        public static string GetActualPath(string strPath)
        {
            int i = strPath.IndexOf("###");
            if (i > 0)
            {
                return strPath.Substring(0, i - 1);
            }
            return strPath;
        }
        public static byte[] GetBytes(string str)
        {
            byte[] bytes = new byte[str.Length * sizeof(char)];
            System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }
    }
}
