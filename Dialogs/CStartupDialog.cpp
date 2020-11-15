///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CStartupDialog object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include <psapi.h>
#include "CStartupDialog.h"
#include "CSettingsDialog.h"
#include "..\XLanceView.h"
#include "..\DriveInformation.h"
#include "..\Common\Registry.h"
#include "..\Resource\Resource.h"
#include "..\Settings\CSettings.h"
#include "..\Utility\CGraphicsDeviceInformation.h"
#include "..\Utility\CAttachedDrives.h"
#include "..\Common\FileIO.h"

// Module Level Vars
static CStartupDialog *pcsudlgThis = NULL;
extern CSettings g_csetApplication;
extern CGraphicsDeviceInformation g_cginfPrimaryDevice;

/**
 * Default constructor, initializes all fields to their defaults.
 */
CStartupDialog::CStartupDialog()
{
	// initialize fields to their defaults
	m_hinstApplication = NULL;
	m_hwndThis = NULL;
	m_dlgsetSettings = NULL;
	m_cadrvDrives = NULL;
	m_hfontLoggingFont = NULL;
	m_hbrBackground = NULL;
	m_strLastError = EMPTY_STRING;

	// set module static so message loop can have access to *this*
	pcsudlgThis = this;
}

/**
 * Constructor which accepts the application HINSTANCE as an argument.
 *
 * @param hInstance
 *
 */
CStartupDialog::CStartupDialog(HINSTANCE hInstance)
{
	try
	{
		// initialize fields 
		m_strLastError = EMPTY_STRING;
		m_dlgsetSettings = NULL;
		m_cadrvDrives = NULL;
		m_hfontLoggingFont = NULL;
		m_hbrBackground = NULL;
		m_hinstApplication = hInstance;

		// set module static so message loop can have access to *this*
		pcsudlgThis = this;
	}
	catch(...)
	{
		// garbage collect, clear references
		m_hinstApplication = NULL;
		if(m_hwndThis)
		{
			DestroyWindow(m_hwndThis);
			m_hwndThis = NULL;
		}

		// set last error
		m_strLastError = _T("While intializing the startup dialog object, an unspecified exception occurred.");
	}
}
                                 
/**
 * Destructor, performs clean-up on fields.
 */
CStartupDialog::~CStartupDialog()
{
	// destroy created GDI objects
	if(m_hfontLoggingFont)
		DeleteObject(m_hfontLoggingFont);
	if(m_hbrBackground)
		DeleteObject(m_hbrBackground);

	// destroy other objects
	if(m_cadrvDrives)
		delete m_cadrvDrives;

	// destroy window object
	if(m_hwndThis)
		DestroyWindow(m_hwndThis);
	if(m_dlgsetSettings)
		delete m_dlgsetSettings;
}

/**
 * Handles the processing of all messages for *this* object's GUI
 * (dialog).
 *
 * @param hwnd
 *
 * @param uMsg
 *
 * @param wParam
 *
 * @param lParam
 *
 * @return 0 if the message is processed by this message pump, 
 * otherwise DefWindowProc() is called and its return value used.
 */
LRESULT CALLBACK CStartupDialog::WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	HWND hwndTemp = NULL;
	HDC hdcTemp = NULL;
	BOOL bReturn = FALSE;
	static BOOL bStartupComplete = FALSE;
	int iReturn = 0;
#ifdef _DEBUG
	LPDEVMODE lpdevmTest = NULL;
#endif

	switch(uMsg)
	{
		case WM_INITDIALOG:
			// set handle
			pcsudlgThis->m_hwndThis = hwnd;

			// load this dialog's preferences
			pcsudlgThis->loadPreferences();
			
			// set icon
			pcsudlgThis->loadIcon();

			// set font for txtLog
			pcsudlgThis->setLoggingFont();

#ifdef _DEBUG
			// Mark 70 character display width
			pcsudlgThis->writeLogEntry(_T("1234567890123456789012345678901234567890123456789012345678901234567890\r\n"));
#endif
			break;

		case WM_CTLCOLOREDIT:
			// get DC
			hdcTemp = (HDC)wParam;

			// validate DC, continue
			if(hdcTemp)
			{
				// create black brush
				if(pcsudlgThis->m_hbrBackground == NULL)
					pcsudlgThis->m_hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

				// Set background color for text
				SetBkColor(hdcTemp, RGB(0, 0, 0));
				
				// set text color
				SetTextColor(hdcTemp, RGB(255, 255, 255));
				
				// return brush
				return (LRESULT)pcsudlgThis->m_hbrBackground;
			}
			break;

		case WM_DESTROY:
			// save preferences
			pcsudlgThis->savePreferences();
			break;
	
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_CMDEXIT:
					// close application
					EndDialog(hwnd, IDCANCEL);
					break;
				
				case IDC_CMDSETTINGS:
					// display settings dialog
					iReturn = pcsudlgThis->showSettings();
					break;

				case IDC_CMDRUN:
#ifdef _DEBUG
					iReturn = g_cginfPrimaryDevice.getGraphicsDeviceModeIndex(_T("1440 x 900 32-bit 60 Hz"));
					if(iReturn > -1)
						lpdevmTest = g_cginfPrimaryDevice.getGraphicsDeviceModeFromIndex(iReturn);
					if(lpdevmTest)
						delete lpdevmTest;
#endif

					// close this dialog and return "OK" (run viewer)
					EndDialog(hwnd, IDOK);
					break;

				default:	// do nothing
					break;
			}
			break;

		case WM_SHOWWINDOW:
			if((BOOL)wParam == TRUE && !bStartupComplete)
			{
				// set "done" flag
				bStartupComplete = TRUE;

				// run checks on exe
				bReturn = pcsudlgThis->runStartupChecks();
				
				// If everything checked out, continue load...
				if(bReturn)
				{
					// validate settings
					if(!pcsudlgThis->validateSettings())
					{
						// move down a two lines
						pcsudlgThis->writeLogEntry(_T("\r\n\r\n"));

						// write any errors...
						pcsudlgThis->writeLogEntry(
							(TCHAR *)pcsudlgThis->m_strLastError.data());
					}
				}

				// get handle for cmdRun
				hwndTemp = GetDlgItem(hwnd, IDC_CMDRUN);
				EnableWindow(hwndTemp, bReturn);
			}
			break;

		case WM_CLOSE:
			// close application
			PostQuitMessage(0);
			break;

		default:
			return 0;//DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	// return "message processed"
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Creates and displays this object's dialog. This object represents a non-
 * modal dialog which contains the settings for the application.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
int CStartupDialog::show()
{
    int iReturn = 0;	// default to pesimistic return val

    try
    {
		// attempt to display dialog box
		iReturn = DialogBox(m_hinstApplication, 
							MAKEINTRESOURCE(IDD_DLGSTARTUP),
							NULL, (DLGPROC)WindowProc);

		// Check handle... if it's good, then display
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("While creating the startup window, an unexpected error occurred.");

			// set fail return val
			iReturn = 0;
		}
    }
    catch(...)
    {
        
        // set fail value
        iReturn = 0;
    }

    // return success / fail val
    return iReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Creates and/or shows the settings dialog. NOTE: this method takes care of
 * the static method non-static member field problem.
 *
 * @return TRUE if the settings dialog is displayed and no errors occur,
 * otherwise FALSE.
 */
BOOL CStartupDialog::showSettings()
{
	BOOL bReturn = TRUE;	// default to optimistic return val

	try
	{
		// Check and see if the dialog has already been loaded/created
		if(m_dlgsetSettings)
			return m_dlgsetSettings->show();

		// Attempt to create the dialog
		m_dlgsetSettings = new CSettingsDialog(m_hinstApplication, m_hwndThis);

		// validate, return appropriate val
		if(m_dlgsetSettings == NULL)
			bReturn = FALSE;
		else if(lstrlen(m_dlgsetSettings->getLastError()) != 0)
		{
			// get error
			m_strLastError = m_dlgsetSettings->getLastError();

			// set fail val
			bReturn = FALSE;
		}
		else
			m_dlgsetSettings->show();
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("The settings dialog could NOT be shown because an exception occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Loads, from the registry, the user's preferences for this dialog.
 *
 * @note This object's GUI (dialog) is centered by the framework at
 * startup (the dialog template's Center property is set to TRUE).
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CStartupDialog::loadPreferences()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		RECT rctThis,
			 rctScreen;
		int iWidth,
			iHeight;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}
		
		// Position window centered of the current monitor initially.
		//	 Get work area
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rctScreen, 0);
		//	 Get this window's dimensions and location
		GetWindowRect(m_hwndThis, &rctThis);
		//	 Calc width and height
		iWidth = rctThis.right - rctThis.left;
		iHeight = rctThis.bottom - rctThis.top;
		//	 Move rectangle to center
		rctThis.left = ((rctScreen.right - rctScreen.left) 
						 - iWidth) / 2;
		rctThis.top = ((rctScreen.bottom - rctScreen.top) 
						- iHeight) / 2;
		//	 Move window to center
		MoveWindow(m_hwndThis, rctThis.left, rctThis.top, 
			iWidth, iHeight, TRUE);

		// initialize structure
		memset(&rctThis, 0, sizeof(rctThis));

		// Attempt to load this dialog's stored location
		GetRegistryBinary(CurrentUser, REG_BASE, 
			REG_SECTION_PREFERENCES,
			REG_VAL_PREFS_LOCATION_STARTUP, &rctThis, 
			sizeof(rctThis), FALSE);

		// Check and see if a stored value was present... if the
		//	 position of the dialog was stored previously then there
		//	 be something other than 0's for the right and bottom
		if(rctThis.bottom != 0 && rctThis.right != 0)
			SetWindowPos(m_hwndThis, HWND_TOP, rctThis.left,
				rctThis.top, 0, 0, SWP_NOSIZE);
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Saves, to the registry, the user's preferences for this dialog.
 *
 *
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CStartupDialog::savePreferences()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		RECT rctThis;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}
		
		// Get this object's dialog's position/size
		GetWindowRect(m_hwndThis, &rctThis);

		// Attempt to save this dialog's location
		SaveRegistryBinary(CurrentUser, REG_BASE, 
			REG_SECTION_PREFERENCES,
			REG_VAL_PREFS_LOCATION_STARTUP, &rctThis, 
			sizeof(rctThis), TRUE);
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Loads the icon for this window at startup.
 *
 * @param hwndThis HWND of this object's GUI (window)
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CStartupDialog::loadIcon()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HICON hiconThis = NULL;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}

		// Validate application instance
		if(m_hinstApplication == NULL)
			return FALSE;

		// attempt to load icon
		hiconThis = LoadIcon(m_hinstApplication, MAKEINTRESOURCE(IDI_APPICON48X48));
		if(hiconThis != NULL)
		{
			// attempt to assign icon
			SendMessage(m_hwndThis, WM_SETICON, ICON_BIG, (LPARAM)hiconThis);
		}
		else
			bReturn = FALSE;
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Sets the FONT object used by the logging control (txtLog).
 *
 * @param hwndThis
 *
 * @return
 */
BOOL CStartupDialog::setLoggingFont()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HWND hwndLog = NULL;
		HDC hdcLog = NULL;
		int iHeight = 0;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}

		// Validate application instance
		if(m_hinstApplication == NULL)
			return FALSE;

		// attempt to get logging control's handle
		hwndLog = GetDlgItem(m_hwndThis, IDC_TXTLOG);
		if(hwndLog == NULL)
			return FALSE;
		
		// Get DC for log control
		hdcLog = GetDC(hwndLog);
		if(hdcLog == NULL)
			return FALSE;

		// calculate desired font size in pixels...
		iHeight = -MulDiv(11, GetDeviceCaps(hdcLog, LOGPIXELSY), 72);

		// Attempt to create desired font (Terminal)
		m_hfontLoggingFont = CreateFont(iHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, 
								DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								0, _T("Terminal"));

		// validate font, continue
		if(m_hfontLoggingFont == NULL)
		{
			// release DC for txtLog
			ReleaseDC(hwndLog, hdcLog);

			// return fail val
			return FALSE;
		}

		// attempt to get existing font
		SendMessage(hwndLog, WM_SETFONT, (WPARAM)m_hfontLoggingFont, 
			(LPARAM)MAKELPARAM(TRUE, 0));

		// release DC for txtLog
		ReleaseDC(hwndLog, hdcLog);
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Performs validation on all application settings which can, if improperly
 * changed, result in major problems. Currently, the settings which are
 * validate are:
 * 
 *	- Graphics Device
 *	- Graphics Mode
 *
 * @return TRUE if all settings validate and no errors occur, otherwise FALSE.
 */
BOOL CStartupDialog::validateSettings()
{
	TCHAR *ptcModeString = NULL;
	BOOL bReturn = FALSE;	// default to optimistic return val

    try
    {
		// After a number of revisions, the below covers the requirements as
		//	 concisely as possible. Basically, if there is no graphics device
		//	 assigned OR the selected graphics device DOES NOT match what
		//	 Windows(r) says is the primary graphics device, we need to reset
		//	 everything to its default, period. All bets are off in these
		//	 cases.

		if(lstrcmp(g_csetApplication.graphicsDevice(), 
			g_cginfPrimaryDevice.graphicsDeviceName()) != 0)
		{
			// set display device
			g_csetApplication.graphicsDevice(g_cginfPrimaryDevice.graphicsDeviceName());

			// set default mode... This involves locating the highest color depth
			//	 1024 x 768 mode.
			ptcModeString = g_cginfPrimaryDevice.getGraphicsDeviceMode(1024, 768);
			if(ptcModeString == NULL)
			{
				// set last error
				m_strLastError = _T("The default graphics device mode could not be set. This application requires a minimum supported resolution of 1024 x 768.");

				// write error message to log
				writeLogEntry(ptcModeString);

				// return fail
				return FALSE;
			}
			
			// set graphics mode
			g_csetApplication.graphicsMode(ptcModeString);

			// un-set always run full-screen option
			g_csetApplication.alwaysLaunchFullScreen(FALSE);

			// return success
			bReturn = TRUE;
		}
		else
		{
			TCHAR *ptcGraphicsDeviceModes = NULL;

			// Check selected graphics mode
			ptcGraphicsDeviceModes = g_cginfPrimaryDevice.graphicsDeviceModes();
			if(ptcGraphicsDeviceModes == NULL)
			{
				// set last error
				m_strLastError = _T("No supported graphics modes were found for the current graphics device.");

				// write error message to log
				writeLogEntry(ptcModeString);

				// return fail
				return FALSE;
			}

			// check and see if the selected mode is valid
			if(_tcsstr(g_csetApplication.graphicsMode(), ptcGraphicsDeviceModes) != 0)
			{
				// Mode isn't valid

				// log message
				writeLogEntry(_T("The selected graphics device mode isn't supported. Reverting to default mode (1024 x 768)."));

				// get mode
				ptcModeString = g_cginfPrimaryDevice.getGraphicsDeviceMode(1024, 768);
				if(ptcModeString == NULL)
				{
					// set last error
					m_strLastError = _T("The default graphics device mode could not be set. This application requires a minimum supported resolution of 1024 x 768.");

					// write error message to log
					writeLogEntry(ptcModeString);

					// return fail
					return FALSE;
				}				

				// set graphics mode
				g_csetApplication.graphicsMode(ptcModeString);	

				// set success return val
				bReturn = TRUE;
			}
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While validating the application settings, an exception occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// if everything went OK, clear last error
	if(bReturn)
	{
		// clear last error
		m_strLastError = EMPTY_STRING;
	}

	// garbage collect mode string
	if(ptcModeString)
	{
		delete[] ptcModeString;
		ptcModeString = NULL;
	}

    // return success / fail val
    return bReturn;
}

/**
 * Manages all startup checks performed.
 *
 * 
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CStartupDialog::runStartupChecks()
{
    BOOL bReturn = FALSE;	// default to optimistic return val

    try
    {
		tstring strLog;
		DWORD dwProcessID = (DWORD)0;

		// print startup log entry
		strLog = _T("Starting ");
		strLog += MAINWINDOW_TITLE;
		strLog += _T("...\r\n\r\n");
		writeLogEntry((TCHAR *)strLog.data());

		// Now, run exe name checks...
		bReturn = applicationNameCheck();
		if(!bReturn)
			return bReturn;

		// Next, write spacer and check exe security
		writeLogEntry(_T("\r\n"));
		//bReturn = applicationSecurityChecks();

		// Now, write spacer and load available drives
		writeLogEntry(_T("\r\n"));
		bReturn = getAvailableDrives();

		//Checking whether .NET framework is installed or not.
		if(ISDotNetFrameworkInstalled()&&ISDependentObjectsPresent())
		{
			bReturn = TRUE;
		}
		else
		{
			bReturn = FALSE;
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("An exception occurred while attempting to run the startup checks.");

        // set fail value
        bReturn = FALSE;
    }

	// if everything went OK, clear last error and write ready message
	if(bReturn)
	{
		// clear last error
		m_strLastError = EMPTY_STRING;

		// write ready message
		writeLogEntry(_T("\r\n\r\nReady."));
	}

    // return success / fail val
    return bReturn;
}

/* check whether the .NET framework is installed or not
 *	returns 1 - if installed
 *			0 - if not installed
 */
BOOL CStartupDialog::ISDotNetFrameworkInstalled()
{
	BOOL bReturn = FALSE;
	tstring strTemp = EMPTY_STRING;
	writeLogEntry(_T("\r\n"));
	strTemp = _T("Checking Microsoft .NET framework 4.0: ");
	writeLogEntry((TCHAR *)strTemp.data());

	bReturn =  (BOOL)GetRegistryNumeric(LocalMachine, "SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4", 
		"Client", "Install", TRUE);

	if(bReturn)
		strTemp = _T("\t---> OK\r\n");
	else
		strTemp = _T("\t---> Not Registered\r\n");
	writeLogEntry((TCHAR *)strTemp.data());


	return bReturn;
}


BOOL CStartupDialog::ISDependentObjectsPresent()
{
	BOOL bReturn = FALSE;
    BOOL bItextSharpReturn = FALSE;
    BOOL bPdfSharpReturn = FALSE;
    BOOL bXLanceDLLReturn = FALSE;
    tstring strLibraryFilename = EMPTY_STRING;
	writeLogEntry(_T("\r\n"));
	strLibraryFilename = g_csetApplication.applicationFolder();
	bItextSharpReturn = ISFileExists(strLibraryFilename, "itextSharp.dll");
	//bPdfSharpReturn = ISFileExists(strLibraryFilename, "PdfSharp.dll");
	bPdfSharpReturn = TRUE;
	bXLanceDLLReturn = ISFileExists(strLibraryFilename, "XLanceDLL.dll");
	if(bItextSharpReturn && bPdfSharpReturn && bXLanceDLLReturn)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CStartupDialog::ISFileExists(tstring CurrentDirPath, tstring FileName)
{
	BOOL bReturn = FALSE;
    DWORD dwReturn = 0;
	tstring strTemp = EMPTY_STRING;
	strTemp = _T("Checking " + FileName + " :");
	writeLogEntry((TCHAR *)strTemp.data());
	
	if(CurrentDirPath[CurrentDirPath.length() - 1] != _T('\\'))
		CurrentDirPath += _T("\\");
	CurrentDirPath += FileName;
	dwReturn = Exists((TCHAR *)CurrentDirPath.c_str()); 
	if(dwReturn)
	{
		bReturn = TRUE;
		strTemp = _T("\t\t---> OK\r\n");
	}
	else
	{
		bReturn = FALSE;
		strTemp = _T("\t\t\t---> Missing\r\n");
	}
	writeLogEntry((TCHAR *)strTemp.data());
	return bReturn;
}

/**
 * Appends the specified text to the logging textbox.
 *
 * @param strEntry the text to append to the "log"
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CStartupDialog::writeLogEntry(TCHAR *strEntry)
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HWND hwndLog = NULL;

		// check entry... make sure there is something to do
		if(lstrlen(strEntry) == 0)
			return bReturn;

		// validate handle for this object's window
		if(m_hwndThis == NULL)
			return FALSE;

		// attempt to get handle for logging textbox
		hwndLog = GetDlgItem(m_hwndThis, IDC_TXTLOG);

		// validate, continue
		if(hwndLog != NULL)
		{
			// append string to "log"
			SendMessage(hwndLog, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)strEntry);

			// Move caret to end of text
			SendMessage(hwndLog, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
		}
		else
			bReturn = FALSE;
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("An exception occurred while attempting to write an entry to the log.");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Verifies that the application name has not been tampered with.
 *
 * @return TRUE if the application's name passes, otherwise
 * FALSE is returned. 
 */
BOOL CStartupDialog::applicationNameCheck()
{
	BOOL bReturn = FALSE;	// default to pesimistic return val

	try
	{
		HANDLE hProcessThis = NULL;
		DWORD dwProcessID = (DWORD)0;
		TCHAR tstrProcessName[MAX_PATH] = EMPTY_STRING,
			  tstrOriginalEXEName[MAX_PATH] = EMPTY_STRING;	
		tstring strTemp = EMPTY_STRING;
		int iReturn = 0;
	
		// Display message indicating what is occurring...
		strTemp = _T("Checking original file name: ");
		writeLogEntry((TCHAR *)strTemp.data());

		// validate application instance
		if(m_hinstApplication == NULL)
		{
			// display error message
			strTemp = _T("The application instance is invalid.\r\n");
			writeLogEntry((TCHAR *)strTemp.data());

			// return fail val
			return bReturn;
		}

		// Get original filename...
		iReturn = LoadString(m_hinstApplication, IDS_FILETITLE_APPLICATION, 
						tstrOriginalEXEName, sizeof(tstrOriginalEXEName));

		// validate, continue
		if(iReturn == 0)
		{
			// display error message
			strTemp = _T("Could not load the application executable's name from the resource file.\r\n");
			writeLogEntry((TCHAR *)strTemp.data());
		}
		strTemp = tstrOriginalEXEName;
		writeLogEntry((TCHAR *)strTemp.data());

		// Next, get the process ID from this object's window
		dwProcessID = GetCurrentProcessId();
		if(dwProcessID == 0)
		{
			// display last error
			strTemp = _T("\r\nCould not get the process ID for this application.\r\n");
			writeLogEntry((TCHAR *)strTemp.data());

			// return fail val
			return bReturn;
		}

		// Now, attempt to open the process...
		hProcessThis = OpenProcess(PROCESS_QUERY_INFORMATION /*PROCESS_ALL_ACCESS*/, FALSE, dwProcessID);

		// validate, continue
		if(hProcessThis != NULL && hProcessThis != INVALID_HANDLE_VALUE)
		{
			DWORD dwSize = (DWORD)0, 
				  dwReturn = (DWORD)0;

			// Next, attempt to get this module's name
			dwSize = sizeof(tstrProcessName);
			// Doesn't work for XP
			//dwReturn = GetModuleFileNameEx(hProcessThis, NULL, tstrProcessName, dwSize);	
			dwReturn = GetModuleFileName(NULL, tstrProcessName, dwSize);

			// validate, continue
			if(dwReturn != (DWORD)0)
			{
				TCHAR *pc = NULL;

				// find last backslash
				for(int i = dwReturn; i > -1; i--)
				{
					if(tstrProcessName[i] == '\\')
					{	
						// mark location
						pc = &tstrProcessName[i];

						// move to beginning of exe name
						pc++;
						
						// we're outta here
						break;
					}
				}
				
				// Check and make sure the file title was found
				if(pc)
				{
					// compare exe names...
					bReturn = (lstrcmp(pc, tstrOriginalEXEName) == 0);

					// display pass or fail message
					if(bReturn)
						// create message
						strTemp = _T("\t---> OK\r\n");
					else
					{
						// create message
						strTemp = _T("\t\t---> NO !\r\n\r\n");
						strTemp += _T("Original filename has been changed.\r\n");
						strTemp += _T("Please rename the program executable file to: ");
						strTemp += _T("\t---> ");
						strTemp += tstrOriginalEXEName;
					}

					// write message
					writeLogEntry((TCHAR *)strTemp.data());
				}
				else
				{
					// display error message
					strTemp = _T("\r\nCould not locate the application executable's name in the fullpath.\r\n");
					writeLogEntry((TCHAR *)strTemp.data());
				}
			}
		}
		else
		{
			// display last error
			strTemp = _T("\r\nCould not retrieve this application's process information.\r\n");
			writeLogEntry((TCHAR *)strTemp.data());
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While running checks on the application's executable, an exception occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Manages the identification and retrieval of information about all currently
 * attached drives.
 * 
 * @note The currently information retrieved for each drive is as follows:
 * 
 *		Type (removable, fixed, CD, etc.)
 *		File System (NTFS, FAT, CDFS, etc.)
 *		Approximated type (Floppy, Hard disk, USB, etc.)
 *
 * @return TRUE if no errors occur, otherwise FALSE.
 */
BOOL CStartupDialog::getAvailableDrives()
{
	BOOL bReturn = FALSE;	// default to pesimistic return val

	try
	{
		DWORD dwReturn = (DWORD)0,
			  dwLength = (DWORD)0;
		tstring strTemp = EMPTY_STRING;

		// Display search message...
		strTemp = _T("Searching for drives...\r\n\r\n");
		writeLogEntry((TCHAR *)strTemp.data());

		// attempt to create attached drives information object
		m_cadrvDrives = new CAttachedDrives();

		// validate, continue
		if(m_cadrvDrives != NULL)
		{
			LPDRIVEINFORMATION lpdrvinfTemp = NULL;
			TCHAR tstrPadding[DRIVETYPE_MAX_DESCRIPTIONLENGTH + 1] = EMPTY_STRING;
			tstring strTemp = EMPTY_STRING,
				        strOutput = EMPTY_STRING;

			// create padding
			memset(tstrPadding, 0, sizeof(tstrPadding)); 
      for (size_t it = 0; it < DRIVETYPE_MAX_DESCRIPTIONLENGTH; ++it)
      {
        tstrPadding[it] = DRIVETYPE_PADDING_CHARACTER;
      }

			// iterate through drive information
			for(int i = 0; i < m_cadrvDrives->attachedDriveCount(); i++)
			{
				// get current drive information object
				lpdrvinfTemp = m_cadrvDrives->driveInformation(i);

				// validate, continue
				if(lpdrvinfTemp != NULL)
				{
					// get drive letter
					strOutput = lpdrvinfTemp->tchDriveLetter;
					strOutput += _T(":\\ ");
					
					// create drive type, padded string
					strTemp = lpdrvinfTemp->tstrDriveType;
					
					// add padding and truncate at max length
					strTemp += tstrPadding;
					strTemp[DRIVETYPE_MAX_DESCRIPTIONLENGTH] = 0;
					
					// append to output string
					strOutput += (TCHAR *)strTemp.data();
					
					// append file system
					strOutput += _T(" ");
					strOutput += lpdrvinfTemp->tstrFileSystem;

					// append new line
					strOutput += _T("\r\n");

					// output string
					writeLogEntry((TCHAR *)strOutput.data());
				}
			}

			// If we made it here, return success
			bReturn = TRUE;
		}
		else
		{
			// display error message
			strTemp = _T("Could not retrieve information for the currently attached drives.\r\n");
			writeLogEntry((TCHAR *)strTemp.data());
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While loading the available drives, an exception occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}