#ifndef _COPTIONSDIALOG_
#define _COPTIONSDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor   
// Purpose:   COptionsDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include "..\StdAfx.h"
#include "..\TimeZone\TimeZoneComboBox.h"
#include "..\ColorTable\ColorTable.h"
#include "..\Settings\CSettings.h"


// Package file object definition
class COptionsDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HINSTANCE m_hinstApplication;
	
	HWND m_hwndThis;
	
	HFONT m_hfontControls;

  //COLORREF colors_[CSettings::ControlsCount][CSettings::ColorsCount];
  HBRUSH brushes_[CSettings::ControlsCount][CSettings::ColorsCount];
  COLORREF custom_colors_[MAX_CUSTOM_COLORS];

  void InitColors();
  HBRUSH GetBrush(int control, int color, bool recreate=false);

  std::deque<CTimeZoneComboBox> m_tz_Combos;
  CTimeZone::TZList m_tz_List;

	tstring m_strLastError;

  std::auto_ptr< CColorTable > ct_;
				
	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Loads, from the registry, the user's preferences for this dialog.
	 */
	BOOL loadPreferences();

	/**
	 * Saves, to the registry, the user's preferences for this dialog.
	 */
	BOOL savePreferences();

	/**
	 * Loads the icon for this window at startup.
	 */
	BOOL loadIcon();	

	/**
	 * Sets the FONT object used by the logging control (txtLog).
	 */
	BOOL setControlFonts();	

	/**
	 * Loads the options from the application settings object.
	 */
	BOOL loadOptions();

  void selectComboFromOptions();

	/**
	 * Saves the options from this dialog to the application settings object.
	 */
	BOOL saveOptions();

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	COptionsDialog();

	/**
	 * Constructor which accepts the application HINSTANCE as an argument.
	 */
	COptionsDialog(HINSTANCE hInstance);
	
	/**
	 * Destructor, performs clean-up.
	 */
	~COptionsDialog();

	///////////////////////////////////////////////////////////////////////////
	// Message Loop
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Handles processing of all messages sent to *this* window.
	 */
	static LRESULT CALLBACK WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, 
		LPARAM lParam);

	///////////////////////////////////////////////////////////////////////////
	// Getter Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Returns the last error encountered, if any.
	 */
	TCHAR *getLastError() {return (TCHAR *)m_strLastError.data();}

	///////////////////////////////////////////////////////////////////////////
	// Setter Methods
	///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	// UI Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Unhides and shows this dialog.
	 */
	int show();
};

#endif // End _COPTIONSDIALOG_
