///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CFileAttributesDialog object implementation
//
//
//
// Date:
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "..\XLanceView.h"
#include "..\Common\Registry.h"
#include "..\Common\FileIO.h"
#include "..\Resource\Resource.h"
#include "CFileAttributesDialog.h"
///////////////////////////////////////////////////////////////////////////////
// Module level vars
///////////////////////////////////////////////////////////////////////////////

static CFileAttributesDialog *pcfadlgThis = NULL;

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////

#define FONTSIZE_DESCRIPTIONLABELS				9
#define STRING_FILESIZE_DIRECTORY				_T("N/A")
#define STRING_OBJECTTYPE_DIRECTORY				_T("Directory")
#define STRING_OBJECTTYPE_MIXED					_T("Files and Directories")
#define STRING_OBJECTTYPE_MULTIPLEDIRECTORIES	_T("Directories")
#define STRING_OBJECTTYPE_MULTIPLEFILES			_T("Files")
#define STRING_FILENAME_MIXED					_T("(Multiple Items)")
#define STRING_GENERAL_MIXED					_T("Various")
#define FORMAT_DIRECTORY_INFORMATION			_T("(%ld files, %ld directories)")
#define FORMAT_DIRECTORY_INFORMATION_FILE		_T("(1 file, %ld directories)")
#define FORMAT_DIRECTORY_INFORMATION_DIR		_T("(%ld files, 1 directory)")

#define THOUSANDS_SEPARATOR						_T(".")
#define DECIMAL_SEPARATOR						_T(".")

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor, initializes all fields to their defaults.
 */
CFileAttributesDialog::CFileAttributesDialog()
{
	// initialize fields to their defaults
	m_hinstApplication = NULL;
	m_hwndParent = NULL;
	m_hwndThis = NULL;
	m_hfontLabels = NULL;
	m_pllstFileSystemObjects = NULL;
	m_strLastError = EMPTY_STRING;
	m_iFileCount = 0;
	m_iDirectoryCount = 0;
	m_dwCumulativeAttributes = (DWORD)0;
	m_dwCumulativeFileSize = (DWORD)0;
	m_bMixedFileSystemObjects = FALSE;
	m_bHasEveryoneGroup = FALSE;

	// set module static so message loop can have access to *this*
	pcfadlgThis = this;
}

/**
 * Constructor which accepts the instance, the parent and a file object list
 * which contains file system objects whose attributes should be displayed.
 *
 * @param hInstance the application instance
 *
 * @param hwndParent handle to the parent window that created this object
 *
 * @param pllstList linked list containing the file system objects whose
 * attributes / permissions should be displayed.
 *
 */
CFileAttributesDialog::CFileAttributesDialog(HINSTANCE hInstance, 
	HWND hwndParent, TCHAR *tstrBasePath, 
	LinkedListEx<FILE_INFORMATION> *pllstList)
{
	try
	{
		// initialize fields 
		m_strLastError = EMPTY_STRING;
		m_hinstApplication = hInstance;
		m_hwndParent = hwndParent;
		m_hwndThis = NULL;
		m_hfontLabels = NULL;
		m_iFileCount = 0;
		m_iDirectoryCount = 0;
		m_dwCumulativeAttributes = (DWORD)0;
		m_dwCumulativeFileSize = (DWORD)0;
		m_bMixedFileSystemObjects = FALSE;
		m_bHasEveryoneGroup = FALSE;

		// initialize list with supplied parameter
		m_pllstFileSystemObjects = pllstList;

		// initialize base path with supplied parameter
		m_strBasePath = tstrBasePath;

		// make sure path is in the expected format
		if(m_strBasePath[m_strBasePath.length() - 1] != _T('\\'))
			m_strBasePath += _T("\\");

		// set module static so message loop can have access to *this*
		pcfadlgThis = this;
	}
	catch(...)
	{
		// garbage collect, clear references
		m_hinstApplication = NULL;
		m_hwndParent = NULL;

		// set last error
		m_strLastError = _T("While intializing the settings dialog object, an unspecified exception occurred.");
	}
}
                                 
/**
 * Destructor, performs clean-up on fields.
 */
CFileAttributesDialog::~CFileAttributesDialog()
{
	// perform final garbage collection

	// DO NOT destroy file system object list... its storage is owned
	//	 (or SHOULD BE owned) by the calling object

	// destroy GDI objects
	if(m_hfontLabels)
	{
		DeleteObject(m_hfontLabels);
		m_hfontLabels = NULL;
	}
}

/**
 * Handles the processing of all messages for *this* object's GUI
 * (dialog).
 *
 * @param hwnd Handle to the window whose message pump this represents. In
 * this case, *this* dialog.
 *
 * @param uMsg Message to be processed
 *
 * @param wParam WPARAM for the current message
 *
 * @param lParam LPARAM for the current message
 *
 * @return 0 if the message is processed by this message pump, 
 * otherwise DefWindowProc() is called and its return value used.
 */
LRESULT CALLBACK CFileAttributesDialog::WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	HWND hwndTemp = NULL;
	int iCtrlID = -1;
	static BOOL bAttributesLoaded = FALSE;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			// set handle
			pcfadlgThis->m_hwndThis = hwnd;

			// set description labels font
			pcfadlgThis->setDescriptionLabelsFont();

			// load this dialog's preferences
			pcfadlgThis->loadPreferences();
			//
			//// load the file's information
			//pcfadlgThis->loadAttributes();

			// set icon
			pcfadlgThis->loadIcon();

			// Register escape key
			RegisterHotKey(hwnd, HOTKEY_ESCAPE, (UINT)0, VK_ESCAPE);
			break;

		case WM_DESTROY:
			// save preferences for this dialog ALWAYS
			pcfadlgThis->savePreferences();

			// Unregister Escape key
			UnregisterHotKey(hwnd, HOTKEY_ESCAPE);
			break;

		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
			break;
	
		case WM_APP:
			switch(wParam)
			{
				case AM_RUNPROCESSES:
					// load the file's information
					pcfadlgThis->loadAttributes();
					break;
				
				default:
					break;
			}
			break;

		case WM_COMMAND:
			switch(wParam)
			{
				case IDC_CMDOK:
					// save changes to file attributes
					pcfadlgThis->saveAttributes();

					// return modal OK
					EndDialog(hwnd, IDOK);
					break;
				
				case IDC_CMDCANCEL:
					// return modal Cancel
					EndDialog(hwnd, IDCANCEL);
					break;
				
				case IDC_CMDPERMISSIONS:
					MessageBox(hwnd, _T("This feature is temporarily disabled."), 
						MAINWINDOW_TITLE, MB_OK);
					break;

				default:	// do nothing
					break;
			}
			break;

		case WM_HOTKEY:		
			EndDialog(hwnd, IDCANCEL);
			break;

		case WM_SHOWWINDOW:
			if(pcfadlgThis->m_hwndParent != NULL)
				PostMessage(pcfadlgThis->m_hwndParent, WM_APP, 
					(WPARAM)AM_LOADINGCOMPLETE, (LPARAM)hwnd);
			break;

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
 * Creates and displays this object's dialog. This object represents a modal
 * dialog which contains the settings for the application; therefore, calling
 * this method halts execution until the user chooses OK or Cancel.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CFileAttributesDialog::show()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
	{
		// attempt to display dialog box
		DialogBox(m_hinstApplication, MAKEINTRESOURCE(IDD_DLGFILEATTRIBUTES1),
					m_hwndParent, (DLGPROC)WindowProc);
    }
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to display the settings dialog.");
        
        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Manages the loading of all attributes for the current file system
 * objects.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CFileAttributesDialog::loadAttributes()
{
	BOOL bReturn = TRUE;

	try
	{
		FILE_INFORMATION *pfiItem = NULL;
		long lFileCount = 0L,		// Differs from m_lFileCount in that this
									//	 represents the number of files in
									//	 the FILE LIST, not the resultant
									//	 total.
			 lDirectoryCount = 0L;	// Differs from m_lDirectoryCount in that
									//	 this represents the number of files in
									//	 the FILE LIST, not the resultant
									//	 total.

		// validate file system object list
		if(m_pllstFileSystemObjects == NULL)
		{
			// set last error
			m_strLastError = _T("The file system object list is invalid.");

			// return fail val
			return FALSE;
		}
		if(m_pllstFileSystemObjects->getLength() == 0)
		{
			// set last error
			m_strLastError = _T("The file system object list is empty.");

			// return fail val
			return FALSE;
		}
		
		// Iterate through items in list and dispatch accordingly...
		for(long lcv = 0L; lcv < m_pllstFileSystemObjects->getLength(); lcv++)
		{
			// get current item
			pfiItem = m_pllstFileSystemObjects->getEntry(lcv);
			
			// validate, continue
			if(pfiItem)
			{
				if(pfiItem->pwfdFileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// get attributes, accumulate return val
					bReturn &= loadDirectoryAttributes(pfiItem);

					// It isn't necessary to increment file or directory count
					//	 here because it is handled elsewhere for directories
					//	 and their sub-directories.
					incrementDirectoryCount();

					// Increment original list contents counter
					lDirectoryCount++;
				}
				else
				{
					// get attributes, accumulate return val
					bReturn &= loadFileAttributes(pfiItem);
					
					// Increment file count
					incrementFileCount();

					// Increment original list contents counter
					lFileCount++;
				}
			}
		}

		// If neither "Everyone" group was present in the file(s)/directory(ies)
		//	 then disable the "Everyone" permissions. NOTE: if missing, this
		//	 indicates that neither group was present in the combined security
		//	 attributes of the file system objects.
		if(!m_bHasEveryoneGroup)
		{
			// Everyone
			EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEREAD), FALSE);
			EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEWRITE), FALSE);
			EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEEXECUTE), FALSE);
		}

		// If there was more than one file system object, then display "combined"
		//	 information and text
		if(m_pllstFileSystemObjects->getLength() > 1)
		{
			TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING,
				  tstrNumber[80] = EMPTY_STRING;
			NUMBERFMT numfmtItem;

			// create number format
			memset(&numfmtItem, 0, sizeof(numfmtItem));
			numfmtItem.lpThousandSep = THOUSANDS_SEPARATOR;
			numfmtItem.lpDecimalSep = DECIMAL_SEPARATOR;
			numfmtItem.Grouping = 3;

			// Set Type to "mixed," if applicable
			if(lFileCount && lDirectoryCount)
				SetDlgItemText(m_hwndThis, IDC_LBLTYPE, STRING_OBJECTTYPE_MIXED);
			// Set Type for multiple instances of same type
			else if(lFileCount > 1)
				SetDlgItemText(m_hwndThis, IDC_LBLTYPE, 
					STRING_OBJECTTYPE_MULTIPLEFILES);
			else if(lDirectoryCount > 1)
				SetDlgItemText(m_hwndThis, IDC_LBLTYPE, 
					STRING_OBJECTTYPE_MULTIPLEDIRECTORIES);
			
			// Change name to reflect multiple file system objects
			SetDlgItemText(m_hwndThis, IDC_LBLFILENAME, STRING_FILENAME_MIXED);
			
			// Clear date/times
			//	 Date Created
			SetDlgItemText(m_hwndThis, IDC_LBLDATECREATED, STRING_GENERAL_MIXED);
			//	 Date Modified
			SetDlgItemText(m_hwndThis, IDC_LBLDATEMODIFIED, STRING_GENERAL_MIXED);
			//	 Date Last Accessed
			SetDlgItemText(m_hwndThis, IDC_LBLDATELASTACCESSED, 
				STRING_GENERAL_MIXED);

			// Set cumulative objects' size
			//	 Create formatted number
			_stprintf(tstrNumber, _T("%u"), m_dwCumulativeFileSize);
			GetNumberFormat(LOCALE_USER_DEFAULT, 0, tstrNumber,
				&numfmtItem, tstrBuffer, sizeof(tstrBuffer));
			_stprintf(tstrBuffer, _T("%s bytes"), tstrBuffer);
			//	 Display
			SetDlgItemText(m_hwndThis, IDC_LBLFILESIZE, tstrBuffer);
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Loads the information and attributes for the current file (the file
 * this object represents, m_strFilename).
 *
 * @return TRUE if all information about and attributes for the file are
 * loaded and no errors occur, otherwise FALSE.
 */
BOOL CFileAttributesDialog::loadFileAttributes(FILE_INFORMATION *pfiFile)
{
	CACLInfo *pacliTemp = NULL;
	HANDLE hfileThis = NULL;
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		WIN32_FIND_DATA *pwfdFile = NULL;
		tstring strFullpath = EMPTY_STRING;

		// validate file system object
		if(pfiFile == NULL)
		{
			// set last error
			m_strLastError = _T("The specified file system object is invalid.");

			// return fail
			return FALSE;
		}
		if(pfiFile->pwfdFileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// set last error
			m_strLastError = _T("The specified file system object is NOT a file.");

			// return fail
			return FALSE;
		}

		// Get pointer to file information
		pwfdFile = pfiFile->pwfdFileInfo;

		// validate base path
		if(m_strBasePath.length() == 0)
		{
			// set last error
			m_strLastError = _T("The base path is invalid.");

			// return fail
			return FALSE;
		}

		// create fullpath to directory... NOTE: m_strBasePath can ONLY be
		//	 set by the constructors and is guaranteed to end in a backslash.
		strFullpath = m_strBasePath;
		strFullpath += pwfdFile->cFileName;
		
		// Attempt to open the file so its information can be retrieved.
		hfileThis = OpenFile_fileIO((TCHAR*)strFullpath.c_str(), FALSE, FALSE);
		if(hfileThis != INVALID_HANDLE_VALUE)
		{
			FILETIME ftmCreated, ftmCreatedLocal,
					 ftmLastAccessed, ftmLastAccessedLocal,
					 ftmModified, ftmModifiedLocal;
			SYSTEMTIME systTemp;
			tstring strObjectName = EMPTY_STRING;
			DWORD dwFileSize = (DWORD)0,
				  dwFileAttributes = (DWORD)0;	
			TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING,
				  *tpc = NULL;

			//// Filename
			SetDlgItemText(m_hwndThis, IDC_LBLFILENAME, pwfdFile->cFileName);

			//// File Size

			//	 Get file size
			dwFileSize = GetFileSize(hfileThis, NULL);
			
			//	 validate, continue
			if(dwFileSize == 0xFFFFFFFF)
			{
				// set last error
				m_strLastError = _T("An error occurred while retrieving the size of the file.");

				// return fail val
				return FALSE;
			}

			// Add to cumulative file size
			m_dwCumulativeFileSize += dwFileSize;

			//	 convert filesize and display
			
			//   convert
			_stprintf(tstrBuffer, _T("%lu bytes"), dwFileSize);
			
			//   get handle for file size
			SetDlgItemText(m_hwndThis, IDC_LBLFILESIZE, tstrBuffer);

			//// File Times
			
			//	 Get file times...
			if(!GetFileTime(hfileThis, &ftmCreated, &ftmLastAccessed, &ftmModified))
			{
				// set last error
				m_strLastError = _T("Could not retrieve the created, last accessed, and modified date/times for the file.");

				// return fail val
				return FALSE;				
			}
			
			//	 Convert to local date/time
			FileTimeToLocalFileTime(&ftmCreated, &ftmCreatedLocal);
			FileTimeToLocalFileTime(&ftmLastAccessed, &ftmLastAccessedLocal);
			FileTimeToLocalFileTime(&ftmModified, &ftmModifiedLocal);

			//	 Create local date/time and display... this is a somewhat arduous
			//	   process.
			
			//	 Date Created
			FileTimeToSystemTime(&ftmCreatedLocal, &systTemp);
			GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &systTemp,
				NULL, tstrBuffer, sizeof(tstrBuffer));
			SetDlgItemText(m_hwndThis, IDC_LBLDATECREATED, tstrBuffer);

			//	 Date Last Accessed
			FileTimeToSystemTime(&ftmLastAccessedLocal, &systTemp);
			GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &systTemp,
				NULL, tstrBuffer, sizeof(tstrBuffer));
			SetDlgItemText(m_hwndThis, IDC_LBLDATELASTACCESSED, tstrBuffer);

			//	 Date Modified
			FileTimeToSystemTime(&ftmModifiedLocal, &systTemp);
			GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &systTemp,
				NULL, tstrBuffer, sizeof(tstrBuffer));
			SetDlgItemText(m_hwndThis, IDC_LBLDATEMODIFIED, tstrBuffer);

			//// File Attributes

			//	 Get file attributes
			dwFileAttributes = GetFileAttributes(strFullpath.c_str());
			if(dwFileAttributes == INVALID_FILE_ATTRIBUTES)
			{
				// set last error
				m_strLastError = _T("Could not retrieve the file attributes for the file.");

				// return fail val
				return FALSE;
			}

			// Check and see if the "Everyone" group has any entries in this file's
			//	 DACL
			pacliTemp = new CACLInfo(EMPTY_STRING);
			if(pacliTemp)
			{
				pacliTemp->setPath((TCHAR *)strFullpath.c_str());
				pacliTemp->Query();
				m_bHasEveryoneGroup = pacliTemp->hasEveryoneGroup();
			}

			// add to cumulative
			m_dwCumulativeAttributes |= dwFileAttributes;

			//	 Read-only
			CheckDlgButton(m_hwndThis, IDC_CHKREADONLY, 
					(m_dwCumulativeAttributes & FILE_ATTRIBUTE_READONLY));
				
			//	 Archive
			CheckDlgButton(m_hwndThis, IDC_CHKARCHIVE, 
					(m_dwCumulativeAttributes & FILE_ATTRIBUTE_ARCHIVE));

			//	 Hidden
			CheckDlgButton(m_hwndThis, IDC_CHKHIDDEN, 
					(m_dwCumulativeAttributes & FILE_ATTRIBUTE_HIDDEN));

			//	 System
			CheckDlgButton(m_hwndThis, IDC_CHKSYSTEM, 
					(m_dwCumulativeAttributes & FILE_ATTRIBUTE_SYSTEM));

			// Add to cumulative permissions
			m_acerCumulativeRights |= *pfiFile->paceFileRights;

			//	 System
			CheckDlgButton(m_hwndThis, IDC_CHKSYSTEMREAD, 
					m_acerCumulativeRights.rightsSystem.btRead);

			CheckDlgButton(m_hwndThis, IDC_CHKSYSTEMWRITE, 
					m_acerCumulativeRights.rightsSystem.btWrite);

			CheckDlgButton(m_hwndThis, IDC_CHKSYSTEMEXECUTE, 
					m_acerCumulativeRights.rightsSystem.btExecute);

			//	 Administrators
			CheckDlgButton(m_hwndThis, IDC_CHKADMINISTRATORSREAD,
				m_acerCumulativeRights.rightsAdministrators.btRead);

			CheckDlgButton(m_hwndThis, IDC_CHKADMINISTRATORSWRITE,
				m_acerCumulativeRights.rightsAdministrators.btWrite);

			CheckDlgButton(m_hwndThis, IDC_CHKADMINISTRATORSEXECUTE,
				m_acerCumulativeRights.rightsAdministrators.btExecute);

			//	 Everyone
			CheckDlgButton(m_hwndThis, IDC_CHKEVERYONEREAD,
				m_acerCumulativeRights.rightsEveryone.btRead);

			CheckDlgButton(m_hwndThis, IDC_CHKEVERYONEWRITE,
				m_acerCumulativeRights.rightsEveryone.btWrite);

			CheckDlgButton(m_hwndThis, IDC_CHKEVERYONEEXECUTE,
				m_acerCumulativeRights.rightsEveryone.btExecute);

			//	 Users
			CheckDlgButton(m_hwndThis, IDC_CHKUSERSREAD, 
				m_acerCumulativeRights.rightsUsers.btRead);

			CheckDlgButton(m_hwndThis, IDC_CHKUSERSWRITE, 
				m_acerCumulativeRights.rightsUsers.btWrite);

			CheckDlgButton(m_hwndThis, IDC_CHKUSERSEXECUTE, 
				m_acerCumulativeRights.rightsUsers.btExecute);
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// garbage collect
	if(hfileThis)
		CloseHandle(hfileThis);
	if(pacliTemp)
	{
		delete pacliTemp;
		pacliTemp = NULL;
	}

    // return success / fail val
    return bReturn;
}

/**
 * Loads the information and attributes for the current directory.
 *
 * @return TRUE if all information about and attributes for the file /
 * directory are loaded and no errors occur, otherwise FALSE.
 */
BOOL CFileAttributesDialog::loadDirectoryAttributes(
	FILE_INFORMATION *pfiDirectory)
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		WIN32_FILE_ATTRIBUTE_DATA wfadObject;
		WIN32_FIND_DATA *pwfdDirectory = NULL;
		tstring strObjectName = EMPTY_STRING,
					strFullpath = EMPTY_STRING;
		FILETIME ftmCreatedLocal,
				 ftmLastAccessedLocal,
				 ftmModifiedLocal;
		SYSTEMTIME systTemp;
		DWORD dwCumulativeAttributes = (DWORD)0;
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING,
			  *tpc = NULL;

		// validate file system object
		if(pfiDirectory == NULL)
		{
			// set last error
			m_strLastError = _T("The specified file system object is invalid.");

			// return fail
			return FALSE;
		}
		if(!(pfiDirectory->pwfdFileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			// set last error
			m_strLastError = _T("The specified file system object is NOT a directory.");

			// return fail
			return FALSE;
		}

		// Get pointer to directory info
		pwfdDirectory = pfiDirectory->pwfdFileInfo;

		// validate base path
		if(m_strBasePath.length() == 0)
		{
			// set last error
			m_strLastError = _T("The base path is invalid.");

			// return fail
			return FALSE;
		}

		// create fullpath to directory... NOTE: m_strBasePath can ONLY be
		//	 set by the constructors and is guaranteed to end in a backslash.
		strFullpath = m_strBasePath;
		strFullpath += pwfdDirectory->cFileName;
		
		// Get extended attribute information
		if(!GetFileAttributesEx((TCHAR *)strFullpath.data(), 
			GetFileExInfoStandard, &wfadObject))
		{
			// set last error
			m_strLastError = _T("Could not retrieve the attributes for the specified file or directory.");

			// return fail val
			return FALSE;
		}

		//// Display information

		// Directory Name
		strObjectName = pwfdDirectory->cFileName;

		// Display object name
		SetDlgItemText(m_hwndThis, IDC_LBLFILENAME, (TCHAR *)strObjectName.data());

		//// Object Type
		SetDlgItemText(m_hwndThis, IDC_LBLTYPE, STRING_OBJECTTYPE_DIRECTORY);
	
		//// File Size
		SetDlgItemText(m_hwndThis, IDC_LBLFILESIZE, STRING_FILESIZE_DIRECTORY);

		//	 Convert to local date/time
		FileTimeToLocalFileTime(&wfadObject.ftCreationTime, &ftmCreatedLocal);
		FileTimeToLocalFileTime(&wfadObject.ftLastAccessTime, &ftmLastAccessedLocal);
		FileTimeToLocalFileTime(&wfadObject.ftLastWriteTime, &ftmModifiedLocal);

		//	 Create local date/time and display... this is a somewhat arduous
		//	   process.
		
		//	 Date Created
		FileTimeToSystemTime(&ftmCreatedLocal, &systTemp);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &systTemp,
			NULL, tstrBuffer, sizeof(tstrBuffer));
		SetDlgItemText(m_hwndThis, IDC_LBLDATECREATED, tstrBuffer);

		//	 Date Last Accessed
		FileTimeToSystemTime(&ftmLastAccessedLocal, &systTemp);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &systTemp,
			NULL, tstrBuffer, sizeof(tstrBuffer));
		SetDlgItemText(m_hwndThis, IDC_LBLDATELASTACCESSED, tstrBuffer);

		//	 Date Modified
		FileTimeToSystemTime(&ftmModifiedLocal, &systTemp);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &systTemp,
			NULL, tstrBuffer, sizeof(tstrBuffer));
		SetDlgItemText(m_hwndThis, IDC_LBLDATEMODIFIED, tstrBuffer);

		//// File Attributes

		// If this is a directory, then get cumulative attributes and file object
		//	 count information
		HWND hwndInfoItem = NULL;

		// Disable attribute items until search is completed.
		//	 Attributes
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKARCHIVE), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKREADONLY), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKSYSTEM), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKHIDDEN), FALSE);
		//	 Permissions
		//		System
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKSYSTEMREAD), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKSYSTEMWRITE), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKSYSTEMEXECUTE), FALSE);
		//		Administrators
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKADMINISTRATORSREAD), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKADMINISTRATORSWRITE), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKADMINISTRATORSEXECUTE), 
			FALSE);
		//		Everyone
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEREAD), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEWRITE), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEEXECUTE), FALSE);
		//		Users
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKUSERSREAD), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKUSERSWRITE), FALSE);
		EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKUSERSEXECUTE), FALSE);

		// get handle to info window items and display them
		//	 File Count
		hwndInfoItem = GetDlgItem(m_hwndThis, IDC_LBLFILES);
		if(hwndInfoItem != NULL)
			ShowWindow(hwndInfoItem, SW_SHOW);
		//	 File Description
		hwndInfoItem = GetDlgItem(m_hwndThis, IDC_LBLFILESDESCR);
		if(hwndInfoItem != NULL)
			ShowWindow(hwndInfoItem, SW_SHOW);
		//	 Directory Count
		hwndInfoItem = GetDlgItem(m_hwndThis, IDC_LBLDIRECTORIES);
		if(hwndInfoItem != NULL)
			ShowWindow(hwndInfoItem, SW_SHOW);
		//	 Directory Descrption
		hwndInfoItem = GetDlgItem(m_hwndThis, IDC_LBLDIRECTORIESDESCR);
		if(hwndInfoItem != NULL)
			ShowWindow(hwndInfoItem, SW_SHOW);

		// get cumulative information
		getDirectoryTreeInformation((TCHAR *)strFullpath.data());

		// transfer attributes to data structure... 
		// NO, strike that!
		//	 Add attributes so that whatever the current directory's
		//	 attributes are they are included in the final.
		wfadObject.dwFileAttributes |= m_dwCumulativeAttributes;

		//if(dwCumulativeAttributes & FILE_ATTRIBUTE_READONLY)
		//	MessageBox(NULL, "Read-only", "Test", MB_OK);
		//if(dwCumulativeAttributes & FILE_ATTRIBUTE_HIDDEN)
		//	MessageBox(NULL, "Hidden", "Test", MB_OK);
		//if(dwCumulativeAttributes & FILE_ATTRIBUTE_SYSTEM)
		//	MessageBox(NULL, "System", "Test", MB_OK);
		//if(dwCumulativeAttributes & FILE_ATTRIBUTE_ARCHIVE)
		//	MessageBox(NULL, "Archive", "Test", MB_OK);

		//	 Get file attributes
		if(wfadObject.dwFileAttributes == INVALID_FILE_ATTRIBUTES)
		{
			// set last error
			m_strLastError = _T("Could not retrieve the file attributes for the file.");

			// return fail val
			return FALSE;
		}

		//	 Read-only
		CheckDlgButton(m_hwndThis, IDC_CHKREADONLY, 
				(wfadObject.dwFileAttributes & FILE_ATTRIBUTE_READONLY));
			
		//	 Archive
		CheckDlgButton(m_hwndThis, IDC_CHKARCHIVE, 
				(wfadObject.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE));

		//	 Hidden
		CheckDlgButton(m_hwndThis, IDC_CHKHIDDEN, 
				(wfadObject.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN));

		//	 System
		CheckDlgButton(m_hwndThis, IDC_CHKSYSTEM, 
				(wfadObject.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM));

		// Permissions
		m_acerCumulativeRights |= *pfiDirectory->paceFileRights;

		//	 System
		CheckDlgButton(m_hwndThis, IDC_CHKSYSTEMREAD, 
				m_acerCumulativeRights.rightsSystem.btRead);

		CheckDlgButton(m_hwndThis, IDC_CHKSYSTEMWRITE, 
				m_acerCumulativeRights.rightsSystem.btWrite);

		CheckDlgButton(m_hwndThis, IDC_CHKSYSTEMEXECUTE, 
				m_acerCumulativeRights.rightsSystem.btExecute);

		//	 Administrators
		CheckDlgButton(m_hwndThis, IDC_CHKADMINISTRATORSREAD,
			m_acerCumulativeRights.rightsAdministrators.btRead);

		CheckDlgButton(m_hwndThis, IDC_CHKADMINISTRATORSWRITE,
			m_acerCumulativeRights.rightsAdministrators.btWrite);

		CheckDlgButton(m_hwndThis, IDC_CHKADMINISTRATORSEXECUTE,
			m_acerCumulativeRights.rightsAdministrators.btExecute);

		//	 Everyone
		CheckDlgButton(m_hwndThis, IDC_CHKEVERYONEREAD,
			m_acerCumulativeRights.rightsEveryone.btRead);

		CheckDlgButton(m_hwndThis, IDC_CHKEVERYONEWRITE,
			m_acerCumulativeRights.rightsEveryone.btWrite);

		CheckDlgButton(m_hwndThis, IDC_CHKEVERYONEEXECUTE,
			m_acerCumulativeRights.rightsEveryone.btExecute);

		//	 Users
		CheckDlgButton(m_hwndThis, IDC_CHKUSERSREAD, 
			m_acerCumulativeRights.rightsUsers.btRead);

		CheckDlgButton(m_hwndThis, IDC_CHKUSERSWRITE, 
			m_acerCumulativeRights.rightsUsers.btWrite);

		CheckDlgButton(m_hwndThis, IDC_CHKUSERSEXECUTE, 
			m_acerCumulativeRights.rightsUsers.btExecute);
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

	// Enable any controls that may have been disabled
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKARCHIVE), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKREADONLY), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKSYSTEM), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKHIDDEN), TRUE);
	//	 Permissions
	//		System
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKSYSTEMREAD), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKSYSTEMWRITE), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKSYSTEMEXECUTE), TRUE);
	//		Administrators
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKADMINISTRATORSREAD), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKADMINISTRATORSWRITE), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKADMINISTRATORSEXECUTE), 
		TRUE);
	//		Everyone
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEREAD), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEWRITE), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKEVERYONEEXECUTE), TRUE);
	//		Users
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKUSERSREAD), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKUSERSWRITE), TRUE);
	EnableWindow(GetDlgItem(m_hwndThis, IDC_CHKUSERSEXECUTE), TRUE);

	// Set focus to first attribute checkbox
	SetFocus(GetDlgItem(m_hwndThis, IDC_CHKARCHIVE));

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Manages the process of applying the attributes selected by the user
 * to all file system objects in the file system object list.
 *
 * @return TRUE if no errors occur and ALL attributes are set properly,
 * otherwise FALSE.
 */
BOOL CFileAttributesDialog::saveAttributes()
{
	BOOL bReturn = TRUE;

	try
	{
		FILE_INFORMATION *pfiItem = NULL;

		// validate file system object list
		if(m_pllstFileSystemObjects == NULL)
		{
			// set last error
			m_strLastError = _T("The file system object list is invalid.");

			// return fail val
			return FALSE;
		}
		if(m_pllstFileSystemObjects->getLength() == 0)
		{
			// set last error
			m_strLastError = _T("The file system object list is empty.");

			// return fail val
			return FALSE;
		}

		// Get permissions and store back in "cumulative" permissions...
		//	 System
		m_acerCumulativeRights.rightsSystem.btRead = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKSYSTEMREAD);
		m_acerCumulativeRights.rightsSystem.btWrite = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKSYSTEMWRITE);
		m_acerCumulativeRights.rightsSystem.btExecute = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKSYSTEMEXECUTE);
		//	 Administrators
		m_acerCumulativeRights.rightsAdministrators.btRead = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKADMINISTRATORSREAD);
		m_acerCumulativeRights.rightsAdministrators.btWrite = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKADMINISTRATORSWRITE);
		m_acerCumulativeRights.rightsAdministrators.btExecute = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKADMINISTRATORSEXECUTE);
		//	 Everyone
		m_acerCumulativeRights.rightsEveryone.btRead = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKEVERYONEREAD);
		m_acerCumulativeRights.rightsEveryone.btWrite = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKEVERYONEWRITE);
		m_acerCumulativeRights.rightsEveryone.btExecute = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKEVERYONEEXECUTE);
		//	 Users
		m_acerCumulativeRights.rightsUsers.btRead = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKUSERSREAD);
		m_acerCumulativeRights.rightsUsers.btWrite = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKUSERSWRITE);
		m_acerCumulativeRights.rightsUsers.btExecute = 
			IsDlgButtonChecked(m_hwndThis, IDC_CHKUSERSEXECUTE);
		
		// Iterate through items in list and dispatch accordingly...
		for(long lcv = 0L; lcv < m_pllstFileSystemObjects->getLength(); lcv++)
		{
			// get current item
			pfiItem = m_pllstFileSystemObjects->getEntry(lcv);
			
			// validate, continue
			if(pfiItem)
			{
				if(pfiItem->pwfdFileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					// set attributes, accumulate return val
					bReturn &= saveDirectoryAttributes(pfiItem);
				else
					// set attributes, accumulate return val
					bReturn &= saveFileAttributes(pfiItem);
			}
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While setting the attributes, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Saves the file attributes for the current file (the file
 * this object represents, m_strFilename). 
 *
 * @return TRUE if the file's attributes are saved and no errors  occur,
 * otherwise FALSE.
 */
BOOL CFileAttributesDialog::saveFileAttributes(FILE_INFORMATION *pfiFile)
{
	CACLInfo *pacliTemp = NULL;
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		WIN32_FIND_DATA *pwfdFile = NULL;
		DWORD dwFileAttributes = (DWORD)0;
		BOOL bNotIndexed = FALSE;
		tstring strFullpath = EMPTY_STRING;

		// validate file system object
		if(pfiFile == NULL)
		{
			// set last error
			m_strLastError = _T("The specified file system object is invalid.");

			// return fail
			return FALSE;
		}
		if(pfiFile->pwfdFileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// set last error
			m_strLastError = _T("The specified file system object is NOT a file.");

			// return fail
			return FALSE;
		}

		// Get pointer to file info
		pwfdFile = pfiFile->pwfdFileInfo;

		// validate base path
		if(m_strBasePath.length() == 0)
		{
			// set last error
			m_strLastError = _T("The base path is invalid.");

			// return fail
			return FALSE;
		}

		// create fullpath to directory... NOTE: m_strBasePath can ONLY be
		//	 set by the constructors and is guaranteed to end in a backslash.
		strFullpath = m_strBasePath;
		strFullpath += pwfdFile->cFileName;

		// attempt to get attributes
		dwFileAttributes = GetFileAttributes((TCHAR *)strFullpath.data());
		if(dwFileAttributes == INVALID_FILE_ATTRIBUTES)
		{
			// set last error
			m_strLastError = _T("Could not retrieve the file attributes for the file.");

			// return fail val
			return FALSE;
		}

		// Check and see if file is indexed... we don't want to mess
		//	 with this.
		if(dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
			bNotIndexed = TRUE;

		// Unset ALL attributes
		dwFileAttributes = dwFileAttributes ^ dwFileAttributes;
		SetFileAttributes((TCHAR *)strFullpath.data(), dwFileAttributes);

		// Reset indexed attribute
		if(bNotIndexed)
			dwFileAttributes |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;

		// Now, set attributes
		if(IsDlgButtonChecked(m_hwndThis, IDC_CHKARCHIVE))
			dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE;
		if(IsDlgButtonChecked(m_hwndThis, IDC_CHKREADONLY))
			dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
		if(IsDlgButtonChecked(m_hwndThis, IDC_CHKHIDDEN))
			dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
		if(IsDlgButtonChecked(m_hwndThis, IDC_CHKSYSTEM))
			dwFileAttributes |= FILE_ATTRIBUTE_SYSTEM;

		// Check and see if "normal" attribute should be set
		if(dwFileAttributes == (DWORD)0)
			dwFileAttributes |= FILE_ATTRIBUTE_NORMAL;

		// Now, set file attributes
		SetFileAttributes((TCHAR *)strFullpath.data(), dwFileAttributes);

		// Finally, set file permissions
		pacliTemp = new CACLInfo((TCHAR *)strFullpath.data());
		if(pacliTemp)
		{
			// query (prepares internal info)
			pacliTemp->Query();

			// set rights
			pacliTemp->SetRights(&m_acerCumulativeRights);
		}	
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("An error occurred while attempting to set the attributes for the file.");

        // set fail value
        bReturn = FALSE;
    }
	
	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// garbage collect
	if(pacliTemp)
	{
		delete pacliTemp;
		pacliTemp = NULL;
	}

    // return success / fail val
    return bReturn;
}

/**
 * Saves the file attributes for the current file. NOTE: changes are applied 
 * to ALL files and sub-directories.
 *
 * @return TRUE if the attributes are set no errors occur, otherwise FALSE.
 */
BOOL CFileAttributesDialog::saveDirectoryAttributes(
	FILE_INFORMATION *pfiDirectory)
{
	CACLInfo *pacliTemp = NULL;
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		WIN32_FILE_ATTRIBUTE_DATA wfadObject;
		WIN32_FIND_DATA *pwfdDirectory = NULL;
		tstring strFullpath = EMPTY_STRING;
		DWORD dwFileAttributes = (DWORD)0;
		BOOL bNotIndexed = FALSE, 
			 bIsFile = FALSE;

		// validate file system object
		if(pfiDirectory == NULL)
		{
			// set last error
			m_strLastError = _T("The specified file system object is invalid.");

			// return fail
			return FALSE;
		}
		if(!(pfiDirectory->pwfdFileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			// set last error
			m_strLastError = _T("The specified file system object is NOT a directory.");

			// return fail
			return FALSE;
		}

		// Get pointer to directory info
		pwfdDirectory = pfiDirectory->pwfdFileInfo;

		// validate base path
		if(m_strBasePath.length() == 0)
		{
			// set last error
			m_strLastError = _T("The base path is invalid.");

			// return fail
			return FALSE;
		}

		// create fullpath to directory... NOTE: m_strBasePath can ONLY be
		//	 set by the constructors and is guaranteed to end in a backslash.
		strFullpath = m_strBasePath;
		strFullpath += pwfdDirectory->cFileName;

		// validate OS version
		if(!isAtLeastWindowsXP())
			return FALSE;

		// validate filename
		if(strFullpath.length() == 0)
			return FALSE;

		// attempt to get attributes
		if(!GetFileAttributesEx((TCHAR *)strFullpath.data(), GetFileExInfoStandard,
			 &wfadObject))
		{
			// set last error
			m_strLastError = _T("Could not retrieve the file attributes for the file.");

			// return fail val
			return FALSE;
		}

		// Check and see if file is indexed... we don't want to mess
		//	 with this.
		if(wfadObject.dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
			bNotIndexed = TRUE;

		// Unset ALL attributes
		wfadObject.dwFileAttributes = wfadObject.dwFileAttributes 
										^ wfadObject.dwFileAttributes;

		// Reset indexed attribute
		if(bNotIndexed)
			wfadObject.dwFileAttributes |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;

		// Now, set attributes
		if(IsDlgButtonChecked(m_hwndThis, IDC_CHKARCHIVE))
			wfadObject.dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE;
		if(IsDlgButtonChecked(m_hwndThis, IDC_CHKREADONLY))
			wfadObject.dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
		if(IsDlgButtonChecked(m_hwndThis, IDC_CHKHIDDEN))
			wfadObject.dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
		if(IsDlgButtonChecked(m_hwndThis, IDC_CHKSYSTEM))
			wfadObject.dwFileAttributes |= FILE_ATTRIBUTE_SYSTEM;

		// Check and see if "normal" attribute should be set
		if(wfadObject.dwFileAttributes == (DWORD)0)
			wfadObject.dwFileAttributes |= FILE_ATTRIBUTE_NORMAL;

		// Set directory attributes
		SetFileAttributes((TCHAR *)strFullpath.data(), wfadObject.dwFileAttributes);

		// Set directory permissions
		pacliTemp = new CACLInfo((TCHAR *)strFullpath.data());
		if(pacliTemp)
		{
			// query (prepares internal info)
			pacliTemp->Query();

			// set rights
			pacliTemp->SetRights(&m_acerCumulativeRights);
		}
		
		// Finally, set attributes on subdirectories and files
		bReturn = setDirectoryTreeAttributes((TCHAR *)strFullpath.data(), 
						wfadObject.dwFileAttributes);
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to save the attributes, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// garbage collect
	if(pacliTemp)
	{
		delete pacliTemp;
		pacliTemp = NULL;
	}

    // return success / fail val
    return bReturn;
}

/**
 * Loads, from the registry, the user's preferences for this dialog.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CFileAttributesDialog::loadPreferences()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		RECT rctThis,
			 rctScreen;
		int iWidth,
			iHeight;

		// check handle
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
			REG_VAL_PREFS_LOCATION_FILEATTRIBUTES, &rctThis, 
			sizeof(rctThis), FALSE);

		// Check and see if a stored value was present... if the
		//	 position of the dialog was stored previously then there
		//	 be something other than 0's for the right and bottom
		if(rctThis.bottom != 0 && rctThis.right != 0)
			SetWindowPos(m_hwndThis, m_hwndParent, rctThis.left,
				rctThis.top, 0, 0, SWP_NOSIZE);
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Saves, to the registry, the user's preferences for this dialog.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CFileAttributesDialog::savePreferences()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		RECT rctThis;

		// Validate this object's handle... if it isn't good, then
		//	 there's nothing we can do.
		if(m_hwndThis == NULL)
			return FALSE;
		
		// Get this object's dialog's position/size
		GetWindowRect(m_hwndThis, &rctThis);

		// Attempt to save this dialog's location
		SaveRegistryBinary(CurrentUser, REG_BASE, 
			REG_SECTION_PREFERENCES,
			REG_VAL_PREFS_LOCATION_FILEATTRIBUTES, &rctThis, 
			sizeof(rctThis), TRUE);
    }
    catch(...)
    {
        // set last error
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Loads the icon for this window at startup.
 *
 * @param hwndThis HWND of this object's GUI (window)
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CFileAttributesDialog::loadIcon()
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

    // return success / fail val
    return bReturn;
}

/**
 * Sets the "information" labels and the frame's font to the internal
 * font.
 *
 * @return
 */
BOOL CFileAttributesDialog::setDescriptionLabelsFont()
{
	HDC hdcTemp = NULL;
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HWND hwndTemp = NULL;
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
		
		// Get DC for dialog
		hdcTemp = GetDC(m_hwndThis);

		// validate, continue
		if(hdcTemp == NULL)
		{
			// set last error
			m_strLastError = _T("Could not retrieve the DC for this dialog.");

			// return fail val
			return FALSE;
		}

		// calculate desired font size in pixels...
		iHeight = -MulDiv(FONTSIZE_DESCRIPTIONLABELS, 
						GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);

		// Attempt to create desired font (Terminal)
		m_hfontLabels = CreateFont(iHeight, 0, 0, 0, FW_BOLD, 0, 0, 0, 
								DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								0, _T("Tahoma"));

		// validate font, continue
		if(m_hfontLabels == NULL)
		{
			// set last error
			m_strLastError = _T("Could not create the font for the description labels.");

			// release DC for this dialog
			ReleaseDC(m_hwndThis, hdcTemp);

			// return fail val
			return FALSE;
		}

		// attempt to get controls and set existing font

		//	 Name
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLFILENAMEDESCR);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}
			
		//	 Type
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLTYPEDESCR);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}

		//	 Additional Info
		//		 Files
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLFILESDESCR);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}
		//		 Directories
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLDIRECTORIESDESCR);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}

		//	 Size
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLFILESIZEDESCR);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}

		//	 Date Created
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLDATECREATEDDESCR);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}

		//	 Date Last Accessed
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLDATELASTACCESSEDDESCR);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}

		//	 Date Modified
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLDATEMODIFIEDDESCR);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}

		//	 Attributes Frame
		hwndTemp = GetDlgItem(m_hwndThis, IDC_FRAATTRIBUTES);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}

		//	 Permissions Frame
		hwndTemp = GetDlgItem(m_hwndThis, IDC_FRAPERMISSIONS);
		if(hwndTemp)
		{
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontLabels, 
				(LPARAM)MAKELPARAM(TRUE, 0));
		}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to set the description labels' fonts, an error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// perform final garbage collection... in this case, make sure that DC
	//	 is released
	if(hdcTemp)
		// release DC for dialog
		ReleaseDC(m_hwndThis, hdcTemp);

    // return success / fail val
    return bReturn;
}

/**
 * Returns whether or not the OS is at least Windows(r) XP.
 *
 * @return TRUE if the Windows(r) version is at least Windows(r) XP,
 * otherwise FALSE.
 */
BOOL CFileAttributesDialog::isAtLeastWindowsXP()
{
    BOOL bReturn = FALSE;	// default to optimistic return val

    try
    {
        DWORD dwMajorVersion, dwVersion;
		
		// Get OS version
        dwVersion = GetVersion();

		// Get Major
        dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

		// check
		if(dwMajorVersion > 4)
			bReturn = TRUE;
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While retrieving the OS version, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Walks the folder structure for the directory specified.
 *
 * @return TRUE if the Windows(r) version is at least Windows(r) XP,
 * otherwise FALSE.
 */
BOOL CFileAttributesDialog::getDirectoryTreeInformation(TCHAR *tstrDirectory)
{
	CACLInfo *pacliTemp = NULL;
	HANDLE hCurrentDirectory = NULL;
    BOOL bReturn = FALSE;	// default to optimistic return val

    try
    {
		WIN32_FIND_DATA wfdItem;
		ACERIGHTS acerightsLocal;
		tstring strBasePath = EMPTY_STRING,
					strTempPath = EMPTY_STRING;
		
		// validate directory param
		if(lstrlen(tstrDirectory) == 0)
			return FALSE;

		// construct initial path and file spec
		strBasePath = tstrDirectory;
		if(strBasePath[strBasePath.length() - 1] != _T('\\'))
			strBasePath += _T("\\");
		strTempPath = strBasePath + _T("*.*");

		// Get first file / directory
		hCurrentDirectory = FindFirstFile((TCHAR *)strTempPath.data(), &wfdItem);
		if(hCurrentDirectory != INVALID_HANDLE_VALUE)
		{
			// create permissions retrieval object
			pacliTemp = new CACLInfo(EMPTY_STRING);
					
			// build path
			strTempPath = strBasePath;
			strTempPath += wfdItem.cFileName;

			// check and see if this is a directory
			if(wfdItem.cFileName[0] != '.' && 
				wfdItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// increment directory count
				incrementDirectoryCount();
				
				// get attributes
				m_dwCumulativeAttributes |= wfdItem.dwFileAttributes;

				// get permissions
				pacliTemp->setPath((TCHAR *)strTempPath.data());
				pacliTemp->Output(acerightsLocal);
				m_acerCumulativeRights |= acerightsLocal;
				m_bHasEveryoneGroup |= pacliTemp->hasEveryoneGroup();

				// walk sub-directory tree
				bReturn = getDirectoryTreeInformation(
							(TCHAR *)strTempPath.data());
			}
			else if(!(wfdItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// increment file count
				incrementFileCount();

				// get attributes
				m_dwCumulativeAttributes |= wfdItem.dwFileAttributes;

				// get permissions
				pacliTemp->setPath((TCHAR *)strTempPath.data());
				pacliTemp->Output(acerightsLocal);
				m_acerCumulativeRights |= acerightsLocal;
				m_bHasEveryoneGroup |= pacliTemp->hasEveryoneGroup();

				// get filesize
				m_dwCumulativeFileSize += wfdItem.nFileSizeLow;
			}

			// Get all subsequent files / directories
			while(FindNextFile(hCurrentDirectory, &wfdItem))
			{
				// build path
				strTempPath = strBasePath;
				strTempPath += wfdItem.cFileName;

				// check and see if this is a directory
				if(wfdItem.cFileName[0] != '.' && 
					wfdItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// increment directory count
					incrementDirectoryCount();

					// get permissions
					pacliTemp->setPath((TCHAR *)strTempPath.data());
					pacliTemp->Output(acerightsLocal);
					m_acerCumulativeRights |= acerightsLocal;
					m_bHasEveryoneGroup |= pacliTemp->hasEveryoneGroup();

					// get attributes
					m_dwCumulativeAttributes |= wfdItem.dwFileAttributes;
					
					// walk sub-directory tree
					bReturn = getDirectoryTreeInformation(
								(TCHAR *)strTempPath.data());
				}
				else if(!(wfdItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					// increment file count
					incrementFileCount();

					// get attributes
					m_dwCumulativeAttributes |= wfdItem.dwFileAttributes;

					// get permissions
					pacliTemp->setPath((TCHAR *)strTempPath.data());
					pacliTemp->Output(acerightsLocal);
					m_acerCumulativeRights |= acerightsLocal;
					m_bHasEveryoneGroup |= pacliTemp->hasEveryoneGroup();

					// get filesize
					m_dwCumulativeFileSize += wfdItem.nFileSizeLow;
				}
			}
		}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While walking the directory tree, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// garbage collection
	if(hCurrentDirectory != NULL && hCurrentDirectory != INVALID_HANDLE_VALUE)
		FindClose(hCurrentDirectory);
	if(pacliTemp)
	{
		delete pacliTemp;
		pacliTemp = NULL;
	}

    // return success / fail val
    return bReturn;
}

/**
 * Assigns the specified attributes to the entire directory tree for the
 * given folder.
 *
 * @return TRUE if the Windows(r) version is at least Windows(r) XP,
 * otherwise FALSE.
 */
BOOL CFileAttributesDialog::setDirectoryTreeAttributes(TCHAR *tstrDirectory, 
	DWORD dwAttributes)
{
	CACLInfo *pacliTemp = NULL;
	HANDLE hCurrentDirectory = NULL;
    BOOL bReturn = FALSE;	// default to optimistic return val

    try
    {
		WIN32_FIND_DATA wfdItem;
		tstring strBasePath = EMPTY_STRING,
					strTempPath = EMPTY_STRING;
		DWORD dwCurrentAttributes = (DWORD)0;
		
		// validate directory param
		if(lstrlen(tstrDirectory) == 0)
			return FALSE;

		// construct initial path and file spec
		strBasePath = tstrDirectory;
		if(strBasePath[strBasePath.length() - 1] != _T('\\'))
			strBasePath += _T("\\");
		strTempPath = strBasePath + _T("*.*");

		// Get first file / directory
		hCurrentDirectory = FindFirstFile((TCHAR *)strTempPath.data(), &wfdItem);
		if(hCurrentDirectory != INVALID_HANDLE_VALUE)
		{
			// create permissions retrieval object
			pacliTemp = new CACLInfo(EMPTY_STRING);

			// build path
			strTempPath = strBasePath;
			strTempPath += wfdItem.cFileName;
				
			// check and see if this is a directory
			if(wfdItem.cFileName[0] != '.' && 
				wfdItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// set attributes

				// Clear existing
				dwCurrentAttributes = GetFileAttributes((TCHAR *)strTempPath.data());
				SetFileAttributes((TCHAR *)strTempPath.data(), 
					dwCurrentAttributes ^ dwCurrentAttributes);
				
				// Set new
				SetFileAttributes((TCHAR *)strTempPath.data(), dwAttributes);

				// get permissions
				pacliTemp->setPath((TCHAR *)strTempPath.data());

				// set new permissions
				pacliTemp->SetRights(&m_acerCumulativeRights);

				// walk sub-directory tree
				bReturn = setDirectoryTreeAttributes((TCHAR *)strTempPath.data(), 
							dwAttributes);
			}
			else if(!(wfdItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// set attributes

				// Clear existing
				dwCurrentAttributes = GetFileAttributes((TCHAR *)strTempPath.data());
				SetFileAttributes((TCHAR *)strTempPath.data(), 
					dwCurrentAttributes ^ dwCurrentAttributes);
				
				// Set new
				SetFileAttributes((TCHAR *)strTempPath.data(), dwAttributes);

				// get permissions
				pacliTemp->setPath((TCHAR *)strTempPath.data());

				// set new permissions
				pacliTemp->SetRights(&m_acerCumulativeRights);
			}

			// Get all subsequent files / directories
			while(FindNextFile(hCurrentDirectory, &wfdItem))
			{
				// build path
				strTempPath = strBasePath;
				strTempPath += wfdItem.cFileName;

				// check and see if this is a directory
				if(wfdItem.cFileName[0] != '.' && 
					wfdItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// set attributes

					// Clear existing
					dwCurrentAttributes = GetFileAttributes((TCHAR *)strTempPath.data());
					SetFileAttributes((TCHAR *)strTempPath.data(), 
						dwCurrentAttributes ^ dwCurrentAttributes);
					
					// Set new
					SetFileAttributes((TCHAR *)strTempPath.data(), dwAttributes);

					// get permissions
					pacliTemp->setPath((TCHAR *)strTempPath.data());

					// set new permissions
					pacliTemp->SetRights(&m_acerCumulativeRights);

					// walk sub-directory tree
					bReturn = setDirectoryTreeAttributes((TCHAR *)strTempPath.data(), 
								dwAttributes);
				}
				else if(!(wfdItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					DWORD dwCurrentAttributes = (DWORD)0;

					// set attributes

					// Clear existing
					dwCurrentAttributes = GetFileAttributes((TCHAR *)strTempPath.data());
					SetFileAttributes((TCHAR *)strTempPath.data(), 
						dwCurrentAttributes ^ dwCurrentAttributes);
					
					// get permissions
					pacliTemp->setPath((TCHAR *)strTempPath.data());

					// set new permissions
					pacliTemp->SetRights(&m_acerCumulativeRights);

					// Set new
					SetFileAttributes((TCHAR *)strTempPath.data(), dwAttributes);
				}
			}
		}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While walking the directory tree, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// garbage collection
	if(hCurrentDirectory != NULL && hCurrentDirectory != INVALID_HANDLE_VALUE)
		FindClose(hCurrentDirectory);
	if(pacliTemp)
	{
		delete pacliTemp;
		pacliTemp = NULL;
	}

    // return success / fail val
    return bReturn;
}

/**
 * Increments the file count and re-displays the information on *this*
 * dialog.
 *
 * @return -1 if an error occurs, otherwise the current file count
 */
int CFileAttributesDialog::incrementFileCount()
{
	int iReturn = -1;

	try
	{
		TCHAR tstrBuffer[100] = EMPTY_STRING;

		// check handle to *this* object
		if(m_hwndThis == NULL)
			return iReturn;

		// increment file count
		m_iFileCount++;

		// select appropriate message text
		_stprintf(tstrBuffer, _T("%ld"), m_iFileCount);

		// display message
		SetDlgItemText(m_hwndThis, IDC_LBLFILES, tstrBuffer);

		// if we made it here, return the file count
		iReturn = m_iFileCount;

		// hold for redraw every 20th item
		if(((float)m_iFileCount / 20.0) == 0.0f)
			doEvents();
	}
	catch(...)
	{
		// reset fail return val, jic...
		iReturn = -1;
	}

	// return current number or fail
	return iReturn;
}

/**
 * Increments the directory count and re-displays the information on *this*
 * dialog.
 *
 * @return -1 if an error occurs, otherwise the current directory count
 */
int CFileAttributesDialog::incrementDirectoryCount()
{
	int iReturn = -1;

	try
	{
		TCHAR tstrBuffer[100] = EMPTY_STRING;

		// check handle to *this* object
		if(m_hwndThis == NULL)
			return iReturn;

		// increment file count
		m_iDirectoryCount++;

		// select appropriate message text
		_stprintf(tstrBuffer, _T("%ld"), m_iDirectoryCount);

		// display message
		SetDlgItemText(m_hwndThis, IDC_LBLDIRECTORIES, tstrBuffer);

		// if we made it here, return the file count
		iReturn = m_iDirectoryCount;

		// hold for redraw every 20th item
		if(((float)m_iFileCount / 20.0) == 0.0f)
			doEvents();
	}
	catch(...)
	{
		// reset fail return val, jic...
		iReturn = -1;
	}

	// return current number or fail
	return iReturn;
}

/**
 * Waits until all painting (more or less) has been completed before
 * returning.
 *
 * @return 
 */
BOOL CFileAttributesDialog::doEvents()
{
	BOOL bReturn = TRUE;

	try
	{
		MSG msg;

		// Hold for re-draw
		while(PeekMessage(&msg, m_hwndThis, WM_PAINT, WM_PAINT, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch(...)
	{
		// set fail return val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}