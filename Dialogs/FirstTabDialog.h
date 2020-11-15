#ifndef _FIRSTTABDIALOG_
#define _FIRSTTABDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor        
// Purpose:   CFirstTabDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include <windows.h>
#include <string>

// Package file object definition
class CFirstTabDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HWND m_hParent;
	
	HWND m_hwndThis;

	//HFONT m_hfontControls;

	//HBRUSH m_hbrBackground;

	tstring m_strLastError;
	
	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Loads, from the registry, the user's preferences for this dialog.
	 */
	//BOOL loadPreferences();

	/**
	 * Saves, to the registry, the user's preferences for this dialog.
	 */
	//BOOL savePreferences();

	/**
	 * Loads the icon for this window at startup.
	 */
	BOOL loadIcon();	

	/**
	 * Sets the FONT object used by the logging control (txtLog).
	 */
	//BOOL setControlFonts();	

	/**
	 * Loads the help text from the resource file.
	 */
	//BOOL loadAboutText();
	
public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CFirstTabDialog();

	/**
	 * Constructor which accepts the application HINSTANCE as an argument.
	 */
	CFirstTabDialog(HWND hParent);
	
	/**
	 * Destructor, performs clean-up.
	 */
	~CFirstTabDialog();

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
	int CreateDialogWindow();
	void HideDialog();
	void ShowDialog();

	HWND GetWindowHandle();

	BOOL refreshLayout(BOOL bAsOnStart);

	void InitializeDialog();
	void OnBnClickedAdd();
};

#endif // End _CABOUTDIALOG_
