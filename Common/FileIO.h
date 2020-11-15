#ifndef _MODFILEIOH_
#define _MODFILEIOH_

#include <shlobj.h>

/*=============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2011 Chad R. Hearn - all rights reserved
'Purpose:   Retrieves the OS version numbers and sets the module vars,
'           m_bNewVersion - to reflect whether the OS is a newer version and
'           m_bUseUnicode - to reflect whether the unicode versions should be
'           used (for NT based OS's)
'
'Date:      05-25-2005, 07-31-2008
'NOTES:     Per Microsoft, this function needs to be updated to use
'			GetVersionEx().
'============================================================================*/
BOOL IsWindows9x();

/*=============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2011 Chad R. Hearn - all rights reserved
'Purpose:   Retrieves the OS version numbers and sets the module vars,
'           m_bNewVersion - to reflect whether the OS is a newer version and
'           m_bUseUnicode - to reflect whether the unicode versions should be
'           used (for NT based OS's)
'
'Date:      05-25-2005, 07-31-2008
'NOTES:     Per Microsoft, this function needs to be updated to use
'			GetVersionEx().
'============================================================================*/
BOOL IsWindowsNT();

/*'==============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2004 Chad R. Hearn - all rights reserved
'Purpose:   Verifies whether a file exists or not
'
'Returns:   TRUE if the specified file exists, otherwise FALSE is returned
'
'Date:      06-08-2004
'NOTES:
'==============================================================================*/
BOOL Exists(LPTSTR lpstrPath, BOOL bCreate = FALSE);

/*'==============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2006 Chad R. Hearn - all rights reserved
'Purpose:   Opens the specified file
'
'Returns:   The file handle if successful, otherwise INVALID_HANDLE_VALUE 
'			is returned
'
'Date:      09-29-2006
'NOTES:		Does not check to see if the file exists
'==============================================================================*/
HANDLE OpenFile_fileIO (LPTSTR lpstrFilename, BOOL bOpenForWriting, BOOL bAppend);

//==============================================================================
//Author(s): Chad R. Hearn
//Legal:     ©2008 Chad R. Hearn - all rights reserved
//Purpose:   Checks to see if the specified folder exists.
//
//
//Returns:   TRUE if the folder exists, otherwise FALSE is returned
//
//Date:      02-18-2008
//NOTES:
//==============================================================================
BOOL FolderExists(LPTSTR lpstrFolder);


BOOL GetFolder (LPTSTR lpstrFilename, LPTSTR lpstrFolder);

/*=============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2004 Chad R. Hearn - all rights reserved
'Purpose:   Creates the entire directory structure, including all intermediate
'           directories, specified by strPath.
'
'Returns:   TRUE if the directory was created, otherwise FALSE is returned
'
'Date:      12-14-2004
'NOTES:
'============================================================================*/
BOOL CreateDirectoryStructure_9x(TCHAR *strPath, BOOL bSilentOnErrors = FALSE);

/*=============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2004 Chad R. Hearn - all rights reserved
'Purpose:   Creates the entire directory structure, including all intermediate
'           directories, specified by strPath. This version of the function
'           requires a minimum of Windows ME or Windows 2000.
'
'Returns:   TRUE if the path is created, otherwise FALSE is returned
'
'Date:      12-14-2004
'NOTES:     This function provides no feedback when an error occurs as far as
'           WHY it occurred.
'============================================================================*/
BOOL CreateDirectoryStructure(TCHAR *strPath, BOOL bSilentOnErrors = FALSE);


BOOL GetEXEVersion(TCHAR* strFilename, WORD& wMajor, WORD& wMinor,
	WORD& wRevision, WORD& wBuild);

/*=============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2006 Chad R. Hearn - all rights reserved
'Purpose:   Removes any file or folder specified by strFileFolder. If a
'           folder is specified, the folder is removed recursively.
'
'
'Returns:   TRUE if no errors were encountered, otherwise FALSE is returned
'
'Date:      05-10-2006
'NOTES:
'============================================================================*/
BOOL RemoveFileFolder(TCHAR *strFileFolder, BOOL bFilesOnly = FALSE);

#endif // end FileIO module
