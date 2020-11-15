///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor     
// Purpose:   CAttachedDrives object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include <windows.h>
#include <TChar.h>
#include "..\XLanceView.h"
#include "..\Common\Registry.h"
#include "CAttachedDrives.h"

///////////////////////////////////////////////////////////////////////////////
// Object constants
///////////////////////////////////////////////////////////////////////////////

#define MAX_DRIVEOBJECTS		26	// Max number of allowed connected drives

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor, initializes all fields to their defaults.
 */
CAttachedDrives::CAttachedDrives()
{
	// initialize fields to their defaults
	m_ardrvinfThis = NULL;
	m_strLastError = EMPTY_STRING;
	m_iDriveCount = 0;

	// get attached drives
	getAvailableDrives();
}
                                 
/**
 * Destructor, performs clean-up on fields.
 */
CAttachedDrives::~CAttachedDrives()
{
	// destroy drive information array
	if(m_ardrvinfThis)
	{
		LPDRIVEINFORMATION lpdrvinfTemp = NULL;
		
		// destroy items in array
		for(int i = 0; i < MAX_DRIVEOBJECTS; i++)
		{
			// get current item
			lpdrvinfTemp = m_ardrvinfThis[i];

			// destroy item
			if(lpdrvinfTemp != NULL)
				delete lpdrvinfTemp;
		}

		// destroy array
		delete[] m_ardrvinfThis;
		m_ardrvinfThis = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Returns a pointer to the internal drive object at the specified index.
 *
 * @param iIndex the index of the drive information object to be retrieved.
 *
 * @return the drive information object if successful, otherwise NULL.
 */
const LPDRIVEINFORMATION CAttachedDrives::driveInformation(int iIndex)
{
	LPDRIVEINFORMATION lpdrvinfReturn = NULL;

	try
	{
		// check drive object array
		if(m_ardrvinfThis == NULL)
			return lpdrvinfReturn;

		// check count
		if(m_iDriveCount == 0)
			return lpdrvinfReturn;

		// check index
		if(iIndex < 0 || iIndex > (m_iDriveCount - 1))
			return lpdrvinfReturn;

		// retrieve object
		lpdrvinfReturn = m_ardrvinfThis[iIndex];
	}
	catch(...)
	{	
		// set last error
		m_strLastError = _T("While retrieving the drive information object, an unexpected error occurred.");
		
		// reset fail val
		lpdrvinfReturn = NULL;
	}

	// clear last error, if applicable
	if(lpdrvinfReturn)
		m_strLastError = EMPTY_STRING;

	// return pointer
	return lpdrvinfReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Manages the identification and retrieval of information about all currently
 * attached drives.
 * 
 * @note The currently information retrieved for each drive is as follows:
 * 
 *		Type (removable, fixed, CD, etc.)
 *		File System (NTFS, FAT, CDFS, etc.)
 *		Approximated type (Floppy, Hard disk, USB, etc.)
 *
 * @return TRUE if no errors occur, otherwise FALSE.
 */
BOOL CAttachedDrives::getAvailableDrives()
{
	TCHAR tstrLogicalDrives[MAX_PATH] = EMPTY_STRING,
		  *pcDriveType = NULL,
		  *pcFileSystem = NULL,
		  *pcDriveInformationEx = NULL; 
	BOOL bReturn = FALSE;	// default to pesimistic return val

	try
	{
		DWORD dwReturn = (DWORD)0,
			  dwLength = (DWORD)0;
		tstring strTemp = EMPTY_STRING;

		// attempt to get drive strings...
		dwLength = sizeof(tstrLogicalDrives);
		dwReturn = GetLogicalDriveStrings(dwLength, tstrLogicalDrives);

		// validate, continue
		if(dwReturn != (DWORD)0 && dwReturn < (dwLength + 1))
		{
			TCHAR *pc = NULL;

			// attempt to create drive object array
			m_ardrvinfThis = new DRIVEINFORMATION*[MAX_DRIVEOBJECTS];
			if(m_ardrvinfThis == NULL)
			{
				// set last error
				m_strLastError = _T("Could not allocate space for drive objects.");

				// return fail val
				return bReturn;
			}

			// initialize array
			for(int i = 0; i < MAX_DRIVEOBJECTS; i++)
			{
				// NULL current object
				m_ardrvinfThis[i] = NULL;
			}

			// Iterate through returned drives... the returned string
			//	 is formatted as a series of null delimited strings.
			//	 There has to be an easier way to do this, but I'll
			//	 be buggered if I can remember how.
			for(pc = &tstrLogicalDrives[0], m_iDriveCount = 0;
				*pc;
				)
			{
				// get drive information
				if(getDriveInformation(pc, pcDriveType, pcFileSystem,
				   pcDriveInformationEx))
				{
					LPDRIVEINFORMATION lpdrvinfTemp = NULL;

					// create new drive info struct...
					lpdrvinfTemp = new DRIVEINFORMATION();

					// validate, continue
					if(lpdrvinfTemp != NULL)
					{
						// assign to array
						m_ardrvinfThis[m_iDriveCount] = lpdrvinfTemp;

						// assign members
						lpdrvinfTemp->tchDriveLetter = pc[0];
						if(pcDriveType)
							_tcscpy(lpdrvinfTemp->tstrDriveType, pcDriveType);
						if(pcFileSystem)
							_tcscpy(lpdrvinfTemp->tstrFileSystem, pcFileSystem);
						if(pcDriveInformationEx)
							_tcscpy(lpdrvinfTemp->tstrMediaType, pcDriveInformationEx);

						// increment count (aka index)
						m_iDriveCount++;
					}

					// destroy buffers for reuse
					if(pcDriveType)
					{
						delete[] pcDriveType;
						pcDriveType = NULL;
					}
					if(pcFileSystem)
					{
						delete[] pcFileSystem;
						pcFileSystem = NULL;
					}
					if(pcDriveInformationEx)
					{
						delete[] pcDriveInformationEx;
						pcDriveInformationEx = NULL;
					}
				}
				
				// increment to next string
        pc = pc + (_tcslen(pc) + 1);
			}

			// If we made it here, return success
			bReturn = TRUE;
		}
		else
		{
			// display error message
			strTemp = _T("Could not retrieve information for the currently attached drives.\r\n");
			//writeLogEntry((TCHAR *)strTemp.data());
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While loading the available drives, an exception occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// perform final garbage collection
	if(pcDriveType)
		delete[] pcDriveType;
	if(pcFileSystem)
		delete[] pcFileSystem;
	if(pcDriveInformationEx)
		delete[] pcDriveInformationEx;

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}


/**
 * Retrieves the type, file system, and extended information for the
 * specified drive.
 *
 * @param tstrDriveSpec the drive whose information should be retrieved.
 * The drive spec MUST BE in one of these formats: "[Drive]:" or "[Drive]:\"
 *
 * @param tstrDriveType
 *
 * @param tstrFileSystem
 *
 * @param tstrExtendedInformation
 *
 * @return TRUE if the drive information is retrieved an no errors occurred,
 * otherwise FALSE.
 */
BOOL CAttachedDrives::getDriveInformation(TCHAR *tstrDriveSpec, 
	TCHAR *&tstrDriveType, TCHAR *&tstrFileSystem, 
	TCHAR *&tstrExtendedInformation)
{
	BOOL bReturn = FALSE;	// default to pesimistic return val

	try
	{
		TCHAR tstrBuffer[MAX_PATH];	
		WORD wDriveType = (WORD)0;
		DWORD dwFileSystemFlags = (DWORD)0,
			  dwMaxComponentSize = (DWORD)0;
		tstring strTemp = EMPTY_STRING;

		// validate drive spec
		if(lstrlen(tstrDriveSpec) == 0)
		{
			// set last error
			m_strLastError = _T("The specified drive spec is invalid.");

			// return fail val
			return bReturn;
		}
		if(lstrlen(tstrDriveSpec) < 2)
		{
			// set last error
			m_strLastError = _T("The specified drive spec appears to be in the wrong format.");
			m_strLastError += _T(" The drive spec should look like 'C:\\' or 'C:'");

			// return fail val
			return bReturn;			
		}
		// This MUST BE done separate from the check above in case element '1'
		//	 DOES NOT exist.
		if(tstrDriveSpec[1] != ':')
		{
			// set last error
			m_strLastError = _T("The specified drive spec appears to be in the wrong format.");
			m_strLastError += _T(" The drive spec should look like 'C:\\' or 'C:'");

			// return fail val
			return bReturn;
		}

		// validate out pointers
		if(tstrDriveType || tstrFileSystem || tstrExtendedInformation)
		{
			// set last error
			m_strLastError = _T("Could not retrieve information for the drive: '");
			m_strLastError += tstrDriveSpec;
			m_strLastError += _T("'. The drive type, file system, and/or extended information parameters contained data.");

			// return fail val
			return bReturn;
		}
		
		// First, get drive type... "Where we go from here is up to you."
		wDriveType = GetDriveType(tstrDriveSpec);

		// Validate drive type
		if(wDriveType == DRIVE_UNKNOWN)
		{
			// set last error
			m_strLastError = _T("Could not retrieve the drive type for drive: '");
			m_strLastError += tstrDriveSpec;
			m_strLastError += _T("'.");

			// return fail val
			return bReturn;
		}

		// Next, get file system information... If this fails, then check and see
		//	 if the drive is removable or not. If it isn't, then we MUST return 
		//	 fail. The file system is used to guess certain other information about
		//	 the drive.
		memset(tstrBuffer, 0, sizeof(tstrBuffer));
		if(!GetVolumeInformation(tstrDriveSpec, NULL, (DWORD)0, NULL, 
			&dwMaxComponentSize, &dwFileSystemFlags, tstrBuffer, 
			sizeof(tstrBuffer)))
		{
			if(wDriveType != DRIVE_REMOVABLE && wDriveType != DRIVE_CDROM)
			{
				// set last error
				m_strLastError = _T("Could not retrieve the file system information for drive: '");
				m_strLastError += tstrDriveSpec;
				m_strLastError += _T("'.");

				// return fail val
				return bReturn;
			}
			else
				lstrcpy(tstrBuffer, FILESYSTEM_DESCRIPTION_NOMEDIA);
		}

		// If the buffer has data, then set the file system out param
		if(lstrlen(tstrBuffer))
		{
			// Set File System out param
			tstrFileSystem = new TCHAR[lstrlen(tstrBuffer) + 1];
			if(tstrFileSystem == NULL)
			{
				// set last error
				m_strLastError = _T("Could not allocate space for file system name.");

				// return fail val
				return bReturn;
			}
			lstrcpy(tstrFileSystem, tstrBuffer);
		}

		// Make an educated guess as to the drive type...
		if(wDriveType == DRIVE_CDROM)
		{
			// Check File System for final tests...
			if(lstrcmp(tstrFileSystem, FILESYSTEM_DESCRIPTION_NOMEDIA) == 0)
			{
				// combine DVD and CDROM if we have no other information
				//	 to determine the type by
				strTemp = DRIVETYPE_DESCRIPTION_DVD;
				strTemp += _T("/");
				strTemp += DRIVETYPE_DESCRIPTION_CDROM;
			}
			else
			{
				if(lstrcmp(tstrFileSystem, FILESYSTEM_DESCRIPTION_UDF))
					strTemp = DRIVETYPE_DESCRIPTION_CDROM;
				else
					strTemp = DRIVETYPE_DESCRIPTION_DVD;
			}
		}
		else if(wDriveType == DRIVE_REMOVABLE)
		{
			// Attempt to determine with complete certainty whether this is a 
			//	 floppy drive or not
			if(!isFloppyDrive(tstrDriveSpec))
				strTemp = DRIVETYPE_DESCRIPTION_REMOVABLE;
			else
				strTemp = DRIVETYPE_DESCRIPTION_FLOPPY;
		}
		else if(wDriveType == DRIVE_REMOTE)
			strTemp = DRIVETYPE_DESCRIPTION_REMOTE;
		else if(wDriveType == DRIVE_FIXED)
			strTemp = DRIVETYPE_DESCRIPTION_FIXED;

		// attempt to set drive type return param
		if(strTemp.length())
		{
			//TCHAR tstrPadding[DRIVETYPE_MAX_DESCRIPTIONLENGTH + 1] = EMPTY_STRING;

			//// create padding
			//memset(tstrPadding, DRIVETYPE_PADDING_CHARACTER, 
			//	DRIVETYPE_MAX_DESCRIPTIONLENGTH); 

			//// add padding and truncate at max length
			//strTemp += tstrPadding;
			//strTemp[DRIVETYPE_MAX_DESCRIPTIONLENGTH] = 0;

			// attempt to allocate space
			tstrDriveType = new TCHAR[strTemp.length() + 1];

			// check
			if(tstrDriveType == NULL)
			{
				// set last error
				m_strLastError = _T("Could not allocate space for drive type.");

				// return fail val
				return bReturn;
			}
			lstrcpy(tstrDriveType, (TCHAR *)strTemp.data());
		}

		// If we made it here, return success!
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While retrieving the drive information for: '");
		m_strLastError += tstrDriveSpec;
		m_strLastError += _T("', and exception occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 *
 *
 *
 *
 *
 */
BOOL CAttachedDrives::isFloppyDrive(TCHAR *tstrDriveSpec)
{
    BOOL bReturn = FALSE;	// default to pesimistic return val

    try
    {
		HANDLE hDevice = NULL;
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;
		
		// validate drive spec
		if(lstrlen(tstrDriveSpec) == 0)
			return bReturn;
		if(lstrlen(tstrDriveSpec) > 3)
			return bReturn;

		// create device name
		lstrcpy(tstrBuffer, DEVICEPATH_PREFIX);
		lstrcat(tstrBuffer, tstrDriveSpec);
		if(lstrlen(tstrBuffer))
		{
			// lose trailing backslash
			if(lstrlen(tstrDriveSpec) > 2)
				tstrBuffer[lstrlen(tstrBuffer) - 1] = 0;

			// attempt to open device
			hDevice = CreateFile(tstrBuffer, 0, 
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL, OPEN_EXISTING, 0, NULL);
			if(hDevice != INVALID_HANDLE_VALUE)
			{
				DISK_GEOMETRY dskgeoDrive;
				DWORD dwBytesReturned = (DWORD)0;
				
				// initialize structure
				memset(&dskgeoDrive, 0, sizeof(dskgeoDrive));
				
				// Query device
				if(DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, 
					NULL, 0, &dskgeoDrive, sizeof(dskgeoDrive),
                    &dwBytesReturned, (LPOVERLAPPED) NULL))
				{
					// check and see if this is a floppy or not
					if(dskgeoDrive.MediaType != Unknown &&
					   dskgeoDrive.MediaType != FixedMedia &&
					   dskgeoDrive.MediaType != RemovableMedia)
						bReturn = TRUE;
				}

				// close device handle
				CloseHandle(hDevice);
			}
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("An exception occurred while attempting to determine if the specified drive is a floppy drive or not.");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}