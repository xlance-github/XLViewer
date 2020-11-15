#ifndef _CSETTINGSDIALOG_
#define _CSETTINGSDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor 
// Purpose: CSettingsDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include "..\StdAfx.h"

// Package file object definition
class CSettingsDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HINSTANCE m_hinstApplication;

	HWND m_hwndThis,
		 m_hwndParent;

	tstring m_strLastError;

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
	 * Loads the application icon and assigns it to this object's dialog.
	 */
	BOOL loadIcon();

	/**
	 * Loads the settings from the application settings object.
	 */
	BOOL loadSettings();

	/**
	 * Saves the settings from this dialog to the application settings object.
	 */
	BOOL saveSettings();

	/** 
	 * Loads all information about the current graphics (display) device.
	 */
	BOOL loadGraphicsDeviceInformation();

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CSettingsDialog();

	/**
	 * Constructor which accepts an HINSTANCE as an argument.
	 */
	CSettingsDialog(HINSTANCE hInstance, HWND hwndParent);

	/**
	 * Destructor, performs clean-up.
	 */
	~CSettingsDialog();

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
	TCHAR *getLastError() {return (TCHAR *)m_strLastError.c_str();}

	///////////////////////////////////////////////////////////////////////////
	// Setter Methods
	///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////////
	// UI Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Creates and displays this object's dialog.
	 */
	BOOL show();
};

#endif // End _CSETTINGSDIALOG_
