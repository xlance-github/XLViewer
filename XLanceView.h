#ifndef _XLANCEVIEW_
#define _XLANCEVIEW_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:  
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <tchar.h>

#define APPCLASS			"XLanceViewer"		
#define MAINWINDOW_TITLE	_T("XLance View")

///////////////////////////////////////////////////////////////////////////////
// Registry Constants
///////////////////////////////////////////////////////////////////////////////

	////	Main "Key"
	#define REG_BASE					_T("Software\\XLance\\XLance View")

	////	Section Names
	#define REG_SECTION_PREFERENCES					_T("Preferences")
	#define REG_SECTION_SETTINGS					_T("Settings")
	#define REG_SECTION_OPTIONS						_T("Options")

	////	Value Names	
	#define REG_VAL_PREFS_LOCATION_SETTINGS			_T("Location-settings")
	#define REG_VAL_PREFS_LOCATION_STARTUP			_T("Location-startup")
	#define REG_VAL_PREFS_LOCATION_FILEATTRIBUTES	_T("Location-file-attributes")
	#define REG_VAL_PREFS_LOCATION_MAINWINDOW		_T("Location-main-window")
	#define REG_VAL_PREFS_LOCATION_CREATEDIRECTORY	_T("Location-create-directory")
	#define REG_VAL_PREFS_LOCATION_OPTIONS			_T("Location-options")
	#define REG_VAL_PREFS_LOCATION_SELECTFILES		_T("Location-select-files")
	#define REG_VAL_PREFS_LOCATION_RENAMEOBJECT		_T("Location-rename-files")
	#define REG_VAL_PREFS_LOCATION_HELP				_T("Location-help")
	#define REG_VAL_PREFS_LOCATION_ABOUT			_T("Location-about")
	#define REG_VAL_PREFS_LOCATION_DWGINFO			_T("Location-DWG-information")
	#define REG_VAL_PREFS_LOCATION_LAYERCONTROL		_T("Location-layer-control")
	#define REG_VAL_PREFS_WINDOWSTATE_MAINWINDOW	_T("Windowstate-main-window")
	#define REG_VAL_SETS_ALWAYSLAUNCHFULLSCREEN		_T("Launch-fullscreen")
	#define REG_VAL_SETS_GRAPHICSDEVICE				_T("Graphics-device")
	#define REG_VAL_SETS_GRAPHICSMODE				_T("Graphics-mode")
	#define REG_VAL_SETS_LASTFOLDER_FILEMANAGER1	_T("Last-folder-FM1")
	#define REG_VAL_SETS_LASTFOLDER_FILEMANAGER2	_T("Last-folder-FM2")
	#define REG_VAL_SETS_TEXTCOLOR_FILEMANAGER1		_T("Textcolor-file-manager1")
	#define REG_VAL_SETS_TEXTCOLOR_FILEMANAGER2		_T("Textcolor-file-manager2")
	#define REG_VAL_SETS_HIGHLIGHT_FILEMANAGER1		_T("Highlight-file-manager1")
	#define REG_VAL_SETS_HIGHLIGHT_FILEMANAGER2		_T("Highlight-file-manager2")
	#define REG_VAL_SETS_TEXTCOLOR_COMMANDPROMPT	_T("Textcolor-command-prompt")
	#define REG_VAL_SETS_BACKGROUND_COMMANDPROMPT	_T("Background-command-prompt")
	#define REG_VAL_SETS_CUSTOMCOLORS				_T("Custom-colors")
	#define REG_VAL_SETS_TZ_BIAS				          _T("TimezoneBias%d")
	#define REG_VAL_SETS_TZ_NAME				          _T("TimezoneName%d")
	
///////////////////////////////////////////////////////////////////////////////
// End Registry Constants
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Application Message Constants
///////////////////////////////////////////////////////////////////////////////

#define AM_LOADINGCOMPLETE			0xBFFF
#define AM_RUNPROCESSES				0xBFFE
#define AM_REFRESHFILEMANAGERS		0xBFFD

///////////////////////////////////////////////////////////////////////////////
// Application Message Constants
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// String Constants
///////////////////////////////////////////////////////////////////////////////

#define EMPTY_STRING				_T("")
#define RESOURCE_TYPE_FILES			_T("FILES")
#define RESOURCE_TYPE_AUTOCAD		_T("AutoCad")
#define RESOURCE_TYPE_ANICURSORS	_T("AnimatedCursors")

///////////////////////////////////////////////////////////////////////////////
// End String Constants
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Hotkey Constants
///////////////////////////////////////////////////////////////////////////////

#define HOTKEY_ESCAPE					10001

///////////////////////////////////////////////////////////////////////////////
// End Hotkey Constants
///////////////////////////////////////////////////////////////////////////////

#endif