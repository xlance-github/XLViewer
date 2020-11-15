///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CGraphicsDeviceInformation object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "..\XLanceView.h"
#include "CGraphicsDeviceInformation.h"

/**
 * Default constructor, initializes this object's fields to their defaults
 * and loads the graphics device's information.
 */
CGraphicsDeviceInformation::CGraphicsDeviceInformation()
{
	// initialize fields
	memset(&m_ddPrimaryDevice, 0, sizeof(m_ddPrimaryDevice));
	m_ddPrimaryDevice.cb = sizeof(m_ddPrimaryDevice);
	memset(&m_devmdCurrent, 0, sizeof(m_devmdCurrent));
	m_pgfxmdSupported = NULL;
	m_strGraphicsDeviceName = EMPTY_STRING;
	m_strGraphicsDeviceModes = EMPTY_STRING;
	m_strLastError = EMPTY_STRING;
	m_iGraphicsDeviceModesCount = 0;

	// get primary display device
	if(getPrimaryGraphicsDevice())
	{
		// get settings for primary graphics device
		if(getGraphicsDeviceInformation())
			createGraphicsDeviceModesString();
	}
}

/**
 * Performs final garbage collection for this object.
 */
CGraphicsDeviceInformation::~CGraphicsDeviceInformation()
{
	// garbage collect
	clearGraphicsModes();
}

///////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Attempts to locate, and return as a string, the graphics device mode 
 * which matches the specified criteria. NOTE: bits per pixel (color depth) 
 * and frequency are optional, and width and height MUST BE non-zero.
 *
 * @note The calling method or function is responsible for destroying the
 * returned string.
 *
 * @param iDesiredWidth
 *
 * @param iDesiredHeight
 *
 * @param iDesiredColorDepth optional parameter, the bits per pixel of the
 * graphics device mode requested. The default is 32-bit.
 *
 * @param iDesiredFrequency optional parameter, the frequency of the graphics
 * device mode being requested
 *
 * @return a pointer to a string that contains the representation of the 
 * graphics device mode requested if a matching mode is found and no errors 
 * occur, otherwise NULL.
 */
TCHAR *CGraphicsDeviceInformation::getGraphicsDeviceMode(int iDesiredWidth, 
	int iDesiredHeight, int iDesiredColorDepth, int iDesiredFrequency)
{
    TCHAR *ptcModeString = NULL;

    try
    {
		GRAPHICS_MODE *pgfxmdTemp = NULL;
		BOOL bMatchFound = FALSE;

		// validate parameters
		if(iDesiredWidth == 0 || iDesiredHeight == 0)
		{
			// set last error
			m_strLastError = _T("Either the requested mode width or height was zero.");

			// return fail val
			return ptcModeString;
		}

		// validate internal modes array
		if(m_pgfxmdSupported == NULL)
		{
			// set last error
			m_strLastError = _T("The interal device modes list is invalid.");

			// return fail val
			return ptcModeString;
		}
		if(m_iGraphicsDeviceModesCount == 0)
		{
			// set last errro
			m_strLastError = _T("The internal device modes count indicates the device has no supported modes.");

			// return fail
			return ptcModeString;
		}

		// Iterate through list...
		for(int iMode = 0; iMode < m_iGraphicsDeviceModesCount; iMode++)
		{
			// get current mode
			pgfxmdTemp = m_pgfxmdSupported[iMode];
			
			// validate, continue
			if(pgfxmdTemp)
			{
				// check against mandatory criteria...
				if(HIWORD(pgfxmdTemp->dwResolution) == iDesiredWidth &&
					LOWORD(pgfxmdTemp->dwResolution) == iDesiredHeight)
				{
					// Width and Height matched
					bMatchFound = TRUE;

					// check optional criteria
					if(iDesiredColorDepth != 0)
					{
						if(pgfxmdTemp->wSupportedBitsPerPixel & iDesiredColorDepth)
							bMatchFound = TRUE;
						else
							bMatchFound = FALSE;
					}
					if(iDesiredFrequency != 0)
					{
						if(pgfxmdTemp->wFrequency == iDesiredFrequency)
							bMatchFound = TRUE;
						else
							bMatchFound = FALSE;
					}
					
					// Check and see if there was a match
					if(bMatchFound)
					{
						// attempt to allocate storage for mode string
						ptcModeString = new TCHAR[80];

						// validate, continue
						if(ptcModeString)
						{
							_stprintf(ptcModeString, _T("%ld x %ld %ld-bit %ld Hz"),
								LOWORD(pgfxmdTemp->dwResolution),
								HIWORD(pgfxmdTemp->dwResolution),
								iDesiredColorDepth,
								pgfxmdTemp->wFrequency);

							// exit loop
							break;
						}
					}
				}
			}
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("");

        // destroy buffer and return fail
		if(ptcModeString)
		{
			delete[] ptcModeString;
			ptcModeString = NULL;
		}
    }

	// reset last error, if applicable
	if(ptcModeString)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return ptcModeString;
}

/**
 * Retrieves the index of the graphics device mode identified by the
 * supplied string. NOTE: the supplied string MUST BE one of the delimited
 * strings returned by graphicsDeviceModes().
 *
 * @param tstrGraphicsDeviceMode the string representation of the graphics
 * device mode requested.
 *
 * @return the index of the requested graphics device mode and no errors
 * occur, otherwise -1
 */
int CGraphicsDeviceInformation::getGraphicsDeviceModeIndex(
	TCHAR *tstrGraphicsDeviceMode)
{
    int iReturn = -1;	// default to pesimistic return val

    try
    {
		tstring strTempGFXDevMode = EMPTY_STRING;
		int iStart,
			iPos,
			iIndex;

		// validate requested graphics device mode
		if(lstrlen(tstrGraphicsDeviceMode) == 0)
			return iReturn;

		// check internal fields
		//	 Modes array
		if(m_pgfxmdSupported == NULL)
			return iReturn;
		if(m_iGraphicsDeviceModesCount == 0)
			return iReturn;
		//	 Modes string
		if(m_strGraphicsDeviceModes.length() == 0)
			return iReturn;

		// iterate through "pieces" of graphics device modes string
		for(iStart = 0L, 
				iPos = m_strGraphicsDeviceModes.find_first_of(_T(";"), iStart), 
				iIndex = 0;
			iPos < (int)m_strGraphicsDeviceModes.length() && iPos != -1;
			iStart = ++iPos, 
				iPos = m_strGraphicsDeviceModes.find_first_of(_T(";"), iStart),
				iIndex++)
		{
			// get current mode
			strTempGFXDevMode = m_strGraphicsDeviceModes.substr(iStart, iPos - iStart);

			// compare with requested
			if(strTempGFXDevMode.compare(tstrGraphicsDeviceMode) == 0)
			{
				// assign return val
				iReturn = iIndex;

				// exit loop
				break;
			}
		}

    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While retrieving the graphics device mode index, an unexpected error occurred.");

        // reset fail value, jic
        iReturn = -1;
    }

	// reset last error, if applicable
	if(iReturn > -1)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return iReturn;
}

/**
 * Retrieves the graphics device mode, from the internal graphics device
 * mode array, at the specified index. This index SHOULD BE the index
 * returned by getGraphicsDeviceModeIndex().
 *
 * @note The calling method or function is responsible for freeing the
 * memory associated with the returned LPDEVMODE using the delete
 * operator.
 *
 * @param iIndex the zero-based index of the graphics device mode 
 * structure to be retrieved.
 *
 * @return pointer to the device mode structure at the specified index
 * if no errors occur, otherwise NULL
 */
LPDEVMODE CGraphicsDeviceInformation::getGraphicsDeviceModeFromIndex(
	int iIndex)
{
    LPDEVMODE lpdevmReturn = NULL;

    try
    {
		GRAPHICS_MODE *pgfxmdTemp = NULL;

		// validate internal graphics device mode array
		if(m_pgfxmdSupported == NULL)
			return lpdevmReturn;
		if(m_iGraphicsDeviceModesCount == 0)
			return lpdevmReturn;

		// validate index
		if(iIndex < 0 || iIndex > (m_iGraphicsDeviceModesCount - 1))
			return lpdevmReturn;

		// get graphics device mode
		pgfxmdTemp = m_pgfxmdSupported[iIndex];

		// validate, continue
		if(pgfxmdTemp)
		{
			DEVMODE devmEnum;
			DWORD dwGraphicsMode = (DWORD)0;

			// initialize device mode struct
			memset(&devmEnum, 0, sizeof(DEVMODE));
			devmEnum.dmSize = sizeof(DEVMODE);

			// enum display settings and compare to internal... if they match,
			//	 then return
			while(EnumDisplaySettings(m_ddPrimaryDevice.DeviceName, 
					dwGraphicsMode++, &devmEnum))
			{	
				if(devmEnum.dmPelsWidth == HIWORD(pgfxmdTemp->dwResolution) &&
					devmEnum.dmPelsHeight == LOWORD(pgfxmdTemp->dwResolution) &&
					devmEnum.dmBitsPerPel == pgfxmdTemp->wSupportedBitsPerPixel &&
					devmEnum.dmDisplayFrequency == pgfxmdTemp->wFrequency)
				{
					// attempt to allocate memory
					lpdevmReturn = new DEVMODE();

					// validate, continue
					if(lpdevmReturn)
					{
						// copy data
						memcpy(lpdevmReturn, &devmEnum, sizeof(DEVMODE));
					}

					// exit loop
					break;
				}
			}

		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While retrieving the graphics device mode by index, an unexpected error occurred.");

        // reset fail value
        lpdevmReturn = NULL;
    }

	// reset last error, if applicable
	if(lpdevmReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return lpdevmReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Get the primary display device and stores its device structure.
 *
 * @return TRUE if the primary graphics device is retrieved and
 * no errors occur, otherwise FALSE.
 */
BOOL CGraphicsDeviceInformation::getPrimaryGraphicsDevice()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		DISPLAY_DEVICE ddTemp;
		int iIndex = 0;

		// intiailize local structure
		memset(&ddTemp, 0, sizeof(ddTemp));
		ddTemp.cb = sizeof(ddTemp);

		// enumerate display devices and locate the primary
		while(EnumDisplayDevices(NULL, iIndex, &ddTemp, (DWORD)0))
		{
			// check and see if this is the primary
			if(ddTemp.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			{
				// assign to this object's field var
				m_ddPrimaryDevice = ddTemp;

				// assign device name
				m_strGraphicsDeviceName = m_ddPrimaryDevice.DeviceString;

				// exit loop
				break;
			}

			// increment device index
			iIndex++;
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While retrieving the primary graphics device, an exception occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Gets supported resolutions and color depths for the primary display
 * (graphics) device.
 *
 * @return TRUE if the information for the primary graphics device is
 * retrieved and no errors occur, otherwise FALSE
 */
BOOL CGraphicsDeviceInformation::getGraphicsDeviceInformation()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {	
		DEVMODE devmDisplay;
		DWORD dwGraphicsMode = (DWORD)0,
			  dwTemp;
		int iModeCount = 0,
			iMode = 0,
			iFinalModeCount = 0;

		// validate display device structure
		if(m_ddPrimaryDevice.DeviceID == 0 || m_ddPrimaryDevice.DeviceName[0] == 0)
		{
			// set last error
			m_strLastError = _T("The primary graphics device is invalid.");

			// return fail val
			return FALSE;
		}

		// intialize DEVMODE structure
		memset(&devmDisplay, 0, sizeof(devmDisplay));
		devmDisplay.dmSize = sizeof(DEVMODE);

		// First, get the current device mode and store. It is IMPERATIVE that we
		//	 be able to get this mode because it allows any application accessing
		//	 this object to know what the original display settings were.
		if(EnumDisplaySettings(m_ddPrimaryDevice.DeviceName, ENUM_CURRENT_SETTINGS, 
			&m_devmdCurrent) == 0)
		{
			// set last error
			m_strLastError = _T("Could not retrieve the current display settings for the primary graphics device.");

			// return fail val
			return FALSE;
		}

		// Next, determine how many supported modes there are
		while(EnumDisplaySettings(m_ddPrimaryDevice.DeviceName, dwGraphicsMode++
			, &devmDisplay))
		{
			// increment mode count
			iModeCount++;
		}

		// validate
		if(iModeCount == 0)
		{
			// set last error
			m_strLastError = _T("No supported graphics mode were detected for this current primary graphics device.");

			// return fail val
			return FALSE;
		}
	
		// Attempt to create modes array
		m_pgfxmdSupported = new GRAPHICS_MODE*[iModeCount];
		if(m_pgfxmdSupported == NULL)
		{
			// set last error
			m_strLastError = _T("Could not allocate storage for graphics mode array.");

			// return fail val
			return FALSE;
		}

		// set internal count field
		m_iGraphicsDeviceModesCount = iModeCount;

		// initialize array
		for(int i = 0; i < m_iGraphicsDeviceModesCount; i++)
			m_pgfxmdSupported[i] = new GRAPHICS_MODE();

		// enumerate all display settings
		dwGraphicsMode = (DWORD)0;
		while(EnumDisplaySettings(m_ddPrimaryDevice.DeviceName, dwGraphicsMode
			, &devmDisplay))
		{
			// get resolution, create DWORD
			dwTemp = MAKELPARAM(devmDisplay.dmPelsHeight, devmDisplay.dmPelsWidth);

			// search array for resolution and frequency... At first this may look
			//	 confusing but what this loop does is consolidate graphics device
			//	 modes that differ by pieces of information we are NOT interested
			//	 in - i.e. they DO NOT differ by resolution, color depth, or
			//	 frequency.
			for(iMode = 0; 
				iMode < m_iGraphicsDeviceModesCount &&
					m_pgfxmdSupported[iMode]->dwResolution != 0;
				iMode++)
			{
				if(m_pgfxmdSupported[iMode]->dwResolution == dwTemp &&
					m_pgfxmdSupported[iMode]->wFrequency == devmDisplay.dmDisplayFrequency &&
					m_pgfxmdSupported[iMode]->wSupportedBitsPerPixel == devmDisplay.dmBitsPerPel)
				{
					// exit loop
					break;
				}
			}

			// assign resolution, color depth, and frequency
			m_pgfxmdSupported[iMode]->dwResolution = dwTemp;
			m_pgfxmdSupported[iMode]->wSupportedBitsPerPixel = 
				m_pgfxmdSupported[iMode]->wSupportedBitsPerPixel 
				| (WORD)devmDisplay.dmBitsPerPel;
			m_pgfxmdSupported[iMode]->wFrequency = 
				(WORD)devmDisplay.dmDisplayFrequency;

			// increment graphics mode
			dwGraphicsMode++;
		}

		// Now, attempt to trim array down to size...
		for(iModeCount = 0, iFinalModeCount = -1; 
			iModeCount < m_iGraphicsDeviceModesCount; 
			iModeCount++)
		{
			if(m_pgfxmdSupported[iModeCount]->dwResolution == (DWORD)0)
			{
				// set "end" mode
				iFinalModeCount = iModeCount;

				// exit loop
				break;
			}
		}
		if(iFinalModeCount == -1)
			iFinalModeCount = iModeCount - 1;

		// compare new count against old count
		if(iMode > -1 && (iFinalModeCount != m_iGraphicsDeviceModesCount))
		{
			GRAPHICS_MODE **pgfxmdTemp = NULL;

			// attempt to create temporary storage
			pgfxmdTemp = new GRAPHICS_MODE*[iFinalModeCount];

			// validate, continue
			if(pgfxmdTemp)
			{
				for(int i = 0; i < iFinalModeCount; i++)
				{
					// create new item
					pgfxmdTemp[i] = new GRAPHICS_MODE();

					// validate...
					if(pgfxmdTemp[i] == NULL)
					{
						// destroy temp array
						delete[] pgfxmdTemp;
						
						// de-ref for post-loop check
						pgfxmdTemp = NULL;

						// exit
						break;
					}

					// copy data
					pgfxmdTemp[i]->dwResolution = m_pgfxmdSupported[i]->dwResolution;
					pgfxmdTemp[i]->wSupportedBitsPerPixel = 
						m_pgfxmdSupported[i]->wSupportedBitsPerPixel;
					pgfxmdTemp[i]->wFrequency = m_pgfxmdSupported[i]->wFrequency;
				}

				// If the temp array was created successfully, destroy graphics
				//	 mode array, then re-assign temporary to it.
				if(pgfxmdTemp)
				{
					// clear graphics mode array
					clearGraphicsModes();

					// assign new count
					m_iGraphicsDeviceModesCount = iFinalModeCount;

					// make temporary array the graphics mode array
					m_pgfxmdSupported = pgfxmdTemp;

					// de-ref temporary... jic
					pgfxmdTemp = NULL;
				}

				// One last step... sort the graphics mode array
				bReturn = sortGraphicsDeviceModes(m_pgfxmdSupported,
							0, m_iGraphicsDeviceModesCount);
			}
		}
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While retrieving the information for the primary graphics device, an exception occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Destroys the graphics mode array.
 */
BOOL CGraphicsDeviceInformation::clearGraphicsModes()
{
	BOOL bReturn = FALSE;

	try
	{
		// destroy array, if applicable
		if(m_pgfxmdSupported)
		{
			GRAPHICS_MODE *pgfxmdTemp = NULL;

			for(int i = 0; i < m_iGraphicsDeviceModesCount; i++)
			{
				// get current mode
				pgfxmdTemp = m_pgfxmdSupported[i];

				// destroy object
				delete pgfxmdTemp;
			}

			// finally, destroy array
			delete[] m_pgfxmdSupported;

			// de-ref
			m_pgfxmdSupported = NULL;
		}

		// set success return val
		bReturn = TRUE;
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While destroying the graphics mode array, an exception occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Reads the graphics modes array and creates a semi-colon delimited list
 * of display modes.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CGraphicsDeviceInformation::createGraphicsDeviceModesString()
{
	BOOL bReturn = FALSE;

	try
	{
		GRAPHICS_MODE *pgfxmdTemp = NULL;
		TCHAR tstrBuffer[80];

		// validate modes array
		if(m_pgfxmdSupported == NULL)
			return bReturn;
		if(m_iGraphicsDeviceModesCount == 0)
			return bReturn;

		// reset modes string
		m_strGraphicsDeviceModes = EMPTY_STRING;

		// iterate through items in the array and create string
		for(int i = 0; i < m_iGraphicsDeviceModesCount; i++)
		{
			// get current mode
			pgfxmdTemp = m_pgfxmdSupported[i];

			// iterate through supported color depths
			for(int iBit = 1; iBit < ((sizeof(DWORD) * 8) + 1); iBit *= 2)
			{
				if(pgfxmdTemp->wSupportedBitsPerPixel & iBit)
				{
					_stprintf(tstrBuffer, _T("%ld x %ld %ld-bit %ld Hz;"),
						HIWORD(pgfxmdTemp->dwResolution),
						LOWORD(pgfxmdTemp->dwResolution),
						iBit,
						pgfxmdTemp->wFrequency);

					// append
					m_strGraphicsDeviceModes += tstrBuffer;
				}
			}
		}	
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While creating the graphics mode string, an exception occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Sorts the graphics device modes array using a recursive bubble sort.
 *
 * @param pgfxmdArray
 *
 * @param iStart
 *
 * @param iLength
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CGraphicsDeviceInformation::sortGraphicsDeviceModes(
	GRAPHICS_MODE **pgfxmdArray, int iStart, int iLength)
{
	BOOL bReturn = TRUE;

	try
	{
		int i,	// Loop var
			iPass = iLength - iStart - 1;	// pass counter
		
		// validate array
		if(pgfxmdArray == NULL)
			return FALSE;

		// validate current pass
		if(iPass < 0)
			return TRUE;
		
		for(i = iStart; i < iPass; i++)
		{
			if(pgfxmdArray[i]->compare(*pgfxmdArray[i + 1]) > 0)
			{
				GRAPHICS_MODE *pgfxmdTemp = NULL;

				// perform swap
				pgfxmdTemp = pgfxmdArray[i];
				pgfxmdArray[i] = pgfxmdArray[i + 1];
				pgfxmdArray[i + 1] = pgfxmdTemp;
			}
		}

		// call until stop condition is met
		sortGraphicsDeviceModes(pgfxmdArray, iStart, iPass);
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While sorting the graphics device modes array, an error occurred.");

        // reset fail value
        bReturn = FALSE;
    }

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}