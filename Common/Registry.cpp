#include <stdafx.h>

#include <strsafe.h>

// registry ENUM for dev GUI
enum REGISTRY_BASEKEY_ENUM 
{
    CurrentUser = (long)HKEY_CURRENT_USER,
    CurrentConfig = (long)HKEY_CURRENT_CONFIG,
    LocalMachine = (long)HKEY_LOCAL_MACHINE,
    ClassesRoot = (long)HKEY_CLASSES_ROOT
};

LPVOID _MODREGISTRY_GetMessageText (long lErrorCode)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				  NULL,
				  lErrorCode,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				  (LPTSTR) &lpMsgBuf,
				  0,
				  NULL);
	return lpMsgBuf;
}

///////////////////////////////////////////////////////////////////////////////////////
// Get/Save functions
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Retrieves a DWORD value from the specified key in the registry. By
//          specifying TRUE for bDoNotCreate, the function will NOT create the key
//          if it doesn't exist.
//          
// Returns: The DWORD value if successful, otherwise 0 is returned
//          
// Date:    05/22/2006
// 
// NOTES:   
///////////////////////////////////////////////////////////////////////////////////////
DWORD GetRegistryNumeric (REGISTRY_BASEKEY_ENUM hBaseKey,
						  LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
						  LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
						  LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
						  BOOL bDoNotCreate)
{
	HKEY hkApp, hkSection;
	DWORD dwDisposition, dwType, dwcbData, dwReturn = (DWORD)0;
	LONG lReturn;

    //Attempt to open the specified key... If bDoNotCreate is specified, then use
    //  RegOpenKeyEx because it will fail if the key does not exist whereas RegCreateKeyEx
    //  will create it if it doesn't exist already.
	if(bDoNotCreate)
	{
		lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, NULL, KEY_READ, &hkApp);
		if(lReturn == ERROR_FILE_NOT_FOUND)
			return (DWORD)0;
	}
	else
		lReturn = RegCreateKeyEx((HKEY)hBaseKey, lpstrApp, 
		              0, _T("Class"), 
					  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
					  NULL, &hkApp, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryNumeric, application key"), MB_OK);
		GlobalFree(lpTemp);
		return (DWORD)0;
	}

	if(bDoNotCreate)
	{
		lReturn = RegOpenKeyEx(hkApp, lpstrSection, DWORD(0), KEY_READ, &hkSection);
		if(lReturn == ERROR_FILE_NOT_FOUND)
		{
			RegCloseKey(hkApp);
			return (DWORD)0;
        }
	}
	else
		lReturn = RegCreateKeyEx(hkApp, lpstrSection, 0, NULL, 
									REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
									NULL, &hkSection, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		RegCloseKey(hkApp);
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryNumeric, section key"), MB_OK);
		GlobalFree(lpTemp);
		return (DWORD)0;
	}

	//first get size of value...
	dwcbData = sizeof(DWORD);
	lReturn  = RegQueryValueEx(hkSection, lpstrKey, 0, &dwType, (LPBYTE)&dwReturn,
					   &dwcbData);

	if(lReturn != ERROR_SUCCESS && lReturn != ERROR_FILE_NOT_FOUND)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryNumeric, value"), MB_OK);
		GlobalFree(lpTemp);
	}

	//garbage collect, exit
	RegCloseKey(hkSection);
	RegCloseKey(hkApp);
	return dwReturn;
}

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Retrieves a DWORD value from the specified key in the registry. By
//          specifying TRUE for bDoNotCreate, the function will NOT create the key
//          if it doesn't exist.
//          
// Returns: The DWORD value if successful, otherwise 0 is returned
//          
// Date:    05/22/2006
// 
// NOTES:   
///////////////////////////////////////////////////////////////////////////////////////
DWORD GetRegistryNumeric (REGISTRY_BASEKEY_ENUM hBaseKey,
						  LPCTSTR lpstrApp,			 // root (i.e. "\Software\Companyname")
						  LPCTSTR lpstrSection,		 // section name (i.e. "\Preferences")
						  LPCTSTR lpstrKey,			 // value to be returned (i.e. "Top")
						  DWORD dwDefault = (DWORD)0,// default value to be returned
						  BOOL bDoNotCreate = TRUE)

{
	HKEY hkApp, hkSection;
	DWORD dwDisposition,
		  dwType,
		  dwcbData,
		  dwReturn;
	LONG lReturn;

	// Chad, come back and fix this... default value is ignored even when key
	//	 doesn't exist.

    //Attempt to open the specified key... If bDoNotCreate is specified, then use
    //  RegOpenKeyEx because it will fail if the key does not exist whereas RegCreateKeyEx
    //  will create it if it doesn't exist already.
	if(bDoNotCreate)
	{
		lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, NULL, KEY_READ, &hkApp);
		if(lReturn != ERROR_SUCCESS)
			return dwDefault;
	}
	else
		lReturn = RegCreateKeyEx((HKEY)hBaseKey, lpstrApp, 
		              0, _T("Class"), 
					  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
					  NULL, &hkApp, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryNumeric, application key"), MB_OK);
		GlobalFree(lpTemp);
		return dwDefault;
	}

	if(bDoNotCreate)
	{
		lReturn = RegOpenKeyEx(hkApp, lpstrSection, DWORD(0), KEY_READ, &hkSection);
		if(lReturn != ERROR_SUCCESS)
		{
			RegCloseKey(hkApp);
			return dwDefault;
        }
	}
	else
		lReturn = RegCreateKeyEx(hkApp, lpstrSection, 0, NULL, 
									REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
									NULL, &hkSection, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		RegCloseKey(hkApp);
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryNumeric, section key"), MB_OK);
		GlobalFree(lpTemp);
		return dwDefault;
	}

	//first get size of value...
	dwcbData = sizeof(DWORD);
	lReturn  = RegQueryValueEx(hkSection, lpstrKey, 0, &dwType, (LPBYTE)&dwReturn,
					   &dwcbData);

	if(lReturn != ERROR_SUCCESS)
	{
		if(lReturn != ERROR_FILE_NOT_FOUND)
		{
			LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
			MessageBox(NULL, lpTemp, _T("GetRegistryNumeric, value"), MB_OK);
			GlobalFree(lpTemp);
		}
		else
			dwReturn = dwDefault;
	}

	//garbage collect, exit
	RegCloseKey(hkSection);
	RegCloseKey(hkApp);
	return dwReturn;
}

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Retrieves a string value from the specified key in the registry. By
//          specifying TRUE for bDoNotCreate, the function will NOT create the key
//          key if it doesn't exist.
//          
// Returns: The string requested if successful, otherwise an empty string is
//          returned.
//          
// Date:    05/22/2006
// 
// NOTES:   
///////////////////////////////////////////////////////////////////////////////////////
LPTSTR GetRegistryString (REGISTRY_BASEKEY_ENUM hBaseKey,
						 LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					     LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
					     LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
					     BOOL bDoNotCreate)
{
	HKEY hkApp, hkSection;
	LPTSTR lpstrReturn = NULL;		// VERY important to init...
	LONG lReturn;
	DWORD dwDisposition, dwType, dwcbData, dwReturn;

	//Attempt to open the specified key... If bDoNotCreate is specified, then use
    //  RegOpenKeyEx because it will fail if the key does not exist whereas RegCreateKeyEx
    //  will create it if it doesn't exist already.
	if(bDoNotCreate)
	{
		lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, NULL, KEY_READ, &hkApp);
		if(lReturn == ERROR_FILE_NOT_FOUND)
			return NULL;
	}
	else
		lReturn = RegCreateKeyEx((HKEY)hBaseKey, lpstrApp, 
		              0, _T("Class"), 
					  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
					  NULL, &hkApp, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryString, application key"), MB_OK);
		GlobalFree(lpTemp);
		return NULL;
	}

	if(bDoNotCreate)
	{
		lReturn = RegOpenKeyEx(hkApp, lpstrSection, DWORD(0), KEY_READ, &hkSection);
		if(lReturn == ERROR_FILE_NOT_FOUND)
		{
			RegCloseKey(hkApp);
			return NULL;
		}
	}
	else
		lReturn = RegCreateKeyEx(hkApp, lpstrSection, 0, NULL, 
									REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
									NULL, &hkSection, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		RegCloseKey(hkApp);
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryString, section key"), MB_OK);
		GlobalFree(lpTemp);
		return NULL;
	}

	// First query the key and see how "large" the data is
	dwReturn = RegQueryValueEx(hkSection, lpstrKey, 0, &dwType, NULL, &dwcbData);
	
	// No error or the "expected" error, then allocate the memory for da string!
	if(dwReturn == ERROR_MORE_DATA || dwReturn == ERROR_SUCCESS)
	{
		lpstrReturn = (LPTSTR)GlobalAlloc(GPTR, dwcbData + sizeof(TCHAR));
    ZeroMemory(lpstrReturn, dwcbData + sizeof(TCHAR));
		if(RegQueryValueEx(hkSection, lpstrKey, 0, &dwType, (LPBYTE)lpstrReturn, &dwcbData) != ERROR_SUCCESS)
		{
			LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
			MessageBox(NULL, lpTemp, _T("GetRegistryString, value"), MB_OK);
			GlobalFree(lpTemp);
		}
	}

	//garbage collect, exit
	RegCloseKey(hkSection);
	RegCloseKey(hkApp);
	return lpstrReturn;
}

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Retrieves byte data from the specified key in the registry. By
//          specifying TRUE for bDoNotCreate, the function will NOT create the key
//          key if it doesn't exist.
//          
// Returns: TRUE if successful, otherwise FALSE is returned
//          
// Date:    05/22/2006
// 
// NOTES:   
///////////////////////////////////////////////////////////////////////////////////////
BOOL GetRegistryBinary (REGISTRY_BASEKEY_ENUM hBaseKey,
						LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					    LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
					    LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
						LPVOID lpvValue,		// buffer for value
						DWORD dwSize,			// size of buffer
					    BOOL bDoNotCreate,
						BOOL bDoNotInitializeBuffer)
{
	HKEY hkApp, hkSection;
	BOOL bReturn = FALSE;
	LONG lReturn;
	DWORD dwDisposition, dwType, dwcbData, dwReturn;
	
	// Init return value
	if(!bDoNotInitializeBuffer)
		ZeroMemory(lpvValue, dwSize);

	//Attempt to open the specified key... If bDoNotCreate is specified, then use
    //  RegOpenKeyEx because it will fail if the key does not exist whereas RegCreateKeyEx
    //  will create it if it doesn't exist already.
	if(bDoNotCreate)
		lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, NULL, KEY_READ, &hkApp);
	else
		lReturn = RegCreateKeyEx((HKEY)hBaseKey, lpstrApp, 
		              0, _T("Class"), 
					  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
					  NULL, &hkApp, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		//LPSTR lpTemp = (LPSTR)_MODREGISTRY_GetMessageText(lReturn);
		//MessageBox(NULL, lpTemp, "GetRegistryBinary, application key", MB_OK);
		//GlobalFree(lpTemp);
		return bReturn;
	}

	if(bDoNotCreate)
		lReturn = RegOpenKeyEx(hkApp, lpstrSection, DWORD(0), KEY_READ, &hkSection);
	else
		lReturn = RegCreateKeyEx(hkApp, lpstrSection, 0, NULL, 
									REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
									NULL, &hkSection, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		RegCloseKey(hkApp);
		//LPSTR lpTemp = (LPSTR)_MODREGISTRY_GetMessageText(lReturn);
		//MessageBox(NULL, lpTemp, "GetRegistryBinary, section key", MB_OK);
		//GlobalFree(lpTemp);
		return bReturn;
	}

	// First query the key and see how "large" the data is
	dwReturn = RegQueryValueEx(hkSection, lpstrKey, 0, &dwType, NULL, &dwcbData);
	
	// No error or the "expected" error, then allocate the memory for da string!
	if(dwReturn == ERROR_MORE_DATA || dwReturn == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hkSection, lpstrKey, 0, &dwType, 
							(LPBYTE)lpvValue, 
							(dwcbData < dwSize ? &dwcbData : &dwSize)) != ERROR_SUCCESS)
		{
			//LPSTR lpTemp = (LPSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
			//MessageBox(NULL, lpTemp, "GetRegistryBinary, value", MB_OK);
			//GlobalFree(lpTemp);
		}
        else
        	bReturn = TRUE;
	}
	else
		bReturn = TRUE;

	//garbage collect, exit
	RegCloseKey(hkSection);
	RegCloseKey(hkApp);
	return bReturn;
}

/**
 * Author:  Chad R. Hearn
 * Legal:   ©2009 Chad R. Hearn - all rights reserved
 * Purpose: Queries the specified value and returns its size.
 *
 * Return: 	Size of the buffer required to hold the value or -1 if an error
 * occurs
 *
 * NOTES:
 */
long GetRegistryValueSize(REGISTRY_BASEKEY_ENUM hBaseKey,
						  LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					      LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
					      LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
					      BOOL bDoNotCreate)
{
	HKEY hkApp, hkSection;
//	LPSTR lpstrReturn = NULL;		// VERY important to init...
	LONG lReturn = -1L;
	DWORD dwDisposition, dwType, dwcbData, dwReturn;

	//Attempt to open the specified key... If bDoNotCreate is specified, then use
    //  RegOpenKeyEx because it will fail if the key does not exist whereas RegCreateKeyEx
    //  will create it if it doesn't exist already.
	if(bDoNotCreate)
	{
		lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, NULL, KEY_READ, &hkApp);
		if(lReturn == ERROR_FILE_NOT_FOUND)
			return -1L;
	}
	else
		lReturn = RegCreateKeyEx((HKEY)hBaseKey, lpstrApp, 
		              0, _T("Class"), 
					  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
					  NULL, &hkApp, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryString, application key"), MB_OK);
		GlobalFree(lpTemp);
		return -1L;
	}

	if(bDoNotCreate)
	{
		lReturn = RegOpenKeyEx(hkApp, lpstrSection, DWORD(0), KEY_READ, &hkSection);
		if(lReturn == ERROR_FILE_NOT_FOUND)
		{
			RegCloseKey(hkApp);
			return -1L;
		}
	}
	else
		lReturn = RegCreateKeyEx(hkApp, lpstrSection, 0, NULL, 
									REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
									NULL, &hkSection, &dwDisposition);

	if(lReturn != ERROR_SUCCESS)
	{
		RegCloseKey(hkApp);
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("GetRegistryString, section key"), MB_OK);
		GlobalFree(lpTemp);
		return -1L;
	}

	// Finally, query the key and see how "large" the data is
	dwReturn = RegQueryValueEx(hkSection, lpstrKey, 0, &dwType, NULL, &dwcbData);

    if(dwReturn == ERROR_SUCCESS)
    	lReturn = dwcbData;

	// return value
    return lReturn;
}

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Stores a DWORD value under the specified key in the registry.
//          
//          
// Returns: TRUE if the value was written to the registry, otherwise FALSE
//          is returned.
//          
// Date:    05/22/2006
// 
// NOTES:   
///////////////////////////////////////////////////////////////////////////////////////
BOOL SaveRegistryNumeric (REGISTRY_BASEKEY_ENUM hBaseKey,
						  LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					      LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
					      LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
					      DWORD   dwValue,	    // value to be stored (DWORD in this case)
						  BOOL bCreate)
{
	HKEY hkApp, hkSection;
	BOOL bReturn = FALSE;
	LONG lReturn;
	DWORD dwDisposition;
	DWORD cbData = sizeof(DWORD);
	
    //Attempt to open the specified key... If bCreate is FALSE, then use RegOpenKeyEx
    //  because it will fail if the key does not exist whereas RegCreateKeyEx
    //  will create it if it doesn't exist already.
	//    Primary key
	if(bCreate)
		lReturn = RegCreateKeyEx((HKEY)hBaseKey, lpstrApp, 0, _T("Class"), 
								 REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
								 NULL, &hkApp, &dwDisposition);
	else
	{
		lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, (DWORD)0, KEY_WRITE, &hkApp);
		if(lReturn == ERROR_FILE_NOT_FOUND)
			return bReturn;
	}

	if(lReturn != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryNumeric, application key"), MB_OK);
		GlobalFree(lpTemp);
		return bReturn;
	}

	//   Subkey
	if(bCreate)
		lReturn = RegCreateKeyEx(hkApp, lpstrSection, 
								 0, _T("Class"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
								 NULL, &hkSection, &dwDisposition);
	else
	{
		lReturn = RegOpenKeyEx(hkApp, lpstrSection, (DWORD)0, KEY_WRITE, &hkSection);
		if(lReturn == ERROR_FILE_NOT_FOUND)
		{
			RegCloseKey(hkApp);
			return bReturn;
		}
	}
	
	if(lReturn != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryNumeric, section key"), MB_OK);
		GlobalFree(lpTemp);
		return bReturn;
	}

	if(RegSetValueEx(hkSection, lpstrKey, 0, REG_DWORD, (LPBYTE)&dwValue, cbData) != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryNumeric, value"), MB_OK);
		GlobalFree(lpTemp);
	}
	else
		bReturn = TRUE;

	//garbage collect, exit
	RegCloseKey(hkSection);
	RegCloseKey(hkApp);
	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Stores a string value under the specified key in the registry.
//
//          
// Returns: TRUE if the value was written to the registry, otherwise FALSE
//          is returned.         
//          

// Date:    05/22/2006
// 
// NOTES:   
///////////////////////////////////////////////////////////////////////////////////////
BOOL SaveRegistryString (REGISTRY_BASEKEY_ENUM hBaseKey,
						 LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					     LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
					     LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
					     LPCTSTR lpstrValue,    // pointer to value to be stored (string in this case)
						 BOOL bCreate)
{
	HKEY hkApp, hkSection;
	LONG lReturn;
	BOOL bReturn = FALSE;
	DWORD dwDisposition;

  size_t length_bytes = 0;
  assert( SUCCEEDED( StringCbLength( lpstrValue, STRSAFE_MAX_CCH * sizeof(TCHAR), &length_bytes ) ) );
  
  DWORD cbData = length_bytes;
	
    //Attempt to open the specified key... If bCreate is FALSE, then use RegOpenKeyEx
    //  because it will fail if the key does not exist whereas RegCreateKeyEx
    //  will create it if it doesn't exist already.
	//    Primary key
	if(bCreate)
		lReturn = RegCreateKeyEx((HKEY)hBaseKey, lpstrApp, 0, _T("Class"), 
								 REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
								 NULL, &hkApp, &dwDisposition);
	else
	{
		lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, (DWORD)0, KEY_WRITE, &hkApp);
		if(lReturn == ERROR_FILE_NOT_FOUND)
        	return bReturn;
	}

	if(lReturn != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryString, application key"), MB_OK);
		GlobalFree(lpTemp);
		return bReturn;
	}
	
	//   Subkey
	if(bCreate)
		lReturn = RegCreateKeyEx(hkApp, lpstrSection, 
								 0, _T("Class"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
								 NULL, &hkSection, &dwDisposition);
	else
	{
		lReturn = RegOpenKeyEx(hkApp, lpstrSection, (DWORD)0, KEY_WRITE, &hkSection);
		if(lReturn == ERROR_FILE_NOT_FOUND)
		{
			RegCloseKey(hkApp);
			return bReturn;
		}
	}

	if(lReturn != ERROR_SUCCESS)
	{
		RegCloseKey(hkApp);
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryString, section key"), MB_OK);
		GlobalFree(lpTemp);
		return bReturn;
	}

    // If a NULL pointer has been passed for the string, then set the buffer
    //	 size to 0.
    if(lpstrValue == NULL)
    	cbData = 0;
	if(RegSetValueEx(hkSection, lpstrKey, 0, REG_SZ, (LPBYTE)lpstrValue, cbData) != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryString, value"), MB_OK);
		GlobalFree(lpTemp);
	}
	else
		bReturn = TRUE;

	//garbage collect, exit
	RegCloseKey(hkSection);
	RegCloseKey(hkApp);
	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Stores the data pointed to by lpvValue under the specified key in the 
//			registry.
//          
// Returns:	TRUE if no errors are encountered, otherwise FALSE is returned         
//          
// Date:    05/22/2006
// 
// NOTES:   
///////////////////////////////////////////////////////////////////////////////////////
BOOL SaveRegistryBinary (REGISTRY_BASEKEY_ENUM hBaseKey,
						 LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
						 LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
						 LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
						 LPVOID  lpvValue,		// pointer to value to be stored (binary)
						 DWORD dwSize,			// sizeof() data pointed to by lpvValue
						 BOOL bCreate)
{
	HKEY hkApp, hkSection;
	LONG lReturn;
	BOOL bReturn = FALSE;
	DWORD dwDisposition;
	DWORD cbData = dwSize;
	
    //Attempt to open the specified key... If bCreate is FALSE, then use RegOpenKeyEx
    //  because it will fail if the key does not exist whereas RegCreateKeyEx
    //  will create it if it doesn't exist already.
	//    Primary key
	if(bCreate)
		lReturn = RegCreateKeyEx((HKEY)hBaseKey, lpstrApp, 0, _T("Class"), 
								 REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
								 NULL, &hkApp, &dwDisposition);
	else
		lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, (DWORD)0, KEY_WRITE, &hkApp);

	if(lReturn != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryBinary, application key"), MB_OK);
		GlobalFree(lpTemp);
		return bReturn;
	}
	
	//   Subkey
	if(bCreate)
		lReturn = RegCreateKeyEx(hkApp, lpstrSection, 
								 0, _T("Class"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
								 NULL, &hkSection, &dwDisposition);
	else
		lReturn = RegOpenKeyEx(hkApp, lpstrSection, (DWORD)0, KEY_WRITE, &hkSection);

	if(lReturn != ERROR_SUCCESS)
	{
		RegCloseKey(hkApp);
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryBinary, section key"), MB_OK);
		GlobalFree(lpTemp);
		return bReturn;
	}

	if(RegSetValueEx(hkSection, lpstrKey, 0, REG_BINARY, (LPBYTE)lpvValue, cbData) != ERROR_SUCCESS)
	{
		LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
		MessageBox(NULL, lpTemp, _T("SaveRegistryBinary, value"), MB_OK);
		GlobalFree(lpTemp);
	}
	else
		bReturn = TRUE;

	//garbage collect, exit
	RegCloseKey(hkSection);
	RegCloseKey(hkApp);
	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Saves a date to the registry. This function is basically a wrapper... it
//			calls SaveRegistryBinary().
//
//          
// Returns:	TRUE if no errors are encountered, otherwise FALSE is returned          
//          
// Date:    05/22/2006
// 
// NOTES:   Fixed size is 8 bytes. This is based on the VB Date data type
///////////////////////////////////////////////////////////////////////////////////////
BOOL SaveRegistryDate (REGISTRY_BASEKEY_ENUM hBaseKey,
					   LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					   LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
					   LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
					   LPVOID  lpvValue,		// pointer to value to be stored (time in this case)
					   BOOL bCreate)
{
	return SaveRegistryBinary(hBaseKey, lpstrApp, lpstrSection, lpstrKey,
							  lpvValue, 8, bCreate);
}

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Saves a float to the registry. This function is basically a wrapper... it
//			calls SaveRegistryBinary().
//
//          
// Returns:	TRUE if no errors are encountered, otherwise FALSE is returned          
//          
// Date:    05/22/2006
// 
// NOTES:   Fixed size is 8 bytes. This is based on the VB Date data type
///////////////////////////////////////////////////////////////////////////////////////
BOOL SaveRegistryFloat (REGISTRY_BASEKEY_ENUM hBaseKey,
					    LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					    LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
					    LPCTSTR lpstrKey,		// value to be returned (i.e. "Top")
					    float *lpfValue,		// pointer to value to be stored (float in this case)
					    BOOL bCreate)
{
	return SaveRegistryBinary(hBaseKey, lpstrApp, lpstrSection, lpstrKey,
							  lpfValue, sizeof(FLOAT), bCreate);
}


///////////////////////////////////////////////////////////////////////////////////////
// Extended functionality functions
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Author:  Chad R. Hearn
// Legal:   ©2006 Chad R. Hearn - all rights reserved
// Purpose: Deletes the value from the registry key specified
//          
// Returns: TRUE if the value was deleted, otherwise FALSE is returned
//          
// Date:    05/22/2006
// 
// NOTES:   
///////////////////////////////////////////////////////////////////////////////////////
BOOL DeleteRegistryValue(REGISTRY_BASEKEY_ENUM hBaseKey,
						 LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					     LPCTSTR lpstrSection,	// section name (i.e. "\Preferences")
						 LPCTSTR lpstrKey,		// value to be deleted (i.e. "Top")
						 BOOL bIgnoreNonExistentKeyError = TRUE)
{
	HKEY hkApp, hkSection;
	LONG lReturn;
	BOOL bReturn = FALSE;

	//Open key. At a minimum, the key must be opened with the KEY_SET_VALUE flag.	
	lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, (DWORD)0, KEY_WRITE, &hkApp);
	if(lReturn != ERROR_SUCCESS)
	{
		if((lReturn == ERROR_FILE_NOT_FOUND && !bIgnoreNonExistentKeyError) ||
			lReturn != ERROR_FILE_NOT_FOUND)
		{
			LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
			MessageBox(NULL, lpTemp, _T("DeleteRegistryValue, application key"), MB_OK);
			GlobalFree(lpTemp);
		}
		return bReturn;
	}

	//   Subkey
	lReturn = RegOpenKeyEx(hkApp, lpstrSection, (DWORD)0, KEY_WRITE, &hkSection);

	if(lReturn != ERROR_SUCCESS)
	{
		if((lReturn == ERROR_FILE_NOT_FOUND && !bIgnoreNonExistentKeyError) ||
			lReturn != ERROR_FILE_NOT_FOUND)
		{
			LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
			MessageBox(NULL, lpTemp, _T("DeleteRegistryValue, section key"), MB_OK);
			GlobalFree(lpTemp);
		}
		RegCloseKey(hkApp);
		return bReturn;
	}
	
    //Kill it!
    lReturn = RegDeleteValue(hkSection, lpstrKey);
    
	//garbage collect, exit
	RegCloseKey(hkSection);
	RegCloseKey(hkApp);

	return (lReturn == ERROR_SUCCESS ? TRUE : FALSE);
}

///////////////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn
// Legal:     ©2011 Chad R. Hearn - all rights reserved
// Purpose:   Deletes the specified key from the registry
//
//
// Returns:   TRUE if the key is deleted successfully, otherwise FALSE
//
// Date:      04-10-2009
// NOTES:
///////////////////////////////////////////////////////////////////////////////////////
BOOL DeleteRegistryKey(REGISTRY_BASEKEY_ENUM hBaseKey,
					   LPCTSTR lpstrApp,		// root (i.e. "\Software\Companyname")
					   LPCTSTR lpstrSection,	// section or key to be deleted
                       BOOL bIgnoreNonExistentKeyError = TRUE)
{
	HKEY hkApp;
	LONG lReturn;
	BOOL bReturn = FALSE;

	//Open key. At a minimum, the key must be opened with the KEY_SET_VALUE flag.	
	lReturn = RegOpenKeyEx((HKEY)hBaseKey, lpstrApp, (DWORD)0, KEY_WRITE, &hkApp);
	if(lReturn != ERROR_SUCCESS)
	{
		if((lReturn == ERROR_FILE_NOT_FOUND && !bIgnoreNonExistentKeyError) ||
			lReturn != ERROR_FILE_NOT_FOUND)
		{
			LPTSTR lpTemp = (LPTSTR)_MODREGISTRY_GetMessageText(lReturn); // GetLastError());
			MessageBox(NULL, lpTemp, _T("DeleteRegistryKey, application key"), MB_OK);
			GlobalFree(lpTemp);
		}
		return bReturn;
	}

    //Kill it!
    lReturn = RegDeleteKey(hkApp, lpstrSection);
    
	//garbage collect, exit
	RegCloseKey(hkApp);

	return (lReturn == ERROR_SUCCESS ? TRUE : FALSE);
}