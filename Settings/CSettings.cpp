///////////////////////////////////////////////////////////////////////////////
// Author(s): 
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CSettings object implementation
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
#include "CSettings.h"


const TCHAR* CSettings::ColorNames[ColorsCount] = {
	_T("Foreground text"), _T("Background"), _T("Selected text"), _T("Selecting band")};
		//,_T("Selecting band2")};//shripad

  
const TCHAR* CSettings::ControlsNames[ControlsCount] = {
  _T("Commandline input"), _T("Commandline output"), _T("File manager 1"), _T("File manager 2")};
				//,_T("File manager 3"), _T("File manager 3")}; //shripad added file manager3
//shripad

/**
 * Default constructor, initializes this object's fields and loads all
 * settings.
 */
CSettings::CSettings()
{
	// initialize fields
	m_strGraphicsDevice = EMPTY_STRING;
	m_strGraphicsMode = EMPTY_STRING;
	m_strLastFolderFileManager1 = EMPTY_STRING;
	m_strLastFolderFileManager2 = EMPTY_STRING;
	m_strLastFolderFileManager3 = EMPTY_STRING;//shripad
	m_strLastFolderFileManager4 = EMPTY_STRING;//shripad
	m_strApplicationFolder = EMPTY_STRING;
	m_bAlwaysLaunchFullScreen = FALSE;

  m_colors[FileManager1][Background] = RGB(0, 0, 0);
  m_colors[FileManager1][SelectedText] = m_colors[FileManager1][ForegroundText] = RGB(255, 255, 255);
  m_colors[FileManager1][SelectingBand] = GetSysColor(COLOR_HIGHLIGHT);

  m_colors[FileManager2][Background] = RGB(0, 0, 0);
	m_colors[FileManager2][SelectedText] = m_colors[FileManager2][ForegroundText] = RGB(255, 255, 255);
  m_colors[FileManager2][SelectingBand] = GetSysColor(COLOR_HIGHLIGHT);

  //shripad
  /*m_colors[FileManager3][Background] = RGB(0, 0, 0);
	m_colors[FileManager3][SelectedText] = m_colors[FileManager3][ForegroundText] = RGB(255, 255, 255);
  m_colors[FileManager3][SelectingBand] = GetSysColor(COLOR_HIGHLIGHT);*/
  //shripad

  m_colors[CommandlineInput][SelectingBand] = m_colors[CommandlineInput][ForegroundText] = RGB(255, 255, 255);
  m_colors[CommandlineInput][Background] = m_colors[CommandlineInput][SelectedText] = RGB(0, 0, 0);

  m_colors[CommandlineOutput][SelectingBand] = m_colors[CommandlineOutput][ForegroundText] = RGB(255, 255, 255);
  m_colors[CommandlineOutput][Background] = m_colors[CommandlineOutput][SelectedText] = RGB(0, 0, 0);

	// Initialize custom colors array to all white
	for(int i = 0; i < MAX_CUSTOM_COLORS; i++)
		m_arclrCustomColors[i] = RGB(255, 255, 255);

	// load application folder
	getApplicationFolder();

	// load settings
	loadSettings();
}

/**
 * Performs final garbage collection for this object and saves all settings.
 */
CSettings::~CSettings()
{
	// save settings...
	saveSettings();

	// garbage collect
}

///////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////


void CSettings::Color(size_t control_num, size_t color_num, const COLORREF& value)
{
  if ((control_num >= ControlsCount) || (color_num >= ColorsCount))
  {
    assert(0);
    return;
  }
  m_colors[control_num][color_num] = value;
}


COLORREF CSettings::Color(size_t control_num, size_t color_num)
{
  if ((control_num >= ControlsCount) || (color_num >= ColorsCount))
  {
    assert(0);
    return 0;
  }
  return m_colors[control_num][color_num];
}


///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Loads, from the registry, all settings for the application. NOTE: this
 * method is called automatically from the constructor.
 *
 * @note getApplicationFolder() MUST BE called prior to calling this 
 * method
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CSettings::loadSettings()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		LPTSTR lptstrBuffer = NULL;
		DWORD dwTempColor = (DWORD)0;

		// Load settings from the registry
		//	 Always launch full screen
		m_bAlwaysLaunchFullScreen = GetRegistryNumeric(CurrentUser, REG_BASE,
										REG_SECTION_SETTINGS,
										REG_VAL_SETS_ALWAYSLAUNCHFULLSCREEN,
										FALSE, TRUE);

		//   Graphics Device (name)
		lptstrBuffer = GetRegistryString(CurrentUser, REG_BASE, 
							REG_SECTION_SETTINGS,
							REG_VAL_SETS_GRAPHICSDEVICE, TRUE);
		if(lptstrBuffer)
		{
			// assign
			m_strGraphicsDevice = lptstrBuffer;

			// destroy buffer
			GlobalFree(lptstrBuffer);
			lptstrBuffer = NULL;
		}
		//   Graphics Mode
		lptstrBuffer = GetRegistryString(CurrentUser, REG_BASE, 
							REG_SECTION_SETTINGS,
							REG_VAL_SETS_GRAPHICSMODE, TRUE);
		if(lptstrBuffer)
		{
			// assign
			m_strGraphicsMode = lptstrBuffer;

			// destroy buffer
			GlobalFree(lptstrBuffer);
			lptstrBuffer = NULL;
		}
		//	 Last browse folder, File Manager 1
		lptstrBuffer = GetRegistryString(CurrentUser, REG_BASE, 
							REG_SECTION_SETTINGS,
							REG_VAL_SETS_LASTFOLDER_FILEMANAGER1, TRUE);
		if(lptstrBuffer)
		{
			// assign
			m_strLastFolderFileManager1 = lptstrBuffer;

			// destroy buffer
			GlobalFree(lptstrBuffer);
			lptstrBuffer = NULL;
		}
		//	 Last browse folder, File Manager 2
		lptstrBuffer = GetRegistryString(CurrentUser, REG_BASE, 
							REG_SECTION_SETTINGS,
							REG_VAL_SETS_LASTFOLDER_FILEMANAGER2, TRUE);
		if(lptstrBuffer)
		{
			// assign
			m_strLastFolderFileManager2 = lptstrBuffer;

			// destroy buffer
			GlobalFree(lptstrBuffer);
			lptstrBuffer = NULL;
		}

		// Do one other thing... check File Manager 1 & 2's initial folders.
		//	 File Manager 1
		if(m_strLastFolderFileManager1.length() == 0)
			m_strLastFolderFileManager1 = m_strApplicationFolder;
		//	 File Manager 2
		if(m_strLastFolderFileManager2.length() == 0)
			m_strLastFolderFileManager2 = m_strApplicationFolder;

		//shripad
		//	 File Manager 2
		if(m_strLastFolderFileManager3.length() == 0)
			m_strLastFolderFileManager3 = m_strApplicationFolder;
		if(m_strLastFolderFileManager4.length() == 0)
			m_strLastFolderFileManager4 = m_strApplicationFolder;
		//shripad

		// Get Colors
    for (size_t control_it = 0; control_it < ControlsCount; ++control_it)
    {
      for (size_t color_it = 0; color_it < ColorsCount; ++color_it)
      {
        tstring reg_str = ControlsNames[control_it];
        reg_str += _T(" ");
        reg_str += ColorNames[color_it];
		    dwTempColor = GetRegistryNumeric(CurrentUser, REG_BASE, 
          REG_SECTION_OPTIONS,
          reg_str.c_str(), COLOR_NONE, TRUE);
		    if (dwTempColor != COLOR_NONE)
          Color(control_it, color_it, dwTempColor); 
      }
    }

		// Custom colors
		GetRegistryBinary(CurrentUser, REG_BASE, REG_SECTION_OPTIONS, 
			REG_VAL_SETS_CUSTOMCOLORS, (VOID *)&m_arclrCustomColors, 
			sizeof(m_arclrCustomColors), TRUE, TRUE);

    TCHAR buf[100];
	long tzBias1[3] = {2,-5,-8};
    for (size_t tz_it = 0; tz_it < TZ_COUNT; ++tz_it)
    {
      ZeroMemory(buf, sizeof(buf));
      _stprintf(buf, REG_VAL_SETS_TZ_NAME, tz_it);
      if (LPTSTR tz_str = GetRegistryString(CurrentUser, REG_BASE, REG_SECTION_OPTIONS, buf, TRUE))
        m_strTZName[tz_it] = tz_str;
	  
      ZeroMemory(buf, sizeof(buf));
      _stprintf(buf, REG_VAL_SETS_TZ_BIAS, tz_it);
      unsigned long ul_val = GetRegistryNumeric(CurrentUser, REG_BASE, REG_SECTION_OPTIONS, buf, 0, TRUE);
      m_tzBias[tz_it] = *reinterpret_cast<long*>( &ul_val );
    }

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
 * Saves, to the registry, all settings for the application. NOTE: this
 * method is called automatically from the destructor.
 *
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CSettings::saveSettings()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		// Save settings to registry
		//	 Always launch full screen
		SaveRegistryNumeric(CurrentUser, REG_BASE, REG_SECTION_SETTINGS,
			REG_VAL_SETS_ALWAYSLAUNCHFULLSCREEN, m_bAlwaysLaunchFullScreen,
			TRUE);
		//	 Graphics Device
		SaveRegistryString(CurrentUser, REG_BASE, REG_SECTION_SETTINGS,
			REG_VAL_SETS_GRAPHICSDEVICE, (TCHAR *)m_strGraphicsDevice.data(),
			TRUE);
		//	 Graphics Resolution
		SaveRegistryString(CurrentUser, REG_BASE, REG_SECTION_SETTINGS,
			REG_VAL_SETS_GRAPHICSMODE, (TCHAR *)m_strGraphicsMode.data(),
			TRUE);
		//	 Last Browse Folder, File Manager 1
		SaveRegistryString(CurrentUser, REG_BASE, REG_SECTION_SETTINGS,
			REG_VAL_SETS_LASTFOLDER_FILEMANAGER1, 
			(TCHAR *)m_strLastFolderFileManager1.data(), TRUE);
		//	 Last Browse Folder, File Manager 2
		SaveRegistryString(CurrentUser, REG_BASE, REG_SECTION_SETTINGS,
			REG_VAL_SETS_LASTFOLDER_FILEMANAGER2, 
			(TCHAR *)m_strLastFolderFileManager2.data(), TRUE);

		// Save Colors
    for (size_t control_it = 0; control_it < ControlsCount; ++control_it)
    {
      for (size_t color_it = 0; color_it < ColorsCount; ++color_it)
      {
        tstring reg_str = ControlsNames[control_it];
        reg_str += _T(" ");
        reg_str += ColorNames[color_it];

		    SaveRegistryNumeric(CurrentUser, REG_BASE, REG_SECTION_OPTIONS,
			    reg_str.c_str(),
			    Color(control_it, color_it), TRUE);
      }
    }

    // Custom Colors
		SaveRegistryBinary(CurrentUser, REG_BASE, REG_SECTION_OPTIONS,
			REG_VAL_SETS_CUSTOMCOLORS, (VOID *)&m_arclrCustomColors,
			sizeof(m_arclrCustomColors), TRUE);

    TCHAR buf[100];
	
    for (size_t tz_it = 0; tz_it < TZ_COUNT; ++tz_it)
    {
      _stprintf(buf, REG_VAL_SETS_TZ_NAME, tz_it);
	  
      SaveRegistryString(CurrentUser, REG_BASE, REG_SECTION_OPTIONS, buf, m_strTZName[tz_it].c_str(), TRUE);
	  _stprintf(buf, REG_VAL_SETS_TZ_BIAS, tz_it);
	  
	  unsigned long ul_val = *reinterpret_cast<unsigned long*>( &m_tzBias[tz_it] );
	  
      SaveRegistryNumeric(CurrentUser, REG_BASE, REG_SECTION_OPTIONS, buf, ul_val, TRUE);
    }

    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Opens the process associated with this application and gets its startup
 * folder.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CSettings::getApplicationFolder()
{
	BOOL bReturn = FALSE;	// default to pesimistic return val

	try
	{
		HANDLE hProcessThis = NULL;
		DWORD dwProcessID = (DWORD)0;
		TCHAR tstrProcessFullpath[MAX_PATH] = EMPTY_STRING;	

		// Next, get the process ID from this object's window
		dwProcessID = GetCurrentProcessId();
		if(dwProcessID == 0)
		{
			// display last error
			m_strLastError = _T("Could not get the process ID for this application.");

			// return fail val
			return bReturn;
		}

		// Now, attempt to open the process...
		hProcessThis = OpenProcess(PROCESS_QUERY_INFORMATION /*PROCESS_ALL_ACCESS*/, FALSE, dwProcessID);

		// validate, continue
		if(hProcessThis != NULL && hProcessThis != INVALID_HANDLE_VALUE)
		{
			DWORD dwSize = (DWORD)0, 
				  dwReturn = (DWORD)0;

			// Next, attempt to get this module's name
			dwSize = sizeof(tstrProcessFullpath);
			// Doesn't work for XP
			//dwReturn = GetModuleFileNameEx(hProcessThis, NULL, tstrProcessName, dwSize);	
			dwReturn = GetModuleFileName(NULL, tstrProcessFullpath, dwSize);

			// validate, continue
			if(dwReturn != (DWORD)0)
			{
				// find last backslash
				for(int i = dwReturn; i > -1; i--)
				{
					if(tstrProcessFullpath[i] == '\\')
					{	
						// move forward and terminate string
						tstrProcessFullpath[++i] = 0;
						
						// we're outta here
						break;
					}
				}

				// assign to member field
				m_strApplicationFolder = tstrProcessFullpath;

				// set success val
				bReturn = TRUE;
			}
		}
		else
		{
			// display last error
			m_strLastError = _T("Could not retrieve this application's process information.");
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While running checks on the application's executable, an exception occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}


bool CSettings::IsValidClockNum(size_t clock_num) const
{
  return ((clock_num >= 0) && (clock_num < TZ_COUNT));  
}


void CSettings::TZName( size_t clock_num, const TCHAR* name )
{
  if (IsValidClockNum(clock_num))
    m_strTZName[clock_num] = name;
}


const TCHAR* CSettings::TZName( size_t clock_num )
{
  if (IsValidClockNum(clock_num))
    return m_strTZName[clock_num].c_str();
  else
    return NULL;
}


long CSettings::TZHourBias( size_t clock_num ) const
{
  if (IsValidClockNum(clock_num))
    return m_tzBias[clock_num];
  else
    return 0;
}


void CSettings::TZHourBias( size_t clock_num, long hour_bias )
{
  if (IsValidClockNum(clock_num))
    m_tzBias[clock_num] = hour_bias;
}
