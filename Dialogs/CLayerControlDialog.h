#ifndef _CLAYERCONTROLDIALOG_
#define _CLAYERCONTROLDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor 
// Purpose:   CLayerControlDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "..\LinkedList.h"
#include "..\DWGLayerInfo.h"

// Package file object definition
class CLayerControlDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HINSTANCE m_hinstApplication;
	
	HWND m_hwndThis;

	HFONT m_hfontControls;

	HBRUSH m_hbrBackground;
	
	LinkedListEx<DWGLAYERINFO> *m_pllstSourceLayers,
							   *m_pllstLayerControl;

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
	 * Copies the contents of the source layers list to the local, temp layers
	 * list. This method insures that the user's changes can be "canceled."
	 */
	BOOL copyLayerInformation();

	/**
	 * Displays the layer information from the layers list in this window's
	 * UI list.
	 */
	BOOL displayLayerInformation();

	/**
	 * Toggles layer visibility.
	 */
	BOOL toggleLayersVisibility(BOOL bVisible = TRUE);

	/**
	 * Changes the currently selected layer's visibility.
	 */
	BOOL changeSelectedLayerVisibility();

	/**
	 * Saves any layer visibility changes to the source list.
	 */
	BOOL saveLayerInformation();

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CLayerControlDialog();

	/**
	 * Constructor which accepts the application HINSTANCE as an argument.
	 */
	CLayerControlDialog(HINSTANCE hInstance, 
		LinkedListEx<DWGLAYERINFO> *pllstLayers);
	
	/**
	 * Destructor, performs clean-up.
	 */
	~CLayerControlDialog();

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

#endif // End _CLAYERCONTROLDIALOG_
