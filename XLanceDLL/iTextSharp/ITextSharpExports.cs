using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using iTextSharp.text;
using iTextSharp.text.pdf;
using System;
using System.Collections;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace XLanceDLL
{
    class CItextSharpExports
    {
        public int m_iPageCount;
        public double m_iFileSize;
        public int m_iPaperSize;
        public PdfReader m_inputFilePdfReader;
        public string m_strLastError;

        /// <summary>
        /// C'tor
        /// </summary>
        /// <param name="strFilePath"></param>
        public CItextSharpExports(string strFilePath)
        {
            LoadFileInformation(strFilePath);
        }

        /// <summary>
        /// to load the file information
        /// </summary>
        /// <param name="strFilePath"></param>
        public void LoadFileInformation(string strFilePath)
        {
            try
            {
                // open the file and read the size
                FileInfo fi = new FileInfo(strFilePath);
                m_iFileSize = fi.Length;

                m_inputFilePdfReader = new PdfReader(strFilePath);
                m_iPageCount = m_inputFilePdfReader.NumberOfPages;
            }
            catch (Exception ex)
            {
                m_iPageCount = -1;
                m_strLastError = ex.Message;
                // MessageBox.Show("Exception in LoadFileInformation" + strFilePath+ " " + ex.Message);
            }
        }
        /// <summary>
        /// will check is valid number or not
        /// </summary>
        /// <param name="text"></param>
        /// <returns></returns>
        public static bool IsNumber(string text)
        {
            Regex regex = new Regex(@"^[-+]?[0-9]*\.?[0-9]+$");
            return regex.IsMatch(text);
        }

        /// <summary>
        /// Will save the tree structure in the pdf file
        /// </summary>
        /// <param name="bListFilePages"></param>
        /// <param name="strOutputPath"></param>
        /// <returns></returns>
        public static bool SaveTreeStructureToPDFOld(Dictionary<int, Dictionary<string, string>> bListFilePages, string strOutputPath)
        {

            //TestCreatePDF();
            //return false;

            bool bReturn = false;
            string strPreviousFile = "previous";
            PdfReader reader = null;
            Document inputDocument = null;
            Document outputDocument = new Document();
            PdfCopy pdfCopyProvider = null;
            PdfImportedPage importedPage = null;
            FileStream objFileStream = new System.IO.FileStream(strOutputPath, System.IO.FileMode.Create);
            PdfWriter pdfWriter = PdfWriter.GetInstance(outputDocument, objFileStream);
            outputDocument.Open();

            pdfWriter.ViewerPreferences = PdfWriter.PageModeUseOutlines;
            PdfOutline rootOutline = null;

            int iCount = 0;
            // Iterate files
            for (int iIndex = 0; iIndex < bListFilePages.Count; iIndex++)
            {
                string strCurrentFile = "Current";
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
                        int iPageNumber = 1;

                        // For simplicity, I am assuming all the pages share the same size
                        // and rotation as the first page:
                        if (iCount == 0)
                        {
                            strCurrentFile = GetFirstFileFromTree(iIndex, bListFilePages);
                            if (strCurrentFile != strPreviousFile)
                            {
                                if (reader != null)
                                {
                                    reader.Close();
                                    reader = null;
                                }
                                reader = new PdfReader(strCurrentFile);
                            }

                            inputDocument = new Document(reader.GetPageSizeWithRotation(iPageNumber));
                            // Initialize an instance of the PdfCopyClass with the source 
                            // document and an output file stream:
                            pdfCopyProvider = new PdfCopy(inputDocument,
                                objFileStream);

                            inputDocument.Open();
                            iCount++;
                        }

                        int i = tempPath.IndexOf("#@$");
                        if (i != -1)
                        {
                            strFilePath = tempPath.Substring(0, i - 1);
                            strPageNumber = tempPath.Substring(i + 3);

                            strCurrentFile = strFilePath;
                            if (strCurrentFile != strPreviousFile)
                            {
                                if (reader != null)
                                {
                                    reader.Close();
                                    reader = null;
                                }
                                reader = new PdfReader(strCurrentFile);
                            }

                            if (IsNumber(strPageNumber))
                            {
                                if (strFilePath.Length > 0 && strPageNumber.Length > 0)
                                {
                                    iPageNumber = Convert.ToInt16(strPageNumber);
                                    importedPage = pdfCopyProvider.GetImportedPage(reader, iPageNumber);
                                    pdfCopyProvider.Add(new Chunk("Chapter 1").SetLocalDestination("1"));
                                    pdfCopyProvider.AddPage(importedPage);
                                }
                            }
                            //Set as previous file here
                            strPreviousFile = strCurrentFile;
                        }
                        else
                        {
                            if (File.Exists(tempPath))
                            {
                                if (tempPath != strPreviousFile)
                                {
                                    if (reader != null)
                                    {
                                        reader.Close();
                                        reader = null;
                                    }
                                    reader = new PdfReader(tempPath);
                                }
                                importedPage = pdfCopyProvider.GetImportedPage(reader, 1);
                                pdfCopyProvider.AddPage(importedPage);

                                //Set as previous file here
                                strPreviousFile = tempPath;
                            }
                            else//Means that it is a foler
                            {
                                string strFile = GetFirstFileFromTree(iIndex, bListFilePages);
                                if (strFile != strPreviousFile)
                                {
                                    if (reader != null)
                                    {
                                        reader.Close();
                                        reader = null;
                                    }
                                    reader = new PdfReader(strFile);
                                }
                                importedPage = pdfCopyProvider.GetImportedPage(reader, 1);
                                pdfCopyProvider.AddPage(importedPage);

                                //Set as previous file here
                                strPreviousFile = strFile;
                            }
                        }

                        if (strBookMarkName.Length > 0)
                        {
                            // Create first page
                            PdfContentByte cb = pdfWriter.DirectContent;
                            PdfOutline root = cb.RootOutline;

                            ArrayList outlineCollection = root.Kids;
                            PdfOutline outline = CheckIfOutlineISPresent(strBookMarkName, outlineCollection);

                            if (rootOutline != null)
                            {
                                rootOutline = new PdfOutline(rootOutline, PdfAction.GotoLocalPage("2", false), strBookMarkName);
                            }
                            else
                            {
                                rootOutline = new PdfOutline(root, PdfAction.GotoLocalPage("1", false), "Chapter 1");
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                }
            }

            if (inputDocument.IsOpen())
            {
                inputDocument.Close();
            }
            if (inputDocument != null)
            {
                inputDocument = null;
            }
            if (outputDocument.IsOpen())
            {
                outputDocument.Close();
            }
            if (outputDocument != null)
            {
                outputDocument = null;
            }

            if (reader != null)
            {
                reader.Close();
                reader = null;
            }
            if (objFileStream != null)
            {
                objFileStream.Close();
                objFileStream = null;
            }

            try
            {
                //if (bReturn)
                //    outputDocument.Save(strOutputPath);
            }
            catch (Exception ex)
            {
                //bReturn = false;
                //MessageBox.Show(ex.Message);
            }
            return true;

        }

        /// <summary>
        /// Will get the first page occurance in the tree from provided start location
        /// </summary>
        /// <param name="iStartIndex"></param>
        /// <param name="bListFilePages"></param>
        /// <returns></returns>
        public static string GetFirstFileFromTree(int iStartIndex, Dictionary<int, Dictionary<string, string>> bListFilePages)
        {
            string strReturn = string.Empty;
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
                            strReturn = tempPath;
                            break;
                        }
                    }
                    strFullPath = tempPath;
                }
                catch (Exception ex)
                {

                }
            }
            return strReturn;
        }

        /// <summary>
        /// Check if outline is present
        /// </summary>
        /// <param name="strBookMarkName"></param>
        /// <param name="outlineCollection"></param>
        /// <returns></returns>
        public static PdfOutline CheckIfOutlineISPresent(string strBookMarkName, ArrayList outlineList)
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

                foreach (PdfOutline outline in outlineList)
                {
                    if (outline.Title == tempParent)
                    {
                        return outline;
                    }
                    else
                    {
                        PdfOutline outlineTemp = CheckIfOutlineISPresent(strBookMarkName, outline.Kids);
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

        /// <summary>
        /// TO get the file size
        /// </summary>
        /// <param name="strFilePath"></param>
        /// <returns></returns>
        public double GetFileSize(string strFilePath)
        {
            return m_iFileSize;
        }

        /// <summary>
        /// To get the page count
        /// </summary>
        /// <param name="strFilePath"></param>
        /// <returns></returns>
        public int GetPageCount(string strFilePath)
        {
            return m_iPageCount;
        }

        /// <summary>
        /// To get the paper size
        /// </summary>
        /// <param name="strFilePath"></param>
        /// <returns></returns>
        public int GetPaperSize(string strFilePath)
        {
            return m_iPaperSize;
        }

        /// <summary>
        /// Return's if last error occured 
        /// </summary>
        /// <returns></returns>
        public string GetLastError()
        {
            return m_strLastError;
        }

        /// <summary>
        /// To get the page width
        /// </summary>
        /// <param name="iPageIndex"></param>
        /// <returns></returns>
        public int GetPageWidth(int iPageIndex)
        {
            try{
                return (int)m_inputFilePdfReader.GetPageSizeWithRotation(iPageIndex).Width;
            }
            catch (Exception ex) { return 0; }
        }

        /// <summary>
        /// to get the page height
        /// </summary>
        /// <param name="iPageIndex"></param>
        /// <returns></returns>
        public int GetPageHeight(int iPageIndex)
        {
            try{
                return (int)m_inputFilePdfReader.GetPageSizeWithRotation(iPageIndex).Height;
            }
            catch (Exception ex) { return 0; }
        }

        public int GetColorMode()
        {
            return 0;//m_inputFilePdfReader..Options.ColorMode;
        }

        public static void ExtractPage(string sourcePdfPath, string outputPdfPath,
                int pageNumber, string password)
        {
            PdfReader reader = null;
            Document document = null;
            PdfCopy pdfCopyProvider = null;
            PdfImportedPage importedPage = null;

            try
            {
                // Intialize a new PdfReader instance with the contents of the source Pdf file:
                reader = new PdfReader(sourcePdfPath);

                // Capture the correct size and orientation for the page:
                document = new Document(reader.GetPageSizeWithRotation(pageNumber));

                // Initialize an instance of the PdfCopyClass with the source 
                // document and an output file stream:
                pdfCopyProvider = new PdfCopy(document,
                    new System.IO.FileStream(outputPdfPath, System.IO.FileMode.Create));

                document.Open();

                // Extract the desired page number:
                importedPage = pdfCopyProvider.GetImportedPage(reader, pageNumber);
                pdfCopyProvider.AddPage(importedPage);
                document.Close();
                reader.Close();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static bool SaveTreeStructureToPDF(Dictionary<int, Dictionary<string, string>> bListFilePages, string strOutputPath)
        {
            bool bReturn = false;
            string strPreviousFile = "previous";
            string strCurrentFile = "Current";

            string OutPutfile = strOutputPath;
            PdfReader inputReader = null;
            PdfOutline outline = null;
            int iPageCount = 1;
            try
            {
                Document outputDoc = new Document();
                using (MemoryStream ms = new MemoryStream())
                {
                    PdfWriter writer = PdfWriter.GetInstance(outputDoc, ms);
                    outputDoc.Open();
                    outputDoc.AddDocListener(writer);
                    for (int iIndex = 0; iIndex < bListFilePages.Count; iIndex++)
                    {
                        Dictionary<string, string> innerDictionary = bListFilePages.ElementAt(iIndex).Value;//.Value.ToString();
                        string strBookMarkName = innerDictionary.ElementAt(0).Key.ToString();
                        string tempPath = innerDictionary.ElementAt(0).Value.ToString();

                        if (tempPath.Length > 0)
                        {
                            string strFilePath;
                            string strPageNumber;
                            PdfImportedPage page = null;
                            int iPageNumber = 1;

                            int i = tempPath.IndexOf("#@$");
                            if (i != -1)
                            {
                                strFilePath = tempPath.Substring(0, i - 1);
                                strPageNumber = tempPath.Substring(i + 3);

                                strCurrentFile = strFilePath;
                                if (strCurrentFile != strPreviousFile)
                                {
                                    if (inputReader != null)
                                    {
                                        inputReader.Close();
                                        inputReader = null;
                                    }
                                    inputReader = new PdfReader(strCurrentFile);
                                }

                                if (IsNumber(strPageNumber))
                                {
                                    if (strFilePath.Length > 0 && strPageNumber.Length > 0)
                                    {
                           
                                        iPageNumber = Convert.ToInt16(strPageNumber);
                                        outputDoc.SetPageSize(inputReader.GetPageSize(iPageNumber));
                                        PdfContentByte cb = writer.DirectContent;
                                        page = writer.GetImportedPage(inputReader, iPageNumber);

                                        ///creat bookmark here and add in template
                                        PdfOutline root = cb.RootOutline;
                                        ArrayList outlineCollection = root.Kids;
                                        outline = CheckIfOutlineISPresent(strBookMarkName, outlineCollection);
                                        if (outline == null)
                                        {
                                            outline = new PdfOutline(root, PdfAction.GotoLocalPage(iPageCount, new iTextSharp.text.pdf.PdfDestination(iTextSharp.text.pdf.PdfDestination.FITH), writer), strBookMarkName);
                                        }
                                        else
                                        {
                                            outline = new PdfOutline(outline, PdfAction.GotoLocalPage(iPageCount, new iTextSharp.text.pdf.PdfDestination(iTextSharp.text.pdf.PdfDestination.FITH), writer), strBookMarkName);
                                        }

                                        outputDoc.NewPage();
                                        Rectangle psize = inputReader.GetPageSizeWithRotation(iPageNumber);
                                        switch (psize.Rotation)
                                        {
                                            case 0:
                                                cb.AddTemplate(page, 1f, 0, 0, 1f, 0, 0);
                                                break;
                                            case 90:
                                                cb.AddTemplate(page, 0, -1f, 1f, 0, 0, psize.Height);
                                                break;
                                            case 180:
                                                cb.AddTemplate(page, -1f, 0, 0, -1f, 0, 0);
                                                break;
                                            case 270:
                                                cb.AddTemplate(page, 0, 1.0F, -1.0F, 0, psize.Width, 0);
                                                break;
                                            default:
                                                break;
                                        }
                                        iPageCount++;
                                    }
                                }
                                strPreviousFile = strCurrentFile;
                            }
                            else
                            {
                                if (File.Exists(tempPath))
                                {
                                    strCurrentFile = tempPath;
                                }
                                else
                                {
                                    strCurrentFile = GetFirstFileFromTree(iIndex, bListFilePages);
                                }

                                if (!File.Exists(strCurrentFile))
                                {
                                    continue;
                                }

                                iPageNumber = 1;
                                PdfContentByte cb = writer.DirectContent;

                                ///creat bookmark here and add in template
                                PdfOutline root = cb.RootOutline;
                                ArrayList outlineCollection = root.Kids;
                                outline = CheckIfOutlineISPresent(strBookMarkName, outlineCollection);
                                if (outline == null)
                                {
                                    outline = new PdfOutline(root, PdfAction.GotoLocalPage(iPageCount, new iTextSharp.text.pdf.PdfDestination(iTextSharp.text.pdf.PdfDestination.FITH), writer), strBookMarkName);
                                }
                                else
                                {
                                    outline = new PdfOutline(outline, PdfAction.GotoLocalPage(iPageCount, new iTextSharp.text.pdf.PdfDestination(iTextSharp.text.pdf.PdfDestination.FITH), writer), strBookMarkName);
                                }
                            }
                        }
                    }
                    if (inputReader != null)
                    {
                        inputReader.Close();
                        inputReader = null;
                    }
                    outputDoc.Close();
                    using (FileStream fs = new FileStream(OutPutfile, FileMode.Create))
                    {
                        // this is the part stumping me; I need to use a PdfStamper to write 
                        // out some values to fields on the form AFTER the pages are removed.
                        // This works, but there doesn't seem to be a form on the copied page...
                        PdfStamper stamper = new PdfStamper(new PdfReader(ms.ToArray()), fs);
                        // write out fields here...
                        stamper.FormFlattening = true;
                        stamper.SetFullCompression();
                        stamper.Close();
                    }
                    bReturn = true;
                }
            }
            catch (Exception ex)
            {
                bReturn = false;
            }
            return bReturn;
        }

        public static void TestCreatePDF1()
        {
            try
            {
                Document document = new Document();

                FileStream objFileStream = new System.IO.FileStream("Output.pdf", System.IO.FileMode.Create);
                PdfWriter writer = PdfWriter.GetInstance(document,
                    objFileStream);
                document.Open();
                document.AddDocListener(writer);

                PdfReader reader = new PdfReader("Input.pdf");

                Document inputDocument = null;
                PdfCopy pdfCopyProvider = null;

                inputDocument = new Document(reader.GetPageSizeWithRotation(1));
                pdfCopyProvider = new PdfCopy(inputDocument,
                    objFileStream);

                inputDocument.Open();

                PdfImportedPage importedPage = null;
                importedPage = pdfCopyProvider.GetImportedPage(reader, 1);
                pdfCopyProvider.Add(new Chunk("Chapter 1").SetLocalDestination("1"));
                pdfCopyProvider.AddPage(importedPage);

                // Code 2
                PdfContentByte cb = writer.DirectContent;
                PdfOutline root = cb.RootOutline;

                // Code 3
                PdfOutline oline1 = new PdfOutline(root,
                PdfAction.GotoLocalPage(1, new iTextSharp.text.pdf.PdfDestination(iTextSharp.text.pdf.PdfDestination.FITH), writer),
                                    "Chapter 1");


                reader.Close();
                //writer.Close();
                inputDocument.Close();
                //objFileStream.Close();
                //document.Close();
            }
            catch (Exception ex)
            {

            }
        }
    }
}
