#ifndef _CSETTINGS_
#define _CSETTINGS_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:	  ©2011 M.Sc. E. Victor  
// Purpose:   CSettings object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include "..\StdAfx.h"


///////////////////////////////////////////////////////////////////////////////
// Constants used by class interface
///////////////////////////////////////////////////////////////////////////////

#define COLOR_NONE				0x01000000	// Unallowed color, used as default
#define MAX_CUSTOM_COLORS				16	// Max number of custom colors
											//	 allowed by Windows(r) API

// Package file object definition
class CSettings
{
public:
  enum Controls
  {
    CommandlineInput = 0,
    CommandlineOutput,
    FileManager1,
    FileManager2,
	//FileManager3,// shri bk color
	//FileManager4,// Ram bk color
    ControlsCount // always must be last
  };
  enum Colors
  {
    ForegroundText = 0,
    Background, 
    SelectedText,
    SelectingBand,
	//SelectingBand2,//shripad
    ColorsCount // always must be last
  };

  static const TCHAR* ControlsNames[ControlsCount];
  static const TCHAR* ColorNames[ColorsCount];

  void Color(size_t control_num, size_t color_num, const COLORREF& value);
  COLORREF Color(size_t control_num, size_t color_num);
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////
	
	tstring m_strGraphicsDevice,
				m_strGraphicsMode,
				m_strLastFolderFileManager1,
				m_strLastFolderFileManager2,
				m_strLastFolderFileManager3,
				m_strLastFolderFileManager4,
				m_strApplicationFolder,
				m_strLastError;

#define TZ_COUNT 3
  tstring m_strTZName[TZ_COUNT];
  long m_tzBias[TZ_COUNT];

	COLORREF 
       /*m_clrHighlightColorFileManager1,
			 m_clrTextColorFileManager1,
			 m_clrHighlightColorFileManager2,
			 m_clrTextColorFileManager2,

       m_clrSelBGFM1,
       m_clrSelFGFM1,

       m_clrSelBGFM2,
       m_clrSelFGFM2,

			 m_clrTextColorCommandPrompt,
			 m_clrBackgroundColorCommandPrompt,*/

       m_colors[ControlsCount][ColorsCount],

			 m_arclrCustomColors[MAX_CUSTOM_COLORS];

	BOOL m_bAlwaysLaunchFullScreen;

	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Loads, from the registry, all settings for the application. NOTE: this
	 * method is called automatically from the constructor.
	 */
	BOOL loadSettings();

	/**
	 * Saves, to the registry, all settings for the application. NOTE: this
	 * method is called automatically from the destructor.
	 */
	BOOL saveSettings();

	/**
	 * Opens the process associated with this application and gets its startup
	 * folder.
	 */
	BOOL getApplicationFolder();

public:
	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CSettings();

	/**
	 * Destructor, performs clean-up.
	 */
	~CSettings();

	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	// Getter Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Gets whether or not the application should automatically launch
	 * full screen or not.
	 */
	BOOL alwaysLaunchFullScreen() {return m_bAlwaysLaunchFullScreen;}

	/**
	 * Gets the name of the current graphics device. NOTE: this should always
	 * be the primary display adapter from Windows(r).
	 */
	TCHAR *graphicsDevice() {return (TCHAR *)m_strGraphicsDevice.data();}
	
	/**
	 * Gets the current resolution for the graphics device.
	 */
	TCHAR *graphicsMode() {return (TCHAR *)m_strGraphicsMode.data();}

	/**
	 * Returns the last browse folder for File Manager One.
	 */
	TCHAR *lastFolderFileManager1() 
		{return (TCHAR *)m_strLastFolderFileManager1.data();}

	/**
	 * Returns the last browse folder for File Manager Two.
	 */
	TCHAR *lastFolderFileManager2() 
		{return (TCHAR *)m_strLastFolderFileManager2.data();}

	//shripad
	/**
	 * Returns the last browse folder for TV File Manager Two.
	 */
	TCHAR *lastFolderFileManager3() 
		{return (TCHAR *)m_strLastFolderFileManager3.data();}
	//shripad

	//shripad
	/**
	 * Returns the last browse folder for TV File Manager Two.
	 */
	TCHAR *lastFolderFileManager4() 
		{return (TCHAR *)m_strLastFolderFileManager4.data();}
	//shripad

	/**
	 * Returns the application folder for *this* application.
	 */
	TCHAR *applicationFolder() {return (TCHAR *)m_strApplicationFolder.data();}

	///////////////////////////////////////////////////////////////////////////
	// Setter Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Sets whether or not the application should automatically launch
	 * full screen.
	 */
	VOID alwaysLaunchFullScreen(BOOL bValue) {m_bAlwaysLaunchFullScreen = bValue;}

	/**
	 * Sets the name of the current graphics device. NOTE: this should always
	 * be the primary display adapter from Windows(r).
	 */
	VOID graphicsDevice(TCHAR *tstrValue) {m_strGraphicsDevice = tstrValue;}

	/**
	 * Sets the current resolution for the graphics device.
	 */
	VOID graphicsMode(TCHAR *tstrValue) {m_strGraphicsMode = tstrValue;}

	/**
	 * Sets the last browse folder for File Manager One.
	 */
	VOID lastFolderFileManager1(TCHAR *tstrValue)
		{
			m_strLastFolderFileManager1 = tstrValue;
			m_strLastFolderFileManager3 = tstrValue;
		}

	/**
	 * Sets the last browse folder for File Manager One.
	 */
	VOID lastFolderFileManager2(TCHAR *tstrValue) 
		{
			m_strLastFolderFileManager2 = tstrValue;
			m_strLastFolderFileManager4 = tstrValue;
		}

	//shripad
	VOID lastFolderFileManager3(TCHAR *tstrValue) 
		{m_strLastFolderFileManager3 = tstrValue;}
	//shripad

  bool IsValidClockNum(size_t clock_num) const;

  void TZName(size_t clock_num, const TCHAR*    name);
  const TCHAR* TZName(size_t clock_num);

  long TZHourBias(size_t clock_num) const;
  void TZHourBias(size_t clock_num, long hour_bias);

	///////////////////////////////////////////////////////////////////////////
	// Exposer Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Exposes the custom colors array for reading and writing
	 */
	COLORREF *getCustomColors() {return &m_arclrCustomColors[0];}
};

#endif // End _CSETTINGS_
