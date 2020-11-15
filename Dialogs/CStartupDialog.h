#ifndef _CSTARTUPDIALOG_
#define _CSTARTUPDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:   ©2011 M.Sc. E. Victor  
// Purpose: CStartupDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include <winioctl.h>


// forward declares
class CSettingsDialog;
class CAttachedDrives;

// CStartupDialog object definition
class CStartupDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HINSTANCE m_hinstApplication;
	
	HWND m_hwndThis;

	HFONT m_hfontLoggingFont;

	HBRUSH m_hbrBackground;

	CSettingsDialog *m_dlgsetSettings;

	CAttachedDrives *m_cadrvDrives;

	tstring m_strLastError;

	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Displays the settings dialog
	 */
	BOOL showSettings();

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
	BOOL setLoggingFont();

	/**
	 * Validates all application settings.
	 */
	BOOL validateSettings();

	/**
	 * Manages the startup checks.
	 */
	BOOL runStartupChecks();

	/**
	 * Verifies the application's exe name hasn't been tampered with.
	 */
	BOOL applicationNameCheck();

	/**
	 * Verifies that no modifications have been made to the actual
	 * exe.
	 */
	BOOL applicationSecurityCheck();
	
	/**
	 * Manages the identification and retrieval of information about all
	 * currently attached drives.
	 */
	BOOL getAvailableDrives();

	/**
	 * Writes entries to the logging control (txtLog).
	 */
	BOOL writeLogEntry(TCHAR *strEntry);

	/* check whether the .NET framework is installed or not
	*	returns 1 - if installed
	*			0 - if not installed
	*/
	BOOL ISDotNetFrameworkInstalled();
	BOOL ISDependentObjectsPresent();
	BOOL ISFileExists(tstring CurrentDirPath, tstring FileName);

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CStartupDialog();

	/**
	 * Constructor which accepts the application HINSTANCE as an argument.
	 */
	CStartupDialog(HINSTANCE hInstance);

	/**
	 * Destructor, performs clean-up.
	 */
	~CStartupDialog();

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

#endif // End _CSTARTUPDIALOG_
