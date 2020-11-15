#ifndef _CGRAPHICSDEVICEINFORMATION_
#define _CGRAPHICSDEVICEINFORMATION_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CGraphicsDeviceInformation object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "..\Graphics_Mode.h"

// Package file object definition
class CGraphicsDeviceInformation
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	DISPLAY_DEVICE m_ddPrimaryDevice;

	DEVMODE m_devmdCurrent;

	GRAPHICS_MODE **m_pgfxmdSupported;

	tstring m_strGraphicsDeviceName,
				m_strGraphicsDeviceModes,
				m_strLastError;

	int m_iGraphicsDeviceModesCount;

	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Destroys the graphics mode array.
	 */
	BOOL clearGraphicsModes();

	/**
	 * Get the primary display device and stores its device structure.
	 */
	BOOL getPrimaryGraphicsDevice();

	/**
	 * Gets supported resolutions and color depths for the primary display
	 * (graphics) device.
	 */
	BOOL getGraphicsDeviceInformation();

	/**
	 * Reads the graphics modes array and creates a semi-colon delimited list
	 * of display modes.
	 */
	BOOL createGraphicsDeviceModesString();

	/**
	 * Sorts the graphics device modes array by resolution then frequency.
	 */
	BOOL sortGraphicsDeviceModes(GRAPHICS_MODE **pgfxmdArray, int iStart, 
		int iLength);

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CGraphicsDeviceInformation();

	/**
	 * Destructor, performs clean-up.
	 */
	~CGraphicsDeviceInformation();

	///////////////////////////////////////////////////////////////////////////
	// Getter Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Returns the last error encountered, if any.
	 */
	TCHAR *getLastError() {return (TCHAR *)m_strLastError.c_str();}

	/**
	 * Returns the current graphics device mode for the primary graphics device.
	 * These are the settings which were recorded at startup by this object.
	 */
	const LPDEVMODE currentGraphicsDeviceMode() {return &m_devmdCurrent;}

	/**
	 * Returns the name of the primary graphics (display) device.
	 */
	TCHAR *graphicsDeviceName() 
	{return (TCHAR *)m_strGraphicsDeviceName.data();}

	/**
	 * Returns the supported graphics mode for the primary graphics (display)
	 * device as a semi-colon delimited string.
	 */
	TCHAR *graphicsDeviceModes()
		{return (TCHAR *)m_strGraphicsDeviceModes.data();}

	/**
	 * Attempts to locate, and return as a string, the graphics device mode 
	 * which matches the specified criteria. NOTE: bits per pixel (color depth) 
	 * and frequency are optional.
	 */
	TCHAR *getGraphicsDeviceMode(int iDesiredWidth, int iDesiredHeight,
		int iDesiredColorDepth = 32, int iDesiredFrequency = 0);

	/**
	 * Retrieves the index of the graphics device mode identified by the
	 * supplied string. NOTE: the supplied string MUST BE one of the delimited
	 * strings returned by graphicsDeviceModes().
	 */
	int getGraphicsDeviceModeIndex(TCHAR *tstrGraphicsDeviceMode);

	/**
	 * Retrieves the graphics device mode, from the internal graphics device
	 * mode array, at the specified index. This index SHOULD BE the index
	 * returned by getGraphicsDeviceModeIndex().
	 */
	LPDEVMODE getGraphicsDeviceModeFromIndex(int iIndex);
};

#endif // End _CGRAPHICSDEVICEINFORMATION_
