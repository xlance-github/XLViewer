#ifndef _CATTACHEDDRIVES_
#define _CATTACHEDDRIVES_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor    
// Purpose:   CAttachedDrives object interface. 
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
#include "..\DriveInformation.h"

// Package file object definition
class CAttachedDrives
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	DRIVEINFORMATION **m_ardrvinfThis;

	tstring m_strLastError;

	int m_iDriveCount;

	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Manages the identification and retrieval of information about all
	 * currently attached drives.
	 */
	BOOL getAvailableDrives();

	/**
	 * Retrieves the type, file system, and extended information for the
	 * specified drive.
	 */
	BOOL getDriveInformation(TCHAR *tstrDriveSpec, TCHAR *&tstrDriveType,
		TCHAR *&tstrFileSystem, TCHAR *&tstrExtendedInformation);

	/**
	 * Determines whether the drive specified by the drive spec is a
	 * floppy drive or not.
	 */
	BOOL isFloppyDrive(TCHAR *tstrDriveSpec);

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CAttachedDrives();

	/**
	 * Destructor, performs clean-up.
	 */
	~CAttachedDrives();

	///////////////////////////////////////////////////////////////////////////
	// Getter Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Returns the last error encountered, if any.
	 */
	TCHAR *getLastError() {return (TCHAR *)m_strLastError.data();}

	/**
	 * Returns the number of currently attached drives
	 */
	int attachedDriveCount() {return m_iDriveCount;}

	/**
	 * Returns a pointer to the internal drive object at the specified index.
	 */
	const LPDRIVEINFORMATION driveInformation(int iIndex);

	///////////////////////////////////////////////////////////////////////////
	// Setter Methods
	///////////////////////////////////////////////////////////////////////////
};

#endif // End _CATTACHEDDRIVES_
