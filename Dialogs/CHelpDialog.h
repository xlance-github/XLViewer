#ifndef _CHELPDIALOG_
#define _CHELPDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor     
// Purpose:   CHelpDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>

// Package file object definition
class CHelpDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HINSTANCE m_hinstApplication;
	
	HWND m_hwndThis;

	HFONT m_hfontControls;

	HBRUSH m_hbrBackground;

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
	 * Loads the icon for this window at startup.
	 */
	BOOL loadIcon();	

	/**
	 * Sets the FONT object used by the logging control (txtLog).
	 */
	BOOL setControlFonts();	

	/**
	 * Loads the help text from the resource file.
	 */
	BOOL loadHelpText();

	/**
	 * Display about dialog.
	 */
	BOOL displayAbout();
	
public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CHelpDialog();

	/**
	 * Constructor which accepts the application HINSTANCE as an argument.
	 */
	CHelpDialog(HINSTANCE hInstance);
	
	/**
	 * Destructor, performs clean-up.
	 */
	~CHelpDialog();

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

#endif // End _CHELPDIALOG_
