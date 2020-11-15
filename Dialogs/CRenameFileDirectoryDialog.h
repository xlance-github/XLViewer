#ifndef _CRENAMEFILEDIRECTORYDIALOG_
#define _CRENAMEFILEDIRECTORYDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor  
// Purpose: CRenameFileDirectoryDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>

// Package file object definition
class CRenameFileDirectoryDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HINSTANCE m_hinstApplication;
	
	HWND m_hwndThis;
	
	HFONT m_hfontControls;

	HBRUSH m_hbrBackground;	

	tstring m_strNewObjectName,
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
	 * Stores the new name specified by the user in *this* object's new object
	 * name field.
	 */
	BOOL saveObjectName();
	
public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CRenameFileDirectoryDialog();

	/**
	 * Constructor which accepts the application HINSTANCE as an argument.
	 */
	CRenameFileDirectoryDialog(HINSTANCE hInstance, TCHAR *tstrFilename);
	
	/**
	 * Destructor, performs clean-up.
	 */
	~CRenameFileDirectoryDialog();
	
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
	TCHAR *getNewObjectName() {return (TCHAR *)m_strNewObjectName.data();}

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

#endif // End _CRENAMEFILEDIRECTORYDIALOG_
