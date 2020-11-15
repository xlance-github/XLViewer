#ifndef _CFILEATTRIBUTESDIALOG_
#define _CFILEATTRIBUTESDIALOG_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CFileAttributesDialog object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <string>
#include "..\LinkedList.h"
#include "..\FileInformation.h"

// Package file object definition
class CFileAttributesDialog
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	HINSTANCE m_hinstApplication;

	HWND m_hwndThis,
		 m_hwndParent;

	HFONT m_hfontLabels;

	LinkedListEx<FILE_INFORMATION> *m_pllstFileSystemObjects;
		
	ACERIGHTS m_acerCumulativeRights;

	tstring m_strBasePath,
				m_strLastError;

	DWORD m_dwCumulativeAttributes,
		  m_dwCumulativeFileSize;

	int m_iFileCount,
		m_iDirectoryCount;

	BOOL m_bMixedFileSystemObjects,
		 m_bHasEveryoneGroup;
	
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
	 * Sets the "information" labels and the frame's font to the internal
	 * font.
	 */
	BOOL setDescriptionLabelsFont();

	/**
	 * Manages the loading of all attributes for the current file system
	 * objects.
	 */
	BOOL loadAttributes();

	/**
	 * Loads the information and attributes for the current file (the file
	 * this object represents, m_strFilename).
	 */
	BOOL loadFileAttributes(FILE_INFORMATION *pfiFile);

	/**
	 * Loads the information and attributes for the current file (the file
	 * this object represents, m_strFilename).
	 */
	BOOL loadDirectoryAttributes(FILE_INFORMATION *pfiDirectory);

	/**
	 * Manages the process of applying the attributes selected by the user
	 * to all file system objects in the file system object list.
	 */
	BOOL saveAttributes();

	/**
	 * Saves the file attributes for the current file (the file
	 * this object represents, m_strFilename). 
	 */
	BOOL saveFileAttributes(FILE_INFORMATION *pfiFile);

	/**
	 * Saves the file attributes for the current file or directory. This method is
	 * ONLY available on machines running at least Windows XP.
	 */
	BOOL saveDirectoryAttributes(FILE_INFORMATION *pfiDirectory);

	/**
	 * Returns whether or not the OS is at least Windows(r) XP.
	 */
	BOOL isAtLeastWindowsXP();

	/**
	 * Walks the folder structure for the directory specified.
	 */
	BOOL getDirectoryTreeInformation(TCHAR *tstrDirectory);

	/**
	 * Assigns the specified attributes to the entire directory tree for the
	 * given folder.
	 */
	BOOL setDirectoryTreeAttributes(TCHAR *tstrDirectory, DWORD dwAttributes);

	/**
	 * Increments the file count and re-displays the information on *this*
	 * dialog.
	 */
	int incrementFileCount();

	/**
	 * Increments the directory count and re-displays the information on *this*
	 * dialog.
	 */
	int incrementDirectoryCount();

	/**
	 * Waits until all painting (more or less) has been completed before
	 * returning.
	 */
	BOOL doEvents();

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CFileAttributesDialog();

	///**
	// * Constructor which accepts the instance, the parent and the fullpath to
	// * the file whose attributes should be displayed.
	// */
	//CFileAttributesDialog(HINSTANCE hInstance, HWND hwndParent, 
	//	TCHAR *tstrFilename);

	/**
	 * Constructor which accepts the instance, the parent and a file object list
	 * which contains file system objects whose attributes should be displayed.
	 */
	CFileAttributesDialog(HINSTANCE hInstance, HWND hwndParent, 
		TCHAR *tstrBasePath, LinkedListEx<FILE_INFORMATION> *pllstList);
	
	/**
	 * Destructor, performs clean-up.
	 */
	~CFileAttributesDialog();

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

#endif // End _CFILEATTRIBUTESDIALOG_
