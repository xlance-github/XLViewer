///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor    
// Purpose:   COptionsDialog object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include <algorithm>
#include <iterator>
#include "COptionsDialog.h"
#include "COptionsDialog.h"
#include "..\XLanceView.h"
#include "..\Common\Registry.h"
#include "..\Settings\CSettings.h"
#include "..\Resource\Resource.h"

// Module Level Vars
static COptionsDialog *pcopdlgThis = NULL;

// Externals
extern CSettings g_csetApplication;

/**
 * Default constructor, initializes all fields to their defaults.
 */
COptionsDialog::COptionsDialog()
{
	// initialize fields to their defaults
	m_hinstApplication = NULL;
	m_hwndThis = NULL;
	m_hfontControls = NULL;
	m_strLastError = EMPTY_STRING;

	// set module static so message loop can have access to *this*
	pcopdlgThis = this;
}

/**
 * Constructor which accepts the application HINSTANCE as an argument.
 *
 * @param hInstance
 *
 */
COptionsDialog::COptionsDialog(HINSTANCE hInstance)
{
	try
	{
		// initialize fields 
		m_strLastError = EMPTY_STRING;
		m_hfontControls = NULL;
		m_hinstApplication = hInstance;

		// set module static so message loop can have access to *this*
		pcopdlgThis = this;
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
COptionsDialog::~COptionsDialog()
{
	// destroy created GDI objects
	if(m_hfontControls)
		DeleteObject(m_hfontControls);

  for (size_t control_it = 0;
    control_it < CSettings::ControlsCount;
    ++control_it)
  {
    for (size_t color_it = 0;
      color_it < CSettings::ColorsCount;
      ++color_it)
    {
      if (brushes_[control_it][color_it])
        DeleteObject(brushes_[control_it][color_it]);
    }
  }

	// destroy window object
	if(m_hwndThis)
		DestroyWindow(m_hwndThis);
}


// initialize local copy of colors... used so changes are NOT
//	 applied to global settings unless user clicks "OK" on this
//	 dialog.
void COptionsDialog::InitColors()
{
  for (size_t color_it = 0; color_it < CSettings::ColorsCount; ++color_it)
    ct_->AddColor(tstring(CSettings::ColorNames[color_it]));

  for (size_t control_it = 0; control_it < CSettings::ControlsCount; ++control_it)
  {
    ct_->AddItem(tstring(CSettings::ControlsNames[control_it]));
    for (size_t color_it = 0; color_it < CSettings::ColorsCount; ++color_it)
    {
      COLORREF c = g_csetApplication.Color(control_it, color_it);
      ct_->Color(true, control_it, color_it, c);
    }
  }

  memset(brushes_, 0, sizeof(brushes_));

  memset(custom_colors_, 0, sizeof(custom_colors_));
}


HBRUSH COptionsDialog::GetBrush(int control, int color, bool recreate)
{
  if (recreate && brushes_[control][color])
  {
    DeleteObject(brushes_[control][color]);
    brushes_[control][color] = 0;
  }

  if (!brushes_[control][color])
  {
    COLORREF c;
    ct_->Color(false, control, color, c);
    brushes_[control][color] = static_cast<HBRUSH>(CreateSolidBrush(c));
  }
  return brushes_[control][color];
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
LRESULT CALLBACK COptionsDialog::WindowProc (HWND hwnd, UINT uMsg, 
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
      {
			  // set handle
			  pcopdlgThis->m_hwndThis = hwnd;
        pcopdlgThis->ct_.reset( new CColorTable( GetDlgItem( hwnd, IDC_COLOR_TABLE )
          , pcopdlgThis->custom_colors_[0], pcopdlgThis->custom_colors_[MAX_CUSTOM_COLORS-1] ) );

			  // load this dialog's preferences
			  pcopdlgThis->loadPreferences();
			
        CTimeZone::EnumAll(pcopdlgThis->m_tz_List);

        // load application options
			  pcopdlgThis->loadOptions();

			  // set icon
			  pcopdlgThis->loadIcon();

			  // Register escape key
			  RegisterHotKey(hwnd, HOTKEY_ESCAPE, (UINT)0, VK_ESCAPE);
			
        UINT tz_combo_ids[] = {IDC_COMBOTZ1CLOCK, IDC_COMBOTZ2, IDC_COMBOTZ3};
        for ( size_t combo_it = 0; combo_it < _countof(tz_combo_ids); ++combo_it )
        {
          CTimeZoneComboBox tz_combo( GetDlgItem(pcopdlgThis->m_hwndThis, tz_combo_ids[combo_it]) );
          tz_combo.Fill();

          pcopdlgThis->m_tz_Combos.push_back( tz_combo );
        }
        pcopdlgThis->selectComboFromOptions();

        for (size_t cc_it = 0; cc_it < MAX_CUSTOM_COLORS; ++cc_it)
          pcopdlgThis->custom_colors_[cc_it] = g_csetApplication.getCustomColors()[cc_it];

        pcopdlgThis->InitColors();

			  // set font for controls
			  //pcopdlgThis->setControlFonts();
			  break;			
      }

		case WM_DESTROY:
			// save preferences for this dialog ALWAYS
			pcopdlgThis->savePreferences();

			// clear handle because window is being closed manually.
			pcopdlgThis->m_hwndThis = NULL;

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
					// save options
					pcopdlgThis->saveOptions();

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

    case WM_NOTIFY:
      {
        if (LOWORD(wParam) == IDC_COLOR_TABLE)
        {
          pcopdlgThis->ct_->ProcessNotify(pcopdlgThis->m_hwndThis, lParam);
        }
        break;
      }

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
int COptionsDialog::show()
{
    int iReturn = 0;	// default to pesimistic return val

    try
    {
		// attempt to display dialog box
		iReturn = DialogBox(m_hinstApplication, 
							MAKEINTRESOURCE(IDD_DLGOPTIONS),
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
BOOL COptionsDialog::loadPreferences()
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
			REG_VAL_PREFS_LOCATION_OPTIONS, &rctThis, 
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
BOOL COptionsDialog::savePreferences()
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
			REG_VAL_PREFS_LOCATION_OPTIONS, &rctThis, 
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
BOOL COptionsDialog::loadIcon()
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
BOOL COptionsDialog::setControlFonts()
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
      0, _T("Lucida Console"));

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
 * Loads the options from the application settings object.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL COptionsDialog::loadOptions()
{
  BOOL bReturn = TRUE;	// default to optimistic return val

  try
  {
    //	 WM_CTLCOLORSTATIC message takes care of displaying
    //	 (loading) the colors for the File Managers and
    //	 integrated command prompt.

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


void COptionsDialog::selectComboFromOptions()
{
  // load select time zones
  UINT hTzComboId[] = {IDC_COMBOTZ1CLOCK, IDC_COMBOTZ2, IDC_COMBOTZ3};
  for (size_t tz_it = 0; tz_it < _countof(hTzComboId); ++tz_it)
  {
    if (HWND hTzCombo = GetDlgItem(pcopdlgThis->m_hwndThis, hTzComboId[tz_it]))
    {
      CTimeZoneFind tzf( g_csetApplication.TZHourBias(tz_it), g_csetApplication.TZName(tz_it) );
      CTimeZone::TZList::const_iterator tz_ci;
      for (unsigned int step = 0; step < 3; ++step )
      {
        tz_ci = std::find_if( m_tz_List.begin(), m_tz_List.end(), tzf);
        if (tz_ci != m_tz_List.end())
          break;
        // trying to load another tz
        switch (step)
        {
        case 0: tzf.UseName(false); break;
        case 1: tzf.SetHourBias(0); break;
        }
      }
      if (tz_ci != m_tz_List.end())
      {
        CTimeZone::TZList::difference_type item_index = std::distance(static_cast< CTimeZone::TZList::const_iterator >( m_tz_List.begin() ), tz_ci);
        ComboBox_SetCurSel(hTzCombo, item_index);
      }
    }
  }
}


/**
 * Saves the options from this dialog to the application settings object.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL COptionsDialog::saveOptions()
{
  BOOL bReturn = TRUE;	// default to optimistic return val

  try
  {
    // Save colors

    for (size_t control_it = 0; control_it < CSettings::ControlsCount; ++control_it)
    {
      for (size_t color_it = 0; color_it < CSettings::ColorsCount; ++color_it)
      {
        COLORREF c;
        ct_->Color(false, control_it, color_it, c);
        g_csetApplication.Color(control_it, color_it, c);
      }
    }

    UINT hTzComboId[] = {IDC_COMBOTZ1CLOCK, IDC_COMBOTZ2, IDC_COMBOTZ3};
    for (size_t tz_it = 0; tz_it < _countof(hTzComboId); ++tz_it)
    {
      if (HWND hTzCombo = GetDlgItem(m_hwndThis, hTzComboId[tz_it]))
      {
        const int item_index = ComboBox_GetCurSel(hTzCombo);
        if (item_index != -1)
        {
          g_csetApplication.TZHourBias(tz_it, m_tz_List[item_index].GetHourBias());

          tstring str;
          WToTChar(m_tz_List[item_index].GetData().StandardName, str);
          g_csetApplication.TZName(tz_it, str.c_str());
        }
      }
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
