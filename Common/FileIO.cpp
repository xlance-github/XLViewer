#include <stdafx.h>
#include <shlobj.h>

/*=============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2005 Chad R. Hearn - all rights reserved
'Purpose:   Retrieves the OS version numbers and sets the module vars,
'           m_bNewVersion - to reflect whether the OS is a newer version and
'           m_bUseUnicode - to reflect whether the unicode versions should be
'           used (for NT based OS's)
'
'Date:      05-25-2005, 07-31-2008
'			01-02-2009 completed CreateDirectoryStructure_9x() for backwards
'					   compatibilitiy - Chad R. Hearn	                                                                           	
'
'NOTES:     Per Microsoft, this function needs to be updated to use
'			GetVersionEx().
'============================================================================*/
BOOL IsWindows9x()
{
	DWORD dwReturn, dwIsNT;

	// using old function for now...
	dwReturn = GetVersion();

	if(dwReturn)
	{
		dwIsNT = ((dwReturn & 0x80000000) == 0);

		if(dwIsNT)
			return FALSE;
		else
			return TRUE;
	}
	else
		return TRUE;	// err on the side of caution...

}

/*=============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2005 Chad R. Hearn - all rights reserved
'Purpose:   Retrieves the OS version numbers and sets the module vars,
'           m_bNewVersion - to reflect whether the OS is a newer version and
'           m_bUseUnicode - to reflect whether the unicode versions should be
'           used (for NT based OS's)
'
'Date:      05-25-2005, 07-31-2008
'NOTES:     Per Microsoft, this function needs to be updated to use
'			GetVersionEx().
'============================================================================*/
BOOL IsWindowsNT()
{
	DWORD dwReturn, dwIsNT;

	// using old function for now...
	dwReturn = GetVersion();

	if(dwReturn)
	{
		dwIsNT = ((dwReturn & 0x80000000) == 0);

		if(dwIsNT)
			return TRUE;
		else
			return FALSE;
	}
	else
		return TRUE;	// err on the side of caution...

}

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
BOOL Exists(LPTSTR lpstrPath, BOOL bCreate = FALSE)
{
	HANDLE hFile;
	DWORD dwCreationDispositionFlags;
	
	// create disposition
    if(!bCreate)
		dwCreationDispositionFlags = OPEN_EXISTING;
	else
        dwCreationDispositionFlags = OPEN_ALWAYS;

    // attempt to open/create file
    hFile = CreateFile(lpstrPath, GENERIC_READ,
    				   FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                       dwCreationDispositionFlags,
					   FILE_ATTRIBUTE_NORMAL, NULL);

	// examine return value
    if(hFile == INVALID_HANDLE_VALUE)
        return FALSE;
    else
        CloseHandle(hFile);

	return TRUE;
}

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
HANDLE OpenFile_fileIO (LPTSTR lpstrFilename, BOOL bOpenForWriting,
						BOOL bAppend)
{
	HANDLE hFile = NULL;
	DWORD dwAccess, dwCreationDisp;
	
	if(bOpenForWriting)
	{
		dwAccess = GENERIC_WRITE;
		dwCreationDisp = CREATE_ALWAYS;
	}
	else
	{
		dwAccess = GENERIC_READ;
		dwCreationDisp = OPEN_EXISTING;
	}

	if(!bAppend)
	{
		// open file normally
		hFile = CreateFile(lpstrFilename, 
						   dwAccess, 
						   FILE_SHARE_READ,
						   NULL,
						   dwCreationDisp,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);
	}
	else
	{
		// for appends, attempt to open existing file...
		hFile = CreateFile(lpstrFilename, 
						   dwAccess, 
						   FILE_SHARE_READ,
						   NULL,
						   OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);
		// if the open existing failed, then create a new
		//   file for appending
		if(hFile == INVALID_HANDLE_VALUE)
			hFile = CreateFile(lpstrFilename, 
							   dwAccess, 
							   FILE_SHARE_READ,
							   NULL,
							   OPEN_ALWAYS,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL);
		// move to end of file for appending
		if(hFile != INVALID_HANDLE_VALUE)
			SetFilePointer(hFile, 0, 0, FILE_END);
	}

	return hFile;
}

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
BOOL FolderExists(LPTSTR lpstrFolder)
{
    TCHAR strCurrentFolder[MAX_PATH];
    TCHAR strRsvFolder[MAX_PATH];
    TCHAR strBuffer[MAX_PATH * 2];
    DWORD dwReturn = 0;

    // resolve folder... if an error occurs, use specified folder
    //   This is all because of Windows 9x systems...
    dwReturn = GetFullPathName(lpstrFolder, _countof(strBuffer), strBuffer, NULL);

    // check return value, set to resolved path if successful. If not, then
    //   set to the incoming folder.
    memset(strRsvFolder, 0, sizeof(strRsvFolder));
    if(dwReturn != 0)
        _stprintf(strRsvFolder, _T("%s"), strBuffer);
    else
        _stprintf(strRsvFolder, _T("%s"), lpstrFolder);

    // get current directory so we can set it back after "checking" to see
    //   if the requested folder exists
    dwReturn = GetCurrentDirectory(_countof(strCurrentFolder), strCurrentFolder);
    if(dwReturn != 0)
    {
        // check the requested folder
        dwReturn = SetCurrentDirectory(strRsvFolder);
        if(dwReturn != 0)
        {
            SetCurrentDirectory(strCurrentFolder);
            return TRUE;
        }
        else
            return FALSE;
    }
    else
        return FALSE;
}

BOOL GetFolder (LPTSTR lpstrFilename, LPTSTR lpstrFolder)
{
    int i;

    if(lpstrFilename)
    {
        for(i = _tcslen(lpstrFilename); i > 0; i--)
        {
            if(lpstrFilename[i] == _T('\\'))
            {
              _tcsncpy(lpstrFolder, lpstrFilename, i+1);
              return TRUE;
            }
            else if(lpstrFilename[i] == _T(':'))
            {
              _tcsncpy(lpstrFolder, lpstrFilename, i);
              lpstrFolder[i + 1] = _T('\\');
              return TRUE;
            }
        }
    }
    return FALSE;
}

/*=============================================================================
'Author(s): Chad R. Hearn
'Legal:     ©2004 Chad R. Hearn - all rights reserved
'Purpose:   Creates the entire directory structure, including all intermediate
'           directories, specified by strPath.
'
'Returns:   TRUE if the directory was created, otherwise FALSE is returned
'
'Date:      01-02-2009 - ported.
'NOTES:		This function is MUCH slower than its big brother
        	CreateDirectoryStructure(), but it is defined here so that we can
            maintain compatibility with older 32-bit systems.
'============================================================================*/
BOOL CreateDirectoryStructure_9x(TCHAR *strPath, BOOL bSilentOnErrors = FALSE)
{
	long lcv,						// loop control variable
         lCount;
    TCHAR strCreatePath[MAX_PATH],	// folder being created
         strBuffer[MAX_PATH],       // temporary buffer
         strCWD[MAX_PATH],			// temp, stores the current working dir
         							//	 prior to this function changing it
         *p;						// temporary pointer
    TCHAR **arstrDirs;				// array of path "pieces" being created

	// verify there is something to do
	if(strPath == NULL || lstrlen(strPath) == 0)
    	return FALSE;

	// Get current working directory and store. If it fails, set the current working
	//	 directory to nothing.
    GetCurrentDirectory(sizeof(strCWD), strCWD);

	// check path length - for an explanation of the "- 7," read Microsoft's
    //	 description of the create/change directory functions. Basically, this
    //	 ensures that any "additions" made by the API functions don't cause an
    //	 access violation.
    if(_tcslen(strPath) < (MAX_PATH - 7))
    {
    	// first, store a copy of strPath... strtok() alters the string it
        //	 searches and we don't want to blow away the path param
        _tcscpy(strBuffer, strPath);

    	// next, count path "pieces"...
        for(lCount = 0, p = _tcstok(strBuffer, _T("\\"));
            p;
            p = _tcstok(NULL, _T("\\")), lCount++);

        // now, create path "pieces" array with the proper dimensions...
        arstrDirs = new TCHAR*[lCount - 1];

        // re-copy the path
        lstrcpy(strBuffer, strPath);

    	// next, retrieve the actual pieces...
    	for(lcv = 0, p = _tcstok(strBuffer, _T("\\")); p; p = _tcstok(NULL, _T("\\")), lcv++)
        	arstrDirs[lcv] = p;

        // zero create path buffer...
        memset(strCreatePath, 0, sizeof(strCreatePath));

        // now, iterate through each piece constructing a path and attempting
        //	 to create it if it does not exist...
        for(lcv = 0; lcv < lCount; lcv++)
        {
        	if(arstrDirs[lcv] && lstrlen(arstrDirs[lcv]))
            {
            	// add new path "piece"
                lstrcat(strCreatePath, arstrDirs[lcv]);
                // attempt to change to this directory
                if(!SetCurrentDirectory(strCreatePath))
                {
                	// attempt to change failed so we need to create this
                    //	 directory
                    if(CreateDirectory(strCreatePath, NULL) == 0)
                    {
                    	// if any create call fails, then we cannot create
                        //	 the directory structure so return a failure
                        //	 value.
                        delete[] arstrDirs;
                        return FALSE;
                    }
                }
                // add trailing backslash so next "piece" can be added
                _tcscat(strCreatePath, _T("\\"));
            }
        }

    	// garbage collect
        delete[] arstrDirs;
    }

	// restore current working directory
    if(lstrlen(strCWD))
    	SetCurrentDirectory(strCWD);

	// if we make it here, return success!
	return TRUE;
}

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
BOOL CreateDirectoryStructure(TCHAR *strPath, BOOL bSilentOnErrors = FALSE)
{
	long lReturn;

	// check path length
	if(_tcslen(strPath) <= MAX_PATH)
	{
		if(!IsWindows9x())
		{
			lReturn = SHCreateDirectoryEx(NULL, strPath, NULL);
			if(lReturn != ERROR_SUCCESS && lReturn != ERROR_ALREADY_EXISTS)
				return FALSE;
			else
				return TRUE;
		}
		else
			return CreateDirectoryStructure_9x(strPath, bSilentOnErrors);
	}
	else
		return FALSE;
}

BOOL GetEXEVersion(TCHAR* strFilename, WORD& wMajor, WORD& wMinor,
	WORD& wRevision, WORD& wBuild)
{
	DWORD dwVersionInfoSize = (DWORD)0,
		  dwHandle = (DWORD)0;
	UINT uVersionBlock = (UINT)0;
	VOID *lpVersionData = NULL,
		 *lpVersionBlock = NULL;
	VS_FIXEDFILEINFO *lpFileInfo;

    // Init version return values
    wMajor = 0;
    wMinor = 0;
    wRevision = 0;
    wBuild = 0;

	// get size of version info structure - this will also return 0 if for
	//	 some reason the version information cannot be acquired
	dwVersionInfoSize = GetFileVersionInfoSize(strFilename, &dwHandle);
	if(dwVersionInfoSize == 0)
		return FALSE;

	lpVersionData = (VOID *)GlobalAlloc(GPTR, dwVersionInfoSize);
	if(lpVersionData == NULL)
		return FALSE;

	// attempt to get file version information
	if(GetFileVersionInfo(strFilename, NULL, dwVersionInfoSize,
		lpVersionData) == 0)
	{
		// free resources
        GlobalFree(lpVersionData);
		// exit
		return FALSE;
	}

	// attempt to query version info for version (Are we there yet???)
	VerQueryValue(lpVersionData, _T("\\"), &lpVersionBlock, &uVersionBlock);


	// Holy cow! This is rediculous... still more to go
	lpFileInfo = (VS_FIXEDFILEINFO *)lpVersionBlock;

	// set version components
    wMajor = HIWORD(lpFileInfo->dwFileVersionMS);
    wMinor = LOWORD(lpFileInfo->dwFileVersionMS);
    wRevision = HIWORD(lpFileInfo->dwFileVersionLS);
    wBuild = LOWORD(lpFileInfo->dwFileVersionLS);

	// free resources
	GlobalFree(lpVersionData);

    return TRUE;
}

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
BOOL RemoveFileFolder(TCHAR *strFileFolder, BOOL bFilesOnly = FALSE)
{
	SHFILEOPSTRUCT shfosRemove;
    long lReturn;
    TCHAR strRsvFileFolder[MAX_PATH];

    // transfer to buffer... for now
    _stprintf(strRsvFileFolder, _T("%s"), strFileFolder);

    // execute the op...
    if(!IsWindows9x())
    {
    	// zero structure
    	memset(&shfosRemove, 0, sizeof(shfosRemove));

        // initialize structure
        shfosRemove.hwnd = NULL;
        shfosRemove.wFunc = FO_DELETE;
        shfosRemove.pFrom = &strRsvFileFolder[0];
        shfosRemove.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
        if(bFilesOnly)
        	shfosRemove.fFlags |= FOF_FILESONLY;

        lReturn = SHFileOperation(&shfosRemove);
        if(lReturn != 0)
            return FALSE;
    }
    else
    {
        if(DeleteFile(strRsvFileFolder) != 0)
        	return TRUE;
    }

    // garbage collect, exit
	return TRUE;
}
