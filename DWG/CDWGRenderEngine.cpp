///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor      
// Purpose:   CDWGRenderEngine object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include <fstream>
#include <math.h>
#include "CDWGRenderEngine.h"
#include "ACColorTable.h"
#include "..\CadImport\sg.h"
#include "..\XLanceView.h"
#include "..\Common\FileIO.h"
#include "..\Resource\Resource.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Object constants
///////////////////////////////////////////////////////////////////////////////

#define HEADER_VERSION_LENGTH				6

#define ACVERSION_R13						_T("AC1012")
#define ACVERSION_R14						_T("AC1014")
#define ACVERSION_R15						_T("AC1015")
#define ACVERSION_UNKNOWN1					_T("AC1016")
#define ACVERSION_UNKNOWN2					_T("AC1017")
#define ACVERSION_R16						_T("AC1018")
#define ACVERSION_UNKNOWN3					_T("AC1019")
#define ACVERSION_UNKNOWN4					_T("AC1020")
#define ACVERSION_R17						_T("AC1021")
#define ACVERSION_NAME_R13					_T("R13")
#define ACVERSION_NAME_R14					_T("R14")
#define ACVERSION_NAME_R15					_T("R15")
#define ACVERSION_NAME_R16					_T("R16")
#define ACVERSION_NAME_R17					_T("R17")

#define STRING_FORMAT_DWGINFORMATION		_T("Filename: \t %s\r\n\r\nSize: \t\t %u byte(s)\r\n\r\nAutoCad Version: %s\r\n\r\nObject Count: \t %ld\r\n\r\nLayer Count: \t %ld\r\n\r\nLayers:\r\n\r\n\t%s")

#define ZOOM_LOWEST_ALLOWED					10
#define ZOOM_INCREMENT						10

#define COUNT_ADDITIONAL_LOADING_STEPS		2

///////////////////////////////////////////////////////////////////////////////
// Structures used by this object
///////////////////////////////////////////////////////////////////////////////

struct FLOATRECT
{
	double left;
	double top;
	double right;
	double bottom;

	FLOATRECT()
	{
		left = top = right = bottom = 0.0f;
	}
};

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor, initializes all fields to their defaults.
 */
CDWGRenderEngine::CDWGRenderEngine()
{
	// initialize fields to their defaults
	m_pllstACColorTable = new LinkedListEx<ACCOLOR_ENTRY>();
	m_pllstLayers = new LinkedListEx<DWGLAYERINFO>();
	m_hmodCADImporter = NULL;
	m_hCADImporterDrawing = NULL;
	m_iZoomFactor = 100;
	m_lEntityCount  = 0L;

	// alleviate setting of objects and several other pitfalls...
	//memset(&m_dwgdatThis, 0, sizeof(Dwg_Data));
	CADEnum = 0L;
	CADCreate = 0L;
	CADClose = 0L;
	CADIs3D = 0L;
	CADGetLastError = 0L;
	CADProhibitCurvesAsPoly = 0L;
	CADLayoutCurrent = 0L;
	CADLayoutName = 0L;
	CADLayoutCount = 0L;
	CADGetBox = 0L;
	CADSetSHXOptions = 0L;
	CADDraw = 0L;
	CADLayer = 0L;
	CADLayerCount = 0L;
	CADVisible = 0L;
	CADGetSection = 0L;

	m_hwndOutputControl = NULL;
	m_hwndProgressControl = NULL;
	m_strLastError = EMPTY_STRING;
	m_strFilename = EMPTY_STRING;
	m_strMessage = EMPTY_STRING;
	m_bDrawingUsesBlack = FALSE;
}

/**
 * Constructor which accepts a DWG filename.
 */
CDWGRenderEngine::CDWGRenderEngine(TCHAR *tstrDWGFilename)
{
	// initialize fields to their defaults
	m_pllstACColorTable = new LinkedListEx<ACCOLOR_ENTRY>();
	m_pllstLayers = new LinkedListEx<DWGLAYERINFO>();
	m_hmodCADImporter = NULL;
	m_hCADImporterDrawing = NULL;
	m_iZoomFactor = 100;
	m_lEntityCount = 0L;

	// alleviate setting of objects and several other pitfalls...
	CADEnum = 0L;
	CADCreate = 0L;
	CADClose = 0L;
	CADIs3D = 0L;
	CADGetLastError = 0L;
	CADProhibitCurvesAsPoly = 0L;
	CADLayoutCurrent = 0L;
	CADLayoutName = 0L;
	CADLayoutCount = 0L;
	CADGetBox = 0L;
	CADSetSHXOptions = 0L;
	CADDraw = 0L;
	CADLayer = 0L;
	CADLayerCount = 0L;
	CADVisible = 0L;
	CADGetSection = 0L;

	m_hwndOutputControl = NULL;
	m_hwndProgressControl = NULL;
	m_strLastError = EMPTY_STRING;
	m_strFilename = EMPTY_STRING;
	m_strMessage = EMPTY_STRING;
	m_bDrawingUsesBlack = FALSE;

	// attempt to initialize drawing object
	loadDrawing(tstrDWGFilename);
}

/**
 * Destructor, performs clean-up.
 */
CDWGRenderEngine::~CDWGRenderEngine()
{
	// garbage collect color table list
	if(m_pllstACColorTable)
	{
		m_pllstACColorTable->clear();

		// destroy list
		delete m_pllstACColorTable;
		m_pllstACColorTable = NULL;
	}
	if(m_pllstLayers)
	{
		// clear list
		m_pllstLayers->clear();

		// destroy list
		delete m_pllstLayers;
		m_pllstLayers = NULL;
	}
	// CAD Importer library
	if(m_hmodCADImporter)
		FreeLibrary(m_hmodCADImporter);
}

///////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Clears the current drawing and attempts to load the one specified.
 *
 * @param tstrNewFilename
 *
 * @return
 */
BOOL CDWGRenderEngine::setFilename(TCHAR *tstrNewFilename)
{
	BOOL bReturn = TRUE;

	try
	{
		// leave processing to loadDrawing()
		bReturn = loadDrawing(tstrNewFilename);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("An unexpected error occurred while attempting to select the file specified.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Outputs the active DWG's information to a string. The calling function
 * method, etc. is responsible for freeing the allocated storage.
 *
 * @note The information which is retrieved for the active drawing is
 * the following:
 *
 *		1) Filename
 *		2) File size
 *		3) AutoCad version
 *		4) Number of layers
 *		5) Layer names
 *		6) Total number of drawing objects 
 *
 *
 * @param ptcOutString out param which on return points to a buffer
 * containing the active drawing's information.
 *
 * @return TRUE if the active drawing's information is retrieved, otherwise
 * FALSE.
 */
BOOL CDWGRenderEngine::getDWGInformation(TCHAR *&ptcOutString)
{
	HANDLE hActiveDWG = NULL;
	TCHAR *ptcVersion = NULL;
	BOOL bReturn = FALSE;

	try
	{
		tstring strFileTitle = EMPTY_STRING,
					strLayers = EMPTY_STRING;
		long lIndex = -1L,
			 lLayerCount = 0L,
			 lObjectCount = 0L,
			 lEstCombinedLength = 0L;
		DWORD dwFileSize = (DWORD)0;

		// validate in/out param
		if(ptcOutString != NULL)
			return FALSE;

		// make sure there is an active drawing
		if(m_hCADImporterDrawing == NULL || m_strFilename.length() == 0)
			return FALSE;

		// check relevant function pointers
		if(CADLayerCount == 0L || CADLayer == 0L || CADGetSection == 0L)
		{
			// set last error
			m_strLastError = _T("One or more of the function pointers for manipulating the drawing is invalid.");

			// return fail val
			return FALSE;
		}

		// Get file title
		lIndex = m_strFilename.find_last_of(_T('\\'), m_strFilename.length());
		if(lIndex > -1L)
			strFileTitle = m_strFilename.substr(lIndex + 1, m_strFilename.length());
		else
			strFileTitle = m_strFilename;
		
		// Get file size
		//	 Open file
		hActiveDWG = OpenFile_fileIO((TCHAR *)m_strFilename.c_str(), FALSE, FALSE);
		//	 Validate file handle
		if(hActiveDWG == INVALID_HANDLE_VALUE)
			return FALSE;
		//	 Get size
		dwFileSize = GetFileSize(hActiveDWG, NULL);
		//	 Close file
		CloseHandle(hActiveDWG);
		//	 De-ref handle
		hActiveDWG = NULL;
		
		// Get object count
		lObjectCount = m_lEntityCount;

		// Get AutoCad version
		if(!getVersion(ptcVersion))
			return FALSE;

		// Get layer count... 
		lLayerCount = CADLayerCount(m_hCADImporterDrawing);

		// Get layers, create name string
		if(lLayerCount)
		{
			PDWGLAYERINFO pdwglinfTemp = NULL;

			for(long lcv = 0L; lcv < lLayerCount; lcv++)
			{
				// get current layer
				pdwglinfTemp = m_pllstLayers->getEntry(lcv);

				// validate layer, continue
				if(pdwglinfTemp)
				{
					// append linefeed and tab
					if(strLayers.length())
						strLayers += _T("\r\n\t");
	
					// add name
					strLayers += pdwglinfTemp->getLayerName();
				}
			}
		}

		// Calculate size
		lEstCombinedLength = strLayers.length() + strFileTitle.length()
								+ lstrlen(ptcVersion)
								+ _tcslen(STRING_FORMAT_DWGINFORMATION)
								+ (8 * 3); // Fudge factor for numerics
		
		// Attempt to create output buffer
		ptcOutString = new TCHAR[lEstCombinedLength + 1];
		if(ptcOutString)
		{
			// format output string
			_stprintf(ptcOutString, STRING_FORMAT_DWGINFORMATION,
				strFileTitle.c_str(), dwFileSize, ptcVersion, lObjectCount,
				lLayerCount, strLayers.c_str());

			// set success return val
			bReturn = TRUE;
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While retrieving the active drawing's information, an unexpected error occurred.");

		// destroy return buffer if it was allocated
		if(ptcOutString)
		{
			delete[] ptcOutString;
			ptcOutString = NULL;
		}

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// garbage collect
	if(hActiveDWG && hActiveDWG != INVALID_HANDLE_VALUE)
	{
		// close file
		CloseHandle(hActiveDWG);

		// de-ref, jic
		hActiveDWG = NULL;
	}
	if(ptcVersion)
	{
		delete[] ptcVersion;
		ptcVersion = NULL;
	}

	// return success / fail val
	return bReturn;
}
	
/**
 * Increments the current zoom by one and redraws the active drawing.
 *
 * @return TRUE if the zoom is incremented and the drawing redraw successfully
 * otherwise FALSE.
 */
BOOL CDWGRenderEngine::zoomIn()
{
	BOOL bReturn = TRUE;

	try
	{
		// make sure there is an active drawing
		if(m_hCADImporterDrawing == NULL)
			return FALSE;

		// increase zoom
		m_iZoomFactor += ZOOM_INCREMENT;

		// redraw active drawing
		bReturn = renderDrawing();
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While increasing the zoom, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Decrements the current zoom by one and redraws the active drawing.
 *
 * @return TRUE if the zoom is decremented and the drawing redrawn successfully
 * otherwise FALSE.
 */
BOOL CDWGRenderEngine::zoomOut()
{
	BOOL bReturn = TRUE;

	try
	{
		// make sure there is an active drawing
		if(m_hCADImporterDrawing == NULL)
			return FALSE;

		// make sure zoom doesn't get reduced below lowest allowed
		if(m_iZoomFactor == ZOOM_LOWEST_ALLOWED)
			return FALSE;
		
		// reduce zoom
		m_iZoomFactor -= ZOOM_INCREMENT;

		// redraw active drawing
		bReturn = renderDrawing();
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While decreasing the zoom, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Parses and stores the AutoCad color table contained in the binary data.
 *
 * @param ptcData contents of the color table file to be loaded. 
 * NOTE: this would be better served as a separate file; however, for
 * reasons of portability the client doesn't want any files to be
 * required for the install.
 *
 * @param dwSize
 *
 * @return TRUE if the color table is loaded and no errors occurred,
 * otherwise FALSE.
 */
BOOL CDWGRenderEngine::parseACColorTable(TCHAR *ptcData, DWORD dwSize)
{
	BOOL bReturn = TRUE;

	try
	{
		PACCOLOR_ENTRY pacclrEntry = NULL;
		tstring strBuffer = EMPTY_STRING,
					strLine = EMPTY_STRING;
		DWORD dwIndex = (DWORD)0;
		TCHAR *ptc = NULL,
			  *ptcLast = NULL;
		BOOL bEndOfLine = FALSE,
			 bInComment = FALSE;

		// validate list
		if(m_pllstACColorTable == NULL)
		{
			// set last error
			m_strLastError = _T("The internal color table list is invalid.");

			// return fail
			return FALSE;
		}

		// validate table data
		if(dwSize == (DWORD)0 || ptcData == NULL)
		{
			// set last error
			m_strLastError = _T("The supplied color table data is invalid.");

			// return fail val
			return FALSE;
		}

		// clear list
		m_pllstACColorTable->clear();

		// begin parse of color table data... the index var is simply a sanity
		//	 check for the loop and protects against rogue data in the color table
		//	 data causing problems.
		for(ptc = ptcLast = ptcData, dwIndex = (DWORD)0; 
			ptc && (dwIndex < dwSize); 
			dwIndex++, ptc++)
		{
			// check current character for "key" characters
			switch(*ptc)
			{
				// End of line... whether CrLf or Lf
				case _T('\r'):
				case _T('\n'):
					bEndOfLine = TRUE;
					bInComment = FALSE;	// ONLY single line comments allowed
					break;

				default:
					// reset end of line flag if we're here
					bEndOfLine = FALSE;
					
					if(!bInComment)
					{
						// check for comment chars
						if(*ptc != _T('/'))
						{
							strBuffer += *ptc;
						}
						else if(*ptcLast == _T('/'))
						{
							// ONLY single line comments allowed
							bInComment = TRUE;
						}
					}
					break;
			}

			// check and see if line should be processed
			if(bEndOfLine && strBuffer.length())
			{
				long lcv;

				// trim any leading or trailing space in the line
				//	 Leading
				for(lcv = 0L; lcv < (long)strBuffer.length(); lcv++)
				{
					if(strBuffer[lcv] != _T(' ') 
					   && strBuffer[lcv] != _T('\t'))
						break;
				}
				strBuffer = strBuffer.substr(lcv, strBuffer.length());
				//	 Trailing
				for(lcv = strBuffer.length() - 1; lcv > -1; lcv--)
				{
					if(strBuffer[lcv] != _T(' ') && strBuffer[lcv] != _T('\t'))
						break;
				}
				strLine = strBuffer.substr(0, lcv + 1);

				// check line's size again, just to be certain
				if(strLine.length())
				{
					TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING,
						  *ptc = NULL,
						  *ptcName = NULL;
					int iIndex = -1,
						iRed = -1,
						iGreen = -1,
						iBlue = -1;
					BOOL bEntryComplete = FALSE;
				
					// copy line to temp buffer for manipulation
					_tcscpy(tstrBuffer, (TCHAR *)strLine.data());

					// separate entry on commas
					ptc = _tcstok(tstrBuffer, _T(","));
					if(ptc)
					{
						// First item MUST BE the index
						iIndex = _tstol(ptc);

						// get next item
						ptc = _tcstok(NULL, _T(","));
						if(ptc)
						{
							// Second item MUST BE the color's name
							ptcName = ptc;

							// get next item
							ptc = _tcstok(NULL, _T(","));
							if(ptc)
							{	
								// Third item MUST BE the red component of the 
								//	 color
								iRed = _tstol(ptc);

								// Get next item
								ptc = _tcstok(NULL, _T(","));
								if(ptc)
								{
									// Fourth item MUST BE the green component
									//	 of the color
									iGreen = _tstol(ptc);

									// Get last item
									ptc = _tcstok(NULL, _T(","));
									if(ptc)
									{
										// Last item MUST BE the blue component
										//	 of the color
										iBlue = _tstol(ptc);

										// set complete flag
										bEntryComplete = TRUE;
									}
								}
							}
						}
					}
					
					// If entry was good, add color
					if(bEntryComplete)
					{
						// create color entry
						pacclrEntry = new ACCOLOR_ENTRY(iIndex,
										RGB(iRed, iGreen, iBlue),
										ptcName);

						// validate, continue
						if(pacclrEntry)
							// add to "table" (list)
							m_pllstACColorTable->add(pacclrEntry);

						// release ref, jic
						pacclrEntry = NULL;
					}
				}
				
				// reset line
				strBuffer = EMPTY_STRING;
			}

			// Store last character
			ptcLast = ptc;
		}

	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Loads the DWG specified and if necessary, the color table as well.
 *
 * @param tstrDWGFilename full path to the AutoCad Drawing file that
 * is to be loaded and rendered.
 *
 * @return TRUE if the drawing is loaded successfully and no errors
 * occur, otherwise FALSE.
 */
BOOL CDWGRenderEngine::loadDrawing(TCHAR *tstrDWGFilename)
{
	BOOL bReturn = TRUE;

	try
	{
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;
		int nErrorCode;

		// Reset message field
		m_strMessage = EMPTY_STRING;

		// prepare progressbar
		if(m_hwndProgressControl)
		{
			SendMessage(m_hwndProgressControl, PBM_SETPOS, (WPARAM)0, 0L);
			SendMessage(m_hwndProgressControl, PBM_SETSTEP, (WPARAM)1, 0L);
			SendMessage(m_hwndProgressControl, PBM_SETRANGE32, (WPARAM)0, 
				COUNT_ADDITIONAL_LOADING_STEPS + (m_hmodCADImporter ? 0 : 1));
		}

		// Validate the library handle
		if(m_hmodCADImporter == NULL)
		{
			// attempt to load it... NOTE: loadCADImporterLibrary() will NOT
			//	 TRUE unless the library was loaded successfully
			if(!loadCADImporterLibrary())
			{
				// KEEP last error from method call above.

				// return fail val
				return FALSE;
			}

			// display progress
			if(m_hwndProgressControl)
			{
				SendMessage(m_hwndProgressControl, PBM_STEPIT, (WPARAM)0, 0L);
				doEvents();
			}
		}

		// validate filename
		if(lstrlen(tstrDWGFilename) == 0)
		{
			// set last error
			m_strLastError = _T("The specified DWG filename is invalid.");

			// return fail
			return FALSE;
		}
		if(!Exists(tstrDWGFilename))
		{
			// set last error
			m_strLastError = _T("The specified DWG file DOES NOT exist.");

			// return fail
			return FALSE;
		}

		// check relevant function pointers
		if(CADClose == 0L || CADSetSHXOptions == 0L || CADCreate == 0L ||
		   CADGetLastError == 0L || CADProhibitCurvesAsPoly == 0L ||
		   CADGetSection == 0L)
		{
			// set last error
			m_strLastError = _T("One or more of the function pointers for manipulating the drawing is invalid.");

			// return fail val
			return FALSE;
		}

		// Get file title for use in messages
		for(long lcv = lstrlen(tstrDWGFilename); lcv > -1; lcv--)
		{
			if(tstrDWGFilename[lcv] == _T('\\'))
			{
				// copy everything AFTER backslash
				_tcscpy(tstrBuffer, &tstrDWGFilename[++lcv]);

				// we're done, exit
				break;
			}
		}

		// if a DWG is active, clear
		if(m_strFilename.length() && m_hCADImporterDrawing)
		{
			// close the drawing
			CADClose(m_hCADImporterDrawing);

			// de-ref object
			m_hCADImporterDrawing = NULL;
		}
		
		// Clear any existing filename at this point...
		m_strFilename = EMPTY_STRING;

		// Reset entity count
		m_lEntityCount = 0L;

		// Attempt to load and create the drawing object
		CADSetSHXOptions("", "", "", 1, 1);					// please call it before CADCreate
    std::string str_fn;
    TToAChar(tstrDWGFilename, str_fn);
    m_hCADImporterDrawing = CADCreate(m_hwndOutputControl, str_fn.c_str());

		// display progress
		if(m_hwndProgressControl)
		{
			SendMessage(m_hwndProgressControl, PBM_STEPIT, (WPARAM)0, 0L);
			doEvents();
		}

		// validate handle, get error code
		if(m_hCADImporterDrawing)
		{
			CADDATA caddtEntities;

			//CADProhibitCurvesAsPoly(hCAD, (int(bGetArcsAsCurves)+1)%2);// 1 => permit conversion of arcs to polyline			
			//ChangeScale(ID_FITWINDOW);
			//RecalculateExtents();
			
			// reset drawing uses black flag
			m_bDrawingUsesBlack = FALSE;

			// set filename
			m_strFilename = tstrDWGFilename;

			// load layer information
			loadLayers();

			// display progress
			if(m_hwndProgressControl)
			{
				SendMessage(m_hwndProgressControl, PBM_STEPIT, (WPARAM)0, 0L);
				doEvents();
			}

			// Get entity count
			if(CADGetSection(m_hCADImporterDrawing, 2, &caddtEntities))
				m_lEntityCount = caddtEntities.Count;
		}
		else
		{
			// get extended error information
      char err_buf[MAX_PATH];
			nErrorCode = CADGetLastError(err_buf);
      AToTChar(err_buf, m_strMessage);

			// set active filename to empty

			// set message to could not open file
			m_strMessage = _T("The file \"") + m_strMessage;
			m_strMessage += _T("\" is not a DWG or is corrupt.");
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to load the drawing, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Manages the process of loading and parsing the AutoCad Color Table from
 * this application's resource file.
 *
 * @return TRUE if the color table is loaded and parsed successfully,
 * otherwise FALSE.
 */
BOOL CDWGRenderEngine::loadACColorTable()
{
	HRSRC hresTable = NULL;
	HGLOBAL hglbTable = NULL;
	BOOL bReturn = TRUE;

	try
	{
		TCHAR *ptcData = NULL;
		DWORD dwTableSize = (DWORD)0;

		// attempt to get handle to help text resource
		hresTable = FindResource(NULL, MAKEINTRESOURCE(IDR_ACCOLORTABLE), 
						RESOURCE_TYPE_AUTOCAD);
		if(hresTable == NULL)
		{
			// set last error
			m_strLastError = _T("Could not locate the color table resource.");

			// return fail
			return FALSE;
		}

		// Get resource size
		dwTableSize = SizeofResource(NULL, hresTable);
		
		// attempt to load help text resource
		hglbTable = LoadResource(NULL, hresTable);

		// validate, continue
		if(hglbTable == NULL)
		{
			// set last error
			m_strLastError = _T("Could not load the color table resource.");

			// return fail
			return FALSE;
		}

		// lock resource and turn it into something "usable"
		ptcData = (TCHAR *)LockResource(hglbTable);
	
		// verify data pointer is good
		if(ptcData == NULL)
		{
			// set last error
			m_strLastError = _T("After loading the color table, the data could not be read.");

			// return fail
			return FALSE;
		}

		// make the call...
		bReturn = parseACColorTable(ptcData, dwTableSize);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("An unexpected error occurred while attempting to load the color table.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// perform garbage collection

	// All resource handles, etc. are automatically freed when this process
	//	 terminates.

	// return success / fail val
	return bReturn;
}

///**
// * Checks the specified drawing file's version. Returns TRUE if the version
// * is supported, otherwise FALSE.
// */
//BOOL CDWGRenderEngine::isSupportedVersion(TCHAR *tstrDWGFilename)
//{
//	BOOL bReturn = TRUE;

	//try
	//{
	//	ifstream ifsDWG;

	//	// validate filename
	//	if(lstrlen(tstrDWGFilename) == 0)
	//	{
	//		// set last error
	//		m_strLastError = _T("The specified DWG filename is invalid.");

	//		// return fail
	//		return FALSE;
	//	}
	//	if(!Exists(tstrDWGFilename))
	//	{
	//		// set last error
	//		m_strLastError = _T("The specified DWG file DOES NOT exist.");

	//		// return fail
	//		return FALSE;
	//	}
	//	
	//	// attempt to open drawing file
	//	ifsDWG.open(tstrDWGFilename, ios::binary | ios::in);
	//	if(ifsDWG)
	//	{
	//		TCHAR tstrBuffer[HEADER_VERSION_LENGTH + 1] = EMPTY_STRING;
	//		
	//		// get version from file
	//		ifsDWG.getline(tstrBuffer, sizeof(tstrBuffer), 0);

	//		// check version
	//		if(_tcscmp(tstrBuffer, ACVERSION_R13) == 0 
	//		   ||
	//		   _tcscmp(tstrBuffer, ACVERSION_R14) == 0)
	//			bReturn = FALSE;

	//		// close input file
	//		ifsDWG.close();
	//	}
	//}
	//catch(...)
	//{
	//	// set last error
	//	m_strLastError = _T("");

	//	// set fail val
	//	bReturn = FALSE;
	//}

	//// clear last error, if applicable
	//if(bReturn)
	//	m_strLastError = EMPTY_STRING;

	// return success / fail val
//	return bReturn;
//}

/**
 * Retrieves the version of AutoCad used to create the active drawing
 * file.
 *
 * @note The calling method or function is responsible for freeing the
 * storage allocated for ptcOutVersion
 *
 * @param ptcOutVersion on return, contains the name of the AutoCad
 * version for the active drawing file.
 *
 * @return TRUE if the version is retrieved and no errors occur,
 * otherwise FALSE
 */
BOOL CDWGRenderEngine::getVersion(TCHAR *&ptcOutVersion)
{
	BOOL bReturn = FALSE;

	try
	{
    basic_ifstream<TCHAR, std::char_traits<TCHAR>> ifsDWG;
		tstring strVersion = EMPTY_STRING;

		// validate active drawing file
		if(m_strFilename.length() == 0 || m_hCADImporterDrawing == NULL)
		{
			// set last error
			m_strLastError = _T("The active DWG filename is invalid.");

			// return fail
			return FALSE;
		}
		
		// make sure out param is empty
		if(ptcOutVersion != NULL)
		{
			// set last error

			// return fail
			return FALSE;
		}

		// attempt to open drawing file
		ifsDWG.open(m_strFilename.c_str(), ios::binary | ios::in);
		if(ifsDWG)
		{
			TCHAR tstrBuffer[HEADER_VERSION_LENGTH + 1] = EMPTY_STRING;
			
			// get version from file
			ifsDWG.getline(tstrBuffer, sizeof(tstrBuffer), 0);

			// check version
			if(_tcscmp(tstrBuffer, ACVERSION_R13) == 0)
				strVersion = ACVERSION_NAME_R13;
			else if(_tcscmp(tstrBuffer, ACVERSION_R14) == 0)
				strVersion = ACVERSION_NAME_R14;
			else if(_tcscmp(tstrBuffer, ACVERSION_R15) == 0)
				strVersion = ACVERSION_NAME_R15;
			else if(_tcscmp(tstrBuffer, ACVERSION_R16) == 0)
				strVersion = ACVERSION_NAME_R16;
			else if(_tcscmp(tstrBuffer, ACVERSION_R17) == 0)
				strVersion = ACVERSION_NAME_R17;

			// close input file
			ifsDWG.close();
		}
		
		// Check and see if version was found
		if(strVersion.length())
		{
			// attempt to allocate storage
			ptcOutVersion = new TCHAR[strVersion.length() + 1];
			
			// validate
			if(ptcOutVersion != NULL)
			{
				// copy
				_tcscpy(ptcOutVersion, strVersion.c_str());

				// set success return val
				bReturn = TRUE;
			}
			else
				// set error
				m_strLastError = _T("Could not allocate return buffer for version.");
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While retrieving the version of the active drawing file, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Loads all layers in the active drawing and stores them in the layers linked
 * list for access. This allows us to control what layers are visible when
 * drawn along with easy access to layer information.
 *
 * @return TRUE if the layers are retrieved successfully, otherwise FALSE.
 */
BOOL CDWGRenderEngine::loadLayers()
{
	CADDATA caddtLayer;
	BOOL bReturn = FALSE;

	try
	{
		HANDLE hDWGLayer = NULL;
		TCHAR tstrBuffer[MAX_PATH + 1] = EMPTY_STRING;
		long lLayerCount = 0L;

		// validate layer list
		if(m_pllstLayers == NULL)
		{
			// set last error
			m_strLastError = _T("The internal layer list object is invalid.");

			// return fail
			return FALSE;
		}

		// validate the active drawing file
		if(m_hCADImporterDrawing == NULL)
		{
			// set last error
			m_strLastError = _T("The active drawing object is invalid.");

			// return fail val
			return FALSE;
		}

		// check relevant function pointers
		if(CADLayerCount == 0L || CADLayer == 0L || CADVisible == 0L)
		{
			// set last error
			m_strLastError = _T("One or more of the function pointers for manipulating the drawing is invalid.");

			// return fail val
			return FALSE;
		}

		// clear any existing entries (NOTE: see constructor for special
		//	 initialize for the layers list)
		m_pllstLayers->clear();

		// Get layer count... 
		lLayerCount = CADLayerCount(m_hCADImporterDrawing);

		// Get layers, create name string
		if(lLayerCount)
		{
			// initialize layer data structure
			memset(&caddtLayer, 0, sizeof(caddtLayer));

			for(long lcv = 0L; lcv < lLayerCount; lcv++)
			{
				// attempt to get the layer and its information
				hDWGLayer = CADLayer(m_hCADImporterDrawing, lcv, &caddtLayer);

				// validate layer, continue
				if(hDWGLayer)
				{
					PDWGLAYERINFO pdwglinfTemp = NULL;
					BOOL bVisible = TRUE;
					
					// attempt to create new layer info object
          tstring add_layer_text_str;
          AToTChar(caddtLayer.Text, add_layer_text_str);
          pdwglinfTemp = new DWGLAYERINFO((TCHAR*)add_layer_text_str.c_str());

					// attempt to get visibility state
          std::string layer_name_str;
          TToAChar(pdwglinfTemp->getLayerName(), layer_name_str);
					bVisible = CADVisible(m_hCADImporterDrawing, 
            layer_name_str.c_str());
					pdwglinfTemp->bEnabled = bVisible;

					// validate, add to list
					if(pdwglinfTemp)
					{
						// add to list
						m_pllstLayers->add(pdwglinfTemp);

						// de-ref, storage now owned by list
						pdwglinfTemp = NULL;
					}

					// Check and see if this layer uses black for its
					//	 drawing color.
					
					// HOW do we do this? Set to TRUE for now...
					m_bDrawingUsesBlack = TRUE;
				}
			}

			// return success
			if(m_pllstLayers->getLength())
				bReturn = TRUE;
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While retrieving the layers in the active drawing, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Returns whether or not there is an active drawing.
 *
 * @return TRUE if there is an active drawing an no errors occur, otherwise
 * FALSE.
 */
BOOL CDWGRenderEngine::hasActiveDrawing()
{
	return (m_hCADImporterDrawing != NULL);
}

/**
 * Manages the rendering process for the active drawing.
 *
 *
 *
 * @return TRUE if the active drawing is rendered and no errors occur,
 * otherwise FALSE.
 */
BOOL CDWGRenderEngine::renderDrawing()
{
	HDC hdcOutputControl = NULL;
	HRGN hrgnOutput = NULL;
	BOOL bReturn = FALSE;
	static BOOL bRendering = FALSE;

	try
	{
	//	Dwg_Object *pdwgobjCurrent = NULL;
	//	RECT rctOutputControl;
	//	POINT ptAbsoluteOffset,
	//		  ptClientCenter,
	//		  ptDrawingCenter,
	//		  ptCenterOffset;
	//	XFORM xformCartesian;
	//	float fRatioX = 0.0f,
	//		  fRatioY = 0.0f,
	//		  fFinalRatio = 0.0f,
	//		  fFinalScale = 0.0f,
	//		  fWidthControl = 0.0f,
	//		  fHeightControl = 0.0f,
	//		  fWidthDrawing = 0.0f,
	//		  fHeightDrawing = 0.0f;
	//	long lcv = 0L,
	//		 lObjectCount = 0L;

		// Make sure we are not currently "in-process"
		if(bRendering)
			return TRUE;

		// make sure there is an active drawing file
		if(m_hCADImporterDrawing == NULL || m_strFilename.length() == 0)
		{
			// set last error
			m_strLastError = _T("Render: there is no active drawing file.");

			// return fail
			return FALSE;
		}

		// make sure there is an output control
		if(m_hwndOutputControl == NULL)
		{
			// set last error
			m_strLastError = _T("Render: the output object is invalid.");

			// return fail val
			return FALSE;
		}

		// Attempt to get DC
		hdcOutputControl = GetDC(m_hwndOutputControl);
		if(hdcOutputControl == NULL)
		{
			// set last error
			m_strLastError = _T("Render: could not get the DC for the output object.");

			// return fail val
			return FALSE;
		}

		// The gaunlet has been run, now get on with it...

		// Set flag indicating the rendering process is proceeding
		bRendering = TRUE;


	//	// Ok, here's the deal... the lines which follow *could* be combined to
	//	//	 make the code more concise; HOWEVER, the code below is expanded for
	//	//	 the sake of readability and upkeep.

	//	// Get the output control's RECT
	//	GetWindowRect(m_hwndOutputControl, &rctOutputControl);

	//	// get usable height and width of control
	//	fWidthControl = (float)rctOutputControl.right - 
	//						(float)rctOutputControl.left;
	//	fHeightControl = (float)rctOutputControl.bottom - 
	//						(float)rctOutputControl.top;
	//	
	//	// get width and height of drawing
	//	fWidthDrawing = (float)m_dwgdatThis.header_vars.EXTMAX_MSPACE.x -
	//							(float)m_dwgdatThis.header_vars.EXTMIN_MSPACE.x;
	//	fHeightDrawing = (float)m_dwgdatThis.header_vars.EXTMAX_MSPACE.y -
	//							(float)m_dwgdatThis.header_vars.EXTMIN_MSPACE.y;
	//	
	//	// create ratios... i.e. drawing size to window size
	//	if(fWidthControl < fWidthDrawing)
	//		fRatioX = fWidthControl / fWidthDrawing;
	//	else
	//		fRatioX = 1.0f;
	//	if(fHeightControl < fHeightDrawing)
	//		fRatioY = fHeightControl / fHeightDrawing;
	//	else
	//		fRatioY = 1.0f;

	//	// figure out which one constricts the rendered drawing the most - i.e.
	//	//	 at 100% zoom, we want the drawing to fit in its entirety.
	//	if(fRatioX < fRatioY)
	//		fFinalRatio = fRatioX;
	//	else
	//		fFinalRatio = fRatioY;

	//	// calculate final zoom based on current zoom factor
	//	fFinalScale = fFinalRatio * ((float)m_iZoomFactor / 100);

	//	// initialize progressbar
	//	if(m_hwndProgressControl)
	//	{
	//		// set range
	//		SendMessage(m_hwndProgressControl, PBM_SETRANGE32, (WPARAM)0, 
	//			(LPARAM)m_dwgdatThis.num_objects);
	//		
	//		// set initial increment
	//		SendMessage(m_hwndProgressControl, PBM_SETSTEP, (WPARAM)1, 0L);

	//		// set initial value
	//		SendMessage(m_hwndProgressControl, PBM_SETPOS, (WPARAM)0, 0L);
	//	}

	//	// begin draw... I'm not, as Dr. Kreahling once put it, "taking it for
	//	//	 granted that the compiler is doing things smartly."
	//	
	//	// get object count, store locally
	//	lObjectCount = m_dwgdatThis.num_objects;

	//	// Set origin of coordinate system
	//	SetGraphicsMode(hdcOutputControl, GM_ADVANCED);
	//	SetMapMode(hdcOutputControl, MM_LOENGLISH); 
	//	
 //       xformCartesian.eM11 = (FLOAT) fFinalScale; 
 //       xformCartesian.eM12 = (FLOAT) 0.0f; 
 //       xformCartesian.eM21 = (FLOAT) 0.0f; 
 //       xformCartesian.eM22 = (FLOAT) -fFinalScale; 
 //       xformCartesian.eDx  = (FLOAT) 0.0f; 
 //       xformCartesian.eDy  = (FLOAT) 0.0f; 
 //       SetWorldTransform(hdcOutputControl, &xformCartesian); 

	//	// Calculate absolute origin for drawing functions
	//	//	 Get client RECT
	//	GetClientRect(m_hwndOutputControl, &rctOutputControl);
	//	//	 Compute base offset from Cartesian origin - i.e. the amount required to
	//	//		to relocate the drawing's lower left corner to (0,0).
	//	ptAbsoluteOffset.x = 0 - (int)floor(m_dwgdatThis.header_vars.EXTMIN_MSPACE.x + 0.5f);
	//	ptAbsoluteOffset.y = 0 - (int)floor(m_dwgdatThis.header_vars.EXTMIN_MSPACE.y + 0.5f);
	//	//	 Get center of client rect
	//	DPtoLP(hdcOutputControl, (LPPOINT)&rctOutputControl, 2);
	//	ptClientCenter.x = (rctOutputControl.right - rctOutputControl.left) / 2;
	//	ptClientCenter.y = (rctOutputControl.bottom - rctOutputControl.top) / 2;
	//	//	 Get center of drawing
	//	ptDrawingCenter.x = floor((m_dwgdatThis.header_vars.EXTMAX_MSPACE.x - 
	//							m_dwgdatThis.header_vars.EXTMIN_MSPACE.x) / 2.0f + 0.5f);
	//	ptDrawingCenter.y = floor((m_dwgdatThis.header_vars.EXTMAX_MSPACE.y - 
	//							m_dwgdatThis.header_vars.EXTMIN_MSPACE.y) / 2.0f + 0.5f);
	//	//	 Calculate center offsets
	//	ptCenterOffset.x = ptClientCenter.x - ptDrawingCenter.x;
	//	ptCenterOffset.y = ptClientCenter.y - ptDrawingCenter.y;
	//	//	 Combine center offset with base origin
	//	ptAbsoluteOffset.x += ptCenterOffset.x;
	//	ptAbsoluteOffset.y += ptCenterOffset.y;
	//	
	//	// Iterate through drawing objects and render
	//	for(lcv = 0L; lcv < lObjectCount; lcv++)
	//	{
	//		// get pointer to object
	//		pdwgobjCurrent = &m_dwgdatThis.object[lcv];

	//		// decide what to do based on object type
	//		switch(pdwgobjCurrent->type)
	//		{
	//			case DWG_TYPE_LINE:
	//				renderLine(pdwgobjCurrent->tio.entity, 
	//					hdcOutputControl, fFinalScale, ptAbsoluteOffset);
	//				break;

	//			case DWG_TYPE_MLINE:
	//				renderMultiLine(pdwgobjCurrent->tio.entity, 
	//					hdcOutputControl, fFinalScale, ptAbsoluteOffset);
	//				break;
	//			
	//			case DWG_TYPE_CIRCLE:
	//				renderCircle(pdwgobjCurrent->tio.entity, hdcOutputControl, 
	//					fFinalScale, ptAbsoluteOffset);
	//				break;

	//			//case DWG_TYPE_POINT:
	//			//	renderPoint(pdwgobjCurrent->tio.entity, hdcOutputControl, 
	//			//		fFinalScale, ptAbsoluteOffset);
	//			//	break;

	//			//case DWG_TYPE_TEXT:
	//			//	renderText(pdwgobjCurrent->tio.entity, hdcOutputControl, 
	//			//		fFinalScale, ptAbsoluteOffset);
	//			//	break;

	//			default:
	//				// next, please
	//				break;
	//		}

	//		// update progressbar
	//		if(m_hwndProgressControl)
	//			SendMessage(m_hwndProgressControl, PBM_SETPOS, (WPARAM)lcv, 
	//				0L);

	//		// let progressbar redraw...
	//		doEvents();
	//	}

		RECT wndrect;
		PARAM s;
		FLOATRECT frectTemp;
		float fScaleX,
			  fScaleY,
			  fWindowHeight,
			  fWindowWidth,
			  fDrawingHeight,
			  fDrawingWidth,
			  fFinalScale;
		POINT ptDrawingCenter,
			  ptWindowCenter;
		
		CADGetBox(m_hCADImporterDrawing, &frectTemp.left, &frectTemp.right,
			&frectTemp.top, &frectTemp.bottom);

		memset(&s, 0, sizeof(s));
		s.hWnd = m_hwndOutputControl;
		s.hDC = hdcOutputControl;
		//s.DrawMode = 0;
		//s.CircleDrawMode = TRUE;
		//s.GetArcsCurves = TRUE;
		//s.GetTextsCurves = bGetTextsAsCurves;
		//s.IsInsideInsert = &nIsInsideInsert;
		s.pvList = (VOID *)m_pllstLayers;
		GetClientRect(m_hwndOutputControl, &wndrect);

		fWindowHeight = (float)(wndrect.bottom - wndrect.top);
		fWindowWidth = (float)(wndrect.right - wndrect.left);

		fDrawingHeight = (float)(frectTemp.top - frectTemp.bottom);
		fDrawingWidth = (float)(frectTemp.right - frectTemp.left);
		
		//if(fWindowWidth < fDrawingWidth)
			fScaleX = fWindowWidth / fDrawingWidth;
		//else
		//	fScaleX = 1.0f;
		//if(fWindowHeight < fDrawingHeight)
			fScaleY = fWindowHeight / fDrawingHeight;
		//else
		//	fScaleY = 1.0f;

		if(fScaleX < fScaleY)
			fFinalScale = fScaleX;
		else
			fFinalScale = fScaleY;
		fFinalScale = fFinalScale * ((float)m_iZoomFactor / 100);

		s.Scale = fFinalScale;
		
		SetMapMode(s.hDC, MM_ANISOTROPIC);
		SetViewportOrgEx(s.hDC, 0, 0, NULL);
		//wndrect.bottom = wndrect.bottom / 2;
		//SetViewportExtEx(hdcOutputControl, wndrect.right, wndrect.bottom, NULL);

		ptWindowCenter.x = (long)floor((float)(wndrect.right - wndrect.left) / 2.0f + 0.5f);
		ptWindowCenter.y = (long)floor((float)(wndrect.bottom - wndrect.top) / 2.0f + 0.5f);

		ptDrawingCenter.x = (long)floor((frectTemp.right - frectTemp.left) / 2.0f + 0.5f);
		ptDrawingCenter.y = (long)floor((frectTemp.top - frectTemp.bottom) / 2.0f + 0.5f);
		ptDrawingCenter.x = (long)floor((float)ptDrawingCenter.x * fFinalScale + 0.5f);
		ptDrawingCenter.y = (long)floor((float)ptDrawingCenter.y * fFinalScale + 0.5f);

		s.offset.x = ptWindowCenter.x - ptDrawingCenter.x;
		s.offset.y = ptWindowCenter.y - ptDrawingCenter.y;
		s.offset.x = s.offset.x + (0 - (long)floor(frectTemp.left * fFinalScale + 0.5f));
		if(frectTemp.bottom < 0.0f)
		{
			s.offset.y = s.offset.y + (0 - (long)floor(frectTemp.bottom * fFinalScale + 0.5f));
		}
		else
		{
			s.offset.y = s.offset.y + (0 - (long)floor(frectTemp.bottom * fFinalScale + 0.5f));
		}
		s.offset.y = wndrect.bottom - s.offset.y;
		
		if(m_bDrawingUsesBlack)
			FillRect(s.hDC, &wndrect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		else
		{
			FillRect(s.hDC, &wndrect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}

		// Create clipping region
		hrgnOutput = CreateRectRgn(wndrect.left, wndrect.top, wndrect.right, wndrect.bottom);
		if(SelectClipRgn(hdcOutputControl, hrgnOutput) == ERROR)
		{
			// Not sure what should happen here since we don't want to abort the entire
			//   draw over this.
		}

		CADEnum(m_hCADImporterDrawing, (int(s.GetArcsCurves) << 3), DoDraw, &s);

		// if we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While rendering the active drawing, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// garbage collect
	if(hdcOutputControl)
	{
		// release 
		ReleaseDC(m_hwndOutputControl, hdcOutputControl);

		// de-ref, jic
		hdcOutputControl = NULL;
	}

	if(hrgnOutput)
	{
		// destroy
		DeleteObject(hrgnOutput);

		// de-ref, jic..
		hrgnOutput = NULL;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// clear rendering flag
	bRendering = FALSE;

	// return success / fail val
	return bReturn;
}

/**
 * Waits until all painting (more or less) has been completed before
 * returning.
 *
 * @return 
 */
BOOL CDWGRenderEngine::doEvents()
{
	BOOL bReturn = TRUE;

	try
	{
		MSG msg;

		// Hold for re-draw
		while(PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch(...)
	{
		// set fail return val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Drawing Methods
///////////////////////////////////////////////////////////////////////////////

///**
// * Renders the line object specified.
// *
// * @param pdwgentLine the line object to be rendered
// *
// * @return TRUE if the line object is rendered successfully, otherwise FALSE
// */
//BOOL CDWGRenderEngine::renderLine(Dwg_Object_Entity *pdwgentLine, 
//	HDC hdcOutput, float fOutputScale, POINT ptAbsoluteOrigin)
//{
//	HPEN hpenObject = NULL,
//		 hpenPrevious = NULL;
//	BOOL bReturn = TRUE;
//
//	try
//	{
//		Dwg_Entity_LINE *pdwglnThis = NULL;
//		RECT rctOffset;
//		POINT ptStart,
//			  ptEnd;
//
//		// validate object
//		if(pdwgentLine == NULL)
//			return FALSE;
//
//		// validate HDC
//		if(hdcOutput == NULL)
//			return FALSE;
//		
//		// validate scale
//		if(fOutputScale == 0.0f)
//			return FALSE;
//		
//		// Make sure this line should be visible... if not, this DOES NOT
//		//	 indicate an error condition
//		if(!layerIsActive(pdwgentLine))
//			return TRUE;
//
//		// Get pointer to line entity, check
//		pdwglnThis = pdwgentLine->tio.LINE;
//		if(pdwglnThis == NULL)
//			return FALSE;
//
//		// Get offset RECT
//		GetClientRect(m_hwndOutputControl, &rctOffset);
//
//		// Create pen
//		hpenObject = createObjectPen(pdwgentLine);
//		hpenPrevious = (HPEN)SelectObject(hdcOutput, hpenObject);
//		
//		// Initialize POINTS
//		memset(&ptStart, 0, sizeof(ptStart));
//		memset(&ptEnd, 0, sizeof(ptEnd));
//
//		// Get coordinates / measurements
//
//		if(pdwglnThis->start.x < m_dwgdatThis.header_vars.EXTMIN_MSPACE.x
//			||
//		   pdwglnThis->start.y < m_dwgdatThis.header_vars.EXTMIN_MSPACE.y
//		    ||
//		   pdwglnThis->end.x < m_dwgdatThis.header_vars.EXTMIN_MSPACE.x
//			||
//		   pdwglnThis->end.y < m_dwgdatThis.header_vars.EXTMIN_MSPACE.y)
//		{
//			return TRUE;
//		}
//
//		//ofstream ofs("C:\\Temp\\DWGout.txt", ios::app | ios::out | ios::in);
//		//if(pdwglnThis->start.x < 0.0f || pdwglnThis->start.y < 0.0f)
//		//{
//		//	ofs << "\r\nLine\r\nStart: " << pdwglnThis->start.x << ", "  << pdwglnThis->start.y << "\r\n";
//		//}
//		//if(pdwglnThis->end.x < 0.0f || pdwglnThis->end.y < 0.0f)
//		//{
//		//	ofs << "\r\nEnd: " << pdwglnThis->end.x << ", "  << pdwglnThis->end.y << "\r\n";
//		//}
//		//ofs.close();
//
//		ptStart.x = floor(pdwglnThis->start.x + 0.5f);
//		ptStart.y = floor(pdwglnThis->start.y + 0.5f);
//		ptEnd.x = floor(pdwglnThis->end.x + 0.5f);
//		ptEnd.y = floor(pdwglnThis->end.y + 0.5f);
//		
//		// Offset by absolute offset
//		ptStart.x += ptAbsoluteOrigin.x;
//		ptStart.y += ptAbsoluteOrigin.y;
//		ptEnd.x += ptAbsoluteOrigin.x;
//		ptEnd.y += ptAbsoluteOrigin.y;
//		
//		// Perform offset and rotation to Cartesian coord space 
//		DPtoLP(hdcOutput, (LPPOINT)&rctOffset, 2);
//		ptStart.y = rctOffset.bottom - ptStart.y;
//		ptEnd.y = rctOffset.bottom - ptEnd.y;
//
//		// Draw
//		MoveToEx(hdcOutput, ptStart.x, ptStart.y, NULL);
//		LineTo(hdcOutput, ptEnd.x, ptEnd.y);
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		bReturn = FALSE;
//	}
//
//	// clear last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// garbage collect
//	if(hpenPrevious)
//	{
//		// restore original pen
//		SelectObject(hdcOutput, hpenPrevious);
//
//		// de-ref
//		hpenPrevious = NULL;
//	}
//	if(hpenObject)
//	{
//		// destory created pen
//		DeleteObject(hpenObject);
//
//		// de-ref
//		hpenObject = NULL;
//	}
//
//	// return success / fail val
//	return bReturn;
//}
//
///**
// * Renders the line object specified.
// *
// * @param pdwgentMultiLine the line object to be rendered
// *
// * @return TRUE if the line object is rendered successfully, otherwise FALSE
// */
//BOOL CDWGRenderEngine::renderMultiLine(Dwg_Object_Entity *pdwgentMultiLine, 
//	HDC hdcOutput, float fOutputScale, POINT ptAbsoluteOffset)
//{
//	HPEN hpenObject = NULL,
//		 hpenPrevious = NULL;
//	POINT *arptPoints = NULL;
//	BOOL bReturn = TRUE;
//
//	try
//	{
//		Dwg_Entity_MLINE *pdwgmlnThis = NULL;
//		RECT rctOffset;
//		POINT ptStart,
//			  ptEnd;
//
//		// validate object
//		if(pdwgentMultiLine == NULL)
//			return FALSE;
//
//		// validate HDC
//		if(hdcOutput == NULL)
//			return FALSE;
//		
//		// validate scale
//		if(fOutputScale == 0.0f)
//			return FALSE;
//		
//		// Make sure this line should be visible... if not, this DOES NOT
//		//	 indicate an error condition
//		if(!layerIsActive(pdwgentMultiLine))
//			return TRUE;
//
//		// Get pointer to line entity, check
//		pdwgmlnThis = pdwgentMultiLine->tio.MLINE;
//		if(pdwgmlnThis == NULL)
//			return FALSE;
//
//		// Get offset RECT
//		GetClientRect(m_hwndOutputControl, &rctOffset);
//
//		// Create pen
//		hpenObject = createObjectPen(pdwgentMultiLine);
//		// Temporary
//		//hpenObject = (HPEN)GetStockObject(WHITE_PEN);
//		hpenPrevious = (HPEN)SelectObject(hdcOutput, hpenObject);
//		
//		arptPoints = new POINT[pdwgmlnThis->num_verts];
//		if(arptPoints == NULL)
//			return FALSE;
//
//		// Initialize POINTS
//		for(UINT uiElement = 0; uiElement < pdwgmlnThis->num_verts; uiElement++)
//		{
//			// Get coordinates / measurements
//			arptPoints[uiElement].x = floor(pdwgmlnThis->verts[uiElement].vertex.x + 
//										0.5f);
//			arptPoints[uiElement].y = floor(pdwgmlnThis->verts[uiElement].vertex.y + 
//										0.5f);
//
//			// Offset by absolute offset
//			arptPoints[uiElement].x += ptAbsoluteOffset.x;
//			arptPoints[uiElement].y += ptAbsoluteOffset.y;
//
//			// Rotate into Cartesian space
//			DPtoLP(hdcOutput, (LPPOINT)&rctOffset, 2);
//			arptPoints[uiElement].y = rctOffset.bottom - arptPoints[uiElement].y;
//		}
//
//		// Draw
//		//PolylineTo(hdcOutput, arptPoints, pdwgmlnThis->num_verts);
//		MoveToEx(hdcOutput, arptPoints[0].x, arptPoints[0].y, NULL);
//		for(UINT uiElement = 0; uiElement < pdwgmlnThis->num_verts; uiElement++)
//		{
//			// draw
//			LineTo(hdcOutput, arptPoints[uiElement].x, arptPoints[uiElement].y);
//
//			// next, please
//			MoveToEx(hdcOutput, arptPoints[uiElement].x, 
//				arptPoints[uiElement].y, NULL);
//		}
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		bReturn = FALSE;
//	}
//
//	// clear last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// garbage collect
//	if(arptPoints)
//	{
//		delete[] arptPoints;
//		arptPoints = NULL;
//	}
//	if(hpenPrevious)
//	{
//		// restore original pen
//		SelectObject(hdcOutput, hpenPrevious);
//
//		// de-ref
//		hpenPrevious = NULL;
//	}
//	if(hpenObject)
//	{
//		// destory created pen
//		DeleteObject(hpenObject);
//
//		// de-ref
//		hpenObject = NULL;
//	}
//
//	// return success / fail val
//	return bReturn;
//}
//
///**
// * Renders the circle object specified.
// *
// * @param pdwgentCircle the circle object to be rendered
// *
// * @return TRUE if the circle object is rendered successfully, otherwise FALSE
// */
//BOOL CDWGRenderEngine::renderCircle(Dwg_Object_Entity *pdwgentCircle, 
//	HDC hdcOutput, float fOutputScale, POINT ptAbsoluteOffset)
//{
//	HPEN hpenObject = NULL,
//		 hpenPrevious = NULL;
//	HBRUSH hbrObject = NULL,
//		   hbrPrevious = NULL;
//	BOOL bReturn = TRUE;
//
//	try
//	{
//		Dwg_Entity_CIRCLE *pdwgcirThis = NULL;
//		RECT rctOffset;
//		POINT ptCenter;
//		double dblRadius = 0.0f;
//
//		// validate object
//		if(pdwgentCircle == NULL)
//			return FALSE;
//
//		// validate HDC
//		if(hdcOutput == NULL)
//			return FALSE;
//		
//		// validate scale
//		if(fOutputScale == 0.0f)
//			return FALSE;
//		
//		// Make sure this line should be visible... if not, this DOES NOT
//		//	 indicate an error condition
//		if(!layerIsActive(pdwgentCircle))
//			return TRUE;
//
//		// Get pointer to circle entity, check
//		pdwgcirThis = pdwgentCircle->tio.CIRCLE;
//		if(pdwgcirThis == NULL)
//			return FALSE;
//
//		// Get offset RECT
//		GetClientRect(m_hwndOutputControl, &rctOffset);
//
//		// Create pen
//		hpenObject = createObjectPen(pdwgentCircle);
//		// Temporary
//		//hpenObject = (HPEN)GetStockObject(WHITE_PEN);
//		hpenPrevious = (HPEN)SelectObject(hdcOutput, hpenObject);
//		
//		// Create brush
//		hbrObject = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
//		hbrPrevious = (HBRUSH)SelectObject(hdcOutput, hbrObject);
//
//		// Initialize POINTS
//		memset(&ptCenter, 0, sizeof(ptCenter));
//		dblRadius = 0.0f;
//
//		// Get coordinates / measurements
//		ptCenter.x = floor(pdwgcirThis->center.x + 0.5f);
//		ptCenter.y = floor(pdwgcirThis->center.y + 0.5f);
//		dblRadius = pdwgcirThis->radius;
//		
//		// Offset by absolute offset
//		ptCenter.x += ptAbsoluteOffset.x;
//		ptCenter.y += ptAbsoluteOffset.y;
//		
//		// Perform offset and rotation to Cartesian coord space 
//		DPtoLP(hdcOutput, (LPPOINT)&rctOffset, 2);
//		ptCenter.y = rctOffset.bottom - ptCenter.y;
//
//		// Draw	
//		Ellipse(hdcOutput, ptCenter.x, ptCenter.y, ptCenter.x + (long)dblRadius,
//			ptCenter.y + (long)dblRadius);
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		bReturn = FALSE;
//	}
//
//	// clear last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// garbage collect
//	if(hpenPrevious)
//	{
//		// restore original pen
//		SelectObject(hdcOutput, hpenPrevious);
//
//		// de-ref
//		hpenPrevious = NULL;
//	}
//	if(hpenObject)
//	{
//		// destory created pen
//		DeleteObject(hpenObject);
//
//		// de-ref
//		hpenObject = NULL;
//	}
//	if(hbrPrevious)
//	{
//		// restore original pen
//		SelectObject(hdcOutput, hbrPrevious);
//
//		// de-ref
//		hbrPrevious = NULL;
//	}
//	if(hbrObject)
//	{
//		// destory created pen
//		DeleteObject(hbrObject);
//
//		// de-ref
//		hbrObject = NULL;
//	}
//
//	// return success / fail val
//	return bReturn;
//}
//
///**
// * Renders the point object specified.
// *
// * @param pdwgentPoint the point object to be rendered
// *
// * @return TRUE if the point object is rendered successfully, otherwise FALSE
// */
//BOOL CDWGRenderEngine::renderPoint(Dwg_Object_Entity *pdwgentPoint, 
//	HDC hdcOutput, float fOutputScale, POINT ptAbsoluteOffset)
//{
//	HPEN hpenObject = NULL,
//		 hpenPrevious = NULL;
//	BOOL bReturn = TRUE;
//
//	try
//	{
//		// validate object
//		if(pdwgentPoint == NULL)
//			return FALSE;
//
//		// validate HDC
//		if(hdcOutput == NULL)
//			return FALSE;
//		
//		// validate scale
//		if(fOutputScale == 0.0f)
//			return FALSE;
//
//		// Get color
//
//		// Create pen
//
//		// Scale coordinates / measurements
//
//		// Draw
//
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		bReturn = FALSE;
//	}
//
//	// clear last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// garbage collect
//	if(hpenPrevious)
//	{
//		// restore original pen
//		SelectObject(hdcOutput, hpenPrevious);
//
//		// de-ref
//		hpenPrevious = NULL;
//	}
//	if(hpenObject)
//	{
//		// destory created pen
//		DeleteObject(hpenObject);
//
//		// de-ref
//		hpenObject = NULL;
//	}
//
//	// return success / fail val
//	return bReturn;
//}
//
///**
// * Renders the text object specified.
// *
// * @param pdwgentText the text object to be rendered
// *
// * @return TRUE if the text object is rendered successfully, otherwise FALSE
// */
//BOOL CDWGRenderEngine::renderText(Dwg_Object_Entity *pdwgentText, 
//	HDC hdcOutput, float fOutputScale, POINT ptAbsoluteOffset)
//{
//	HPEN hpenObject = NULL,
//		 hpenPrevious = NULL;
//	BOOL bReturn = TRUE;
//
//	try
//	{
//		Dwg_Entity_TEXT *pdwgtxtThis = NULL;
//		RECT rctOffset,
//			 rctObject;
//
//		// validate object
//		if(pdwgentText == NULL)
//			return FALSE;
//
//		// validate HDC
//		if(hdcOutput == NULL)
//			return FALSE;
//		
//		// validate scale
//		if(fOutputScale == 0.0f)
//			return FALSE;
//		
//		// Make sure this line should be visible... if not, this DOES NOT
//		//	 indicate an error condition
//		if(!layerIsActive(pdwgentText))
//			return TRUE;
//
//		// Get pointer to line entity, check
//		pdwgtxtThis = pdwgentText->tio.TEXT;
//		if(pdwgtxtThis == NULL)
//			return FALSE;
//
//		// Get offset RECT
//		GetClientRect(m_hwndOutputControl, &rctOffset);
//
//		// Create pen
//		hpenObject = createObjectPen(pdwgentText);
//		hpenPrevious = (HPEN)SelectObject(hdcOutput, hpenObject);
//		
//		// Initialize "POINT's"
//		memset(&rctObject, 0, sizeof(rctObject));
//
//		// Get coordinates / measurements
//		rctObject.left = floor(pdwgtxtThis->insertion_pt.x + 0.5f);
//		//rctObject.right = ?
//		rctObject.top = floor(pdwgtxtThis->insertion_pt.y + 0.5f);
//		rctObject.bottom = rctObject.left + floor(pdwgtxtThis->height + 0.5f);
//
//		// Offset by absolute offset
//		rctObject.left += ptAbsoluteOffset.x;
//		rctObject.top += ptAbsoluteOffset.y;
//		rctObject.bottom += ptAbsoluteOffset.y;
//		
//		// Perform offset and rotation to Cartesian coord space 
//		DPtoLP(hdcOutput, (LPPOINT)&rctOffset, 2);
//		rctObject.top = rctOffset.bottom - rctObject.top;
//		rctObject.bottom = rctOffset.bottom - rctObject.bottom;
//
//		// Draw
//		DrawText(hdcOutput, (TCHAR *)pdwgtxtThis->text_value, 
//			lstrlen((TCHAR *)pdwgtxtThis->text_value), &rctObject, 
//			DT_SINGLELINE | DT_VCENTER);
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		bReturn = FALSE;
//	}
//
//	// clear last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// garbage collect
//	if(hpenPrevious)
//	{
//		// restore original pen
//		SelectObject(hdcOutput, hpenPrevious);
//
//		// de-ref
//		hpenPrevious = NULL;
//	}
//	if(hpenObject)
//	{
//		// destory created pen
//		DeleteObject(hpenObject);
//
//		// de-ref
//		hpenObject = NULL;
//	}
//
//	// return success / fail val
//	return bReturn;
//}
//
///**
// * Returns whether or not the layer that the specified object is on is
// * active.
// *
// * @param pdwgentObject the object whose layer is to be examined
// *
// * @return TRUE if the parent layer for the object is active and no
// * errors occur, otherwise FALSE.
// */
//BOOL CDWGRenderEngine::layerIsActive(Dwg_Object_Entity *pdwgentObject)
//{
//	BOOL bReturn = FALSE;
//
//	try
//	{
//		Dwg_Object_LAYER *pdwgobjLayer = NULL;
//
//		// validate entity
//		if(pdwgentObject == NULL)
//			return FALSE;
//		
//		// make sure there is an active drawing
//		if(m_strFilename.length() == 0 || (m_dwgdatThis.header.section == NULL))
//			return FALSE;
//
//		// make sure the layer list is good
//		if(m_pllstLayers == NULL)
//			return FALSE;
//		if(m_pllstLayers->getLength() == 0)
//			return FALSE;
//
//		// get layer for entity
//		pdwgobjLayer = dwg_get_entity_layer(pdwgentObject);
//
//		// validate, continue
//		if(pdwgobjLayer)
//		{
//			DWGLAYERINFO *pdwglinfTemp = NULL;
//			long lcv,
//				 lCount;
//
//			// iterate through layers and attempt to locate the one specified
//			for(lcv = 0L, lCount = m_pllstLayers->getLength(); 
//				lcv < lCount;
//				lcv++)
//			{
//				// get current layer info
//				pdwglinfTemp = m_pllstLayers->getEntry(lcv);
//				
//				// validate, continue
//				if(pdwglinfTemp)
//				{
//					if(pdwglinfTemp->pdwgobjLayer == pdwgobjLayer)
//					{
//						// set return val
//						bReturn = pdwglinfTemp->bEnabled;
//						
//						// layer found, we're outta here
//						break;
//					}
//				}
//			}
//		}
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("While retrieving the specified entity's layer's state, an unexpected error occurred.");
//
//		// reset fail val
//		bReturn = FALSE;
//	}
//
//	// clear last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// return success / fail val
//	return bReturn;
//}
//
///**
// * Creates a pen using the proper color for the object specified.
// *
// * @param pdwgentObject
// *
// * @return the created pen if successful, otherwise NULL
// */
//HPEN CDWGRenderEngine::createObjectPen(Dwg_Object_Entity *pdwgentObject)
//{
//	HPEN hpenReturn = NULL;
//
//	try
//	{
//		Dwg_Object_LAYER *pdwgobjLayer = NULL;
//
//		// validate entity
//		if(pdwgentObject == NULL)
//			return NULL;
//
//		// attempt to get layer for object
//		pdwgobjLayer = dwg_get_entity_layer(pdwgentObject);
//
//		// validate, continue
//		if(pdwgobjLayer)
//		{
//			COLORREF clrLayer;
//			
//			// get this layer's color
//			clrLayer = getLayerColor(pdwgobjLayer);
//
//			// create the pen
//			hpenReturn = CreatePen(PS_SOLID, 1, clrLayer);
//		}
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		if(hpenReturn)
//			DeleteObject(hpenReturn);
//		hpenReturn = NULL;
//	}
//
//	// clear last error, if applicable
//	if(hpenReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// return success / fail val
//	return hpenReturn;
//}
//
///**
// * Returns the color for the specified layer. If the layer isn't found or
// * an error occurs, white (255,255,255) is returned.
// *
// * @param pdwgobjLayer The layer whose color is to be looked up (retrieved)
// *
// * @return White (255, 255, 255) if an error occurs or the color table
// * entry is missing, otherwise the color for the layer
// */
//COLORREF CDWGRenderEngine::getLayerColor(Dwg_Object_LAYER *pdwgobjLayer)
//{
//	COLORREF clrReturn = RGB(255, 255, 255);
//
//	try
//	{
//		PACCOLOR_ENTRY pacclrentTemp = NULL;
//		long lcv,
//			 lEntries;
//
//		// validate layer
//		if(pdwgobjLayer == NULL)
//			return clrReturn;
//		
//		// validate color table
//		if(m_pllstACColorTable == NULL)
//			return clrReturn;
//		if(m_pllstACColorTable->getLength() == 0)
//			return clrReturn;
//
//		// iterate through color table and find the specified color index
//		for(lcv = 0L, lEntries = m_pllstACColorTable->getLength();
//			lcv < lEntries;
//			lcv++)
//		{
//			// get current table entry
//			pacclrentTemp = m_pllstACColorTable->getEntry(lcv);
//
//			// validate, continue
//			if(pacclrentTemp)
//			{
//				if(pacclrentTemp->iIndex == pdwgobjLayer->color.index)
//				{
//					// set return color
//					clrReturn = pacclrentTemp->clrColor;
//					
//					// color was found, we're outta here
//					break;
//				}
//			}
//		}
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("While retrieving the color for the specified layer, an unexpected error occurred.");
//
//		// set fail val
//		clrReturn = RGB(255, 255, 255);
//	}
//
//	// clear last error, if we made it here
//	m_strLastError = EMPTY_STRING;
//
//	// return success / fail val
//	return clrReturn;
//}

/**
 * Loads the CADImporter library and assigns all internal object and function
 * pointers to their respective pointers in this class.
 *
 * @note In an actual production environment, this method would call a method
 * which would extract the library from the exe's resource file if the library
 * was not found.
 *
 * @return TRUE if the library is located and loaded successfully, otherwise
 * FALSE.
 */
BOOL CDWGRenderEngine::loadCADImporterLibrary()
{
	BOOL bReturn = TRUE;

	try
	{
		// validate module handle... NOTE: this should cause a FAIL return value
		//	 because the module handle *could be* invalid and therfore no
		//	 assumptions can be made about its state.
		if(m_hmodCADImporter)
		{
			// set last error
			m_strLastError = _T("The CAD Importer module's handle is already in use.");

			// return fail val
			return FALSE;
		}

		// attempt to load CADImporter library
		m_hmodCADImporter = LoadLibrary(_T("CADImporter.dll"));
		if(m_hmodCADImporter == NULL)
		{
			// set last error
			m_strLastError = _T("CADImporter.dll could NOT be loaded.");

			// return fail val
			return FALSE;
		}
		
		// attempt to get function pointers in library
		CADEnum = (CADENUM) GetProcAddress(m_hmodCADImporter, "CADEnum");
		CADCreate = (CADCREATE) GetProcAddress(m_hmodCADImporter, "CADCreate");
		CADClose = (CADCLOSE) GetProcAddress(m_hmodCADImporter, "CADClose");	
		CADIs3D = (CADIS3D) GetProcAddress(m_hmodCADImporter, "CADIs3D");
		CADGetLastError = (CADGETLASTERROR) GetProcAddress(m_hmodCADImporter, "CADGetLastError");
		CADProhibitCurvesAsPoly = (CADPROHIBITCURESASPOLY) GetProcAddress(m_hmodCADImporter, "CADProhibitCurvesAsPoly");
		CADLayoutCount = (CADLAYOUTCOUNT) GetProcAddress(m_hmodCADImporter, "CADLayoutCount");
		CADLayoutCurrent = (CADLAYOUTCURRENT) GetProcAddress(m_hmodCADImporter, "CADLayoutCurrent");
		CADLayoutName = (CADLAYOUTNAME) GetProcAddress(m_hmodCADImporter, "CADLayoutName");
		CADGetBox = (CADGETBOX) GetProcAddress(m_hmodCADImporter, "CADGetBox");
		CADSetSHXOptions = (CADSETSHXOPTIONS) GetProcAddress(m_hmodCADImporter, "CADSetSHXOptions");
		CADDraw = (CADDRAW)GetProcAddress(m_hmodCADImporter, "CADDraw");
		CADLayerCount = (CADLAYERCOUNT)GetProcAddress(m_hmodCADImporter, "CADLayerCount");
		CADLayer = (CADLAYER)GetProcAddress(m_hmodCADImporter, "CADLayer");
		CADVisible = (CADVISIBLE)GetProcAddress(m_hmodCADImporter, "CADVisible");
		CADGetSection = (CADGETSECTION)GetProcAddress(m_hmodCADImporter, "CADGetSection");
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to load the DWG library, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

	//BOOL bReturn = TRUE;

	//try
	//{


	//}
	//catch(...)
	//{
	//	// set last error
	//	m_strLastError = _T("");

	//	// set fail val
	//	bReturn = FALSE;
	//}

	//// clear last error, if applicable
	//if(bReturn)
	//	m_strLastError = EMPTY_STRING;

	//// return success / fail val
	//return bReturn;