///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CCreateDirectoryDialog object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "CCreateDirectoryDialog.h"
#include "..\XLanceView.h"
#include "..\Common\Registry.h"
#include "..\Resource\Resource.h"

// Module Level Vars
static CCreateDirectoryDialog *pccddlgThis = NULL;

/**
 * Default constructor, initializes all fields to their defaults.
 */
CCreateDirectoryDialog::CCreateDirectoryDialog()
{
	// initialize fields to their defaults
	m_hinstApplication = NULL;
	m_hwndThis = NULL;
	m_hbrBackground = NULL;
	m_hfontControls = NULL;
	m_strDirectoryName = EMPTY_STRING;
	m_strLastError = EMPTY_STRING;

	// set module static so message loop can have access to *this*
	pccddlgThis = this;
}

/**
 * Constructor which accepts the application HINSTANCE as an argument.
 *
 * @param hInstance
 *
 */
CCreateDirectoryDialog::CCreateDirectoryDialog(HINSTANCE hInstance)
{
	try
	{
		// initialize fields 
		m_strDirectoryName = EMPTY_STRING;
		m_strLastError = EMPTY_STRING;
		m_hbrBackground = NULL;
		m_hfontControls = NULL;
		m_hinstApplication = hInstance;

		// set module static so message loop can have access to *this*
		pccddlgThis = this;
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
CCreateDirectoryDialog::~CCreateDirectoryDialog()
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
LRESULT CALLBACK CCreateDirectoryDialog::WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	HWND hwndTemp = NULL;
	HDC hdcTemp = NULL;
	BOOL bReturn = FALSE;
	static BOOL bStartupComplete = FALSE;
	int iReturn = 0;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			// set handle
			pccddlgThis->m_hwndThis = hwnd;

			// load this dialog's preferences
			pccddlgThis->loadPreferences();
			
			// set icon
			pccddlgThis->loadIcon();

			// set font for controls
			//pccddlgThis->setControlFonts();

			// Register escape key
			RegisterHotKey(hwnd, HOTKEY_ESCAPE, (UINT)0, VK_ESCAPE);
			break;			

		//case WM_CTLCOLOREDIT:
		//case WM_CTLCOLORDLG:
		//case WM_CTLCOLORSTATIC:
		//	// get DC
		//	hdcTemp = (HDC)wParam;

		//	// validate DC, continue
		//	if(hdcTemp)
		//	{
		//		// create black brush
		//		if(pccddlgThis->m_hbrBackground == NULL)
		//			pccddlgThis->m_hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

		//		// Set background color for text
		//		SetBkColor(hdcTemp, RGB(0, 0, 0));
		//		
		//		// set text color
		//		SetTextColor(hdcTemp, RGB(255, 255, 255));
		//		
		//		// return brush
		//		return (LRESULT)pccddlgThis->m_hbrBackground;
		//	}
		//	break;

		case WM_DESTROY:
			// save preferences for this dialog ALWAYS
			pccddlgThis->savePreferences();

			// clear handle because window is being closed manually.
			pccddlgThis->m_hwndThis = NULL;

			// Unregister Escape key
			UnregisterHotKey(hwnd, HOTKEY_ESCAPE);
			break;

		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
			break;
	
		case WM_COMMAND:
			switch(wParam)
			{
				case IDC_CMDOK:
					// get directory name
					pccddlgThis->saveDirectoryName();

					// return modal OK
					EndDialog(hwnd, IDOK);
					break;
				
				case IDC_CMDCANCEL:
					// return modal Cancel
					EndDialog(hwnd, IDCANCEL);
					break;

				default:	// do nothing
					break;
			}
			break;

		case WM_HOTKEY:		
			EndDialog(hwnd, IDCANCEL);
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
int CCreateDirectoryDialog::show()
{
    int iReturn = 0;	// default to pesimistic return val

    try
    {
		// attempt to display dialog box
		iReturn = DialogBox(m_hinstApplication, 
							MAKEINTRESOURCE(IDD_DLGCREATEDIRECTORY),
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
BOOL CCreateDirectoryDialog::loadPreferences()
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
			REG_VAL_PREFS_LOCATION_CREATEDIRECTORY, &rctThis, 
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
BOOL CCreateDirectoryDialog::savePreferences()
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
			REG_VAL_PREFS_LOCATION_CREATEDIRECTORY, &rctThis, 
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
BOOL CCreateDirectoryDialog::loadIcon()
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
BOOL CCreateDirectoryDialog::setControlFonts()
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

		// attempt to get logging control's handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTDIRECTORYNAME);
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
 * Stores the directory name specified by the user in *this* object's
 * directory name field.
 */
BOOL CCreateDirectoryDialog::saveDirectoryName()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		TCHAR tstrBuffer[MAX_PATH];

		// validate this object's handle
		if(m_hwndThis == NULL)
			return FALSE;

		// get directory name from textbox
		GetDlgItemText(m_hwndThis, IDC_TXTDIRECTORYNAME, tstrBuffer, 
			sizeof(tstrBuffer));

		// store in member field
		m_strDirectoryName = tstrBuffer;

		// check returned directory name
		if(m_strDirectoryName.length() == 0)
			bReturn = FALSE;
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While saving the new directory's name, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}