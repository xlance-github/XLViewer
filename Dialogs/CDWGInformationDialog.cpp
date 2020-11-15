///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CDWGInformationDialog object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "CDWGInformationDialog.h"
#include "..\XLanceView.h"
#include "..\Common\Registry.h"
#include "..\Settings\CSettings.h"
#include "..\Resource\Resource.h"

// Module Level Vars
static CDWGInformationDialog *pcdwginfThis = NULL;

// Externals
extern CSettings g_csetApplication;

/**
 * Default constructor, initializes all fields to their defaults.
 */
CDWGInformationDialog::CDWGInformationDialog()
{
	// initialize fields to their defaults
	m_hinstApplication = NULL;
	m_hwndThis = NULL;
	m_hbrBackground = NULL;
	m_hfontControls = NULL;
	m_strLastError = EMPTY_STRING;
	m_strDWGInformation = EMPTY_STRING;

	// set module static so message loop can have access to *this*
	pcdwginfThis = this;
}

/**
 * Constructor which accepts the application HINSTANCE as an argument.
 *
 * @param hInstance
 *
 */
CDWGInformationDialog::CDWGInformationDialog(HINSTANCE hInstance, 
	TCHAR *ptcDWGInformationDialogString)
{
	try
	{
		// initialize fields 
		m_strLastError = EMPTY_STRING;
		m_hbrBackground = NULL;
		m_hfontControls = NULL;
		m_hinstApplication = hInstance;
		m_strDWGInformation = ptcDWGInformationDialogString;

		// set module static so message loop can have access to *this*
		pcdwginfThis = this;
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
CDWGInformationDialog::~CDWGInformationDialog()
{
	// destroy created GDI objects
	if(m_hfontControls)
		DeleteObject(m_hfontControls);
	if(m_hbrBackground)
		DeleteObject(m_hbrBackground);

	// destroy window object
	if(m_hwndThis)
		DestroyWindow(m_hwndThis);
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
LRESULT CALLBACK CDWGInformationDialog::WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	HWND hwndTemp = NULL;
	HDC hdcTemp = NULL;
	BOOL bReturn = FALSE;
	static BOOL bStartupComplete = FALSE;
	int iReturn = 0,
		iCtrlID = 0;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			// set handle
			pcdwginfThis->m_hwndThis = hwnd;

			// load this dialog's preferences
			pcdwginfThis->loadPreferences();

			// set icon
			pcdwginfThis->loadIcon();

			// set font for controls
			pcdwginfThis->setControlFonts();

			// load information
			pcdwginfThis->loadDWGInformation();

			// Register escape key
			RegisterHotKey(hwnd, HOTKEY_ESCAPE, (UINT)0, VK_ESCAPE);
			break;			

		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORDLG:
			// get DC
			hdcTemp = (HDC)wParam;

			// validate DC, continue
			if(hdcTemp)
			{
				// create black brush
				if(pcdwginfThis->m_hbrBackground == NULL)
					pcdwginfThis->m_hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

				if(wParam == WM_CTLCOLORSTATIC)
					SetBkMode(hdcTemp, TRANSPARENT);
				
				// Set background color for text
				SetBkColor(hdcTemp, RGB(0, 0, 0));
				
				// set text color
				SetTextColor(hdcTemp, RGB(255, 255, 255));
				
				// return brush
				return (LRESULT)pcdwginfThis->m_hbrBackground;
			}
			break;

		case WM_DESTROY:
			// save preferences for this dialog ALWAYS
			pcdwginfThis->savePreferences();

			// clear handle because window is being closed manually.
			pcdwginfThis->m_hwndThis = NULL;

			// Unregister Escape key
			UnregisterHotKey(hwnd, HOTKEY_ESCAPE);
			break;

		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
			break;
	
		case WM_HOTKEY:		
			EndDialog(hwnd, IDCANCEL);
			break;

		case WM_SHOWWINDOW:
			// remove "auto" selection
			hwndTemp = GetDlgItem(hwnd, IDC_TXTDWGINFORMATION);

			// validate, continue
			if(hwndTemp != NULL)
				// clear selection and move caret to start of text
				SendMessage(hwndTemp, EM_SETSEL, (WPARAM)0, 0L);
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
int CDWGInformationDialog::show()
{
    int iReturn = 0;	// default to pesimistic return val

    try
    {
		// attempt to display dialog box
		iReturn = DialogBox(m_hinstApplication, 
							MAKEINTRESOURCE(IDD_DLGDWGINFORMATION),
							NULL, (DLGPROC)WindowProc);
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
 * Loads, from the registry, the user's preferences for this dialog.
 *
 * @note This object's GUI (dialog) is centered by the framework at
 * startup (the dialog template's Center property is set to TRUE).
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CDWGInformationDialog::loadPreferences()
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
			REG_VAL_PREFS_LOCATION_DWGINFO, &rctThis, 
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

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

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
BOOL CDWGInformationDialog::savePreferences()
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
			REG_VAL_PREFS_LOCATION_DWGINFO, &rctThis, 
			sizeof(rctThis), TRUE);
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

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
BOOL CDWGInformationDialog::loadIcon()
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

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

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
BOOL CDWGInformationDialog::setControlFonts()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HWND hwndTemp = NULL;
		HDC hdcTemp = NULL;
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

		// attempt to get help text control's handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTDWGINFORMATION);
		if(hwndTemp == NULL)
			return FALSE;
		
		// Get DC for log control
		hdcTemp = GetDC(hwndTemp);
		if(hdcTemp == NULL)
			return FALSE;

		// calculate desired font size in pixels...
		iHeight = -MulDiv(11, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);

		// Attempt to create desired font (Terminal)
		m_hfontControls = CreateFont(iHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, 
								DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								0, _T("Terminal"));

		// validate font, continue
		if(m_hfontControls == NULL)
		{
			// release DC for txtLog
			ReleaseDC(hwndTemp, hdcTemp);

			// return fail val
			return FALSE;
		}

		// attempt to get existing font
		SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls, 
			(LPARAM)MAKELPARAM(TRUE, 0));

		// Attempt to get label's handle

		// release DC for txtLog
		ReleaseDC(hwndTemp, hdcTemp);
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Loads the drawing information provided into the UI for display.
 */
BOOL CDWGInformationDialog::loadDWGInformation()
{
	BOOL bReturn = TRUE;

	try
	{
		HWND hwndTemp = NULL;

		// validate *this* object's window handle
		if(m_hwndThis == NULL)
			return FALSE;

		// validate incoming text
		if(m_strDWGInformation.length() == 0)
			return FALSE;

		// Set text
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTDWGINFORMATION);
		if(hwndTemp)
		{
			// set text
			SendMessage(hwndTemp, WM_SETTEXT, (WPARAM)m_strDWGInformation.length(),
				(LPARAM)m_strDWGInformation.c_str());
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}