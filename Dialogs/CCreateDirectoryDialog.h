#ifndef _CCREATEDIRECTORYDIALOG_
#define _CCREATEDIRECTORYDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CCreateDirectoryDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>

// Package file object definition
class CCreateDirectoryDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HINSTANCE m_hinstApplication;
	
	HWND m_hwndThis;
	
	HFONT m_hfontControls;

	HBRUSH m_hbrBackground;	

	tstring m_strDirectoryName,
				m_strLastError;
	
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
	 * Stores the directory name specified by the user in *this* object's
	 * directory name field.
	 */
	BOOL saveDirectoryName();
	
public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CCreateDirectoryDialog();

	/**
	 * Constructor which accepts the application HINSTANCE as an argument.
	 */
	CCreateDirectoryDialog(HINSTANCE hInstance);
	
	/**
	 * Destructor, performs clean-up.
	 */
	~CCreateDirectoryDialog();

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

	/**
	 * Returns the name of the new directory that should be created.
	 */
	TCHAR *getDirectoryName() {return (TCHAR *)m_strDirectoryName.data();}

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

#endif // End _CCREATEDIRECTORYDIALOG_
