///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CSettingsDialog object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "..\XLanceView.h"
#include "CSettingsDialog.h"
#include "..\Resource\Resource.h"
#include "..\Common\Registry.h"
#include "..\Settings\CSettings.h"
#include "..\Utility\CGraphicsDeviceInformation.h"

///////////////////////////////////////////////////////////////////////////////
// Module level vars
///////////////////////////////////////////////////////////////////////////////

static CSettingsDialog *pcsetdlgThis = NULL;
extern CSettings g_csetApplication;
extern CGraphicsDeviceInformation g_cginfPrimaryDevice;

/**
 * Default constructor, initializes all fields to their defaults.
 */
CSettingsDialog::CSettingsDialog()
{
	// initialize fields to their defaults
	m_hinstApplication = NULL;
	m_hwndParent = NULL;
	m_hwndThis = NULL;
	m_strLastError = _T("");

	// set module static so message loop can have access to *this*
	pcsetdlgThis = this;
}

/**
 * Constructor which accepts an HINSTANCE as an argument.
 *
 * @param hInstance
 *
 */
CSettingsDialog::CSettingsDialog(HINSTANCE hInstance, HWND hwndParent)
{
	try
	{
		// initialize fields 
		m_strLastError = _T("");
		m_hinstApplication = hInstance;
		m_hwndParent = hwndParent;
		m_hwndThis = NULL;

		// set module static so message loop can have access to *this*
		pcsetdlgThis = this;
	}
	catch(...)
	{
		// garbage collect, clear references
		m_hinstApplication = NULL;
		m_hwndParent = NULL;

		// set last error
		m_strLastError = _T("While intializing the settings dialog object, an unspecified exception occurred.");
	}
}
                                 
/**
 * Destructor, performs clean-up on fields.
 */
CSettingsDialog::~CSettingsDialog()
{
	// perform final garbage collection
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
LRESULT CALLBACK CSettingsDialog::WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	HWND hwndTemp = NULL;
	BOOL bEnabled = FALSE;

	switch(uMsg)
	{
		case WM_INITDIALOG:
      {
			  // store handle
			  pcsetdlgThis->m_hwndThis = hwnd;

			  // load this dialog's preferences
			  pcsetdlgThis->loadPreferences();
			
			  // load display information
			  pcsetdlgThis->loadGraphicsDeviceInformation();

			  // load application settings
			  pcsetdlgThis->loadSettings();

			  // set icon
			  pcsetdlgThis->loadIcon();

			  // User should ALWAYS be able to change resolution regardless of whether
			  //	 they choose launch full screen at startup. - Chad R. Hearn 03-28-2011

			  // get checked state for run full screen option
			  //bEnabled = IsDlgButtonChecked(hwnd, IDC_CHKRUNFULLSCREEN);
			
			  // get handle to graphics device mode combobox
			  //hwndTemp = GetDlgItem(hwnd, IDC_CBOGRAPHICSDEVICEMODES);

			  // enable or disable graphics device mode selection
			  //EnableWindow(hwndTemp, bEnabled);

			  RegisterHotKey(hwnd, HOTKEY_ESCAPE, (UINT)0, VK_ESCAPE);
        
        break;
      }
		case WM_DESTROY:
			// save preferences for this dialog ALWAYS
			pcsetdlgThis->savePreferences();

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
          {
					  // save application settings
					  pcsetdlgThis->saveSettings();                      

					  // return modal OK
					  EndDialog(hwnd, IDOK);
					  break;
          }
				case IDC_CMDCANCEL:
					// return modal Cancel
					EndDialog(hwnd, IDCANCEL);
					break;

				case IDC_CHKRUNFULLSCREEN:
					// User should ALWAYS be able to change resolution regardless of whether
					//	 they choose launch full screen at startup. - Chad R. Hearn 03-28-2011

					//// get checked state for run full screen option
					//bEnabled = IsDlgButtonChecked(hwnd, IDC_CHKRUNFULLSCREEN);
					//
					//// get handle to graphics device mode combobox
					//hwndTemp = GetDlgItem(hwnd, IDC_CBOGRAPHICSDEVICEMODES);

					//// enable or disable graphics device mode selection
					//EnableWindow(hwndTemp, bEnabled);

					break;

				case ID_ACCLEXITFULLSCREEN:
					// Return modal Cancel
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
 * Creates and displays this object's dialog. This object represents a modal
 * dialog which contains the settings for the application; therefore, calling
 * this method halts execution until the user chooses OK or Cancel.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CSettingsDialog::show()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
	{
		// attempt to display dialog box
		DialogBox(m_hinstApplication, MAKEINTRESOURCE(IDD_DLGSETTINGS),
					m_hwndParent, (DLGPROC)WindowProc); 
    }
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to display the settings dialog.");
        
        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Loads, from the registry, the user's preferences for this dialog.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CSettingsDialog::loadPreferences()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		RECT rctThis,
			 rctScreen;
		int iWidth, iHeight;

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

		// re-initialize structure
		memset(&rctThis, 0, sizeof(rctThis));

		// Attempt to load this dialog's stored location
		GetRegistryBinary(CurrentUser, REG_BASE, 
			REG_SECTION_PREFERENCES,
			REG_VAL_PREFS_LOCATION_SETTINGS, &rctThis, 
			sizeof(rctThis), FALSE);

		// Check and see if a stored value was present... if the
		//	 position of the dialog was stored previously then there
		//	 be something other than 0's for the right and bottom
		if(rctThis.bottom != 0 && rctThis.right != 0)
			SetWindowPos(m_hwndThis, m_hwndParent, rctThis.left,
				rctThis.top, 0, 0, SWP_NOSIZE);
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Saves, to the registry, the user's preferences for this dialog.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CSettingsDialog::savePreferences()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		RECT rctThis;

		// Validate this object's handle... if it isn't good, then
		//	 there's nothing we can do.
		if(m_hwndThis == NULL)
			return FALSE;
		
		// Get this object's dialog's position/size
		GetWindowRect(m_hwndThis, &rctThis);

		// Attempt to save this dialog's location
		SaveRegistryBinary(CurrentUser, REG_BASE, 
			REG_SECTION_PREFERENCES,
			REG_VAL_PREFS_LOCATION_SETTINGS, &rctThis, 
			sizeof(rctThis), TRUE);
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("");

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
 */
BOOL CSettingsDialog::loadIcon()
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
        m_strLastError = _T("");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Loads the settings from the application settings object.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CSettingsDialog::loadSettings()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HWND hwndTemp = NULL;
		int iIndex = -1;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}

		// load settings from settings object

		// Always launch full screen
		CheckDlgButton(m_hwndThis, IDC_CHKRUNFULLSCREEN, 
			(g_csetApplication.alwaysLaunchFullScreen() ? BST_CHECKED : BST_UNCHECKED));

		// Graphics Device
		SetDlgItemText(m_hwndThis, IDC_LBLDEVICENAME, 
			g_csetApplication.graphicsDevice());

		// Graphics Mode
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CBOGRAPHICSDEVICEMODES);
		if(hwndTemp)
		{
			// get index of string
			iIndex = SendMessage(hwndTemp, CB_FINDSTRINGEXACT, (WPARAM)-1,
						(LPARAM)g_csetApplication.graphicsMode());

			// attempt to select string no matter what
			SendMessage(hwndTemp, CB_SETCURSEL, (WPARAM)iIndex, (LPARAM)0);
		}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Saves the settings from this dialog to the application settings object.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CSettingsDialog::saveSettings()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HWND hwndTemp = NULL;
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}

		// Save settings

		// Always launch full screen
		g_csetApplication.alwaysLaunchFullScreen(
			IsDlgButtonChecked(m_hwndThis, IDC_CHKRUNFULLSCREEN));

		// Graphics Resolution
		GetDlgItemText(m_hwndThis, IDC_CBOGRAPHICSDEVICEMODES, tstrBuffer, 
			sizeof(tstrBuffer));
		if(lstrlen(tstrBuffer))
		{
			// store selected resolution
			g_csetApplication.graphicsMode(tstrBuffer);
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Loads all information about the current graphics (display) device.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CSettingsDialog::loadGraphicsDeviceInformation()
{
    BOOL bReturn = TRUE;	// default to optimistic return val
	TCHAR *pcBuffer = NULL;

    try
    {
		HWND hwndModes = NULL;
		TCHAR *pc = NULL;
		long lLength = 0L;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
			return FALSE;

		// validate device modes
		lLength = lstrlen(g_cginfPrimaryDevice.graphicsDeviceModes());
		if(lLength == 0)
		{
			// set last error
			m_strLastError = _T("There are no graphics modes for the primary graphics device.");

			// return fail val
			return FALSE;
		}

		// get handle to modes combobox, validate
		hwndModes = GetDlgItem(m_hwndThis, IDC_CBOGRAPHICSDEVICEMODES);
		if(hwndModes == NULL)
		{
			// set last error
			m_strLastError = _T("Could not access the graphics device modes list.");

			// return fail val
			return FALSE;
		}

		// allocate storage
		pcBuffer = new TCHAR[lLength + 1];
		if(pcBuffer == NULL)
		{
			// set last error
			m_strLastError = _T("Could not allocate space for the local copy of the graphics modes.");

			// return fail val
			return FALSE;
		}

		// store locally for parse
		lstrcpy(pcBuffer, g_cginfPrimaryDevice.graphicsDeviceModes());

		// iterate through parts and add
		for(pc = _tcstok(pcBuffer, _T(";"));
			pc;
			pc = _tcstok(NULL, _T(";")))
		{
			// add mode to list
			SendMessage(hwndModes, CB_ADDSTRING, (WPARAM)0, (LPARAM)pc);
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("");

        // set fail value
        bReturn = FALSE;
    }

	// reset last error, if appropriate
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// garbage collect
	if(pcBuffer)
		delete[] pcBuffer;

    // return success / fail val
    return bReturn;
}
