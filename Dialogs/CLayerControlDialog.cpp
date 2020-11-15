///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CLayerControlDialog object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "CLayerControlDialog.h"
#include "..\XLanceView.h"
#include "..\Common\Registry.h"
#include "..\Settings\CSettings.h"
#include "..\Resource\Resource.h"

// Module Level Vars
static CLayerControlDialog *pclyrctldlgThis = NULL;

// Externals
extern CSettings g_csetApplication;

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////

#define STRING_LISTHEADER			_T("ON/OFF  Layer Name")
#define STRING_LISTHEADERSEPARATOR	_T("========================================")
#define STRING_FORMAT_LAYER			_T("%-6s  %s")
#define STRING_LAYERINDICATOR_ON	_T("ON")
#define STRING_LAYERINDICATOR_OFF	_T("OFF")

#define INDEX_FIRST_LAYER			2

///////////////////////////////////////////////////////////////////////////////
// Object Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor, initializes all fields to their defaults.
 */
CLayerControlDialog::CLayerControlDialog()
{
	// initialize fields to their defaults
	m_hinstApplication = NULL;
	m_hwndThis = NULL;
	m_hbrBackground = NULL;
	m_hfontControls = NULL;
	m_pllstLayerControl = NULL;
	m_pllstSourceLayers = NULL;
	m_strLastError = EMPTY_STRING;

	// set module static so message loop can have access to *this*
	pclyrctldlgThis = this;
}

/**
 * Constructor which accepts the application HINSTANCE as an argument.
 *
 * @param hInstance
 *
 */
CLayerControlDialog::CLayerControlDialog(HINSTANCE hInstance, 
	LinkedListEx<DWGLAYERINFO> *pllstLayers)
{
	try
	{
		// initialize fields 
		m_strLastError = EMPTY_STRING;
		m_hbrBackground = NULL;
		m_hfontControls = NULL;
		m_hinstApplication = hInstance;
		m_pllstSourceLayers = pllstLayers;
		m_pllstLayerControl = new LinkedListEx<DWGLAYERINFO>();

		// Copy list data from actual list to local, temp list
		copyLayerInformation();

		// set module static so message loop can have access to *this*
		pclyrctldlgThis = this;
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
CLayerControlDialog::~CLayerControlDialog()
{
	// destroy created GDI objects
	if(m_hfontControls)
		DeleteObject(m_hfontControls);
	if(m_hbrBackground)
		DeleteObject(m_hbrBackground);

	// destroy window object
	if(m_hwndThis)
		DestroyWindow(m_hwndThis);

	// destroy temp list and its objects
	if(m_pllstLayerControl)
	{
		DWGLAYERINFO *pdwglinfTemp = NULL;

		// Since we're going to manually destroy the list objects,
		//	 set list so it doesn't destroy the objects when its
		//	 clear() method is called
		m_pllstLayerControl->setAutoDestroy(FALSE);
		
		// iterate through list and remove items
		for(; m_pllstLayerControl->getLength(); )
		{
			// get current layer info object
			pdwglinfTemp = m_pllstLayerControl->remove(0);

			// validate, destroy
			if(pdwglinfTemp)
			{
				//// destroy layer object
				//if(pdwglinfTemp->pdwgobjLayer)
				//	delete pdwglinfTemp->pdwgobjLayer;

				// delete info object
				delete pdwglinfTemp;
				pdwglinfTemp = NULL;
			}
		}

		// clear any wayward items...
		m_pllstLayerControl->clear();

		// finally, destroy list
		delete m_pllstLayerControl;
		m_pllstLayerControl;
	}
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
LRESULT CALLBACK CLayerControlDialog::WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	HWND hwndTemp = NULL;
	RECT rctTemp;
	HDC hdcTemp = NULL;
	BOOL bReturn = FALSE;
	static BOOL bStartupComplete = FALSE;
	int iReturn = 0,
		iCtrlID = 0;
	long lReturn = 0L;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			// set handle
			pclyrctldlgThis->m_hwndThis = hwnd;

			// load this dialog's preferences
			pclyrctldlgThis->loadPreferences();

			// set icon
			pclyrctldlgThis->loadIcon();

			// set font for controls
			pclyrctldlgThis->setControlFonts();

			// display layer information
			pclyrctldlgThis->displayLayerInformation();

			// Register escape key
			RegisterHotKey(hwnd, HOTKEY_ESCAPE, (UINT)0, VK_ESCAPE);
			break;			

		//case WM_CTLCOLORLISTBOX:
		//case WM_CTLCOLORSTATIC:
		//case WM_CTLCOLORDLG:
		//	// get DC
		//	hdcTemp = (HDC)wParam;

		//	// validate DC, continue
		//	if(hdcTemp)
		//	{
		//		// create black brush
		//		if(pclyrctldlgThis->m_hbrBackground == NULL)
		//			pclyrctldlgThis->m_hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

		//		if(wParam == WM_CTLCOLORSTATIC)
		//			SetBkMode(hdcTemp, TRANSPARENT);
		//		
		//		// Set background color for text
		//		SetBkColor(hdcTemp, RGB(0, 0, 0));
		//		
		//		// set text color
		//		SetTextColor(hdcTemp, RGB(255, 255, 255));
		//		
		//		// return brush
		//		return (LRESULT)pclyrctldlgThis->m_hbrBackground;
		//	}
		//	break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_CMDSELECTALL:
					// select all layers
					pclyrctldlgThis->toggleLayersVisibility();
					break;

				case IDC_CMDDESELECTALL:
					// de-select all layers
					pclyrctldlgThis->toggleLayersVisibility(FALSE);
					break;

				case IDC_CMDOK:
					// save changes to layers
					pclyrctldlgThis->saveLayerInformation();

					// return modal OK
					EndDialog(hwnd, IDOK);
					break;
				
				case IDC_CMDCANCEL:
					// return modal Cancel
					EndDialog(hwnd, IDCANCEL);
					break;

				default:
					break;
			}
			switch(HIWORD(wParam))
			{
				case LBN_DBLCLK:
					// There's only ONE listbox on this window, so there's no need to
					//	 check the identifier
					pclyrctldlgThis->changeSelectedLayerVisibility();
					break;

				default:
					break;
			}
			break;

		case WM_VKEYTOITEM:
			switch(LOWORD(wParam))
			{
				case VK_SPACE:	// WOW, this was more complicated than I would have imagined.

					// get layer listbox's handle
					hwndTemp = (HWND)lParam;

					// validate, continue
					if(hwndTemp)
					{
						// get currently selected item
						lReturn = HIWORD(wParam);

						// validate, continue
						if(lReturn != LB_ERR)
						{
							// get item's RECT struct
							lReturn = SendMessage(hwndTemp, LB_GETITEMRECT,
										(WPARAM)lReturn, (LPARAM)&rctTemp);
							
							if(lReturn != LB_ERR)
							{	
								// simulate a double-click
								PostMessage(hwndTemp, WM_LBUTTONDBLCLK, 
									(WPARAM)MK_LBUTTON, 
									MAKELPARAM(rctTemp.left + 1,
											   rctTemp.top + 1));
							}
						}
					}
					break;

				default:
					break;
			}
			return -1;

		case WM_DESTROY:
			// save preferences for this dialog ALWAYS
			pclyrctldlgThis->savePreferences();

			// clear handle because window is being closed manually.
			pclyrctldlgThis->m_hwndThis = NULL;

			// Unregister Escape key
			UnregisterHotKey(hwnd, HOTKEY_ESCAPE);
			break;

		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
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
int CLayerControlDialog::show()
{
    int iReturn = 0;	// default to pesimistic return val

    try
    {
		// attempt to display dialog box
		iReturn = DialogBox(m_hinstApplication, 
							MAKEINTRESOURCE(IDD_DLGLAYERCONTROL),
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
BOOL CLayerControlDialog::loadPreferences()
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
			REG_VAL_PREFS_LOCATION_LAYERCONTROL, &rctThis, 
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
BOOL CLayerControlDialog::savePreferences()
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
			REG_VAL_PREFS_LOCATION_LAYERCONTROL, &rctThis, 
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
BOOL CLayerControlDialog::loadIcon()
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
BOOL CLayerControlDialog::setControlFonts()
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
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTLAYERS);
		if(hwndTemp == NULL)
			return FALSE;
		
		// Get DC for layer list control
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

		// Attempt change button's fonts
		//	 Select All
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDSELECTALL);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	 Deselect All
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDDESELECTALL);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	 OK
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDOK);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	 Cancel
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDCANCEL);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls, 
				(LPARAM)MAKELPARAM(TRUE, 0));

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
 * Copies the contents of the source layers list to the local, temp layers
 * list. This method insures that the user's changes can be "canceled."
 *
 * @return TRUE if the source list's contents are copied successfully,
 * otherwise FALSE
 */
BOOL CLayerControlDialog::copyLayerInformation()
{
	BOOL bReturn = TRUE;

	try
	{
		DWGLAYERINFO *pdwglinfSource = NULL,
					 *pdwglinfTemp = NULL;

		// validate source layers list
		if(m_pllstSourceLayers == NULL)
		{
			// set last error
			m_strLastError = _T("The source layers list is invalid.");

			// return fail val
			return FALSE;
		}

		// validate local, temp list
		if(m_pllstLayerControl == NULL)
		{
			// set last errr
			m_strLastError = _T("The internal layer list object is invalid.");

			// return fail val
			return FALSE;
		}

		// Make sure there is something to copy
		if(m_pllstSourceLayers->getLength() == 0)
			return TRUE;

		// Create local copy of layers
		for(long lcv = 0L; lcv < m_pllstSourceLayers->getLength(); lcv++)
		{
			// get current layer info object
			pdwglinfSource = m_pllstSourceLayers->getEntry(lcv);

			// validate, continue
			if(pdwglinfSource == NULL)
				continue;
			
			// create new layer info object
			pdwglinfTemp = new DWGLAYERINFO();
			
			// validate, continue
			if(pdwglinfTemp == NULL)
			{
				// set fail val
				bReturn = FALSE;
				
				// attempt to get next layer anyway
				continue;
			}

			// Copy visible/enabled "property"
			pdwglinfTemp->bEnabled = pdwglinfSource->bEnabled;

			// Copy layer name
			pdwglinfTemp->setLayerName(pdwglinfSource->getLayerName());

			// add to temp layer list
			if(pdwglinfTemp)
				m_pllstLayerControl->add(pdwglinfTemp);
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While copying the source layer information, an unexpected error occurred.");

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
 * Displays the layer information from the layers list in this window's
 * UI list.
 *
 * @return TRUE if the layer information is displayed and no errors
 * occur, otherwise FALSE
 */
BOOL CLayerControlDialog::displayLayerInformation()
{
	BOOL bReturn = TRUE;

	try
	{
		DWGLAYERINFO *pdwglinfTemp = NULL;
		HWND hwndLayersList = NULL;

		// validate internal layer list
		if(m_pllstLayerControl == NULL)
		{
			// set last error
			m_strLastError = _T("");

			// return fail val
			return FALSE;
		}

		// make sure there's something to do
		if(m_pllstLayerControl->getLength() == 0)
		{
			// nothing to do, display no layers message

			// return success
			return TRUE;
		}

		// Validate *this* window's handle
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("");

			// return fail val
			return FALSE;
		}

		// Get handle to layers list
		hwndLayersList = GetDlgItem(m_hwndThis, IDC_LSTLAYERS);
		if(hwndLayersList == NULL)
		{
			// set last error
			m_strLastError = _T("");

			// return fail val
			return FALSE;
		}
		
		// Clear list
		SendMessage(hwndLayersList, LB_RESETCONTENT, (WPARAM)0, 0L);

		// Add list header
		SendMessage(hwndLayersList, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)STRING_LISTHEADER);

		// Add list separator
		SendMessage(hwndLayersList, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)STRING_LISTHEADERSEPARATOR);

		// Iterate through layer control list
		for(long lcv = 0L; lcv < m_pllstLayerControl->getLength(); lcv++)
		{
			// get current layer
			pdwglinfTemp = m_pllstLayerControl->getEntry(lcv);

			// validate, continue
			if(pdwglinfTemp)
			{
				TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;

				// format string
				_stprintf(tstrBuffer, STRING_FORMAT_LAYER,
					(pdwglinfTemp->bEnabled ? 
						STRING_LAYERINDICATOR_ON : STRING_LAYERINDICATOR_OFF),
					pdwglinfTemp->getLayerName());

				// add string
				SendMessage(hwndLayersList, LB_ADDSTRING,
					(WPARAM)0, (LPARAM)tstrBuffer);
			}
		}

		// set selection to first layer
		SendMessage(hwndLayersList, LB_SETCURSEL, (WPARAM)INDEX_FIRST_LAYER, 0L);
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While displaying the layer information, an unexpected error occurred.");

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
 * Toggles layers visibility.
 *
 * @param bVisible
 *
 * @return
 */
BOOL CLayerControlDialog::toggleLayersVisibility(BOOL bVisible)
{
	BOOL bReturn = TRUE;

	try
	{
		DWGLAYERINFO *pdwglinfTemp = NULL;

		// validate internal layer list
		if(m_pllstLayerControl == NULL)
		{
			// set last error
			m_strLastError = _T("");

			// return fail val
			return FALSE;
		}

		// make sure there's something to do
		if(m_pllstLayerControl->getLength() == 0)
		{
			// nothing to do, display no layers message

			// return success
			return TRUE;
		}
			
		// iterate through all layers and set their "visiblity" to
		//	 whatever is specified
		for(long lcv = 0L; lcv < m_pllstLayerControl->getLength(); lcv++)
		{
			// get current layer info object
			pdwglinfTemp = m_pllstLayerControl->getEntry(lcv);

			// validate, continue
			if(pdwglinfTemp)
			{
				// set visibility
				pdwglinfTemp->bEnabled = bVisible;
			}
		}

		// repopulate list with changes
		bReturn = displayLayerInformation();
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While changing layer visibility, an unexpected error occurred.");

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
 * Saves any layer visibility changes to the source list.
 *
 * @return TRUE if all layer information is saved to the source layer
 * list, otherwise FALSE.
 */
BOOL CLayerControlDialog::saveLayerInformation()
{
	BOOL bReturn = TRUE;

	try
	{
		DWGLAYERINFO *pdwglinfSource = NULL,
					 *pdwglinfTemp = NULL;

		// validate source layers list
		if(m_pllstSourceLayers == NULL)
		{
			// set last error
			m_strLastError = _T("The source layers list is invalid.");

			// return fail val
			return FALSE;
		}

		// validate local, temp list
		if(m_pllstLayerControl == NULL)
		{
			// set last errr
			m_strLastError = _T("The internal layer list object is invalid.");

			// return fail val
			return FALSE;
		}

		// Make sure there is something to copy
		if(m_pllstLayerControl->getLength() == 0)
			return TRUE;

		// Create local copy of layers
		for(long lcv = 0L; lcv < m_pllstLayerControl->getLength(); lcv++)
		{
			// get source and current layer info objects
			pdwglinfSource = m_pllstSourceLayers->getEntry(lcv);
			pdwglinfTemp = m_pllstLayerControl->getEntry(lcv);

			// validate, continue
			if(pdwglinfSource == NULL || pdwglinfTemp == NULL)
				continue;
			if(pdwglinfSource->getLayerName() == NULL
				||
			   pdwglinfTemp->getLayerName() == NULL)
				continue;
			
			// Copy visible/enabled "property"
			pdwglinfSource->bEnabled = pdwglinfTemp->bEnabled;
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While saving the layer information, an unexpected error occurred.");

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
 * Changes the currently selected layer's visibility.
 *
 * @return TRUE if the currently selected layer's visibility is changed and
 * no errors occur, otherwise FALSE
 */
BOOL CLayerControlDialog::changeSelectedLayerVisibility()
{
	BOOL bReturn = TRUE;

	try
	{
		HWND hwndLayersList = NULL;
		DWGLAYERINFO *pdwglinfTemp = NULL;
		long lIndex = LB_ERR;

		// validate internal layer list
		if(m_pllstLayerControl == NULL)
		{
			// set last error
			m_strLastError = _T("");

			// return fail val
			return FALSE;
		}

		// make sure there's something to do
		if(m_pllstLayerControl->getLength() == 0)
		{
			// nothing to do, display no layers message

			// return success
			return TRUE;
		}

		// Get handle to layers list
		hwndLayersList = GetDlgItem(m_hwndThis, IDC_LSTLAYERS);
		if(hwndLayersList == NULL)
		{
			// set last error
			m_strLastError = _T("Could not retrieve the handle to the layers listbox.");

			// return fail val
			return FALSE;
		}

		// Get selected item, validate
		lIndex = SendMessage(hwndLayersList, LB_GETCURSEL, (WPARAM)0, 0L);
		if(lIndex == LB_ERR)
			return TRUE;

		// offset for header and separator
		lIndex -= INDEX_FIRST_LAYER;
		if(lIndex < 0)
			return FALSE;

		// get current layer info object
		pdwglinfTemp = m_pllstLayerControl->getEntry(lIndex);

		// validate, continue
		if(pdwglinfTemp)
		{
			// set visibility
			pdwglinfTemp->bEnabled = !pdwglinfTemp->bEnabled;
		}

		// repopulate list with changes
		bReturn = displayLayerInformation();

		// set back to selected item
		//	 Restore offset
		lIndex += INDEX_FIRST_LAYER;
		//	 Re-select item
		SendMessage(hwndLayersList, LB_SETCURSEL, (WPARAM)lIndex, 0L);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While changing the currently selected layer's visibility, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}