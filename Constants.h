#ifndef _CONSTANTS_
#define _CONSTANTS_
#endif
///////////////////////////////////////////////////////////////////////////////
// Module Constants
///////////////////////////////////////////////////////////////////////////////

// Layout Positioning and Count
#define LAYOUT_FILEMANAGER1_WIDTH			0.25f
#define LAYOUT_FILEMANAGER1_HEIGHT			0.40f
#define LAYOUT_FILEMANAGER2_WIDTH			0.25f
#define LAYOUT_FILEMANAGER2_HEIGHT			0.40f
#define LAYOUT_VIEWER_WIDTH					0.75f
#define LAYOUT_VIEWER_HEIGHT				0.80f
#define LAYOUT_PROGRESSBAR_WIDTH			25
#define LAYOUT_PROGRESSBAR_HEIGHT			0.0125f
#define LAYOUT_COMMANDPROMPT_WIDTH			0.99f
#define LAYOUT_COMMANDPROMPT_HEIGHT			0.15f
#define LAYOUT_COMMANDBUTTONS_WIDTH			0.99f
#define LAYOUT_COUNT_COMMANDBUTTONS			12.0f
#define LAYOUT_SPACING						2
#define LAYOUT_TOP_SPACING					4
#define LAYOUT_RADIUS_COMMANDBUTTONS		7
#define LAYOUT_PERCENTAGE_WIDTH				30
#define LAYOUT_TABPAGE_WIDTH				108

// Layout Strings
#define LAYOUT_STRING_NOFILESELECTED		_T("No File Currently Selected")
#define LAYOUT_STRING_LOADINGFILELIST		_T("Loading File List...")

// File Manager
#define SEARCHKEY_DIRECTORY			_T("<DIR>")
#define SEARCHKEY_DRIVE				_T("<DRIVE>")
#define SEARCHKEY_STARTPOS_FILENAME 29
#define FOLDER_MACHINEROOT			_T("****")
#define FOLDER_PARENT				_T("...")
#define FOLDER_ROOT					_T("\\\\")
#define FORMAT_DIRECTORY			_T("<DIR>         | %-*s | %02d.%02d.%4d | %02d:%02d | %s | %s |")
//#define FORMAT_DIRECTORY_TV			_T("%s  <DIR>  %02d.%02d.%4d %02d:%02d %s %s")
#define FORMAT_DIRECTORY_TV			_T("%s  <DIR>  %02d.%02d.%4d %s")
#define FORMAT_FILE					_T("%13s | %-*s | %02d.%02d.%4d | %02d:%02d | %s | %s |")
#define FORMAT_FILE_TV				_T("%s  <FILE> %Ld %02d.%02d.%4d %s")

#define FORMAT_FILE1				_T("%s")
#define FORMAT_DRIVE				_T("%c: <DRIVE> %-*s %s")
#define FORMAT_DRIVE1				_T("%c:")
#define INDEX_MACHINEROOT			0
#define INDEX_PARENTFOLDER			1
#define THOUSANDS_SEPARATOR			_T(".")
#define DECIMAL_SEPARATOR			_T(".")
#define MAX_FILEOBJECT_NAMELENGTH	25

// File Objects
#define FILEEXTENSIONS_AUTOCAD_ALL	_T("DWG,dwg,Dwg,dWg,dwG")
#define MASK_ALLFILESDIRECTORIES	_T("*")
#define MASK_ALLFILES				_T("*.*")
#define FILENAME_CADIMPORTERLIBRARY _T("CADImporter.dll")

// API Constants
#define SHIFTED						0x8000

// Messages
#define MESSAGE_TRIALVERSION		_T("xlance@gmx.at")

//Parth Software Solution
#define TVFOLDER_ROOT					_T("NODE 0000")
//Parth Software Solution



#pragma pack(1)
typedef struct
{														
	int						eMessageInfo;				
	int						iSizeOfData;
	BYTE					bData[4000];
}XLV_PIPE_DATA_REG, *LPXLV_PIPE_DATA_REG;
#pragma pack()

const int SIZE_OF_XLV_PIPE_DATA_REG = sizeof(XLV_PIPE_DATA_REG);

//Named Pipe contants
const int MAX_PIPE_LENGTH = 6144;
const int MAX_PIPE_RETRY_TIMEOUT  = 1000;
const int MAX_PIPE_RETRY_COUNT  = 3;

// C++ Server
static const char* _NAMED_PIPE_SERVER			= "\\\\.\\pipe\\UIServer";
static const char* _NAMED_PIPE_CLIENT			= "\\\\.\\pipe\\UIClient";