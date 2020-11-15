using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PdfSharp.Pdf;
using PdfSharp.Pdf.IO;
using System.IO;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace XLanceDLL
{
    class PdfSharpExports
    {
        public int iPageCount;
        public double iFileSize;
        public int iPaperSize;
        public PdfDocument inputDocument1;
        public string m_strLastError;

        public PdfSharpExports(string strFilePath)
        {
            LoadFileInformation(strFilePath);
        }
        
        public void LoadFileInformation(string strFilePath)
        {
            try
            {
                // open the file and read the size
                FileInfo fi = new FileInfo(strFilePath);
                iFileSize = fi.Length;

                inputDocument1 = PdfReader.Open(strFilePath, PdfDocumentOpenMode.Import);
                iPageCount = inputDocument1.PageCount;  
            }
            catch (Exception ex)
            {
                iPageCount = -1;
                m_strLastError = ex.Message;
               // MessageBox.Show("Exception in LoadFileInformation" + strFilePath+ " " + ex.Message);
            }
        }

        public static bool IsNumber(string text)
        {
            Regex regex = new Regex(@"^[-+]?[0-9]*\.?[0-9]+$");
            return regex.IsMatch(text);
        }

        public static bool SaveTreeStructureToPDF(Dictionary<int, Dictionary<string, string>> bListFilePages, string strOutputPath)
        {
            bool bReturn = false;
            string strPreviousFile = "previous";
            string strCurrentFile = "Current";

                // Open the output document
                PdfDocument outputDocument = new PdfDocument();
                PdfDocument inputDocument = new PdfDocument();

                // Iterate files
                for (int iIndex = 0; iIndex < bListFilePages.Count; iIndex++)
                {
                    try
                    {
                        Dictionary<string, string> innerDictionary = bListFilePages.ElementAt(iIndex).Value;//.Value.ToString();
                        string strBookMarkName = innerDictionary.ElementAt(0).Key.ToString();
                        string tempPath = innerDictionary.ElementAt(0).Value.ToString();

                        if (tempPath.Length > 0)
                        {
                            string strFilePath;
                            string strPageNumber;
                            PdfPage page = null;
                            int iPageNumber = 0;


                            int i = tempPath.IndexOf("#@$");
                            if (i != -1)
                            {
                                strFilePath = tempPath.Substring(0, i - 1);
                                strPageNumber = tempPath.Substring(i + 3);

                                strCurrentFile = strFilePath;
                                if (strCurrentFile != strPreviousFile)
                                    inputDocument = PdfReader.Open(strCurrentFile, PdfDocumentOpenMode.Import);

                                if (IsNumber(strPageNumber))
                                {
                                    if (strFilePath.Length > 0 && strPageNumber.Length > 0)
                                    {
                                        iPageNumber = Convert.ToInt16(strPageNumber);
                                        // ...and add it to the output document.
                                        page = outputDocument.AddPage(inputDocument.Pages[iPageNumber - 1]);
                                    }
                                }
                                strPreviousFile = strCurrentFile;
                            }
                            else
                            {
                                if (File.Exists(tempPath))
                                {
                                    inputDocument = PdfReader.Open(tempPath, PdfDocumentOpenMode.Import);
                                    page = outputDocument.AddPage(inputDocument.Pages[iPageNumber]);
                                }
                                else
                                {
                                    page = GetFirstFilePageFromTree(iIndex, bListFilePages);
                                    page = outputDocument.AddPage(page);
                                }
                            }

                            if (strBookMarkName.Length > 0)
                            {
                                // Create first page
                                PdfOutline.PdfOutlineCollection outlineCollection = outputDocument.Outlines;
                                PdfOutline outline = CheckIfOutlineISPresent(strBookMarkName, outlineCollection);

                                if (outline != null)
                                {
                                    outline.Outlines.Add(strBookMarkName, page, true,
                                      PdfOutlineStyle.Bold, PdfSharp.Drawing.XColors.Black);
                                }
                                else
                                {
                                    // Create the root bookmark. You can set the style and the color.
                                    outputDocument.Outlines.Add(strBookMarkName, page, true,
                                     PdfOutlineStyle.Bold, PdfSharp.Drawing.XColors.Black);
                                }
                                bReturn = true;
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                    }
                }

                try
                {
                    if (bReturn)
                        outputDocument.Save(strOutputPath);
                }
                catch (Exception ex)
                {
                    bReturn = false;
                    MessageBox.Show(ex.Message);
                } 
            return bReturn;
        }

        /// <summary>
        /// Ger the first page for the first pdf file find in the tree path.
        /// </summary>
        /// <param name="bListFilePages"></param>
        /// <param name="strOutputPath"></param>
        /// <returns></returns>
        public static PdfPage GetFirstFilePageFromTree(int iStartIndex, Dictionary<int, Dictionary<string, string>> bListFilePages)
        {
            PdfPage pageReturn = new PdfPage();
            PdfDocument InputDoc = new PdfDocument();
            string strFullPath = string.Empty;
            // Iterate files
            for (int iIndex = iStartIndex; iIndex < bListFilePages.Count; iIndex++)
            {
                try
                {
                    Dictionary<string, string> innerDictionary = bListFilePages.ElementAt(iIndex).Value;
                    string tempPath = innerDictionary.ElementAt(0).Value.ToString();
                    if (tempPath.Contains(strFullPath) == true)
                    {
                        if (File.Exists(tempPath))
                        {
                            InputDoc = PdfReader.Open(tempPath, PdfDocumentOpenMode.Import);
                            pageReturn = InputDoc.Pages[0];
                            break;
                        }
                    }
                    strFullPath = tempPath;
                }
                catch (Exception ex)
                {

                }
            }
            return pageReturn;
        }

        public static PdfOutline CheckIfOutlineISPresent(string strBookMarkName, PdfOutline.PdfOutlineCollection outlineCollection)
        {            
            string tempParent = "";
            try
            {
                int i = strBookMarkName.LastIndexOf('\\');
                if (i == -1)
                    return null;

                tempParent = strBookMarkName.Substring(0, i);
                if (tempParent == null)
                    return null;

                foreach (PdfOutline outline in outlineCollection)
                {
                    if (outline.Title == tempParent)
                    {
                        return outline;
                    }
                    else
                    {
                        PdfOutline outlineTemp = CheckIfOutlineISPresent(strBookMarkName, outline.Outlines);
                        if (outlineTemp != null)
                        {
                            return outlineTemp;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            return null;
        }

        public double GetFileSize(string strFilePath)
        {
            return iFileSize;
        }
        public int GetPageCount(string strFilePath)
        {
            return iPageCount;
        }
        public int GetPaperSize(string strFilePath)
        {
            return iPaperSize;
        }
        public string GetLastError()
        {
               return m_strLastError;
        }
        public int GetPageWidth(int iPageIndex)
        {
             return inputDocument1.Pages[iPageIndex].Width;
        }
        public int GetPageHeight(int iPageIndex)
        {
            return pdfExport.inputDocument1.Pages[iPageIndex].Height;
        }
    }
}



//// Create a new PDF document
//PdfDocument document = new PdfDocument();
//// Create a font
//XFont font = new XFont("Verdana", 16);
 
//// Create first page
//PdfPage page = document.AddPage();
//XGraphics gfx = XGraphics.FromPdfPage(page);
//gfx.DrawString("Page 1", font, XBrushes.Black, 20, 50, XStringFormat.Default);
 
//// Create the root bookmark. You can set the style and the color.
//PdfOutline outline = document.Outlines.Add("Root", page, true,
//  PdfOutlineStyle.Bold, XColors.Red);
 
//// Create some more pages
//for (int idx = 2; idx <= 5; idx++)
//{
//  page = document.AddPage();
//  gfx = XGraphics.FromPdfPage(page);
 
//  string text = "Page " + idx.ToString();
//  gfx.DrawString(text, font, XBrushes.Black, 20, 50, XStringFormat.Default);
 
//  // Create a sub bookmark
//  outline.Outlines.Add(text, page, true);
//}
 
//// Save the document...
//string filename = "Bookmarks.pdf";
//document.Save(filename);
//// ...and start a viewer.
//Process.Start(filename);