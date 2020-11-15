#ifndef _MODREGISTRYH_
#define _MODREGISTRYH_

#include <windows.h>

// registry ENUM for dev GUI
enum REGISTRY_BASEKEY_ENUM
{
    CurrentUser = (long)HKEY_CURRENT_USER,
    CurrentConfig = (long)HKEY_CURRENT_CONFIG,
    LocalMachine = (long)HKEY_LOCAL_MACHINE,
    ClassesRoot = (long)HKEY_CLASSES_ROOT
};

LPVOID _MODREGISTRY_GetMessageText (long lErrorCode);

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
						  BOOL bDoNotCreate);

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
						  BOOL bDoNotCreate = TRUE);

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
					      BOOL bDoNotCreate);

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
					     BOOL bDoNotCreate);

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
						BOOL bDoNotInitializeBuffer = FALSE);

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
						  BOOL bCreate);

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
						 BOOL bCreate);

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
						 BOOL bCreate);

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
					   BOOL bCreate);

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
					    BOOL bCreate);

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
						 BOOL bIgnoreNonExistentKeyError = TRUE);


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
                       BOOL bIgnoreNonExistentKeyError = TRUE);

#endif	// end _MODREGISTRYH_ define
