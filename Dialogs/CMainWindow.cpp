///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   CMainWindow object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <StdAfx.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <time.h>
#include <memory>
#include <math.h>
#include <fstream>
#include <map>
#include <vector>
#include <shellapi.h>
#include <Winerror.h>
#ifdef _DEBUG
	#include <strstream>
#endif
#include "..\XLanceView.h"
#include "..\LinkedList.h"
#include "..\Common\Registry.h"
#include "..\Common\FileIO.h"
#include "..\Resource\Resource.h"
#include "..\Settings\CSettings.h"
#include "..\Splitter\easysplit.h"
#include "..\Utility\CGraphicsDeviceInformation.h"
#include "..\Utility\CAttachedDrives.h"
#include "..\Utility\CCapturedCommandPrompt.h"
#include "..\DWG\CDWGRenderEngine.h"
#include "..\Communication\XlvCommunicator.h"
#include "CMainWindow.h"
#include "CCreateDirectoryDialog.h"
#include "CSettingsDialog.h"
#include "COptionsDialog.h"
#include "CFileAttributesDialog.h"
#include "CSelectFilesDialog.h"
#include "CRenameFileDirectoryDialog.h"
#include "CHelpDialog.h"
#include "CDWGInformationDialog.h"
#include "CLayerControlDialog.h"
#include "Win32TreeView.h"


using namespace std;
#pragma warning( disable : 4996 )

const LPTSTR ZEROTHTAB    = "Zero Page";
const LPTSTR FIRSTTAB    = "First Page";
const LPTSTR SECONDTAB   = "Second Page";
const LPTSTR THIRDTAB    = "Third Page";

///////////////////////////////////////////////////////////////////////////////
// Module level vars
///////////////////////////////////////////////////////////////////////////////

static CMainWindow *pcmwndThis = NULL;
extern CSettings g_csetApplication;
extern CGraphicsDeviceInformation g_cginfPrimaryDevice;
extern HWND g_hwndApplication;
extern HINSTANCE hAppInstance;
int CMainWindow::m_iRange = 1;
int CMainWindow::m_TempiRange = 1;



///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor, initializes all fields to their defaults.
 */
CMainWindow::CMainWindow()
{
	// initialize fields to their defaults
	m_hinstApplication = NULL;
	m_hwndThis = NULL;
	m_hwndActiveFileManager = NULL;
	m_hfontControls = NULL;
	m_hbrControlBackgrounds = NULL;
	m_hbrCommandPromptBackground = NULL;
	m_hbrTitlebarBackground = NULL;
	m_wndprocPreviousFileManager1 = NULL;
	m_wndprocPreviousFileManager2 = NULL;
	m_wndprocPreviousCommandPrompt = NULL;
	m_wndprocPreviousCommandButtons = NULL;
	m_wndprocPreviousProductName = NULL;
	m_wndprocPreviousTrialVersion = NULL;
	m_pllstFileManager1 = new LinkedListEx<FILE_INFORMATION>();
	m_pllstFileManager2 = new LinkedListEx<FILE_INFORMATION>();
	
	m_pllstActiveFileManager = NULL;
	m_arrctCommandButtons = NULL;
	m_ariFileManager1Selection = NULL;
	m_ariFileManager2Selection = NULL;
	m_iCurrentSelectionCountFileManager1 = 0L;
	m_iCurrentSelectionCountFileManager2 = 0L;
	m_iLastButtonIndex = -1;
	m_iLastButtonState = -1;
	m_ccapcmdThis = NULL;
	m_cdwgengThis = NULL;
	m_strLastError = EMPTY_STRING;
	m_iWindowState = SIZE_RESTORED;
	m_aseActiveSort = aseCustom;
	m_bGraphicsDeviceModeChanged = FALSE;
	m_bIsChangingWindowMode = FALSE;
	m_bInFullScreenMode = FALSE;
	m_pFirstTabDlg = NULL;
	// set module static so message loop can have access to *this*
	pcmwndThis = this;

	//Parth Software Solution
	Root = NULL;
	Root2 = NULL;

	m_ActNameSort = aseNameAsc;
	m_ActExtSort = aseExtensionAsc;
	m_ActSizeSort = aseSizeAsc;
	m_ActDateSort = aseDateAsc;
	m_ActDirSort = aseDirAsc;
	//Parth Software Solution
}

/**
 * Constructor which accepts the instance, the parent and the fullpath to
 * the file whose attributes should be displayed.
 *
 * @param hInstance the application instance
 *
 */
CMainWindow::CMainWindow(HINSTANCE hInstance)
{
	try
	{
		// initialize fields 
		m_hwndThis = NULL;
		m_hwndActiveFileManager = NULL;
		m_hwndProgressBar = NULL;
		m_hfontControls = NULL;
		m_hbrControlBackgrounds = NULL;
		m_hbrCommandPromptBackground = NULL;
		m_hbrTitlebarBackground = NULL;
		m_wndprocPreviousFileManager1 = NULL;
		m_wndprocPreviousFileManager2 = NULL;
		m_wndprocPreviousCommandPrompt = NULL;
		m_wndprocPreviousCommandButtons = NULL;
		m_wndprocPreviousProductName = NULL;
		m_wndprocPreviousTrialVersion = NULL;
		m_pllstFileManager1 = new LinkedListEx<FILE_INFORMATION>();
		m_pllstFileManager2 = new LinkedListEx<FILE_INFORMATION>();
		m_pllstActiveFileManager = NULL;
		m_arrctCommandButtons = NULL;
		m_ariFileManager1Selection = NULL;
		m_ariFileManager2Selection = NULL;
		m_iCurrentSelectionCountFileManager1 = 0L;
		m_iCurrentSelectionCountFileManager2 = 0L;
		m_iLastButtonIndex = -1;
		m_iLastButtonState = -1;
		m_ccapcmdThis = NULL;
		m_cdwgengThis = NULL;
		m_strLastError = EMPTY_STRING;
		m_iWindowState = SIZE_RESTORED;
		m_aseActiveSort = aseCustom;
		m_bGraphicsDeviceModeChanged = FALSE;
		m_bIsChangingWindowMode = FALSE;
		m_bInFullScreenMode = FALSE;
		m_hinstApplication = hInstance;
		m_pFirstTabDlg = NULL;
		// set module static so message loop can have access to *this*
		pcmwndThis = this;

		//Parth Software Solution
		Root = NULL;
		Root2 = NULL;

		m_ActNameSort = aseNameAsc;
		m_ActExtSort = aseExtensionAsc;
		m_ActSizeSort = aseSizeAsc;
		m_ActDateSort = aseDateAsc;
		m_ActDirSort = aseDirAsc;
		//Parth Software Solution
	}
	catch(...)
	{
		// garbage collect, clear references
		m_hinstApplication = NULL;

		// set last error
		m_strLastError = _T("While intializing the settings dialog object, an unspecified exception occurred.");
	}
}
                                 
/**
 * Destructor, performs clean-up on fields.
 */
CMainWindow::~CMainWindow()
{
	if(m_pFirstTabDlg != NULL)
	{
		delete m_pFirstTabDlg;
		m_pFirstTabDlg = NULL;	
	}
	// perform final garbage collection
	if(m_ccapcmdThis)
		delete m_ccapcmdThis;
	if(m_cdwgengThis)
		delete m_cdwgengThis;
	
	// File Manager file lists
	//	 1
	if(m_pllstFileManager1)
	{
		// clear list's contents
		m_pllstFileManager1->clear();

		// destroy list
		delete m_pllstFileManager1;
	}
	//	 2
	if(m_pllstFileManager2)
	{
		// clear list's contents
		m_pllstFileManager2->clear();

		// destroy list
		delete m_pllstFileManager2;
	}
	
	// Command Button rectangle array
	if(m_arrctCommandButtons)
	{
		RECT *prctTemp = NULL;

		// destroy contents
		for(int i = 0; i < LAYOUT_COUNT_COMMANDBUTTONS; i++)
		{
			// get rect
			prctTemp = m_arrctCommandButtons[i];

			// destroy
			delete prctTemp;

			// deref
			m_arrctCommandButtons[i] = NULL;
		}

		// destroy array
		delete[] m_arrctCommandButtons;
		m_arrctCommandButtons = NULL;
	}
	// Command Button image array
	if(m_arbmpCommandButtons)
	{
		// destroy contents
		for(int i = 0; i < LAYOUT_COUNT_COMMANDBUTTONS; i++)
		{
			// destroy
			if(m_arbmpCommandButtons[i])
				DeleteObject(m_arbmpCommandButtons[i]);
			
			// deref
			m_arbmpCommandButtons[i] = NULL;
		}

		// destroy array - not needed since array is NOT
		//	 allocated dynamically
	}

	// File Manager Current Selection Lists
	//	 1
	if(m_ariFileManager1Selection)
		delete[] m_ariFileManager1Selection;
	//	 2
	if(m_ariFileManager2Selection)
		delete[] m_ariFileManager2Selection;

	// destroy GDI objects
	if(m_hfontControls)
		DeleteObject(m_hfontControls);
	if(m_hbrControlBackgrounds)
		DeleteObject(m_hbrControlBackgrounds);
	if(m_hbrCommandPromptBackground)
		DeleteObject(m_hbrCommandPromptBackground);
	if(m_hbrTitlebarBackground)
		DeleteObject(m_hbrTitlebarBackground);
}

/**
 * Handles the processing of all messages for *this* object's GUI
 * (dialog).
 *
 * @param hwnd Handle to the window whose message pump this represents. In
 * this case, *this* dialog.
 *
 * @param uMsg Message to be processed
 *
 * @param wParam WPARAM for the current message
 *
 * @param lParam LPARAM for the current message
 *
 * @return 0 if the message is processed by this message pump, 
 * otherwise DefWindowProc() is called and its return value used.
 */
LRESULT CALLBACK CMainWindow::WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	PMEASUREITEMSTRUCT pmisTemp = NULL;
	PDRAWITEMSTRUCT pdisTemp = NULL;
	POINT ptTemp;
	HWND hwndTemp = NULL;
	HDC hdcTemp = NULL;
	HBRUSH hbrTemp = NULL,
		hbrReturn = NULL;
	RECT *lprctTemp = NULL,
		rctTemp;
	int iState = 0,
		iCtrlID = 0;

	int iDlgCntrlID = GetDlgCtrlID(pcmwndThis->m_hwndActiveFileManager);

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			// set handles
			pcmwndThis->m_hwndThis = hwnd;
			g_hwndApplication = hwnd;

			// load this dialog's preferences
			pcmwndThis->loadPreferences();

			// set icon
			pcmwndThis->loadIcon();

			// set control fonts
			pcmwndThis->setFonts();

			// Initialize command prompt
			pcmwndThis->initializeCommandPrompt();

			// check run full screen option
			pcmwndThis->initializeWindow();

			// Force initial layout refresh
			pcmwndThis->refreshLayout(TRUE);

			// subclass controls
			pcmwndThis->subclassControls();

			// Wait for all redraws to finalize...
			pcmwndThis->doEvents();

			// initialize File Manager windows
			pcmwndThis->initializeFileManagers();

			// initialize Tab Control
			pcmwndThis->initializeTabControl();

			//set the window text to handle the FindWindow
			SetWindowText(hwnd, "XLance View");

			//start named pipe thread here.
			pcmwndThis->StartNamedPipeThread();

			//Create all tab Pages
			pcmwndThis->CreateTabPageDialogs();

			// set focus to File Manager 1 and make it active
			hwndTemp = GetDlgItem(hwnd, IDC_TVFILEMANAGER1);
			if(hwndTemp)
			{
				// move input focus
				SetFocus(hwndTemp);

				// make the active File Manager
				pcmwndThis->m_hwndActiveFileManager = hwndTemp;
				pcmwndThis->m_pllstActiveFileManager =
					pcmwndThis->m_pllstFileManager1;

				// set active folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, 
					IDC_TXTCMDPROMPTCONSOLE);
				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
					g_csetApplication.lastFolderFileManager1(), hwndTemp);
			}

			//Parth Software Solution
			// set focus to tree view File Manager and make it active
			hwndTemp = GetDlgItem(hwnd, IDC_TVFILEMANAGER1);
			if(hwndTemp)
			{
				// move input focus
				SetFocus(hwndTemp);

				// make the active File Manager
				pcmwndThis->m_hwndActiveFileManager = hwndTemp;
				pcmwndThis->m_pllstActiveFileManager =
					pcmwndThis->m_pllstFileManager1;

				// set active folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, 
					IDC_TXTCMDPROMPTCONSOLE);

				//Parth Software Solution
				//set root item
				//Parth Software Solution
				//pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
				//	  g_csetApplication.lastFolderFileManager1(), hwndTemp);
				//Parth Software Solution
			}
			//Parth Software Solution

			SetTimer(hwnd, 500, 1000, NULL);
			pcmwndThis->progress_bar_handle( GetDlgItem(hwnd, IDC_PRGBRMAIN) );

			HWND hCP = GetDlgItem(hwnd, IDC_TXTCMDPROMPT);
			SendMessage( 
				hCP,              // handle to destination window 
				EM_SETEVENTMASK,          // message to send
				0,          // not used; must be zero
				SendMessage(hCP,EM_GETEVENTMASK,0,0) | ENM_SELCHANGE);
			HWND hCPC = GetDlgItem(hwnd, IDC_TXTCMDPROMPTCONSOLE);
			SendMessage( 
				hCPC,              // handle to destination window 
				EM_SETEVENTMASK,          // message to send
				0,          // not used; must be zero
				SendMessage(hCPC,EM_GETEVENTMASK,0,0) | ENM_SELCHANGE);

			pcmwndThis->SetPercentage(0);

			pcmwndThis->EditsBkColor();

			// Initialize DWG render engine... moved this to the last item
			//	 because in the event the CAD Importer library needs to be
			//	 extracted, I don't want the application to appear like
			//	 something went wrong or is hung.
			pcmwndThis->initializeDWGRenderEngine();


			break;
		}
	case WM_CHAR:
		switch(wParam)
		{
		case 9:
			// Get Shift key's state
			iState = GetKeyState(VK_SHIFT);
			// Set Focus to File Manager 1
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
				IDC_TVFILEMANAGER1);
			if(hwndTemp)
				SetFocus(hwndTemp);

			// swallow message
			return 0L;
		}
	case WM_DESTROY:
		{
			// restore graphics device mode
			pcmwndThis->restoreGraphicsDeviceMode();

			// perform shutdown processes, if applicable
			if(!pcmwndThis->isChangingGraphicModes())
			{
				// save preferences for this dialog
				pcmwndThis->savePreferences();

				// shutdown application
				PostQuitMessage(0);
			}

			// de-ref handle
			pcmwndThis->m_hwndThis = NULL;
			break;
		}
	case WM_CLOSE:
		{
			//DestroyWindow(hwnd); 'from this getting the crash since the message loop not getting stop.
			PostQuitMessage(0);
			break;
		}
	case WM_CTLCOLORSTATIC:
		{			
			HWND hControlWnd = (HWND)lParam;
			iCtrlID = GetDlgCtrlID(hControlWnd);
			HDC hdc = reinterpret_cast<HDC>(wParam);
			HBRUSH ret_br = 0;
			if(iCtrlID == IDC_STATIC_PROGRESS_TEXT)
			{
				SetBkColor(hdc, RGB(0,0,0));
				SetTextColor(hdc, RGB(255,255,255));
				ret_br = (HBRUSH)CreateSolidBrush(RGB(0,0,0));
			}
			return reinterpret_cast<LRESULT>(ret_br);
		}
		break;
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORDLG:
		{
			// get control ID
			HWND hControlWnd = (HWND)lParam;
			iCtrlID = GetDlgCtrlID(hControlWnd);
			HDC hdc = reinterpret_cast<HDC>(wParam);
			COLORREF bg = 0;
			HBRUSH ret_br = 0;
			if (!pcmwndThis->SetColorsForDC(hdc, iCtrlID, false, &bg))
			{
				ret_br = (HBRUSH)GetStockObject(SYSTEM_FIXED_FONT);
			}
			else
			{
				CtrlID2Brush::const_iterator c2b_it = pcmwndThis->ctrl2br.find(iCtrlID);
				if (c2b_it == pcmwndThis->ctrl2br.end())
				{
					HBRUSH ret_br = CreateSolidBrush(bg);
					pcmwndThis->ctrl2br[iCtrlID] = ret_br;
					return reinterpret_cast<LRESULT>(ret_br);
				}
				ret_br = c2b_it->second;
			}
			return reinterpret_cast<LRESULT>(ret_br);
		}
		break;

	case WM_PAINT:
		// get button bar's handle
		hwndTemp = GetDlgItem(hwnd, IDC_PICBUTTONS);

		// validate, continue
		if(hwndTemp)
			InvalidateRect(hwndTemp, NULL, TRUE);

		// exit, do not indicate message was processed
		return FALSE;

	case WM_SIZE:
		{
			// store window state
			if(wParam == SIZE_MAXIMIZED)
				pcmwndThis->m_iWindowState = SW_MAXIMIZE;
			else if(wParam == SIZE_RESTORED)
				pcmwndThis->m_iWindowState = SW_NORMAL;
			else if(wParam == SIZE_MINIMIZED)
			{
				pcmwndThis->m_iWindowState = SW_MINIMIZE;
				break;//Need to break here ** Parth Info Solution
			}
			// refresh layout
			pcmwndThis->refreshLayout();
			break;
		}
	case WM_SIZING:
		// Attempt to get rectangle
		lprctTemp = (RECT *)lParam;
		if(lprctTemp != NULL)
		{
			// Constrain size to 640x480
			if((lprctTemp->right - lprctTemp->left) < 640)
				lprctTemp->right = lprctTemp->left + 640;
			if((lprctTemp->bottom - lprctTemp->top) < 480)
				lprctTemp->bottom = lprctTemp->top + 480;
		}
		break;

	case WM_TIMER:
		{
			if (HWND hTimeLabel = GetDlgItem(hwnd, IDC_LBLTIME))
			{
				time_t current_time;
				time(&current_time);
				tm* gmt_time = gmtime(&current_time);
				tm cur_gmt_time = *gmt_time;
				TCHAR buf[1000] = _T("\0");
				for (size_t clock_it = 0; clock_it < TZ_COUNT; ++clock_it)
				{
					long hour_bias = g_csetApplication.TZHourBias(clock_it);

					tm cur_tz_time = cur_gmt_time;
					cur_tz_time.tm_hour -= hour_bias;

					time_t cur_timet = mktime(&cur_tz_time);
					cur_tz_time = *localtime(&cur_timet);

					if (clock_it)
						_stprintf(buf+_tcslen(buf), _T(", "));
					_tcsftime(buf+_tcslen(buf), 100, _T("%H:%M:%S"), &cur_tz_time);
					_stprintf(buf+_tcslen(buf), _T(" GMT%c%d"), hour_bias<=0?_T('+'):_T('-'), abs(hour_bias));
				}

				// count label size
				HDC hLabelDC = GetDC(hTimeLabel);
				SIZE szText;
				GetTextExtentPoint32(hLabelDC, buf, _tcslen(buf), &szText);
				SetWindowPos(hTimeLabel, 0, 0, 0, szText.cx, szText.cy, SWP_NOMOVE | SWP_NOZORDER);
				SetWindowText(hTimeLabel, buf);
			}
			break;
		}

	case WM_APP:
		switch(wParam)
		{
		case AM_LOADINGCOMPLETE:
			PostMessage((HWND)lParam, WM_APP, (WPARAM)AM_RUNPROCESSES, 0L);
			break;

		default:
			break;
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			//Custom messages here
		case ID_UPDATE_TAB_CONTROL:
			// display help window
			pcmwndThis->displayHelp();

			break;

		case IDC_CMDF1:
			// display help window
			pcmwndThis->displayHelp();
			break;

			

		case IDC_CMDF2:
			// retrieve info for the active DWG
			pcmwndThis->getDWGInformation();
			break;

		case IDC_CMDF3:
			// store current position preferences before changing to 
			//	 full screen
			pcmwndThis->savePreferences();

			// change this object's windowstate
			pcmwndThis->changeWindowMode(TRUE);
			// refresh layout
			pcmwndThis->refreshLayout();
			break;

		case IDC_CMDF4:
			// open the active DWG in AutoCad
			pcmwndThis->openInAutoCad();
			break;

		case IDC_CMDF5:
			// copy files / directories
			pcmwndThis->copyFilesAndDirectories();
			break;

		case IDC_CMDF6:
		case IDC_CMD_SHIFT_F6:
			{
				int iState = GetKeyState(VK_SHIFT);
				if (iState & SHIFTED)
				{
					// Rename files / directories
					return pcmwndThis->renameFilesAndDirectories();
				}
				else
				{
					// Move files / directories between file managers
					return pcmwndThis->operateFSOFromTo(FO_MOVE);
				}
				break;
			}
		case IDC_CMDF7:
			// Create new directory
			pcmwndThis->createNewDirectory();
			break;

		case IDC_CMDF8:
			// Delete files / directories
			pcmwndThis->deleteFilesAndDirectories();
			break;

		case IDC_CMDF9:
			// Display options dialog
			pcmwndThis->displayOptions();
			break;

		case IDC_CMDF10:
			//DestroyWindow(hwnd);'from this getting the crash since the message loop not getting stop.
			PostQuitMessage(WM_QUIT);
			break;

		case IDC_CMDF11:
			// display file attributes / permissions for selected file or directory
			pcmwndThis->displayFileAttributesAndPermissions();
			break;

		case IDC_CMDF12:
			// display settings dialog
			pcmwndThis->displaySettings();	
			break;

		case ID_ACCLZOOMIN:
			// increase zoom
			pcmwndThis->m_cdwgengThis->zoomIn();
			break;

		case ID_ACCLZOOMOUT:
			// decrease zoom
			pcmwndThis->m_cdwgengThis->zoomOut();
			break;

		case ID_ACCLLAYERS:
			pcmwndThis->displayLayerControl();
			break;

		case ID_ACCLSELECTALL:
			// Select all files in currently active file manager.
			pcmwndThis->selectAllFileObjects();
			break;

		case ID_ACCLEXITFULLSCREEN:
			// restore window to "windowed" state
			pcmwndThis->changeWindowMode();
			pcmwndThis->refreshLayout();
			break;

		case ID_ACCLSORTBYNAME:
		case IDC_BTNNAME1:
		case IDC_BTNNAME2:
			if(iDlgCntrlID == IDC_TVFILEMANAGER1)
				pcmwndThis->SortTV1AsPerName1(IDC_TVFILEMANAGER1);
			else
				pcmwndThis->SortTV1AsPerName1(IDC_TVFILEMANAGER2);
			break;
			// sort active file manager's file list by name... reverse
			//	 sort if already sorted this way

			//// set active sort
			//if(pcmwndThis->m_aseActiveSort == aseNameAsc)
			//	pcmwndThis->m_aseActiveSort = aseNameDesc;
			//else
			//	pcmwndThis->m_aseActiveSort = aseNameAsc;

			//// clear and re-display active File Manager's contents
			//pcmwndThis->sortDirectoryListing();

			//// repopulate "lookup" list
			//pcmwndThis->copyFileListToLookupList();
			//break;

		case ID_ACCLSORTBYEXT:
		case IDC_BTNEXT1:
		case IDC_BTNEXT2:
			if(iDlgCntrlID == IDC_TVFILEMANAGER1)
				pcmwndThis->SortTV1AsPerExt1(IDC_TVFILEMANAGER1);
			else
				pcmwndThis->SortTV1AsPerExt1(IDC_TVFILEMANAGER2);
			break;					// sort active file manager's file list by date... reverse
			//	 sort if already sorted this way

			// set active sort
			//if(pcmwndThis->m_aseActiveSort == aseDateAsc)
			//	pcmwndThis->m_aseActiveSort = aseDateDesc;
			//else
			//	pcmwndThis->m_aseActiveSort = aseDateAsc;

			//// clear and re-display active File Manager's contents
			//pcmwndThis->sortDirectoryListing();

			//// repopulate "lookup" list
			//pcmwndThis->copyFileListToLookupList();
			//break;

		case ID_ACCLSORTBYSIZE:
		case IDC_BTNSZ1:
		case IDC_BTNSZ2:
			if(iDlgCntrlID == IDC_TVFILEMANAGER1)
				pcmwndThis->SortTV1AsPerSize1(IDC_TVFILEMANAGER1);
			else
				pcmwndThis->SortTV1AsPerSize1(IDC_TVFILEMANAGER2);
			break;
			//// sort active file manager's file list by size... reverse
			////	 sort if already sorted this way
			//
			//// set active sort
			//if(pcmwndThis->m_aseActiveSort == aseSizeAsc)
			//	pcmwndThis->m_aseActiveSort = aseSizeDesc;
			//else
			//	pcmwndThis->m_aseActiveSort = aseSizeAsc;

			//// clear and re-display active File Manager's contents
			//pcmwndThis->sortDirectoryListing();

			//// repopulate "lookup" list
			//pcmwndThis->copyFileListToLookupList();
			//break;

		case ID_ACCLSORTBYTYPE:
		case IDC_BTNALTR1:
		case IDC_BTNALTR2:
			if(iDlgCntrlID == IDC_TVFILEMANAGER1)
				pcmwndThis->SortTV1AsPerAlt1(IDC_TVFILEMANAGER1);
			else
				pcmwndThis->SortTV1AsPerAlt1(IDC_TVFILEMANAGER2);
			break;
			// sort active file manager's file list by type... reverse
			//	 sort if already sorted this way

			// set active sort
			//if(pcmwndThis->m_aseActiveSort == aseTypeAsc)
			//	pcmwndThis->m_aseActiveSort = aseTypeDesc;
			//else
			//	pcmwndThis->m_aseActiveSort = aseTypeAsc;

			//// clear and re-display active File Manager's contents
			//pcmwndThis->sortDirectoryListing();

			//// repopulate "lookup" list
			//pcmwndThis->copyFileListToLookupList();
			//break;

		case ID_ACCLSORTBYDATE:
		case IDC_BTNDATE1:
		case IDC_BTNDATE2:
			if(iDlgCntrlID == IDC_TVFILEMANAGER1)
				pcmwndThis->SortTV1AsPerDate1(IDC_TVFILEMANAGER1);
			else
				pcmwndThis->SortTV1AsPerDate1(IDC_TVFILEMANAGER2);
			break;

		case ID_ACCLSORTBYPERM:
		case IDC_BTNDIR1:
		case IDC_BTNDIR2:
			if(iDlgCntrlID == IDC_TVFILEMANAGER1)
				pcmwndThis->SortTVAsPerDir(IDC_TVFILEMANAGER1);
			else
				pcmwndThis->SortTVAsPerDir(IDC_TVFILEMANAGER2);
			break;



			// sort active file manager's file list by type... reverse
			//	 sort if already sorted this way

			// set active sort
			//if(pcmwndThis->m_aseActiveSort == aseExtensionAsc)
			//	pcmwndThis->m_aseActiveSort = aseExtensionDesc;
			//else
			//	pcmwndThis->m_aseActiveSort = aseExtensionAsc;

			//// clear and re-display active File Manager's contents
			//pcmwndThis->sortDirectoryListing();

			//// repopulate "lookup" list
			//pcmwndThis->copyFileListToLookupList();
			//break;

			//Parth Software Solution
			// handle event for sorted buttons
			//case IDC_BTNALTR1:
			//	pcmwndThis->SortTV1AsPerPermission1(IDC_TVFILEMANAGER1);
			//	break;


			//case IDC_BTNNAME2:
			//	pcmwndThis->SortTV1AsPerName1(IDC_TVFILEMANAGER2);
			//	break;
			//case IDC_BTNEXT2:
			//	pcmwndThis->SortTV1AsPerExt1(IDC_TVFILEMANAGER2);
			//	break;
			//case IDC_BTNSZ2:
			//	pcmwndThis->SortTV1AsPerSize1(IDC_TVFILEMANAGER2);
			//	break;
			//case IDC_BTNDATE2:
			//	pcmwndThis->SortTV1AsPerDate1(IDC_TVFILEMANAGER2);
			//	break;

			//case IDC_BTNPERM2:
			//	pcmwndThis->SortTV1AsPerPermission1(IDC_TVFILEMANAGER2);
			//	break;
			//Parth Software Solution
		default:	// do nothing
			break;
		}
		break;

	case WM_LBUTTONUP:
		// get handle to product label
		hwndTemp = GetDlgItem(hwnd, IDC_LBLPRODUCTNAME);

		// check, continue
		if(hwndTemp)
		{
			// get window rect
			GetWindowRect(hwndTemp, &rctTemp);

			// create point
			ptTemp.x = LOWORD(lParam);
			ptTemp.y = HIWORD(lParam);

			// check and see if mouse falls within rectangle... NOTE: no
			//	 translation of the point is required because this only
			//	 applies (the actions) if the viewer is in fullscreen
			//	 mode.
			if(PtInRect(&rctTemp, ptTemp))
			{
				// restore window to "windowed" state
				pcmwndThis->changeWindowMode();

				// nothing else to do, exit
				break;
			}
		}

		// get handle to trial label
		//hwndTemp = GetDlgItem(hwnd, IDC_LBLTRIALVERSION);

		// check, continue
		///if(hwndTemp)
		//{
		//	// get window rect
		//	GetWindowRect(hwndTemp, &rctTemp);

		//	// create point
		//	ptTemp.x = LOWORD(lParam);
		//	ptTemp.y = HIWORD(lParam);

		//	// check and see if mouse falls within rectangle... NOTE: no
		//	//	 translation of the point is required because this only
		//	//	 applies (the actions) if the viewer is in fullscreen
		//	//	 mode.
		//	if(PtInRect(&rctTemp, ptTemp))
		//	{
		//		// display trial message
		//		MessageBox(hwnd, MESSAGE_TRIALVERSION, MAINWINDOW_TITLE,
		//			MB_OK | MB_ICONINFORMATION);
		//	}
		//}

		break;

	case WM_MEASUREITEM:
		// get pointer to structure
		pmisTemp = (PMEASUREITEMSTRUCT)lParam; 

		// attempt to get control's handle
		hwndTemp = GetDlgItem(hwnd, pmisTemp->CtlID);

		// Validate, continue
		if(hwndTemp != NULL)
		{
			// attempt to get DC for control
			hdcTemp = GetDC(hwndTemp);

			// Validate, continue
			if(hdcTemp != NULL)
			{					
				// Get item height relative to resolution
				pmisTemp->itemHeight = abs(-MulDiv(9, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72));

				// release DC
				ReleaseDC(hwndTemp, hdcTemp);
			}
		}
		return TRUE;

	case WM_DRAWITEM:
		{
			{
				pdisTemp = (PDRAWITEMSTRUCT) lParam;

				HDC hdc = pdisTemp->hDC;
				COLORREF bg = 0, fg = 0;
				if (pcmwndThis->SetColorsForDC(hdc, pdisTemp->CtlID, (pdisTemp->itemState & ODS_SELECTED) != 0, &bg, &fg))
				{
					switch (pdisTemp->CtlType)
					{
					case ODT_LISTBOX:
						{
							TCHAR achBuffer[MAX_PATH * 2];
							SendMessage(pdisTemp->hwndItem, LB_GETTEXT, pdisTemp->itemID, reinterpret_cast<LPARAM>(achBuffer));

							RECT rct;
							GetClientRect(pdisTemp->hwndItem, &rct);
							rct.top = pdisTemp->rcItem.top;
							rct.bottom = pdisTemp->rcItem.bottom;

							HBRUSH hbr = reinterpret_cast<HBRUSH>(CreateSolidBrush(bg));
							FillRect(hdc, &rct, hbr);
							DeleteObject(hbr);

							TextOut(hdc, pdisTemp->rcItem.left, pdisTemp->rcItem.top, achBuffer, lstrlen(achBuffer));
							break;
						}
					}
				}
			}

			return TRUE;
		}

	case WM_NOTIFY:
		{		
			HWND hWndTabcntrl = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TAB_CONTROL);		
			NMHDR* pHdr = reinterpret_cast<NMHDR*>(lParam);
			if (pHdr->hwndFrom == hWndTabcntrl
				&& pHdr->code == TCN_SELCHANGE)
			{
				int i = TabCtrl_GetCurSel(hWndTabcntrl);
				pcmwndThis->ShowActivePage(i);//show active page here & starts from 1
			}
			else if (pHdr->code == EN_SELCHANGE)
			{
				SELCHANGE sc = *reinterpret_cast<SELCHANGE*>(lParam);
				UINT ctrlID = GetDlgCtrlID(pHdr->hwndFrom);

				LRESULT em = SendMessage(sc.nmhdr.hwndFrom, EM_GETEVENTMASK, (WPARAM)0, (LPARAM)0);
				SendMessage(sc.nmhdr.hwndFrom, EM_SETEVENTMASK, (WPARAM)0, (LPARAM)0);

				COLORREF bg, fg, sfg, sbg;
				pcmwndThis->SetColorsForDC((HDC)INVALID_HANDLE_VALUE, ctrlID, false, &bg, &fg);
				pcmwndThis->SetColorsForDC((HDC)INVALID_HANDLE_VALUE, ctrlID, true, &sbg, &sfg);

				CHARFORMAT2 cf;
				memset(&cf, 0, sizeof(cf));
				cf.cbSize = sizeof(cf);
				cf.dwMask = CFM_BACKCOLOR | CFM_COLOR;
				cf.crTextColor = fg;
				cf.crBackColor = bg;
				SendMessage(pHdr->hwndFrom, EM_SETCHARFORMAT, SCF_ALL, reinterpret_cast<LPARAM>(&cf));

				static LONG sb;
				if (sc.seltyp == SEL_EMPTY) // пользователь начал выделение, запомнить начальную позицию
				{
					sb = sc.chrg.cpMin;
				}
				if (sc.chrg.cpMin >= sb) // left to right
				{
					SendMessage(sc.nmhdr.hwndFrom, EM_SETSEL, (WPARAM)sb, (LPARAM)sc.chrg.cpMax);
				}
				else
					SendMessage(sc.nmhdr.hwndFrom, EM_SETSEL, (WPARAM)sc.chrg.cpMin, (LPARAM)sb);
				cf.crTextColor = sfg;
				cf.crBackColor = sbg;
				SendMessage(sc.nmhdr.hwndFrom, EM_SETCHARFORMAT, (WPARAM)SCF_SELECTION, (LPARAM)&cf);

				// off selection, leave formatting
				if (sc.chrg.cpMin >= sb) // left to right
				{
					int minus = 0;
					if (sc.chrg.cpMax>sb)
						//minus = 1;
						SendMessage(sc.nmhdr.hwndFrom, EM_SETSEL, (WPARAM)sc.chrg.cpMax-minus, (LPARAM)sc.chrg.cpMax-minus);
				}
				else // right to left
					SendMessage(sc.nmhdr.hwndFrom, EM_SETSEL, (WPARAM)sc.chrg.cpMin, (LPARAM)sc.chrg.cpMin);

				SendMessage(sc.nmhdr.hwndFrom, EM_SETEVENTMASK, (WPARAM)0, (LPARAM)em);
			}

			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

	default:
		return 0;//DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	// return "message processed"
	return 1;
}

bool CMainWindow::SetColorsForDC(HDC hdc, UINT ctrl_id, bool selected, COLORREF* bg, COLORREF* fg)
{
  typedef std::map<UINT, size_t> CtrlIDToNum;
  static CtrlIDToNum id2n;
  if (id2n.empty())
  {
    id2n[IDC_LSTFILEMANAGER1] = CSettings::FileManager1;
    id2n[IDC_LSTFILEMANAGER2] = CSettings::FileManager2;
	//Parth Software Solution
	//id2n[IDC_TVFILEMANAGER1] = CSettings::FileManager3;
	//id2n[IDC_TVFILEMANAGER2] = CSettings::FileManager4;
	//Parth Software Solution
    id2n[IDC_TXTCMDPROMPTCONSOLE] = CSettings::CommandlineOutput;
    id2n[IDC_TXTCMDPROMPT] = CSettings::CommandlineInput;
  }

  CtrlIDToNum::const_iterator c2n_it = id2n.find(ctrl_id);
  if (c2n_it != id2n.end())
  {
    CSettings::Colors fg_num = CSettings::ForegroundText
      , bg_num = CSettings::Background;
    if (selected)
    {
      fg_num = CSettings::SelectedText;
      bg_num = CSettings::SelectingBand;
    }
    if (fg)
      *fg = g_csetApplication.Color(c2n_it->second, fg_num);
    if (bg)
      *bg = g_csetApplication.Color(c2n_it->second, bg_num);

    SetTextColor(hdc, g_csetApplication.Color(c2n_it->second, fg_num));
    SetBkColor(hdc, g_csetApplication.Color(c2n_it->second, bg_num));
    
    return true;
  }
  return false;
}


//Parth Software Solution

/**
 * Handles processing of all messages sent to File Manager 1.
 */
LRESULT CALLBACK CMainWindow::FileManager4WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	static BOOL bFocused = FALSE,
				bShifted = FALSE;
	static long lCaretIndex = -1L,
				lAnchorIndex = -1L;
	HWND hwndTemp = NULL;
	int iState = 0,
		iCtrlState = 0,
		iCtrlID = 0,
		iIndex = 0;
	long lItemCount = 0L;

	// since we are only previewing messages, ALWAYS pass message to original
	//	 WindowProc.

	switch(uMsg)
	{
		case WM_KEYDOWN:
			// If this File Manager doesn't have focus, then "set" focus.
			if(!bFocused)
			{
				// set focused flag
				bFocused = TRUE;

				// set Active fields to FM's fields
				pcmwndThis->m_hwndActiveFileManager = hwnd;
				pcmwndThis->m_pllstActiveFileManager = 
					pcmwndThis->m_pllstFileManager2;

				OutputDebugString("\n>>>> FileManager1WindowProc: WM_KEYDOWN");

				// set integrated command prompt's current folder to FM's folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
					g_csetApplication.lastFolderFileManager4(), hwndTemp);

				// Set temporary file list to this File Manager's file list
				pcmwndThis->copyFileListToLookupList();

				// set last selection
				pcmwndThis->setCurrentSelection();

				//set selected text and band color
				pcmwndThis->SetSelectedTextAndBand();
			}

			// retain original processing for left and right arrows
			if(wParam == VK_RIGHT || wParam == VK_LEFT ||
				wParam == VK_HOME || wParam == VK_END ||
				wParam == VK_PRIOR || wParam == VK_NEXT)
				break;

			//Handle Enter key Event code is same as double click event
			if(wParam == VK_RETURN)
			{
				// execute the appropriate "action" for the selected item
				if(!pcmwndThis->executeSelectedItem_TV(IDC_TVFILEMANAGER2))
					break;

				// set integrated command prompt's current folder to FM's folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
					g_csetApplication.lastFolderFileManager4(), hwndTemp);
				break;
			}

			if(wParam == VK_UP || wParam == VK_DOWN)
			{
				// get control key state
				iCtrlState = GetKeyState(VK_CONTROL);
				iState = GetKeyState(VK_SHIFT);
				
				// Set anchor index if needed... the anchor index is set
				//	 anytime the shift state goes from NOT being shifted
				//	 to shifted (shift key down)
				if((iState & SHIFTED) && !bShifted)
				{
					// set shifted flag
					bShifted = TRUE;

					// set anchor index at proper location
					//SendMessage(hwnd, LB_SETANCHORINDEX, (WPARAM)lCaretIndex, 
					//	0L);//ntf

					// store anchor index
					lAnchorIndex = lCaretIndex;
				}

				// If Shift AND Control are held down, treat it like Shift
				//	 ONLY.
				if((iCtrlState & SHIFTED) && (iState & SHIFTED))
					// clear control state
					iCtrlState = 0;

				// get item count
				lItemCount = 0;
				//SendMessage(hwnd, LB_GETCOUNT, (WPARAM)0, 0L);//ntf

				// get caret index
				lCaretIndex = 0;
				//SendMessage(hwnd, LB_GETCARETINDEX, (WPARAM)0, 0L);//ntf

				HTREEITEM hItem = NULL;
				hItem = TreeView_GetSelection(hwnd);
				// determine action based on key, shift, and control states
				switch(wParam)
				{
					case VK_DOWN:
						pcmwndThis->Selected = pcmwndThis->SelectNextItem(hwnd, hItem);
						//// If neither Control or Shift is held, clear any selection
						////	 other than the current
						if((iState & SHIFTED) && !(iCtrlState & SHIFTED))
							TreeView_SetCheckState(hwnd, hItem, !TreeView_GetCheckState(hwnd, hItem))

						//if(!(iState & SHIFTED) && !(iCtrlState & SHIFTED))
						//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
						//
						//// clear any extraneous portion of the selection
						//if((lAnchorIndex > lCaretIndex) && !(iCtrlState & SHIFTED))
						//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, lCaretIndex); 

						//// move index unless we're on the last item
						//if(lCaretIndex < (lItemCount - 1))
						//	lCaretIndex++;

						//// set caret
						//SendMessage(hwnd, LB_SETCARETINDEX, (WPARAM)lCaretIndex, FALSE);

						//// set selection
						//if(!(iCtrlState & SHIFTED))
						//	SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, lCaretIndex);
						
						break;

					case VK_UP:
						pcmwndThis->Selected = pcmwndThis->SelectPreviousItem(hwnd, hItem);
						// If neither Control or Shift is held, clear any selection
						//	 other than the current
						//if(!(iState & SHIFTED) && !(iCtrlState & SHIFTED))
						//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
						//
						//// clear any extraneous portion of the selection
						//if((lAnchorIndex < lCaretIndex) && !(iCtrlState & SHIFTED))
						//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, lCaretIndex);
						//
						//// move index unless we're on the last item
						//if(lCaretIndex > 0L)
						//	lCaretIndex--;

						//// set caret
						//SendMessage(hwnd, LB_SETCARETINDEX, (WPARAM)lCaretIndex, FALSE);

						//// set selection
						//if(!(iCtrlState & SHIFTED))
						//	SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, lCaretIndex);
						break;

					default:
						break;
				}
			}
      else if (wParam == VK_SPACE)
      {
        pcmwndThis->toggleCurrentFMSelection(hwnd);
      }

			return 0;

		case WM_KEYUP:
			
			// retain original processing for left and right arrows
			if(wParam == VK_RIGHT || wParam == VK_LEFT||
				wParam == VK_HOME || wParam == VK_END ||
				wParam == VK_PRIOR || wParam == VK_NEXT)
				break;

			// reset shifted flag if shift key was released
			if(wParam == VK_SHIFT)
				bShifted = FALSE;

			return 0;
	
		case WM_SETFOCUS:
		case WM_MOUSEACTIVATE:
			if(!bFocused)
			{
				// set focused flag
				bFocused = TRUE;

				OutputDebugString("\n>>>> FileManager1WindowProc: WM_MOUSEACTIVATE");
				// set Active fields to FM's fields
				pcmwndThis->m_hwndActiveFileManager = hwnd;
				pcmwndThis->m_pllstActiveFileManager = 
					pcmwndThis->m_pllstFileManager1;

				// set integrated command prompt's current folder to FM's folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
					g_csetApplication.lastFolderFileManager4(), hwndTemp);

				// Set temporary file list to this File Manager's file list
				pcmwndThis->copyFileListToLookupList();

				// set last selection
				pcmwndThis->setCurrentSelection();

				//set selected text and band color
				pcmwndThis->SetSelectedTextAndBand();
			}
			break;

		case WM_KILLFOCUS:
				bFocused = FALSE;
			// get ID of control receiving focus...
			//iCtrlID = GetDlgCtrlID((HWND)wParam);

			//// only perform kill focus if control is one of the primaries.
			//if(iCtrlID != IDC_LSTFILEMANAGER1)
			//{
			//	// set focused flag to FALSE
			//	bFocused = FALSE;

			//	// get the current selection
			//	pcmwndThis->getCurrentSelection(hwnd);

			//	// Kill selection in "other" File Manager
			//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
			//}
			break;

		case WM_DESTROY:
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
				(LONG)pcmwndThis->m_wndprocPreviousFileManager1);
			break;

		//case WM_LBUTTONUP:
		//	// execute the appropriate "action" for the selected item
		//	pcmwndThis->executeSelectedItem_TV();

		//	// set integrated command prompt's current folder to FM's folder
		//	hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		//	pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
		//		g_csetApplication.lastFolderFileManager1(), hwndTemp);
		//	break;
		//case WM_NOTIFY:
		//	{
		//		switch(LOWORD(wParam))
		//		{
		//		case IDC_TVFILEMANAGER1:
		//		case IDC_TVFILEMANAGER2:
		//			if(((LPNMHDR)lParam)->code == NM_CLICK )
		//			{
		//				// execute the appropriate "action" for the selected item
		//				if(!pcmwndThis->executeSelectedItem_TV(IDC_TVFILEMANAGER2))
		//					break;

		//				// set integrated command prompt's current folder to FM's folder
		//				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		//				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
		//					g_csetApplication.lastFolderFileManager4(), hwndTemp);
		//			}					
		//			break;
		//		}
		//	}
		//	break;
		case WM_LBUTTONDBLCLK:
			// execute the appropriate "action" for the selected item
			if(!pcmwndThis->executeSelectedItem_TV(IDC_TVFILEMANAGER2))
				break;

			// set integrated command prompt's current folder to FM's folder
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
			pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
				g_csetApplication.lastFolderFileManager4(), hwndTemp);
			break;

		case WM_CHAR:
			// Make this the active File Manager
			pcmwndThis->m_hwndActiveFileManager = hwnd;
			OutputDebugString("\n>>>> FileManager3WindowProc: WM_CHAR");
			
			// Get Control key's state
			iCtrlState = GetKeyState(VK_CONTROL);

			// Process key press
			switch(wParam)
			{
				case 9:
					// Get Shift key's state
					iState = GetKeyState(VK_SHIFT);
					if(iState & SHIFTED)
						// Set Focus to File Manager 1
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
										IDC_TVFILEMANAGER1);
					else
						// Set Focus to Command Prompt
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
						IDC_TXTCMDPROMPTCONSOLE);
					if(hwndTemp)
						SetFocus(hwndTemp);

					return 0;

				case 13:
					// execute the appropriate "action" for the selected item
					pcmwndThis->executeSelectedItem();

					// set integrated command prompt's current folder to FM's folder
					hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
					pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
						g_csetApplication.lastFolderFileManager4(), hwndTemp);
					
					return 0;

				case 32:
					if(iCtrlState & SHIFTED)
					{
						// Toggle the currently selected item
						pcmwndThis->toggleSelectedFileObject();
					}
					return 0;

				case 42: // '*'
					// Select files by mask
					pcmwndThis->selectFilesByMask();
					return 0;

				default:
					// get temp (lookup) list's handle
					hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
									IDC_LSTFILEMANAGERTEMP);
					
					// valdiate, continue
					if(hwndTemp)
					{
						// forward the message
						SendMessage(hwndTemp, WM_CHAR, wParam, lParam);

						// get any resultant selection
						iIndex = SendMessage(hwndTemp, LB_GETCURSEL, (WPARAM)0, 0L);

						// check, mark selection in *this* File Manager
						if(iIndex != LB_ERR)
						{
							// clear existing selection
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1);

							// make new selection
							SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, iIndex);
						}
					}
					return 0;
			}
			break;

		default:
			break;
	}

	// return "processed message"
	return CallWindowProc(pcmwndThis->m_wndprocPreviousFileManager4, 
			hwnd, uMsg, wParam, lParam);
}



/**
 * Handles processing of all messages sent to File Manager 1.
 */
LRESULT CALLBACK CMainWindow::FileManager3WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	static BOOL bFocused = FALSE,
		bShifted = FALSE;
	static long lCaretIndex = -1L,
		lAnchorIndex = -1L;
	HWND hwndTemp = NULL;
	int iState = 0,
		iCtrlState = 0,
		iCtrlID = 0,
		iIndex = 0;
	long lItemCount = 0L;

	if(!hwnd)
		return 0;

	HDC hdcTemp = NULL;

	int iCntrlID = GetDlgCtrlID(hwnd);

	// since we are only previewing messages, ALWAYS pass message to original
	//	 WindowProc.

	switch(uMsg)
	{
	case WM_KEYDOWN:
		// If this File Manager doesn't have focus, then "set" focus.
		if(!bFocused)
		{
			// set focused flag
			bFocused = TRUE;

			// set Active fields to FM's fields
			pcmwndThis->m_hwndActiveFileManager = hwnd;
			pcmwndThis->m_pllstActiveFileManager = 
				pcmwndThis->m_pllstFileManager1;

			OutputDebugString("\n>>>> FileManager1WindowProc: WM_KEYDOWN");

			// set integrated command prompt's current folder to FM's folder
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
			pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
				g_csetApplication.lastFolderFileManager3(), hwndTemp);

			// Set temporary file list to this File Manager's file list
			pcmwndThis->copyFileListToLookupList();

			// set last selection
			pcmwndThis->setCurrentSelection();

			//set selected text and band color
			pcmwndThis->SetSelectedTextAndBand();

		}

		// retain original processing for left and right arrows
		if(wParam == VK_RIGHT || wParam == VK_LEFT ||
			wParam == VK_HOME || wParam == VK_END ||
			wParam == VK_PRIOR || wParam == VK_NEXT)
			break;

		//Handle Enter key Event code is same as double click event
		if(wParam == VK_RETURN)
		{
			// execute the appropriate "action" for the selected item
			if(!pcmwndThis->executeSelectedItem_TV(IDC_TVFILEMANAGER1))
				break;

			// set integrated command prompt's current folder to FM's folder
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
			pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
				g_csetApplication.lastFolderFileManager3(), hwndTemp);
			break;
		}		
		//break;
		//case WM_PAINT:
		//	PAINTSTRUCT psTemp;
		//	// get HDC, prepare DC
		//	hdcTemp = BeginPaint(hwnd, &psTemp);

		//	// validate, continue
		//	if(hdcTemp != NULL)
		//	{
		//		HDC memDC = CreateCompatibleDC(	psTemp.hdc );
		//		RECT rcClip, rcClient;
		//		GetClipBox(memDC, &rcClip );
		//		GetClientRect(hwnd, &rcClient);

		//		// Set clip region to be same as that in paint DC
		//		HRGN rgn;
		//		rgn = CreateRectRgnIndirect( &rcClip );
		//		SelectClipRgn(memDC, rgn);
		//		DeleteObject(&rgn);

		//		// First let the control do its default drawing.
		//		CallWindowProc( pcmwndThis->m_wndprocPreviousFileManager3,hwnd, WM_PAINT, (WPARAM)&memDC, 0 );

		//		if(pcmwndThis->m_hwndActiveFileManager)
		//		{
		//			TCHAR szItem[MAX_PATH] = {0};
		//			HTREEITEM hItem = TreeView_GetSelection(pcmwndThis->m_hwndActiveFileManager);
		//			if(CWin32TreeView::GetItemText( pcmwndThis->m_hwndActiveFileManager, hItem, szItem, MAX_PATH ))
		//			{
		//				RECT rect;
		//				TreeView_GetItemRect( pcmwndThis->m_hwndActiveFileManager, hItem, &rect, TRUE );
		//				SetBkColor(memDC, RGB(255, 0, 0) );
		//				TextOut(memDC, rect.left, rect.top, szItem,0 );
		//			}
		//		}
		//		//psTemp.hdc = memDC;
		//		/*BitBlt(hdcTemp, rcClip.left, rcClip.top, rcClip.right - rcClip.left, rcClip.bottom - rcClip.top, memDC, 
		//		rcClip.left, rcClip.top, SRCCOPY );*/
		//	}
		//	// End process
		//	EndPaint(hwnd, &psTemp);
		//	return 0;

		if(wParam == VK_UP || wParam == VK_DOWN)
		{
			// get control key state
			iCtrlState = GetKeyState(VK_CONTROL);
			iState = GetKeyState(VK_SHIFT);

			// Set anchor index if needed... the anchor index is set
			//	 anytime the shift state goes from NOT being shifted
			//	 to shifted (shift key down)
			if((iState & SHIFTED) && !bShifted)
			{
				// set shifted flag
				bShifted = TRUE;

				// set anchor index at proper location
				//SendMessage(hwnd, LB_SETANCHORINDEX, (WPARAM)lCaretIndex, 
				//	0L);//ntf

				// store anchor index
				lAnchorIndex = lCaretIndex;
			}

			// If Shift AND Control are held down, treat it like Shift
			//	 ONLY.
			if((iCtrlState & SHIFTED) && (iState & SHIFTED))
				// clear control state
				iCtrlState = 0;

			// get item count
			lItemCount = 0;
			//SendMessage(hwnd, LB_GETCOUNT, (WPARAM)0, 0L);//ntf

			// get caret index
			lCaretIndex = 0;
			//SendMessage(hwnd, LB_GETCARETINDEX, (WPARAM)0, 0L);//ntf

			HTREEITEM hItem = NULL;
			hItem = TreeView_GetSelection(hwnd);
			// determine action based on key, shift, and control states
			switch(wParam)
			{
			case VK_DOWN:
				//Select the next item
				pcmwndThis->Selected = pcmwndThis->SelectNextItem(hwnd, hItem);
				//// If neither Control or Shift is held, clear any selection
				////	 other than the current
				if((iState & SHIFTED) && !(iCtrlState & SHIFTED))
					TreeView_SetCheckState(hwnd, hItem, !TreeView_GetCheckState(hwnd, hItem))
					

					//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
					//
					//// clear any extraneous portion of the selection
					//if((lAnchorIndex > lCaretIndex) && !(iCtrlState & SHIFTED))
					//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, lCaretIndex); 

					//// move index unless we're on the last item
					//if(lCaretIndex < (lItemCount - 1))
					//	lCaretIndex++;

					//// set caret
					//SendMessage(hwnd, LB_SETCARETINDEX, (WPARAM)lCaretIndex, FALSE);

					//// set selection
					//if(!(iCtrlState & SHIFTED))
					//	SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, lCaretIndex);

					break;

			case VK_UP:
				//Select the previous item
				pcmwndThis->Selected = pcmwndThis->SelectPreviousItem(hwnd, hItem);

				// If neither Control or Shift is held, clear any selection
				//	 other than the current
				if(!(iState & SHIFTED) && !(iCtrlState & SHIFTED))
					SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);

				// clear any extraneous portion of the selection
				if((lAnchorIndex < lCaretIndex) && !(iCtrlState & SHIFTED))
					SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, lCaretIndex);

				// move index unless we're on the last item
				if(lCaretIndex > 0L)
					lCaretIndex--;

				// set caret
				SendMessage(hwnd, LB_SETCARETINDEX, (WPARAM)lCaretIndex, FALSE);

				// set selection
				if(!(iCtrlState & SHIFTED))
					SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, lCaretIndex);
				break;

			default:
				break;
			}
		}
		else if (wParam == VK_SPACE)
		{
			pcmwndThis->toggleCurrentFMSelection(hwnd);
		}

		return 0;

	case WM_KEYUP:

		// retain original processing for left and right arrows
		if(wParam == VK_RIGHT || wParam == VK_LEFT||
			wParam == VK_HOME || wParam == VK_END ||
			wParam == VK_PRIOR || wParam == VK_NEXT)
			break;

		// reset shifted flag if shift key was released
		if(wParam == VK_SHIFT)
			bShifted = FALSE;

		return 0;

	case WM_SETFOCUS:
	case WM_MOUSEACTIVATE:
		if(!bFocused)
		{
			// set focused flag
			bFocused = TRUE;

			OutputDebugString("\n>>>> FileManager1WindowProc: WM_MOUSEACTIVATE");
			// set Active fields to FM's fields
			pcmwndThis->m_hwndActiveFileManager = hwnd;
			pcmwndThis->m_pllstActiveFileManager = 
				pcmwndThis->m_pllstFileManager1;

			// set integrated command prompt's current folder to FM's folder
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
			pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
				g_csetApplication.lastFolderFileManager3(), hwndTemp);

			// Set temporary file list to this File Manager's file list
			//pcmwndThis->copyFileListToLookupList();

			// set last selection
			//pcmwndThis->setCurrentSelection();

			//set selected text and band color
			pcmwndThis->SetSelectedTextAndBand();
		}
		break;

	case WM_KILLFOCUS:
		bFocused = FALSE;
		// get ID of control receiving focus...
		//iCtrlID = GetDlgCtrlID((HWND)wParam);

		//// only perform kill focus if control is one of the primaries.
		//if(iCtrlID != IDC_LSTFILEMANAGER1)
		//{
		//	// set focused flag to FALSE
		//	bFocused = FALSE;

		//	// get the current selection
		//	pcmwndThis->getCurrentSelection(hwnd);

		//	// Kill selection in "other" File Manager
		//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
		//}
		break;

	case WM_DESTROY:
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
			(LONG)pcmwndThis->m_wndprocPreviousFileManager3);
		break;

		//case WM_LBUTTONUP:
		//	// execute the appropriate "action" for the selected item
		//	pcmwndThis->executeSelectedItem_TV();

		//	// set integrated command prompt's current folder to FM's folder
		//	hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		//	pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
		//		g_csetApplication.lastFolderFileManager1(), hwndTemp);
		//	break;
		//case WM_LBUTTONUP:
		//case WM_NOTIFY:
	case WM_LBUTTONDBLCLK:
		// execute the appropriate "action" for the selected item
		if(!pcmwndThis->executeSelectedItem_TV(IDC_TVFILEMANAGER1))
			break;

		// set integrated command prompt's current folder to FM's folder
		hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
			g_csetApplication.lastFolderFileManager3(), hwndTemp);
		break;

	case WM_CHAR:
		// Make this the active File Manager
		pcmwndThis->m_hwndActiveFileManager = hwnd;
		OutputDebugString("\n>>>> FileManager3WindowProc: WM_CHAR");

		// Get Control key's state
		iCtrlState = GetKeyState(VK_CONTROL);

		// Process key press
		switch(wParam)
		{
		case 9:
			// Get Shift key's state
			iState = GetKeyState(VK_SHIFT);
			if(!(iState & SHIFTED))
				// Set Focus to File Manager 2
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
				IDC_TVFILEMANAGER2);
			else
				// Set Focus to Command Prompt
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
				IDC_TXTCMDPROMPT);
			if(hwndTemp)
				SetFocus(hwndTemp);

			return 0;

		case 13:
			// execute the appropriate "action" for the selected item
			pcmwndThis->executeSelectedItem();

			// set integrated command prompt's current folder to FM's folder
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
			pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
				g_csetApplication.lastFolderFileManager3(), hwndTemp);

			return 0;

		case 32:
			if(iCtrlState/* & SHIFTED*/)
			{
				// Toggle the currently selected item
				pcmwndThis->toggleSelectedFileObject();
			}
			return 0;

		case 42: // '*'
			// Select files by mask
			pcmwndThis->selectFilesByMask();
			return 0;

		default:
			// get temp (lookup) list's handle
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
				IDC_LSTFILEMANAGERTEMP);

			// valdiate, continue
			if(hwndTemp)
			{
				// forward the message
				SendMessage(hwndTemp, WM_CHAR, wParam, lParam);

				// get any resultant selection
				iIndex = SendMessage(hwndTemp, LB_GETCURSEL, (WPARAM)0, 0L);

				// check, mark selection in *this* File Manager
				if(iIndex != LB_ERR)
				{
					// clear existing selection
					SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1);

					// make new selection
					SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, iIndex);
				}
			}
			return 0;
		}
		break;

	default:
		break;
	}

	// return "processed message"
	return CallWindowProc(pcmwndThis->m_wndprocPreviousFileManager3, 
		hwnd, uMsg, wParam, lParam);
}

//Parth Software Solution

/**
 * Handles processing of all messages sent to File Manager 1.
 */
LRESULT CALLBACK CMainWindow::FileManager1WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	static BOOL bFocused = FALSE,
				bShifted = FALSE;
	static long lCaretIndex = -1L,
				lAnchorIndex = -1L;
	HWND hwndTemp = NULL;
	int iState = 0,
		iCtrlState = 0,
		iCtrlID = 0,
		iIndex = 0;
	long lItemCount = 0L;

	// since we are only previewing messages, ALWAYS pass message to original
	//	 WindowProc.

	switch(uMsg)
	{
		case WM_KEYDOWN:
			// If this File Manager doesn't have focus, then "set" focus.
			if(!bFocused)
			{
				// set focused flag
				bFocused = TRUE;

				// set Active fields to FM's fields
				pcmwndThis->m_hwndActiveFileManager = hwnd;
				pcmwndThis->m_pllstActiveFileManager = 
					pcmwndThis->m_pllstFileManager1;

				OutputDebugString("\n>>>> FileManager1WindowProc: WM_KEYDOWN");

				// set integrated command prompt's current folder to FM's folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
					g_csetApplication.lastFolderFileManager1(), hwndTemp);

				// Set temporary file list to this File Manager's file list
				pcmwndThis->copyFileListToLookupList();

				// set last selection
				pcmwndThis->setCurrentSelection();

			}

			// retain original processing for left and right arrows
			if(wParam == VK_RIGHT || wParam == VK_LEFT ||
				wParam == VK_HOME || wParam == VK_END ||
				wParam == VK_PRIOR || wParam == VK_NEXT)
				break;

			if(wParam == VK_UP || wParam == VK_DOWN)
			{
				// get control key state
				iCtrlState = GetKeyState(VK_CONTROL);
				iState = GetKeyState(VK_SHIFT);
				
				// Set anchor index if needed... the anchor index is set
				//	 anytime the shift state goes from NOT being shifted
				//	 to shifted (shift key down)
				if((iState & SHIFTED) && !bShifted)
				{
					// set shifted flag
					bShifted = TRUE;

					// set anchor index at proper location
					SendMessage(hwnd, LB_SETANCHORINDEX, (WPARAM)lCaretIndex, 
						0L);

					// store anchor index
					lAnchorIndex = lCaretIndex;
				}

				// If Shift AND Control are held down, treat it like Shift
				//	 ONLY.
				if((iCtrlState & SHIFTED) && (iState & SHIFTED))
					// clear control state
					iCtrlState = 0;

				// get item count
				lItemCount = SendMessage(hwnd, LB_GETCOUNT, (WPARAM)0, 0L);

				// get caret index
				lCaretIndex = SendMessage(hwnd, LB_GETCARETINDEX, (WPARAM)0, 0L);

				// determine action based on key, shift, and control states
				switch(wParam)
				{
					case VK_DOWN:
						// If neither Control or Shift is held, clear any selection
						//	 other than the current
						if(!(iState & SHIFTED) && !(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
						
						// clear any extraneous portion of the selection
						if((lAnchorIndex > lCaretIndex) && !(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, lCaretIndex); 

						// move index unless we're on the last item
						if(lCaretIndex < (lItemCount - 1))
							lCaretIndex++;

						// set caret
						SendMessage(hwnd, LB_SETCARETINDEX, (WPARAM)lCaretIndex, FALSE);

						// set selection
						if(!(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, lCaretIndex);
						
						break;

					case VK_UP:
						// If neither Control or Shift is held, clear any selection
						//	 other than the current
						if(!(iState & SHIFTED) && !(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
						
						// clear any extraneous portion of the selection
						if((lAnchorIndex < lCaretIndex) && !(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, lCaretIndex);
						
						// move index unless we're on the last item
						if(lCaretIndex > 0L)
							lCaretIndex--;

						// set caret
						SendMessage(hwnd, LB_SETCARETINDEX, (WPARAM)lCaretIndex, FALSE);

						// set selection
						if(!(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, lCaretIndex);
						break;

					default:
						break;
				}
			}
      else if (wParam == VK_SPACE)
      {
        pcmwndThis->toggleCurrentFMSelection(hwnd);
      }

			return 0;

		case WM_KEYUP:
			
			// retain original processing for left and right arrows
			if(wParam == VK_RIGHT || wParam == VK_LEFT||
				wParam == VK_HOME || wParam == VK_END ||
				wParam == VK_PRIOR || wParam == VK_NEXT)
				break;

			// reset shifted flag if shift key was released
			if(wParam == VK_SHIFT)
				bShifted = FALSE;

			return 0;
	
		case WM_SETFOCUS:
		case WM_MOUSEACTIVATE:
			if(!bFocused)
			{
				// set focused flag
				bFocused = TRUE;

				OutputDebugString("\n>>>> FileManager1WindowProc: WM_MOUSEACTIVATE");
				// set Active fields to FM's fields
				pcmwndThis->m_hwndActiveFileManager = hwnd;
				pcmwndThis->m_pllstActiveFileManager = 
					pcmwndThis->m_pllstFileManager1;

				// set integrated command prompt's current folder to FM's folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
					g_csetApplication.lastFolderFileManager1(), hwndTemp);

				// Set temporary file list to this File Manager's file list
				//pcmwndThis->copyFileListToLookupList();

				// set last selection
				//pcmwndThis->setCurrentSelection();
			}
			break;

		case WM_KILLFOCUS:
				bFocused = FALSE;
			// get ID of control receiving focus...
			//iCtrlID = GetDlgCtrlID((HWND)wParam);

			//// only perform kill focus if control is one of the primaries.
			//if(iCtrlID != IDC_LSTFILEMANAGER1)
			//{
			//	// set focused flag to FALSE
			//	bFocused = FALSE;

			//	// get the current selection
			//	pcmwndThis->getCurrentSelection(hwnd);

			//	// Kill selection in "other" File Manager
			//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
			//}
			break;

		case WM_DESTROY:
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
				(LONG)pcmwndThis->m_wndprocPreviousFileManager1);
			break;

		case WM_LBUTTONDBLCLK:
			// execute the appropriate "action" for the selected item
			pcmwndThis->executeSelectedItem();

			// set integrated command prompt's current folder to FM's folder
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
			pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
				g_csetApplication.lastFolderFileManager1(), hwndTemp);
			break;

		case WM_CHAR:
			// Make this the active File Manager
			pcmwndThis->m_hwndActiveFileManager = hwnd;
			OutputDebugString("\n>>>> FileManager1WindowProc: WM_CHAR");
			
			// Get Control key's state
			iCtrlState = GetKeyState(VK_CONTROL);

			// Process key press
			switch(wParam)
			{
				case 9:
					// Get Shift key's state
					iState = GetKeyState(VK_SHIFT);
					if(!(iState & SHIFTED))
						// Set Focus to File Manager 2
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
										IDC_LSTFILEMANAGER2);
					else
						// Set Focus to Command Prompt
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
										IDC_TXTCMDPROMPT);
					if(hwndTemp)
						SetFocus(hwndTemp);

					return 0;

				case 13:
					// execute the appropriate "action" for the selected item
					pcmwndThis->executeSelectedItem();

					// set integrated command prompt's current folder to FM's folder
					hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
					pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
						g_csetApplication.lastFolderFileManager1(), hwndTemp);
					
					return 0;

				case 32:
					if(iCtrlState & SHIFTED)
					{
						// Toggle the currently selected item
						pcmwndThis->toggleSelectedFileObject();
					}
					return 0;

				case 42: // '*'
					// Select files by mask
					pcmwndThis->selectFilesByMask();
					return 0;

				default:
					// get temp (lookup) list's handle
					hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
									IDC_LSTFILEMANAGERTEMP);
					
					// valdiate, continue
					if(hwndTemp)
					{
						// forward the message
						SendMessage(hwndTemp, WM_CHAR, wParam, lParam);

						// get any resultant selection
						iIndex = SendMessage(hwndTemp, LB_GETCURSEL, (WPARAM)0, 0L);

						// check, mark selection in *this* File Manager
						if(iIndex != LB_ERR)
						{
							// clear existing selection
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1);

							// make new selection
							SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, iIndex);
						}
					}
					return 0;
			}
			break;

		default:
			break;
	}

	// return "processed message"
	return CallWindowProc(pcmwndThis->m_wndprocPreviousFileManager1, 
			hwnd, uMsg, wParam, lParam);
}

void CMainWindow::toggleCurrentFMSelection(HWND hwnd)
{
	HTREEITEM hItem = TreeView_GetSelection(hwnd);
	int iState = TreeView_GetCheckState(hwnd, hItem)==1?1:0;
	TreeView_SetCheckState(hwnd, hItem, !iState);
}

/**
 * Handles processing of all messages sent to File Manager 2.
 */
LRESULT CALLBACK CMainWindow::FileManager2WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	static BOOL bFocused = FALSE,
				bShifted = FALSE;
	static long lCaretIndex = -1L,
				lAnchorIndex = -1L;
	HWND hwndTemp = NULL;
	int iState = 0,
		iCtrlState = 0,
		iCtrlID = 0,
		iIndex = 0;
	long lItemCount = 0L;

	// since we are only previewing messages, ALWAYS pass message to original
	//	 WindowProc.

	switch(uMsg)
	{
		case WM_KEYDOWN:
			// If this File Manager doesn't have focus, then "set" focus.
			if(!bFocused)
			{
				// set focused flag
				bFocused = TRUE;

				// set Active fields to FM's fields
				pcmwndThis->m_hwndActiveFileManager = hwnd;
				pcmwndThis->m_pllstActiveFileManager = 
					pcmwndThis->m_pllstFileManager2;
				OutputDebugString("\n>>>> FileManager2WindowProc: WM_KEYDOWN");

				// set integrated command prompt's current folder to FM's folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, 
								IDC_TXTCMDPROMPTCONSOLE);
				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
					g_csetApplication.lastFolderFileManager2(), hwndTemp);

				// Set temporary file list to this File Manager's file list
				pcmwndThis->copyFileListToLookupList();

				// set last selection
				pcmwndThis->setCurrentSelection();
			}

			// retain original processing for left and right arrows
			if(wParam == VK_RIGHT || wParam == VK_LEFT ||
				wParam == VK_HOME || wParam == VK_END ||
				wParam == VK_PRIOR || wParam == VK_NEXT)
				break;

			if(wParam == VK_UP || wParam == VK_DOWN)
			{
				// get control key state
				iCtrlState = GetKeyState(VK_CONTROL);
				iState = GetKeyState(VK_SHIFT);
				
				// Set anchor index if needed... the anchor index is set
				//	 anytime the shift state goes from NOT being shifted
				//	 to shifted (shift key down)
				if((iState & SHIFTED) && !bShifted)
				{
					// set shifted flag
					bShifted = TRUE;

					// set anchor index at proper location
					SendMessage(hwnd, LB_SETANCHORINDEX, (WPARAM)lCaretIndex, 
						0L);

					// store anchor index
					lAnchorIndex = lCaretIndex;
				}

				// If Shift AND Control are held down, treat it like Shift
				//	 ONLY.
				if((iCtrlState & SHIFTED) && (iState & SHIFTED))
					// clear control state
					iCtrlState = 0;

				// get item count
				lItemCount = SendMessage(hwnd, LB_GETCOUNT, (WPARAM)0, 0L);

				// get caret index
				lCaretIndex = SendMessage(hwnd, LB_GETCARETINDEX, (WPARAM)0, 0L);

				// determine action based on key, shift, and control states
				switch(wParam)
				{
					case VK_DOWN:
						// If neither Control or Shift is held, clear any selection
						//	 other than the current
						if(!(iState & SHIFTED) && !(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
						
						// clear any extraneous portion of the selection
						if((lAnchorIndex > lCaretIndex) && !(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, lCaretIndex); 

						// move index unless we're on the last item
						if(lCaretIndex < (lItemCount - 1))
							lCaretIndex++;

						// set caret
						SendMessage(hwnd, LB_SETCARETINDEX, (WPARAM)lCaretIndex, FALSE);

						// set selection
						if(!(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, lCaretIndex);
						
						break;

					case VK_UP:
						// If neither Control or Shift is held, clear any selection
						//	 other than the current
						if(!(iState & SHIFTED) && !(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
						
						// clear any extraneous portion of the selection
						if((lAnchorIndex < lCaretIndex) && !(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, lCaretIndex);
						
						// move index unless we're on the last item
						if(lCaretIndex > 0L)
							lCaretIndex--;

						// set caret
						SendMessage(hwnd, LB_SETCARETINDEX, (WPARAM)lCaretIndex, FALSE);

						// set selection
						if(!(iCtrlState & SHIFTED))
							SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, lCaretIndex);
						break;

					default:
						break;
				}
			}
      else if (wParam == VK_SPACE)
      {
        pcmwndThis->toggleCurrentFMSelection(hwnd);
      }

			return 0;

		case WM_KEYUP:
			
			// retain original processing for left and right arrows
			if(wParam == VK_RIGHT || wParam == VK_LEFT||
				wParam == VK_HOME || wParam == VK_END ||
				wParam == VK_PRIOR || wParam == VK_NEXT)
				break;

			// reset shifted flag if shift key was released
			if(wParam == VK_SHIFT)
				bShifted = FALSE;

			return 0;

		case WM_SETFOCUS:
		case WM_MOUSEACTIVATE:
			if(!bFocused)
			{
				// set focused flag
				bFocused = TRUE;

				// set Active fields to this FM's fields
				pcmwndThis->m_hwndActiveFileManager = hwnd;
				pcmwndThis->m_pllstActiveFileManager =
					pcmwndThis->m_pllstFileManager2;
				OutputDebugString("\n>>>> FileManager2WindowProc: WM_MOUSEACTIVATE");

				// set integrated command prompt's current folder to 
				//	 FM's folder
				hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
				pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
					g_csetApplication.lastFolderFileManager2(), hwndTemp);

				// Set temporary file list to this File Manager's file list
				pcmwndThis->copyFileListToLookupList();

				// set last selection
				pcmwndThis->setCurrentSelection();
			}
			break;

		case WM_KILLFOCUS:
				bFocused = FALSE;
			// get ID of control receiving focus...
			//iCtrlID = GetDlgCtrlID((HWND)wParam);

			//// only perform kill focus if control is one of the primaries.
			//if(iCtrlID != IDC_LSTFILEMANAGER2)
			//{
			//	// set focused flag to FALSE
			//	bFocused = FALSE;

			//	// get the current selection
			//	pcmwndThis->getCurrentSelection(hwnd);

			//	// Kill selection in "other" File Manager
			//	SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1L);
			//}
			break;

		case WM_DESTROY:
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
				(LONG)pcmwndThis->m_wndprocPreviousFileManager2);
			break;

		case WM_LBUTTONDBLCLK:
			// execute the appropriate "action" for the selected item
			pcmwndThis->executeSelectedItem();

			// set integrated command prompt's current folder to 
			//	 FM's folder
			hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
			pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
				g_csetApplication.lastFolderFileManager2(), hwndTemp);
			break;

		case WM_CHAR:
			// Make this the active File Manager
			pcmwndThis->m_hwndActiveFileManager = hwnd;
			OutputDebugString("\n>>>> FileManager2WindowProc: WM_CHAR");
			
			// Get Control key's state
			iCtrlState = GetKeyState(VK_CONTROL);

			// Process key press
			switch(wParam)
			{
				case 9:
					// Get Shift key's state
					iState = GetKeyState(VK_SHIFT);
					if(!(iState & SHIFTED))
						// Set Focus to File Manager 2
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
										IDC_TXTCMDPROMPTCONSOLE);
					else
						// Set Focus to Command Prompt
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
										IDC_LSTFILEMANAGER2);
					if(hwndTemp)
						SetFocus(hwndTemp);
					
					return 0;

				case 13:
					// execute the appropriate "action" for the selected item
					pcmwndThis->executeSelectedItem();

					// set integrated command prompt's current folder to FM's folder
					hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
					pcmwndThis->m_ccapcmdThis->setCurrentDirectory(
						g_csetApplication.lastFolderFileManager2(), hwndTemp);
					
					return 0;

				case 32:
					if(iCtrlState & SHIFTED)
					{
						// Toggle the currently selected item
						pcmwndThis->toggleSelectedFileObject();
					}
					return 0;

				case 42: // '*'
					// Select files by mask
					pcmwndThis->selectFilesByMask();
					return 0;

				default:
					// get temp (lookup) list's handle
					hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
									IDC_LSTFILEMANAGERTEMP);
					
					// valdiate, continue
					if(hwndTemp)
					{
						// forward the message
						SendMessage(hwndTemp, WM_CHAR, wParam, lParam);

						// get any resultant selection
						iIndex = SendMessage(hwndTemp, LB_GETCURSEL, (WPARAM)0, 0L);

						// check, mark selection in *this* File Manager
						if(iIndex != LB_ERR)
						{
							// clear existing selection
							SendMessage(hwnd, LB_SETSEL, (WPARAM)FALSE, -1);

							// make new selection
							SendMessage(hwnd, LB_SETSEL, (WPARAM)TRUE, iIndex);
						}
					}
					return 0;
			}
			break;

		default:
			break;
	}

	// return "processed message"
	return CallWindowProc(pcmwndThis->m_wndprocPreviousFileManager2, 
			hwnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK CMainWindow::CommandPromptConsoleWindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	HWND hwndTemp = NULL;
	int iState = 0;

  switch(uMsg)
	{
		case WM_CHAR:
			switch(wParam)
			{
				case 9:
					// Get Shift key's state
					iState = GetKeyState(VK_SHIFT);
					if(!(iState & SHIFTED))
						// Set Focus to File Manager 2
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
										IDC_TXTCMDPROMPT);
					else
						// Set Focus to Command Prompt
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
										IDC_TVFILEMANAGER2);
					if(hwndTemp)
						SetFocus(hwndTemp);

					// swallow message
					return 0L;
      }
  }

	// return "processed message"
	return CallWindowProc(pcmwndThis->m_wndprocPreviousCommandPrompt, 
			hwnd, uMsg, wParam, lParam);
}

/**
 * Handles processing of all messages sent to the command prompt.
 */
LRESULT CALLBACK CMainWindow::CommandPromptWindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	// since we are only previewing messages, ALWAYS pass message to original
	//	 WindowProc.
	HWND hwndTemp = NULL;
	HDC hdcTemp = NULL;
	BOOL bShouldRefresh = FALSE;
	int iState = 0;

	switch(uMsg)
	{
		case WM_APP:
			switch(wParam)
			{
				case AM_REFRESHFILEMANAGERS:
					pcmwndThis->initializeFileManagers();
					break;

				default:
					break;
			}
			break;

		case WM_SETFOCUS:
		case WM_MOUSEACTIVATE:
			InvalidateRect(hwnd, NULL, TRUE);
			break;

		case WM_KILLFOCUS:
			InvalidateRect(hwnd, NULL, TRUE);
			break;

		case WM_DESTROY:
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
				(LONG)pcmwndThis->m_wndprocPreviousCommandPrompt);
			break;

		case WM_CHAR:
			switch(wParam)
			{
				case 9:
					// Get Shift key's state
					iState = GetKeyState(VK_SHIFT);
					if(!(iState & SHIFTED))
						// Set Focus to File Manager 2
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
											IDC_TVFILEMANAGER1);
					else
						// Set Focus to Command Prompt
						hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis,
										IDC_TXTCMDPROMPTCONSOLE);
					if(hwndTemp)
						SetFocus(hwndTemp);

					// swallow message
					return 0L;

				default:
					// get output window
					hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, 
										IDC_TXTCMDPROMPTCONSOLE);

					// pass input to integrated command prompt
					pcmwndThis->m_ccapcmdThis->pipeInput(pcmwndThis->m_hwndThis, hwnd, hwndTemp, 
						uMsg, wParam, lParam);

					// Had to throw this out and do it elsewhere... For commands
					//	 which require a response, the file managers were
					//	 refreshing before anything happened.
					//
					//// check refresh flag...
					//if(pcmwndThis->m_ccapcmdThis->getShouldRefresh())
					//	pcmwndThis->initializeFileManagers();

					// If key was the ENTER key, swallow message so no "DING!" is
					//	 heard.
					if(wParam == VK_RETURN)
						return 0;

					// exit
					break;
			}
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_UP:
					// Previous item in "doskey" list
					break;

				case VK_DOWN:
					// Next item in "doskey" list
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	// return "processed message"
	return CallWindowProc(pcmwndThis->m_wndprocPreviousCommandPrompt, 
			hwnd, uMsg, wParam, lParam);
}

///**
// * Handles processing of all messages sent to the product name label.
// */
//LRESULT CALLBACK CMainWindow::ProductNameWindowProc (HWND hwnd, UINT uMsg, 
//	WPARAM wParam, LPARAM lParam)
//{
//	// since we are only previewing messages, ALWAYS pass message to original
//	//	 WindowProc.
//	HWND hwndTemp = NULL;
//	HDC hdcTemp = NULL;
//	BOOL bShouldRefresh = FALSE;
//	int iState = 0;
//
//	switch(uMsg)
//	{
//		case WM_DESTROY:
//			SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
//				(LONG)pcmwndThis->m_wndprocPreviousProductName);
//			break;
//
//		case WM_LBUTTONUP:
//			// restore window to "windowed" state
//			pcmwndThis->changeWindowMode();
//			break;
//
//		default:
//			break;
//	}
//
//	// return "processed message"
//	return CallWindowProc(pcmwndThis->m_wndprocPreviousProductName, 
//			hwnd, uMsg, wParam, lParam);
//}
//
///**
// * Handles processing of all messages sent to the trial version label.
// */
//LRESULT CALLBACK CMainWindow::TrialVersionWindowProc (HWND hwnd, UINT uMsg, 
//	WPARAM wParam, LPARAM lParam)
//{
//	// since we are only previewing messages, ALWAYS pass message to original
//	//	 WindowProc.
//	HWND hwndTemp = NULL;
//	HDC hdcTemp = NULL;
//	BOOL bShouldRefresh = FALSE;
//	int iState = 0;
//
//	switch(uMsg)
//	{
//		case WM_DESTROY:
//			SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
//				(LONG)pcmwndThis->m_wndprocPreviousTrialVersion);
//			break;
//
//		case WM_LBUTTONUP:
//			// display emal address
//
//			break;
//
//		default:
//			break;
//	}
//
//	// return "processed message"
//	return CallWindowProc(pcmwndThis->m_wndprocPreviousTrialVersion, 
//			hwnd, uMsg, wParam, lParam);
//}

///**
// * Handles processing of all messages sent to the function buttons.
// */
//LRESULT CALLBACK CMainWindow::CommandButtonsWindowProc (HWND hwnd, 
//	UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	static BOOL bTrackingSet = FALSE;
//	TRACKMOUSEEVENT tmevTemp;
//	PAINTSTRUCT psTemp;
//	POINT ptTemp;
//	HDC hdcTemp = NULL;
//	RECT rctTemp;
//	int iCtrlID = 0;
//	// since we are only previewing messages, ALWAYS pass message to original
//	//	 WindowProc.
//
//	switch(uMsg)
//	{
//		case WM_MOUSELEAVE:
//			// set mouse left flag
//			pcmwndThis->m_bMouseLeftCommandButtons = TRUE;
//			
//			// Order full repaint
//			InvalidateRect(hwnd, NULL, TRUE);
//
//			bTrackingSet = FALSE;
//			break;
//
//		case WM_MOUSEMOVE:
//			// track / highlight button being selected
//			if(!bTrackingSet)
//			{
//				// initialize structure
//				memset(&tmevTemp, 0, sizeof(tmevTemp));
//				tmevTemp.cbSize = sizeof(tmevTemp);
//				tmevTemp.hwndTrack = hwnd;
//				tmevTemp.dwFlags = TME_LEAVE;
//
//				// order tracking of event
//				TrackMouseEvent(&tmevTemp);
//
//				// set initialize flag
//				bTrackingSet = TRUE;
//			}
//
//			// get coord
//			ptTemp.x = LOWORD(lParam);
//			ptTemp.y = HIWORD(lParam);
//			
//			// Get DC
//			hdcTemp = GetDC(hwnd);
//			
//			// validate, continue
//			if(hdcTemp != NULL)
//			{
//				// perform "highlight" of button mouse is over
//				pcmwndThis->highlightCommandButton(hdcTemp, ptTemp);
//				
//				// Release DC
//				ReleaseDC(hwnd, hdcTemp);
//			}
//			break;
//
//		case WM_LBUTTONDOWN:
//			// get coord
//			ptTemp.x = LOWORD(lParam);
//			ptTemp.y = HIWORD(lParam);
//
//			// check point
//			if((iCtrlID = pcmwndThis->getCommandButtonAtPoint(ptTemp)))
//			{
//				// fire command message
//				PostMessage(pcmwndThis->m_hwndThis, WM_COMMAND, 
//					(WPARAM)MAKELPARAM(iCtrlID, 0), (LPARAM)hwnd);
//			}
//			break;
//
//		case WM_DESTROY:
//			SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
//				(LONG)pcmwndThis->m_wndprocPreviousCommandButtons);
//			break;
//
//		case WM_PAINT:
//			// get HDC, prepare DC
//           hdcTemp = BeginPaint(hwnd, &psTemp);
//
//			// validate, continue
//			if(hdcTemp != NULL)
//			{
//				// Get button bar's rectangle
//				GetWindowRect(hwnd, &rctTemp);
//
//				// Convert to local coords
//				ScreenToClient(hwnd, (LPPOINT)&rctTemp.left);
//				ScreenToClient(hwnd, (LPPOINT)&rctTemp.right);
//
//				// paint 
//				pcmwndThis->drawCommandButtons(hwnd, hdcTemp, rctTemp);
//			}
//
//			// End process
//            EndPaint(hwnd, &psTemp);
//			return 0L;
//			break;
//
//		default:
//			break;
//	}
//
//	// return "processed message"
//	return CallWindowProc(pcmwndThis->m_wndprocPreviousCommandPrompt, 
//			hwnd, uMsg, wParam, lParam);
//}

///////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Creates and displays this object's dialog. This object represents a modal
 * dialog which contains the settings for the application; therefore, calling
 * this method halts execution until the user chooses OK or Cancel.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CMainWindow::show(BOOL bFullScreen)
{
    BOOL bReturn = FALSE;	// default to optimistic return val

    try
	{
		// attempt to display dialog box
		if(bFullScreen)
			CreateDialog(m_hinstApplication, MAKEINTRESOURCE(IDD_DLGMAIN),
						NULL, (DLGPROC)WindowProc);
		else
			CreateDialog(m_hinstApplication, 
				MAKEINTRESOURCE(IDD_DLGMAIN_WINDOWED), NULL, 
				(DLGPROC)WindowProc);

		// return success
		bReturn = TRUE;
    }
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to display the main window.");
        
        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Loads, from the registry, the user's preferences for this dialog.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CMainWindow::loadPreferences()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		RECT rctThis,
			 rctScreen;
		int iWidth,
			iHeight;

		// check handle
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}

		// Position window centered of the current monitor initially.
		//	 Get work area
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rctScreen, 0);
		//	 Get this window's dimensions and location
		GetWindowRect(m_hwndThis, &rctThis);
		//	 Calc width and height
		iWidth = 1024;
		iHeight = 768;
		//	 Move rectangle to center
		rctThis.left = ((rctScreen.right - rctScreen.left) 
						 - iWidth) / 2;
		rctThis.top = ((rctScreen.bottom - rctScreen.top) 
						- iHeight) / 2;
		//	 Move window to center
		MoveWindow(m_hwndThis, rctThis.left, rctThis.top, 
			iWidth, iHeight, TRUE);

		// initialize structure
		memset(&rctThis, 0, sizeof(rctThis));

		// Attempt to load this dialog's stored location
		GetRegistryBinary(CurrentUser, REG_BASE, 
			REG_SECTION_PREFERENCES,
			REG_VAL_PREFS_LOCATION_MAINWINDOW, &rctThis, 
			sizeof(rctThis), FALSE);

		// Attempt to get this dialog's windowstate. NOTE: this
		//	 is applied by show().
		m_iWindowState = GetRegistryNumeric(CurrentUser, REG_BASE,
							REG_SECTION_PREFERENCES,
							REG_VAL_PREFS_WINDOWSTATE_MAINWINDOW,
							SW_NORMAL, TRUE);

		// Check and see if a stored value was present... if the
		//	 position of the dialog was stored previously then there
		//	 be something other than 0's for the right and bottom
		if(rctThis.bottom != 0 && rctThis.right != 0 &&
			!g_csetApplication.alwaysLaunchFullScreen())
		{
			// Move window into place ONLY if windowstate isn't maximize
			if(m_iWindowState != SW_MAXIMIZE)
				MoveWindow(m_hwndThis, rctThis.left, rctThis.top, 
					rctThis.right - rctThis.left , rctThis.bottom - rctThis.top,
					TRUE);
		}

		// Display Window
		ShowWindow(m_hwndThis, m_iWindowState);
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
 * Saves, to the registry, the user's preferences for this dialog.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CMainWindow::savePreferences()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		RECT rctThis;

		// Validate this object's handle... if it isn't good, then
		//	 there's nothing we can do.
		if(m_hwndThis == NULL)
			return FALSE;
		
		// Get this object's dialog's position/size
		GetWindowRect(m_hwndThis, &rctThis);

		// Attempt to save this dialog's location
		SaveRegistryBinary(CurrentUser, REG_BASE, 
			REG_SECTION_PREFERENCES,
			REG_VAL_PREFS_LOCATION_MAINWINDOW, &rctThis, 
			sizeof(rctThis), TRUE);

		// Attempt to save this dialog's windowstate
		SaveRegistryNumeric(CurrentUser, REG_BASE,
			REG_SECTION_PREFERENCES,
			REG_VAL_PREFS_WINDOWSTATE_MAINWINDOW,
			m_iWindowState, TRUE);
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
 * Loads the icon for this window at startup.
 *
 * @param hwndThis HWND of this object's GUI (window)
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CMainWindow::loadIcon()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HICON hiconThis = NULL;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}

		// Validate application instance
		if(m_hinstApplication == NULL)
			return FALSE;

		// attempt to load icon
		hiconThis = LoadIcon(m_hinstApplication, MAKEINTRESOURCE(IDI_APPICON48X48));
		if(hiconThis != NULL)
		{
			// attempt to assign icon
			SendMessage(m_hwndThis, WM_SETICON, ICON_BIG, (LPARAM)hiconThis);
		}
		else
			bReturn = FALSE;
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to set the main window icon, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Sets the font for all controls to a DOS-like Terminal font.
 *
 *
 *
 */
BOOL CMainWindow::setFonts()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HWND hwndTemp = NULL;
		HDC hdcLog = NULL;

		// Validate handle... if it isn't good, then there's 
		//	  nothing we can do.
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
	 	}

		// attempt to get logging control's handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPT);
		if(hwndTemp == NULL)
			return FALSE;
		
		// Get DC for log control
		hdcLog = GetDC(hwndTemp);
		if(hdcLog == NULL)
			return FALSE;

		// calculate desired font size in pixels...
		m_iControlFontHeight = -MulDiv(9, GetDeviceCaps(hdcLog, LOGPIXELSY), 72);

		// Attempt to create desired font (Terminal)
		m_hfontControls = CreateFont(m_iControlFontHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, 
								DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								0, _T("Lucida Console"));

		// validate font, continue
		if(m_hfontControls == NULL)
		{
			// release DC for txtLog
			ReleaseDC(hwndTemp, hdcLog);

			// return fail val
			return FALSE;
		}

		// attempt to set font
		SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls, 
			(LPARAM)MAKELPARAM(TRUE, 0));

		// release DC for txtLog
		ReleaseDC(hwndTemp, hdcLog);

		// Now, repeat for each "text" control on *this* window

		//   Command Prompt Console
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	 Viewer
		hwndTemp = GetDlgItem(m_hwndThis, IDC_PICDWG);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	 Drawn Buttons
		hwndTemp = GetDlgItem(m_hwndThis, IDC_PICBUTTONS);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	 File Manager 1
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER1);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));

		//Parth Software Solution
		//	 File Manager 1
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER1);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));

		// if back color for tree view is black or same as balck color then
		// set opposite color to text w.r.t background

		//
		COLORREF tpcr = g_csetApplication.Color(CSettings::FileManager1, CSettings::Background);
		SendMessage(hwndTemp, TVM_GETBKCOLOR, 0, (LPARAM)tpcr );

		if(tpcr == 0)
		{
			COLORREF tpcr = RGB(255,255,255);
			SendMessage(hwndTemp, TVM_SETTEXTCOLOR, 0,(LPARAM)tpcr );  
		
		}
		//


		hwndTemp = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER2);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));

		tpcr = g_csetApplication.Color(CSettings::FileManager1, CSettings::Background);
		SendMessage(hwndTemp, TVM_GETBKCOLOR, 0, (LPARAM)tpcr );

		if(tpcr == 0)
		{
			COLORREF tpcr = RGB(255,255,255);
			SendMessage(hwndTemp, TVM_SETTEXTCOLOR, 0,(LPARAM)tpcr );  
		
		}
		//Parth Software Solution

		//	 File Manager 2
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER2);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	 Loading "shield"
		hwndTemp = GetDlgItem(m_hwndThis, IDC_PICLOADING);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//Parth Software Solution
		//buttons for sorting tree
		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNNAME1);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//do it for all buttons
		//Parth Software Solution

		//	 Buttons
		//	  F1
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF1);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F2
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF2);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F3
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF3);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F4
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF4);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F5
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF5);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F6
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF6);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F7
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF7);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F8
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF8);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F9
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF9);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F10
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF10);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F11
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF11);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  F12
		hwndTemp = GetDlgItem(m_hwndThis, IDC_CMDF12);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  Product Name
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLPRODUCTNAME);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  Date
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLCURRENTDATE);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
				(LPARAM)MAKELPARAM(TRUE, 0));
		//	  Trial Version
		//hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLTRIALVERSION);
		//if(hwndTemp)
		//	SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
		//		(LPARAM)MAKELPARAM(TRUE, 0));
    // Time label
    hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLTIME);
    if(hwndTemp)
      SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
      (LPARAM)MAKELPARAM(TRUE, 0));
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to set the control fonts, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Changes the current graphics device mode to the one selected by the
 * user.
 */
BOOL CMainWindow::changeGraphicsDeviceMode()
{
	LPDEVMODE lpdevmNewMode = NULL;
	BOOL bReturn = FALSE;	// default to pesimistic return val

    try
    {
		int iIndex = -1;

		// quickly validate graphics device settings...
		if(lstrlen(g_csetApplication.graphicsMode()) == 0)
			return bReturn;
		
		// attempt to get graphics device mode's index
		iIndex = g_cginfPrimaryDevice.getGraphicsDeviceModeIndex(
					g_csetApplication.graphicsMode());

		// validate, continue
		if(iIndex == -1)
			return bReturn;

		lpdevmNewMode = g_cginfPrimaryDevice.getGraphicsDeviceModeFromIndex(iIndex);
		if(lpdevmNewMode)
		{
			long lReturn = 0L;

			// attempt to change display settings...
			lReturn = ChangeDisplaySettings(lpdevmNewMode, CDS_FULLSCREEN);

			// check return val
			if(lReturn == DISP_CHANGE_SUCCESSFUL)
				bReturn = TRUE;
		}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to set the graphics device mode, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// garbage collect
	if(lpdevmNewMode)
	{
		delete lpdevmNewMode;	
		lpdevmNewMode = NULL;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Restores the original graphics device mode.
 */
BOOL CMainWindow::restoreGraphicsDeviceMode()
{
	LPDEVMODE lpdevmCurrent = NULL;
	BOOL bReturn = FALSE;	// default to pesimistic return val

    try
    {
		// quickly validate graphics device settings...
		if(lstrlen(g_csetApplication.graphicsMode()) == 0)
			return bReturn;
		
		// attempt to get "current" (referring to user's settings) graphics
		//	 device mode
		lpdevmCurrent = g_cginfPrimaryDevice.currentGraphicsDeviceMode();

		// validate, continue
		if(lpdevmCurrent)
		{
			long lReturn = 0L;

			// attempt to change display settings...
			lReturn = ChangeDisplaySettings(lpdevmCurrent, 0);

			// check return val
			if(lReturn == DISP_CHANGE_SUCCESSFUL)
				bReturn = TRUE;			
		}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to restore the graphics device mode, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Calculates control sizes and sets the graphics device mode (if applicable)
 * selected by the user.
 *
 * @return TRUE if the layout is intialized and no errors occur, otherwise
 * FALSE.
 */ 
BOOL CMainWindow::refreshLayout(BOOL bAsOnStart)
{
	BOOL bReturn = TRUE;	// default to ok val

	try
	{
		SYSTEMTIME systNow;
		HWND hwndTemp = NULL;
		HDC hdcTemp = NULL;
		RECT rctThis,
			rctFileManager1,
			rctFileManager2,
			rctTabDlg,
			rctViewer,
			rctProgressbar,
			rctCommandPrompt,
			rctCommandPromptConsole,
			rctCommandButtons,
			rctProductName,
			rctCurrentDate,
			rctTrialVersion,
			rctTemp;
		TCHAR tstrBuffer[80] = EMPTY_STRING;
		tstring strTemp = EMPTY_STRING;
		float fWindowWidth = 0.0f,
			fWindowHeight = 0.0f,
			fCalcdValue = 0.0f;
		BOOL bProductNameVisible = FALSE;

		// check handle
		if(m_hwndThis == NULL)
		{
			// watcha going to do now, PL??!
			m_strLastError = _T("The internal window handle is invalid. Could not refresh layout.");

			// return fail val
			return FALSE;
		}

		// Check font handle
		if(m_hfontControls == NULL)
		{
			// watcha going to do now, PL??!
			m_strLastError = _T("The default font handle is invalid. Could not refresh layout.");

			// return fail val
			return FALSE;
		}

		// get window height and width for calculations below
		GetClientRect(m_hwndThis, &rctThis);
		fWindowWidth = (float)(rctThis.right - rctThis.left);
		fWindowHeight = (float)(rctThis.bottom - rctThis.top);

		//if(!bAsOnStart)
		//{
		//	pcmwndThis->ShowActivePage(TabCtrl_GetCurSel(GetDlgItem(m_hwndThis, IDC_TAB_CONTROL))
		//		+1);//show active page here & starts from 1
		//}

		// Calculate control sizes and location
		//	 Product Name
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLPRODUCTNAME);
		if(hwndTemp)
		{
			LONG_PTR lptrStyles = NULL;

			// get current size and location
			GetWindowRect(hwndTemp, &rctProductName);

			// move location back to left,top corner
			rctProductName.right -= rctProductName.left;
			rctProductName.left = 0;
			rctProductName.bottom -= rctProductName.top - LAYOUT_TOP_SPACING;
			rctProductName.top = LAYOUT_TOP_SPACING;

			// If we are in full screen mode, then the Product Name label is visible
			//	 If we are NOT in full screen mode then Product Name label isn't
			//	 visible and the viewer should take up the whole area.
			lptrStyles = GetWindowLongPtr(hwndTemp, GWL_STYLE);
			if(lptrStyles & WS_VISIBLE)
				bProductNameVisible = TRUE;
		}
		//	 Current Date
		memset(&rctCurrentDate, 0, sizeof(rctCurrentDate));
		memset(&rctTrialVersion, 0 , sizeof(rctTrialVersion));
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLCURRENTDATE);
		if(hwndTemp)
		{
			// get current size and location
			GetWindowRect(hwndTemp, &rctCurrentDate);

			// get current size of trail version label
			//hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLTRIALVERSION);
			//if(hwndTemp)
			//	GetWindowRect(hwndTemp, &rctTrialVersion);

			// compute appropriate width so date label fills space
			//	 out to trial version label
			rctCurrentDate.right = (long)fWindowWidth - 
				(rctTrialVersion.right - rctTrialVersion.left) -
				rctProductName.right;
			rctCurrentDate.left = rctProductName.right;			

			// move location back to top, right of product name
			rctCurrentDate.bottom = rctProductName.bottom;
			rctCurrentDate.top = 0;
		}
		////	 Trial Version
		//if(rctTrialVersion.right == 0)
		//{
		//	// get handle
		//	//hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLTRIALVERSION);

		//	// get rect
		//	GetWindowRect(hwndTemp, &rctTrialVersion);
		//}
		//if(rctTrialVersion.right != 0)
		//{
		//	rctTrialVersion.top = LAYOUT_TOP_SPACING;
		//	rctTrialVersion.bottom = rctProductName.bottom;
		//	rctTrialVersion.left = rctCurrentDate.right;
		//	rctTrialVersion.right = (long)fWindowWidth;
		//}

		// get position of view/command splitter
		HWND hVCViewSpl = GetDlgItem(m_hwndThis, IDC_VIEW_COMMAND_SPLITTER_NEW);
		int vc_view_spltr_height = 0;
		int vc_view_spltr_pos = 0;
		if (!bAsOnStart)
		{
			vc_view_spltr_pos = SendMessage(hVCViewSpl, ESM_GETPOS, 0, 0) - 4;
			vc_view_spltr_pos -= 1;//Need to be -1 here Parth Software Solution
		}
		else
			vc_view_spltr_pos = (int)(fWindowHeight * LAYOUT_VIEWER_HEIGHT);		
		{
			RECT vc_view_spl_rect;
			GetChildRect(hVCViewSpl, &vc_view_spl_rect);
			vc_view_spltr_height = vc_view_spl_rect.bottom - vc_view_spl_rect.top;    

			if(vc_view_spltr_pos > ((int)fWindowHeight - 24 - 23 - 15))//the positon of the scroll should not to be gone 
				//down the command window.
			{
				vc_view_spltr_pos  =  (int)fWindowHeight - 24 - 23 - 15;
			}
			else if(vc_view_spltr_pos < ((int)fWindowHeight )/3)
			{
				vc_view_spltr_pos  =  (int)fWindowHeight/3;
			}
			MoveWindow(hVCViewSpl, vc_view_spl_rect.left, vc_view_spltr_pos
				, (int)fWindowWidth, vc_view_spltr_height, FALSE);

		}

		// same for file manager/Tab Area splitter and move it there.
		HWND hTabViewSpl = GetDlgItem(m_hwndThis, IDC_FILEMANAGERS_TAB_SPLITTER);
		int fm_tab_spltr_width = 0;
		int fm_tab_spltr_pos = 0;
		if (!bAsOnStart)
			fm_tab_spltr_pos = SendMessage(hTabViewSpl,ESM_GETPOS,0,0)-4;
		else
			fm_tab_spltr_pos = (int)(fWindowWidth * LAYOUT_FILEMANAGER1_WIDTH); 

		if(fm_tab_spltr_pos > ((int)fWindowWidth - LAYOUT_PROGRESSBAR_WIDTH - LAYOUT_TABPAGE_WIDTH))
		{
			fm_tab_spltr_pos  =  (int)fWindowWidth - LAYOUT_PROGRESSBAR_WIDTH - 20 - LAYOUT_TABPAGE_WIDTH;
		}
		/*else if(fm_tab_spltr_pos < 221)
		{
			fm_tab_spltr_pos = 221;
		}*/

		// same for file Tab Area & view splitter and move it there
		HWND hFmViewSpl = GetDlgItem(m_hwndThis, IDC_TABMANAGER_VIEW_SPLITTER);
		int tab_view_spltr_width = 0;
		int tab_view_spltr_pos = 0;
		if (!bAsOnStart)
			tab_view_spltr_pos = SendMessage(hFmViewSpl, ESM_GETPOS, 0, 0 ) - 4;
		else
			tab_view_spltr_pos = (int)(fWindowWidth * LAYOUT_VIEWER_WIDTH) ; 


		if (tab_view_spltr_pos > ((int)fWindowWidth - LAYOUT_PROGRESSBAR_WIDTH))
		{
			tab_view_spltr_pos = (int)fWindowWidth - LAYOUT_PROGRESSBAR_WIDTH - 10;
		}
		else if(fm_tab_spltr_pos + LAYOUT_TABPAGE_WIDTH  >= tab_view_spltr_pos)
		{
			tab_view_spltr_pos = fm_tab_spltr_pos + 10 + LAYOUT_TABPAGE_WIDTH;
		}

		RECT Tab_view_spl_rect;
		GetChildRect(hTabViewSpl, &Tab_view_spl_rect);
		MoveWindow(hTabViewSpl, fm_tab_spltr_pos, rctProductName.bottom - 15
			, tab_view_spltr_width = Tab_view_spl_rect.right-Tab_view_spl_rect.left, vc_view_spltr_pos - rctProductName.bottom + 6, FALSE);

		RECT fm_view_spl_rect;
		GetChildRect(hFmViewSpl, &fm_view_spl_rect);
		MoveWindow(hFmViewSpl, tab_view_spltr_pos, rctProductName.bottom - 15
			, tab_view_spltr_width = fm_view_spl_rect.right-fm_view_spl_rect.left , vc_view_spltr_pos - rctProductName.bottom + 6, FALSE);


		//	 File Manager 1
		memset(&rctFileManager1, 0, sizeof(rctFileManager1));
		if(bProductNameVisible)
			rctFileManager1.top = rctProductName.bottom;
		rctFileManager1.right = fm_tab_spltr_pos;
		rctFileManager1.bottom = (vc_view_spltr_pos - rctFileManager1.top - LAYOUT_SPACING ) / 2;

		//	 File Manager 2
		memset(&rctFileManager2, 0, sizeof(rctFileManager2));
		rctFileManager2.top = rctFileManager1.bottom + LAYOUT_SPACING;
		rctFileManager2.right =  rctFileManager1.right;
		if(m_bInFullScreenMode)
			//Adjust in full screen mode only ::Parth Software
			rctFileManager2.bottom = rctFileManager2.top + (rctFileManager1.bottom - rctFileManager1.top) + 41;
		else
			rctFileManager2.bottom = rctFileManager2.top + (rctFileManager1.bottom - rctFileManager1.top);
		//if(bProductNameVisible)
		//	rctFileManager2.bottom = 
		//		rctFileManager2.top + 
		//			(long)(fWindowHeight * LAYOUT_FILEMANAGER1_HEIGHT) + 
		//				(rctFileManager1.top / 2);

		//Tab Dialog
		memset(&rctTabDlg, 0, sizeof(rctTabDlg));
		rctTabDlg.left = rctFileManager1.right + tab_view_spltr_width;
		rctTabDlg.right = (long)tab_view_spltr_pos;
		rctTabDlg.bottom = rctFileManager2.bottom;
		if(bProductNameVisible)
			rctTabDlg.top = rctProductName.bottom;

		//Viewer
		memset(&rctViewer, 0, sizeof(rctViewer));
		rctViewer.left = rctTabDlg.right + tab_view_spltr_width;
		rctViewer.right = (long)fWindowWidth - LAYOUT_PROGRESSBAR_WIDTH;
		rctViewer.bottom = rctFileManager2.bottom;
		if(bProductNameVisible)
			rctViewer.top = rctProductName.bottom;


		// from bottom to top - all controls in this order

		//	 Command buttons... these occupy the spaces in between. So, get
		//		total area available to command buttons, then reduce the
		//		RECT to the width of ONE command button (offsets will be
		//		calculated below.
		int command_button_height = 0;
		{
			RECT cmd_btn_rect;
			GetWindowRect(GetDlgItem(m_hwndThis, IDC_CMDF1), &cmd_btn_rect);
			command_button_height = cmd_btn_rect.bottom - cmd_btn_rect.top;
		}
		memset(&rctCommandButtons, 0, sizeof(rctCommandButtons));
		rctCommandButtons.bottom = (long)fWindowHeight;
		rctCommandButtons.top = rctCommandButtons.bottom - command_button_height;
		rctCommandButtons.right = (long)fWindowWidth;

		//	 Command prompt's editor window
		memset(&rctCommandPrompt, 0, sizeof(rctCommandPrompt));
		rctCommandPrompt.bottom = rctCommandButtons.top - LAYOUT_SPACING;
		rctCommandPrompt.top = rctCommandPrompt.bottom - 
			(abs(m_iControlFontHeight) + 
			(4 * GetSystemMetrics(SM_CYDLGFRAME)));
		rctCommandPrompt.right = (long)fWindowWidth - LAYOUT_PROGRESSBAR_WIDTH - LAYOUT_PERCENTAGE_WIDTH;

		//	 Command prompt's console window
		memset(&rctCommandPromptConsole, 0, sizeof(rctCommandPromptConsole));
		rctCommandPromptConsole.top = vc_view_spltr_pos + vc_view_spltr_height;
		rctCommandPromptConsole.right = (long)fWindowWidth - LAYOUT_PROGRESSBAR_WIDTH;
		rctCommandPromptConsole.bottom = rctCommandPrompt.top - LAYOUT_SPACING;

		// adjust to one command button
		//	 Store actual real number size of button
		fCalcdValue = (float)rctCommandButtons.right / 
			(float)LAYOUT_COUNT_COMMANDBUTTONS;
		//	 Calculate integer size of button
		rctCommandButtons.right = (long)((float)rctCommandButtons.right / 
			(float)LAYOUT_COUNT_COMMANDBUTTONS);
		//	 Find difference between real and integer values
		fCalcdValue -= (float)rctCommandButtons.right;
		//	 Calculate "remainder"
		fCalcdValue *= LAYOUT_COUNT_COMMANDBUTTONS;
		//	 Round "remainder"

		fCalcdValue = floor(fCalcdValue + 0.5f);

		//Progressbar
		memset(&rctProgressbar, 0, sizeof(rctProgressbar));
		//int progress_bar_height = (int)(fWindowWidth * LAYOUT_PROGRESSBAR_WIDTH);
		rctProgressbar.left = (long)( fWindowWidth - 25);
		rctProgressbar.bottom = rctCommandPrompt.top - 2;
		if(!bProductNameVisible)
			rctProgressbar.top = rctProductName.bottom - 20;
		else
			rctProgressbar.top = rctProductName.bottom;
		rctProgressbar.right = (long)fWindowWidth;

		//	 File Manager 1
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER1);
		MoveWindow(hwndTemp, rctFileManager1.left, rctFileManager1.top,
			rctFileManager1.right - rctFileManager1.left,
			rctFileManager1.bottom - rctFileManager1.top, FALSE);

		//Parth Software Solution
		//set btn size of sorted buttons
		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNNAME1);
		//rctProductName.bottom
		RECT rectName1;
		int BtnWidth = (rctFileManager1.right - rctFileManager1.left)/7 + 14;//;39;

		rectName1.left = rctFileManager1.left;
		rectName1.bottom = rctProductName.bottom;


		MoveWindow(hwndTemp, rectName1.left , rectName1.bottom ,
			BtnWidth/*39*/,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNEXT1);
		MoveWindow(hwndTemp, rectName1.left + BtnWidth  , rectName1.bottom ,
			BtnWidth/*39*/,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNSZ1);
		MoveWindow(hwndTemp, rectName1.left + (BtnWidth*2)  , rectName1.bottom ,
			BtnWidth/*39*/,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNDATE1);
		MoveWindow(hwndTemp, rectName1.left + (BtnWidth*3)  , rectName1.bottom ,
			BtnWidth/*39*/,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNALTR1);
		MoveWindow(hwndTemp, rectName1.left + (BtnWidth*4)  , rectName1.bottom ,
			BtnWidth/*39*/,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNDIR1);
		MoveWindow(hwndTemp, rectName1.left + (BtnWidth*5)  , rectName1.bottom ,
			BtnWidth/*65*/,	20,FALSE);
		//
		//


		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNNAME2);
		MoveWindow(hwndTemp, rctFileManager2.left, rctFileManager2.top ,
			BtnWidth,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNEXT2);
		MoveWindow(hwndTemp, rctFileManager2.left + BtnWidth  , rctFileManager2.top ,
			BtnWidth,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNSZ2);
		MoveWindow(hwndTemp, rctFileManager2.left + (BtnWidth*2)  , rctFileManager2.top ,
			BtnWidth,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNDATE2);
		MoveWindow(hwndTemp, rctFileManager2.left + (BtnWidth*3)  , rctFileManager2.top ,
			BtnWidth,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNALTR2);
		MoveWindow(hwndTemp, rctFileManager2.left + (BtnWidth*4)  , rctFileManager2.top ,
			BtnWidth,	20,FALSE);

		hwndTemp = GetDlgItem(m_hwndThis, IDC_BTNDIR2);
		MoveWindow(hwndTemp, rctFileManager2.left + (BtnWidth*5)  , rctFileManager2.top ,
			BtnWidth,	20,FALSE);
		//rctProductName.bottom
		/*MoveWindow(hwndTemp, rctFileManager1.left , rctProductName.bottom ,
		39,	20,FALSE);*/
		//Parth Software Solution
		//Parth Software Solution

		if(!bProductNameVisible)
			rctFileManager1.top += 40;
		else
			rctFileManager1.top += 20;

		//	 File Manager 1
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER1);
		MoveWindow(hwndTemp, rctFileManager1.left /*+20*/, rctFileManager1.top,
			rctFileManager1.right - rctFileManager1.left,
			rctFileManager1.bottom - rctFileManager1.top, FALSE);
		//Parth Software Solution

		//	 File Manager 2
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER2);
		MoveWindow(hwndTemp, rctFileManager2.left, rctFileManager2.top,
			rctFileManager2.right - rctFileManager2.left,
			rctFileManager2.bottom - rctFileManager2.top , FALSE);

		//	 File Manager 2 (tree control)
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER2);
		MoveWindow(hwndTemp, rctFileManager2.left, rctFileManager2.top + 20,
			rctFileManager2.right - rctFileManager2.left,
			rctFileManager2.bottom - rctFileManager2.top - 20, FALSE);
		//Parth Software Solution

		//Tab Dialog
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TAB_CONTROL);
		MoveWindow(hwndTemp, rctTabDlg.left, rctTabDlg.top,
			rctTabDlg.right - rctTabDlg.left,
			rctTabDlg.bottom - rctTabDlg.top, FALSE);


		//Set the tab control window position
		SetTabControlwindowPostion(0, 20, rctTabDlg.right - rctTabDlg.left, rctTabDlg.bottom - rctTabDlg.top);

		//View
		hwndTemp = GetDlgItem(m_hwndThis, IDC_RICHEDIT_VIEW);
		MoveWindow(hwndTemp, rctViewer.left, rctViewer.top,
			rctViewer.right - rctViewer.left,
			rctViewer.bottom - rctViewer.top, FALSE);


		//	 Product Name
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLPRODUCTNAME);
		strTemp = _T("  ");
		strTemp += MAINWINDOW_TITLE;
		SetDlgItemText(m_hwndThis, IDC_LBLPRODUCTNAME, (TCHAR *)strTemp.c_str());
		MoveWindow(hwndTemp, 
			rctProductName.left, rctProductName.top,
			rctProductName.right - rctProductName.left,
			(rctProductName.bottom - rctProductName.top), FALSE);

		//	 Current Date
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLCURRENTDATE);
		GetLocalTime(&systNow);
		GetDateFormat(LOCALE_USER_DEFAULT, 0, &systNow,
			_T("dd MMMM yyyy"), &tstrBuffer[0], sizeof(tstrBuffer));
		SetDlgItemText(m_hwndThis, IDC_LBLCURRENTDATE, tstrBuffer);
		MoveWindow(hwndTemp, 
			rctCurrentDate.left, rctProductName.top, 
			rctCurrentDate.right - rctCurrentDate.left,
			(rctCurrentDate.bottom - rctProductName.top), FALSE);

		////	 Trial Version
		//strTemp = _T("Evaluation Version  ");
		//hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLTRIALVERSION);
		//SetDlgItemText(m_hwndThis, IDC_LBLTRIALVERSION, (TCHAR *)strTemp.c_str());
		//MoveWindow(hwndTemp, 
		//	rctTrialVersion.left, rctProductName.top, 
		//	rctTrialVersion.right - rctTrialVersion.left,
		//	(rctTrialVersion.bottom - rctProductName.top), FALSE);

		//GMT Time 
		RECT Time_rect;
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLTIME);
		GetChildRect(m_hwndThis, &Time_rect);
		MoveWindow(hwndTemp, 
			rctCurrentDate.left + 125, rctProductName.top, 
			Time_rect.right - Time_rect.left,
			(Time_rect.bottom - rctProductName.top), FALSE);

		//	 Progressbar
		hwndTemp = GetDlgItem(m_hwndThis, IDC_PRGBRMAIN);
		MoveWindow(hwndTemp, rctProgressbar.left, rctProgressbar.top,
			rctProgressbar.right - rctProgressbar.left,
			rctProgressbar.bottom - rctProgressbar.top, FALSE);

		//	 Command prompt's console window
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		MoveWindow(hwndTemp, rctCommandPromptConsole.left, rctCommandPromptConsole.top,
			rctCommandPromptConsole.right - rctCommandPromptConsole.left,
			rctCommandPromptConsole.bottom - rctCommandPromptConsole.top, FALSE);

		//	 Command prompt's editor window
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPT);
		MoveWindow(hwndTemp, rctCommandPrompt.left, rctCommandPrompt.top,
			rctCommandPrompt.right - rctCommandPrompt.left,
			rctCommandPrompt.bottom - rctCommandPrompt.top, FALSE);

		//	 Progressbar
		hwndTemp = GetDlgItem(m_hwndThis, IDC_STATIC_PROGRESS_TEXT);
		MoveWindow(hwndTemp, rctCommandPromptConsole.right - 25, rctCommandPrompt.top,
			50,
			22, FALSE);

		//Percentage
		hwndTemp = GetDlgItem(m_hwndThis, IDC_STATIC_PROGRESS_TEXT);
		if(hwndTemp)
			SendMessage(hwndTemp, WM_SETFONT, (WPARAM)m_hfontControls,
			(LPARAM)MAKELPARAM(TRUE, 0));


		//	 Command button bar (drawn)
		hwndTemp = GetDlgItem(m_hwndThis, IDC_PICBUTTONS);
		MoveWindow(hwndTemp, 0, rctCommandButtons.top,
			(int)fWindowWidth,
			rctCommandButtons.bottom - rctCommandButtons.top, FALSE);

		//	 Command buttons
		for(int i = 0; i < LAYOUT_COUNT_COMMANDBUTTONS; i++)
		{
			// get button
			hwndTemp = GetDlgItem(m_hwndThis, (IDC_CMDF1 + i));

			if(hwndTemp != NULL)
			{
				// add remainder...
				rctTemp.left = (long)(fCalcdValue / 2.9f);

				// calculate proper location and size...
				rctTemp.left += i * (rctCommandButtons.right);
				rctTemp.top = rctCommandButtons.top;
				rctTemp.right = rctTemp.left + rctCommandButtons.right;
				rctTemp.bottom = rctCommandButtons.bottom;

				// locate
				MoveWindow(hwndTemp, rctTemp.left, rctTemp.top,
					rctTemp.right - rctTemp.left,
					rctTemp.bottom - rctTemp.top, FALSE);
			}
		}

		HWND hWndTabcntrl = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TAB_CONTROL);		
		int i = TabCtrl_GetCurSel(hWndTabcntrl);
		ShowActivePage(i);//show active page here & starts from 1

		//if(m_strFullFilepath.length() > 0)
		//{
		//	tstring strFilename;
		//	TCHAR *ptcExtension = NULL;
		//	strFilename = m_strFullFilepath;

		//	// get extension
		//	if(strFilename.length() > 3)
		//		ptcExtension = &strFilename[strFilename.length() - 3];

		//	// validate, continue
		//	if(ptcExtension)
		//	{
		//		if(_tcsstr(FILEEXTENSIONS_AUTOCAD_ALL, 
		//			ptcExtension) != NULL)
		//		{
		//			// attempt to open and render drawing
		//			if(m_cdwgengThis)
		//			{
		//				m_cdwgengThis->setFilename(
		//				(TCHAR *)strFilename.data());
		//				m_cdwgengThis->renderDrawing();
		//			}
		//			// order redraw...
		//			//InvalidateRect(m_hwndThis, NULL, TRUE);
		//		}
		//		else
		//		{
		//			// do nothing for now
		//		}
		//	}

		//}
		//Order a repaint
		InvalidateRect(m_hwndThis, NULL, TRUE);
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to initialize the layout, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

  // return success / fail val
  return bReturn;
}

/**
 * Determines whether the current graphics mode should be changed and 
 * maximizes this window accordingly.
 *
 * @return TRUE if the window is initialized and no errors occur, otherwise
 * FALSE
 */
BOOL CMainWindow::initializeWindow()
{
	BOOL bReturn = FALSE;	// default to failure val

	try
	{
		LONG_PTR lptrStyle = 0L;

		// check run full screen setting... if this option isn't set, then
		//	 there's nothing to do but return SUCCESS.
		//
		// Addendum: If this object is changing "window modes," then we want
		//	 to perform the processes here independent of the full screen
		//	 setting.
		if(!g_csetApplication.alwaysLaunchFullScreen())
			return TRUE;
		
		// check *this* object's handle
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The internal window handle is invalid.");

			// return fail val
			return FALSE;
		}

		// perform actual settings change
		bReturn = m_bGraphicsDeviceModeChanged = changeGraphicsDeviceMode();

		// Maxmize this way for now... this way cause a flash since the window is
		//	 being shown and THEN maximized.
		ShowWindow(m_hwndThis, SW_MAXIMIZE);
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to initialize the window, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Initializes File Manager 1 & 2 by loading the directing listing for their
 * last browse folder.
 *
 * @return TRUE if the file managers are initialized and no errors occur,
 * otherwise FALSE
 */
BOOL CMainWindow::initializeFileManagers()
{
	BOOL bReturn = TRUE;	// default to failure val

	try
	{
		HWND hwndFM1 = NULL,
			 hwndFM2 = NULL,
			 hwndFM3 = NULL,
			 hwndFM4 = NULL;
		
		// validate *this* object's handle
		if(m_hwndThis == NULL)
			return FALSE;

		// get File Manager handles
		hwndFM1 = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER1);
		hwndFM2 = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER2);
		//Parth Software Solution	
		hwndFM3 = GetDlgItem(m_hwndThis,IDC_TVFILEMANAGER1);
		hwndFM4 =  GetDlgItem(m_hwndThis,IDC_TVFILEMANAGER2);
		if(hwndFM3 == NULL || hwndFM4 == NULL)
			return FALSE;
		//Parth Software Solution
		// check File Manager handles
		if(hwndFM3 == NULL || hwndFM4 == NULL)
			return FALSE;

		// clear File Managers
		SendMessage(hwndFM1, LB_RESETCONTENT, (WPARAM)0, 0L);
		SendMessage(hwndFM2, LB_RESETCONTENT, (WPARAM)0, 0L);
		//Parth Software Solution
		InitCommonControls();	    // Init tree control
		TreeView_DeleteAllItems(hwndFM3);
		//Parth Software Solution

		// Get directory listings... if the machine root is specified, then
		//	 we need to get the drive listing, otherwise just get a
		//	 standard "directory" listing.
		//		File Manager 1
		if(lstrcmp(g_csetApplication.lastFolderFileManager1(), FOLDER_ROOT))
		{
			if(!getDirectoryListing(g_csetApplication.lastFolderFileManager1(),
				 hwndFM1, m_pllstFileManager1))
				return FALSE;

			// Select parent folder in list
			SendMessage(hwndFM1, LB_SETSEL, (WPARAM)TRUE, (LPARAM)INDEX_PARENTFOLDER);
			
		}
		else
		{
			if(!getDriveListing(hwndFM1))
				return FALSE;
		}

		//		File Manager 2
		if(lstrcmp(g_csetApplication.lastFolderFileManager2(), FOLDER_ROOT))
		{
			if(!getDirectoryListing(g_csetApplication.lastFolderFileManager2(),
				 hwndFM2, m_pllstFileManager2))
				return FALSE;
		}
		else
		{
			if(!getDriveListing(hwndFM2))
				return FALSE;
		}

		//Parth Software Solution
		// get root directories first
		if(!getDriveListing_TV(hwndFM3))
				return FALSE;	

		if(!getDriveListing_TV(hwndFM4))
				return FALSE;
		//Parth Software Solution
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to initialize the file managers, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

//Parth Software Solution
/**
 * Gets the directory listing for the specified folder and then displays it by
 * calling displayDirectoryListing().
 *
 * @param tstrFullpath
 *
 * @param hwndOutputControl
 *
 * @param arlLookupArray
 *
 * @return TRUE if the directory listing is retrieved and the control populated
 * successfully, otherwise FALSE
 */
BOOL CMainWindow::getDirectoryListing_TV(const TCHAR *tstrFullpath, HWND hwndOutputControl,
	LinkedListEx<FILE_INFORMATION> *pllstOutput, bool bUsingCheckBox, bool bDisplyResult, bool bDiplayListUsingOnlyPath)
{
	CACLInfo *pcaclItem = NULL;
	HANDLE hFolderListing = NULL;
	BOOL bReturn = FALSE;	// default to failure val

	try
	{
		WIN32_FIND_DATA wfdItem;
		ACERIGHTS aceItem;
		FILE_INFORMATION *pfinfTemp = NULL;
		NUMBERFMT numfmtItem;
		tstring strBasePath = EMPTY_STRING,
					strFullpath = EMPTY_STRING;
		TCHAR tstrFileSpec[MAX_PATH] = EMPTY_STRING,
			  //tstrBuffer[MAX_PATH] = EMPTY_STRING,
			  tstrNumber[80] = EMPTY_STRING,
			  tstrFullpathLocal[MAX_PATH] = EMPTY_STRING;
		long lIndex = -1L;

		// validate fullpath and handle params
		if(lstrlen(tstrFullpath) == 0)
			return bReturn;
		if(hwndOutputControl == NULL)
			return bReturn;

		// Make sure folder exists... if it doesn't, revert to application
		//	 folder.

		// Preprocess folder... this method is called with fullpaths AND
		//	 file specs!
		strFullpath = tstrFullpath;
		lIndex = strFullpath.find_last_of(_T("\\"));
		if(lIndex > -1)
			strFullpath = strFullpath.substr(0, lIndex + 1);
		// Now, resume checks
		if(!FolderExists((TCHAR *)strFullpath.c_str()) && strFullpath.find_last_of(_T(":")) != -1)
		{
			m_strLastError = "No Media in drive";
			// display this one...
			WrappedMessageBox( m_strLastError.c_str(),
				MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
			return false;
			//_tcscpy(tstrFullpathLocal, g_csetApplication.applicationFolder());	
		}
		else
			_tcscpy(tstrFullpathLocal, tstrFullpath);

		// validate file list param
		if(pllstOutput == NULL)
			return bReturn;

		// Clear File List's contents
		pllstOutput->clear();

		// Clear contents
		//SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0,
		//	(LPARAM)0);
		//InitCommonControls();
		//TreeView_DeleteAllItems(hwndOutputControl);

		// Add wait message... the order in which the following steps
		//	 are executed is of the UPTMOST importance!
		//lstrcpy(tstrBuffer, _T("Loading File List in tree view explorer..."));
		//SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0,
		//	(LPARAM)tstrBuffer);

		//
			//TVINSERTSTRUCT tvinsert;//TV_INSERTSTRUCT
			//tvinsert.hParent=NULL;			// top most level no need handle
			//tvinsert.hInsertAfter=TVI_ROOT; // work as root level
   //         tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	  //      tvinsert.item.pszText=tstrBuffer;
			//tvinsert.item.iImage=0;
			//tvinsert.item.iSelectedImage=1;
		//

		// Allow wait message to repaint
		doEvents();

		// Suspend drawing to speed things up
		SendMessage(hwndOutputControl, WM_SETREDRAW, (WPARAM)FALSE,
			0L);

		// Clear wait message
		//SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0,
		//	(LPARAM)0);
		//TreeView_DeleteAllItems(hwndOutputControl);

		// Add "Machine Root" entry
		/*SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)FOLDER_MACHINEROOT);

		// Add "Up One Level" entry
		SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)FOLDER_PARENT);*/

		// create filespec
		if(_tcsstr(tstrFullpathLocal, _T("*")) == NULL)
		{
			if(tstrFullpathLocal[lstrlen(tstrFullpathLocal) - 1] == _T('\\'))
				_stprintf(tstrFileSpec, _T("%s*"), tstrFullpathLocal);
			else
				_stprintf(tstrFileSpec, _T("%s\\*"), tstrFullpathLocal);
		}
		else
			_stprintf(tstrFileSpec, _T("%s"), tstrFullpathLocal);

		// create number format
		memset(&numfmtItem, 0, sizeof(numfmtItem));
		numfmtItem.lpThousandSep = THOUSANDS_SEPARATOR;
		numfmtItem.lpDecimalSep = DECIMAL_SEPARATOR;
		numfmtItem.Grouping = 3;

		bool bIsVirtualPath = false;
		//for loop for adding the virtual folders in the list
		std::vector<tstring>::const_iterator itVirtualFolders;
		for( itVirtualFolders = m_vVirtualFolders.begin(); itVirtualFolders !=  m_vVirtualFolders.end(); 
			itVirtualFolders++)
		{
			tstring tsVirtualFolders;

			// construct fullpath
			tstring strItemPath;
			tsVirtualFolders = (*itVirtualFolders).c_str();
			int iIndex = tsVirtualFolders.find_last_of(_T("\\"));
			if(iIndex > -1)
			{
				strItemPath = tsVirtualFolders.substr(iIndex + 1,  tsVirtualFolders.length());
				tsVirtualFolders = tsVirtualFolders.substr(0, iIndex + 1);
			}
			strBasePath = strFullpath;

			// attempt to create ACL object
			pcaclItem = new CACLInfo(EMPTY_STRING);

			if(strFullpath.compare(tsVirtualFolders) == 0)
			{
				wchar_t date[80], time[80];
				SYSTEMTIME st;
				FILETIME ftModtime;
				/*
				* Use SetFileTime() to change the file modification time
				* to the current time.
				*/
				GetSystemTime(&st);
				if (GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL,
					date, sizeof date / sizeof date[0]) == 0 ||
					GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, NULL,
					time, sizeof time / sizeof time[0]) == 0)
					return false;
				SystemTimeToFileTime(&st, &ftModtime);

				memset(&wfdItem, 0, sizeof(wfdItem));
				_tcscpy(wfdItem.cFileName,  strItemPath.c_str());
				wfdItem.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
				wfdItem.ftCreationTime = ftModtime;
				wfdItem.ftLastAccessTime = ftModtime;
				wfdItem.ftLastWriteTime = ftModtime;

				// Get ACL for file/directory
				pcaclItem->setPath((TCHAR*)tstrFullpath);
				pcaclItem->Output(aceItem);

				// Create and add to active File List
				pfinfTemp = new FILE_INFORMATION(wfdItem, aceItem);
				if(pfinfTemp)
				{
					// add to list
					pllstOutput->add(pfinfTemp);
					bIsVirtualPath = true;
					// de-ref
					pfinfTemp = NULL;
				}
			}
		}
		tstring strTemp;
		if(bIsVirtualPath)
		{
			if(strBasePath[strBasePath.length() - 1] == _T('\\'))
				strBasePath[strBasePath.length() - 1] = '\0';

			//To avoid pointer assignation from other location
			strTemp = strBasePath;
			tstrFullpath = strTemp.c_str();
			
			// create filespec
			if(_tcsstr(tstrFullpath, _T("*")) == NULL)
			{
				if(tstrFullpath[lstrlen(tstrFullpath) - 1] == _T('\\'))
					_stprintf(tstrFileSpec, _T("%s*"), tstrFullpath);
				else
					_stprintf(tstrFileSpec, _T("%s\\*"), tstrFullpath);
			}
			else
				_stprintf(tstrFileSpec, _T("%s"), tstrFullpath);

		}

		// attempt to get first file/folder
		hFolderListing = FindFirstFile(tstrFileSpec, &wfdItem);
		if(hFolderListing == INVALID_HANDLE_VALUE)
		{	
			// NO FILES - Make certain to RESET UI!!!!

			// Enable drawing
			SendMessage(hwndOutputControl, WM_SETREDRAW, (WPARAM)TRUE,
				0L);

			// reset last error
			m_strLastError = EMPTY_STRING;

			// return success
			return TRUE;
		}

		// construct base path
		strBasePath.assign(tstrFullpath);
		if(strBasePath[strBasePath.length() - 1] != _T('\\'))
			strBasePath += _T("\\");

		// attempt to create ACL object
		pcaclItem = new CACLInfo(EMPTY_STRING);

		// Examine returned file system object
		if(wfdItem.cFileName[0] != _T('.'))
		{
			// construct fullpath
			strFullpath = strBasePath;
			strFullpath += wfdItem.cFileName;

			// Get ACL for file/directory
			pcaclItem->setPath((TCHAR *)strFullpath.data());
			pcaclItem->Output(aceItem);

			// Create and add to active File List
			pfinfTemp = new FILE_INFORMATION(wfdItem, aceItem);
			if(pfinfTemp)
			{
				// add to list
				pllstOutput->add(pfinfTemp);

				// de-ref
				pfinfTemp = NULL;
			}
		}

		// Iterate through remaining file system objects
		while(FindNextFile(hFolderListing, &wfdItem))
		{
			// Make sure this isn't the parent / current directory
			if(wfdItem.cFileName[0] != _T('.'))
			{
				// construct fullpath
				strFullpath = strBasePath;
				strFullpath += wfdItem.cFileName;

				// Get ACL for file/directory
				pcaclItem->setPath((TCHAR *)strFullpath.data());
				pcaclItem->Output(aceItem);

				// Create and add to active File List
				pfinfTemp = new FILE_INFORMATION(wfdItem, aceItem);
				if(pfinfTemp)
				{
					// add to list
					pllstOutput->add(pfinfTemp);

					// de-ref
					pfinfTemp = NULL;
				}
			}
		}


		if(bDisplyResult)
		{
			// If we made it here, call display and return it's return value
			if(bUsingCheckBox)
				bReturn = displayDirectoryListingForCheckedItem_TV(hwndOutputControl, pllstOutput);
			else if(bDiplayListUsingOnlyPath)
				bReturn = displaySortedItems_TV(hwndOutputControl, pllstOutput, tstrFullpath);				
			else
				bReturn = displayDirectoryListing_TV(hwndOutputControl, pllstOutput);
		}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to get the directory listing, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// Close find handle
	if(hFolderListing != NULL && hFolderListing != INVALID_HANDLE_VALUE)
		FindClose(hFolderListing);

	// Enable drawing
	SendMessage(hwndOutputControl, WM_SETREDRAW, (WPARAM)TRUE,
		0L);

// clear last error, if applicable
if(bReturn)
	m_strLastError = EMPTY_STRING;

// perform final garbage collection
if(pcaclItem)
{
	delete pcaclItem;
	pcaclItem = NULL;
}

// return success / fail val
return bReturn;
}
//Parth Software Solution

BOOL CMainWindow::initializeTabControl()
{
	BOOL bReturn = TRUE;	// default to failure val

	try
	{		
		//HWND hWndTabcntrl = GetDlgItem(m_hwndThis, IDC_TAB_CONTROL);
		//InsertTabItem(hWndTabcntrl, "First Page", 0);
		//InsertTabItem(hWndTabcntrl, "Second Page", 1);
		//InsertTabItem(hWndTabcntrl, "Third Page", 2);
		/*InsertTabItem(hWndTabcntrl, "Third Page", 2);
		InsertTabItem(hWndTabcntrl, "Fourth Page", 3);
		InsertTabItem(hWndTabcntrl, "Fifth Page", 4);
		InsertTabItem(hWndTabcntrl, "Sixth Page", 5);
		InsertTabItem(hWndTabcntrl, "Seventh Page", 6);
		InsertTabItem(hWndTabcntrl, "Eighth Page", 7);*/
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to initialize the file managers, an unexpected error occurred.");

		// set fail value
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}


/**
 * Gets the directory listing for the specified folder and then displays it by
 * calling displayDirectoryListing().
 *
 * @param tstrFullpath
 *
 * @param hwndOutputControl
 *
 * @param arlLookupArray
 *
 * @return TRUE if the directory listing is retrieved and the control populated
 * successfully, otherwise FALSE
 */
BOOL CMainWindow::getDirectoryListing(const TCHAR *tstrFullpath, HWND hwndOutputControl,
	LinkedListEx<FILE_INFORMATION> *pllstOutput)
{
	CACLInfo *pcaclItem = NULL;
	HANDLE hFolderListing = NULL;
	BOOL bReturn = FALSE;	// default to failure val

	try
	{
		WIN32_FIND_DATA wfdItem;
		ACERIGHTS aceItem;
		FILE_INFORMATION *pfinfTemp = NULL;
		NUMBERFMT numfmtItem;
		tstring strBasePath = EMPTY_STRING,
					strFullpath = EMPTY_STRING;
		TCHAR tstrFileSpec[MAX_PATH] = EMPTY_STRING,
			  tstrBuffer[MAX_PATH] = EMPTY_STRING,
			  tstrNumber[80] = EMPTY_STRING,
			  tstrFullpathLocal[MAX_PATH] = EMPTY_STRING;
		long lIndex = -1L;

		// validate fullpath and handle params
		if(lstrlen(tstrFullpath) == 0)
			return bReturn;
		if(hwndOutputControl == NULL)
			return bReturn;

		// Make sure folder exists... if it doesn't, revert to application
		//	 folder.

		// Preprocess folder... this method is called with fullpaths AND
		//	 file specs!
		strFullpath = tstrFullpath;
		lIndex = strFullpath.find_last_of(_T("\\"));
		if(lIndex > -1)
			strFullpath = strFullpath.substr(0, lIndex + 1);
		// Now, resume checks
		if(!FolderExists((TCHAR *)strFullpath.c_str()))
			_tcscpy(tstrFullpathLocal, g_csetApplication.applicationFolder());	
		else
			_tcscpy(tstrFullpathLocal, tstrFullpath);

		// validate file list param
		if(pllstOutput == NULL)
			return bReturn;

		// Clear File List's contents
		pllstOutput->clear();

		// Clear contents
		SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0,
			(LPARAM)0);

		// Add wait message... the order in which the following steps
		//	 are executed is of the UPTMOST importance!
		lstrcpy(tstrBuffer, _T("Loading File List..."));
		SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0,
			(LPARAM)tstrBuffer);

		// Allow wait message to repaint
		doEvents();

		// Suspend drawing to speed things up
		SendMessage(hwndOutputControl, WM_SETREDRAW, (WPARAM)FALSE,
			0L);

		// Clear wait message
		SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0,
			(LPARAM)0);

		// Add "Machine Root" entry
		SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)FOLDER_MACHINEROOT);

		// Add "Up One Level" entry
		SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)FOLDER_PARENT);

		// create filespec
		if(_tcsstr(tstrFullpathLocal, _T("*")) == NULL)
		{
			if(tstrFullpathLocal[lstrlen(tstrFullpathLocal) - 1] == _T('\\'))
				_stprintf(tstrFileSpec, _T("%s*"), tstrFullpathLocal);
			else
				_stprintf(tstrFileSpec, _T("%s\\*"), tstrFullpathLocal);
		}
		else
			_stprintf(tstrFileSpec, _T("%s"), tstrFullpathLocal);

		// create number format
		memset(&numfmtItem, 0, sizeof(numfmtItem));
		numfmtItem.lpThousandSep = THOUSANDS_SEPARATOR;
		numfmtItem.lpDecimalSep = DECIMAL_SEPARATOR;
		numfmtItem.Grouping = 3;

		// attempt to get first file/folder
		hFolderListing = FindFirstFile(tstrFileSpec, &wfdItem);
		if(hFolderListing == INVALID_HANDLE_VALUE)
		{	
			// NO FILES - Make certain to RESET UI!!!!

			// Enable drawing
			SendMessage(hwndOutputControl, WM_SETREDRAW, (WPARAM)TRUE,
				0L);

			// reset last error
			m_strLastError = EMPTY_STRING;

			// return success
			return TRUE;
		}
		
		// construct base path
		strBasePath = tstrFullpath;
		if(strBasePath[strBasePath.length() - 1] != _T('\\'))
			strBasePath += _T("\\");

		// attempt to create ACL object
		pcaclItem = new CACLInfo(EMPTY_STRING);

		// Examine returned file system object
		if(wfdItem.cFileName[0] != _T('.'))
		{
			// construct fullpath
			strFullpath = strBasePath;
			strFullpath += wfdItem.cFileName;

			// Get ACL for file/directory
			pcaclItem->setPath((TCHAR *)strFullpath.data());
			pcaclItem->Output(aceItem);

			// Create and add to active File List
			pfinfTemp = new FILE_INFORMATION(wfdItem, aceItem);
			if(pfinfTemp)
			{
				// add to list
				pllstOutput->add(pfinfTemp);

				// de-ref
				pfinfTemp = NULL;
			}
		}

		// Iterate through remaining file system objects
		while(FindNextFile(hFolderListing, &wfdItem))
		{
			// Make sure this isn't the parent / current directory
			if(wfdItem.cFileName[0] != _T('.'))
			{
				// construct fullpath
				strFullpath = strBasePath;
				strFullpath += wfdItem.cFileName;

				// Get ACL for file/directory
				pcaclItem->setPath((TCHAR *)strFullpath.data());
				pcaclItem->Output(aceItem);

				// Create and add to active File List
				pfinfTemp = new FILE_INFORMATION(wfdItem, aceItem);
				if(pfinfTemp)
				{
					// add to list
					pllstOutput->add(pfinfTemp);

					// de-ref
					pfinfTemp = NULL;
				}
			}
		}

		// If we made it here, call display and return it's return value
		bReturn = displayDirectoryListing(hwndOutputControl, pllstOutput);
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to get the directory listing, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// Close find handle
	if(hFolderListing != NULL && hFolderListing != INVALID_HANDLE_VALUE)
		FindClose(hFolderListing);

	// Enable drawing
	SendMessage(hwndOutputControl, WM_SETREDRAW, (WPARAM)TRUE,
		0L);

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;
	
	// perform final garbage collection
	if(pcaclItem)
	{
		delete pcaclItem;
		pcaclItem = NULL;
	}

    // return success / fail val
    return bReturn;
}


/**
 * Displays the directory listing for the specified folder using the
 * specified control and "active" sort.
 *
 * @param hwndOutputControl
 *
 * @param pllstOutput
 *
 * @return
 */
BOOL CMainWindow::displayDirectoryListing_TV(HWND hwndOutputControl, 
	LinkedListEx<FILE_INFORMATION> *pllstOutput)
{
	static BOOL bDisplayingDirectoryList = FALSE;
	BOOL bReturn = FALSE;	// default to failure val

	 try
	{
		WIN32_FIND_DATA *pwfdItem = NULL;
		ACERIGHTS *pacerightsItem = NULL;
		FILETIME ftLocal;
		SYSTEMTIME systLocal;
		NUMBERFMT numfmtItem;
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING,
			  tstrNumber[80] = EMPTY_STRING;
		long lLongestFileObjectName = 0L;
		tstring strAttributes = EMPTY_STRING,
					strPermissions = EMPTY_STRING;
		BOOL bSortSucceeded = TRUE;
		//Parth Software Solution
		int iDlgCntrlID = GetDlgCtrlID(hwndOutputControl);
		//Parth Software Solution

		// check "working" flag
		if(bDisplayingDirectoryList)
			return TRUE;
		bDisplayingDirectoryList = TRUE;

		// validate handle and list params
		if(pllstOutput == NULL)
			return bReturn;
		if(hwndOutputControl == NULL)
			return bReturn;

		// Clear contents
		//SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0,
		//	(LPARAM)0);
		//TreeView_DeleteAllItems(hwndOutputControl);

		// Add "Machine Root" entry
		//SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
		//	(LPARAM)FOLDER_MACHINEROOT);

		// Add "Up One Level" entry
		//SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
		//	(LPARAM)FOLDER_PARENT);
		TVINSERTSTRUCT tvinsert;//TV_INSERTSTRUCT
		//TVINSERTSTRUCT tvinsert2;
		BOOL retval = FALSE;
		
		HTREEITEM currRoot = NULL,
				  currparent = NULL;
		int iID1 = GetDlgCtrlID(hwndOutputControl);
		if(iID1 == IDC_TVFILEMANAGER1)
		{
			currRoot = Root;
		}
		
		if(iID1 == IDC_TVFILEMANAGER2)
		{
			currRoot = Root2;
		}
		if(!currRoot)
		{
			// creating image list and put it into the tree control
			//====================================================//
			
			hImageList=ImageList_Create(16,16,ILC_COLOR16,2,10);					  // Macro: 16x16:16bit with 2 pics [array]
			hBitMap=LoadBitmap(m_hinstApplication,MAKEINTRESOURCE(IDB_TREE));					  // load the picture from the resource
			ImageList_Add(hImageList,hBitMap,NULL);								      // Macro: Attach the image, to the image list
			DeleteObject(hBitMap);													  // no need it after loading the bitmap
		    //SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control
			SendDlgItemMessage(m_hwndThis,iID1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control
			
			tvinsert.hParent=NULL;			// top most level no need handle
			tvinsert.hInsertAfter=TVI_ROOT; // work as root level
            tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;//|TVI_SORT;
			
	        tvinsert.item.pszText= TVFOLDER_ROOT;//"My Computer";
			tvinsert.item.iImage=0;
			tvinsert.item.iSelectedImage=1;
			//Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
			currparent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iID1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
			currRoot=Parent;
			Before=Parent;                   // handle of the before root
			tvinsert.hParent=currparent;         // handle of the above data
			
			// code for Treeview2
			//tvinsert2.hParent=NULL;			// top most level no need handle
			//tvinsert2.hInsertAfter=TVI_ROOT; // work as root level
   //         tvinsert2.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;//|TVI_SORT;
			//
	  //      tvinsert2.item.pszText= TVFOLDER_ROOT;//"My Computer";
			//tvinsert2.item.iImage=0;
			//tvinsert2.item.iSelectedImage=1;
			//Parent2=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER2,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
			//Root2=Parent2;
			//Before2=Parent2;                   // handle of the before root
			//tvinsert2.hParent=Parent2;         // handle of the above data
			//
		}

		// create number format
		memset(&numfmtItem, 0, sizeof(numfmtItem));
		numfmtItem.lpThousandSep = THOUSANDS_SEPARATOR;
		numfmtItem.lpDecimalSep = DECIMAL_SEPARATOR;
		numfmtItem.Grouping = 3;



		// Get longest file object name
		for(long lcv = 0L; lcv < pllstOutput->getLength(); lcv++)
		{
			// Get current file object's information
			pwfdItem = pllstOutput->getEntry(lcv)->pwfdFileInfo;

			// check object's name
			if(lstrlen(pwfdItem->cFileName) > lLongestFileObjectName)
				lLongestFileObjectName = lstrlen(pwfdItem->cFileName);
		}

		if(m_aseActiveSort >= aseNameAsc && m_aseActiveSort <= aseDirDesc)
		{
			if(m_pllstActiveFileManager)
			{
				SetPercentage(0);
				SetPercentageRange(m_pllstActiveFileManager->getLength());
			}
		}
		
		// Determine what sort to perform
		switch(m_aseActiveSort)
		{
			case aseCustom:
				bSortSucceeded = sortFileListByCustom(0, 
					m_pllstActiveFileManager->getLength());
				break;
			case aseNameAsc:
			case aseNameDesc:
				bSortSucceeded = sortFileListByName(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseTypeAsc:
			case aseTypeDesc:
				bSortSucceeded = sortFileListByType(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseSizeAsc:
			case aseSizeDesc:
				bSortSucceeded = sortFileListBySize(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseDateAsc:
			case aseDateDesc:
				bSortSucceeded = sortFileListByDate(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseExtensionAsc:
			case aseExtensionDesc:
				bSortSucceeded = sortFileListByExtension(0,
					m_pllstActiveFileManager->getLength());
				break;

				//Parth Software Solution bug fixing
			case aseDirAsc:
			case aseDirDesc:
				bSortSucceeded = sortFileListByDirOrFileFirst(0,
					m_pllstActiveFileManager->getLength());
				break;
				//Parth Software Solution

			default:
				break;
		}

		if(m_aseActiveSort >= aseNameAsc && m_aseActiveSort <= aseDirDesc)
		{
			if(m_pllstActiveFileManager)
			{
				SetPercentage(100);
			}
		}

		// check sort 
		if(!bSortSucceeded)
			WrappedMessageBox(
				_T("Could not sort the active file list."),
				MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

		// iterate through file objects
		int iID = GetDlgCtrlID(hwndOutputControl);
		if(iID <= -1L)
			return FALSE;

		//Selected=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)Selected);
		Selected= (HTREEITEM)SendDlgItemMessage(m_hwndThis,iID, TVM_GETNEXTITEM, TVGN_CARET, (LPARAM)Selected);
		if(Selected == NULL)
		{
			return false;
		}

		if(hwndOutputControl)
			DeleteAllChildItems(hwndOutputControl, Selected);		

		for(long lcv = 0L; lcv < pllstOutput->getLength(); lcv++)
		{
			// Get current file object's information
			pwfdItem = pllstOutput->getEntry(lcv)->pwfdFileInfo;

			// Get current file object's rights
			pacerightsItem = pllstOutput->getEntry(lcv)->paceFileRights;

			// Make sure this isn't the parent / current directory
			if(pwfdItem != NULL)
			{
				//	 Convert date modified time
				//		For read-only file systems, the last accessed time
				//			will be zero (not present). Use the date created
				//			instead (this is what Windows(r) actually does).
				if(pwfdItem->ftLastAccessTime.dwLowDateTime &&
					pwfdItem->ftLastAccessTime.dwHighDateTime)
					FileTimeToLocalFileTime(&pwfdItem->ftLastAccessTime, &ftLocal);
				else
					FileTimeToLocalFileTime(&pwfdItem->ftCreationTime, &ftLocal);

				//	Convert to system time
				FileTimeToSystemTime(&ftLocal, &systLocal);

				// Create attributes string
				//	 Content Not Indexed
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
					strAttributes = _T("N");
				else
					strAttributes = _T("_");
				//	 Archive
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
					strAttributes += _T("A");
				else
					strAttributes += _T("_");
				//	 Read-only
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					strAttributes += _T("R");
				else
					strAttributes += _T("_");
				//	 Hidden
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
					strAttributes += _T("H");
				else
					strAttributes += _T("_");
				//	 System
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
					strAttributes += _T("S");
				else
					strAttributes += _T("_");
				//	 Compressed
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
					strAttributes += _T("C");
				else
					strAttributes += _T("_");
				//	 Encrypted
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
					strAttributes += _T("E");
				else
					strAttributes += _T("_");

				// Create permissions string
				strPermissions = pacerightsItem->toString();

				//	 Get format for type
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					_stprintf(tstrBuffer, FORMAT_DIRECTORY, lLongestFileObjectName,
						pwfdItem->cFileName,
						systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
						systLocal.wHour, systLocal.wMinute,
						strAttributes.c_str(),
						strPermissions.c_str());

					//	 create output string
					// Parth Software Solution bug fixing
					//_stprintf(tstrNumber, _T("%u"),pwfdItem->cFileName);
					//_stprintf(tstrBuffer, FORMAT_FILE1,pwfdItem->cFileName);
					//#define FORMAT_DIRECTORY			_T("<DIR>         %-*s %02d.%02d.%4d %02d:%02d %s %s")
					//#define FORMAT_DIRECTORY_TV			_T("%s  <DIR>  %02d.%02d.%4d %02d:%02d %s %s")
					//_stprintf(tstrBuffer, FORMAT_DIRECTORY_TV,
					//	pwfdItem->cFileName,
					//	systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
					//	//systLocal.wHour, systLocal.wMinute
					//	//strAttributes.c_str(),
					//	strPermissions.c_str());
					// Parth Software Solution
				}
				else
				{
					// Format file size
					_stprintf(tstrNumber, _T("%u"), pwfdItem->nFileSizeLow);
					GetNumberFormat(LOCALE_USER_DEFAULT, 0, tstrNumber,
						&numfmtItem, tstrBuffer, sizeof(tstrBuffer));
					lstrcpy(tstrNumber, tstrBuffer);

					// create output string
					if(lstrlen(pwfdItem->cFileName))
						_stprintf(tstrBuffer, FORMAT_FILE, tstrNumber, 
							lLongestFileObjectName, pwfdItem->cFileName,
							systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
							systLocal.wHour, systLocal.wMinute,
							strAttributes.c_str(),
							strPermissions.c_str());
				}
				
				//	 display string
				//SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
				//	(LPARAM)tstrBuffer);				
				
				//if(Selected != NULL)
				{
					tvinsert.hParent = Selected;
					tvinsert.hInsertAfter=TVI_LAST; 
					tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
					tvinsert.item.iImage=0;
					tvinsert.item.iSelectedImage=1;
					tvinsert.item.pszText= tstrBuffer;
					//Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
					Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iID,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
					
				}
			}
		}

		// If we made it here, return success
		bReturn = TRUE;
	
		// Compute gestimate for horizontal scrollbar width
		///HDC hdcTemp = GetDC(hwndOutputControl);
		//if(hdcTemp)
		//{
			//RECT rctTemp;
			//SIZE sszText;
			//TCHAR tstrBuffer[MAX_PATH * 2];
			//long lScrollRange = 0L;

			// get RECT for active File Manager
			//GetWindowRect(hwndOutputControl, &rctTemp);

			// Get text for first "normalized" item
			/*SendMessage(hwndOutputControl, LB_GETTEXT, 
				(WPARAM)INDEX_PARENTFOLDER + 1, (LPARAM)tstrBuffer);

			// get text width
			GetTextExtentPoint32(hdcTemp, tstrBuffer, lstrlen(tstrBuffer),
				&sszText);

			// Calc scroll range
			lScrollRange = sszText.cx + abs(sszText.cx - (rctTemp.right - rctTemp.left));

			// Set scrollbar's range to total required - control's width
			SendMessage(hwndOutputControl, LB_SETHORIZONTALEXTENT, (WPARAM)lScrollRange,*/ 
			//	0L);//ntf

			// Release DC
			///ReleaseDC(hwndOutputControl, hdcTemp);
		//}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to display the directory listing, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// clear "working" flag
	bDisplayingDirectoryList = FALSE;

    // return success / fail val
    return bReturn;
}


/**
 * Displays the directory listing for the specified folder using the
 * specified control and "active" sort.
 *
 * @param hwndOutputControl
 *
 * @param pllstOutput
 *
 * @return
 */
BOOL CMainWindow::displayDirectoryListing(HWND hwndOutputControl, 
	LinkedListEx<FILE_INFORMATION> *pllstOutput)
{
	static BOOL bDisplayingDirectoryList = FALSE;
	BOOL bReturn = FALSE;	// default to failure val

	try
	{
		WIN32_FIND_DATA *pwfdItem = NULL;
		ACERIGHTS *pacerightsItem = NULL;
		FILETIME ftLocal;
		SYSTEMTIME systLocal;
		NUMBERFMT numfmtItem;
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING,
			  tstrNumber[80] = EMPTY_STRING;
		long lLongestFileObjectName = 0L;
		tstring strAttributes = EMPTY_STRING,
					strPermissions = EMPTY_STRING;
		BOOL bSortSucceeded = TRUE;

		// check "working" flag
		if(bDisplayingDirectoryList)
			return TRUE;
		bDisplayingDirectoryList = TRUE;

		// validate handle and list params
		if(pllstOutput == NULL)
			return bReturn;
		if(hwndOutputControl == NULL)
			return bReturn;

		// Clear contents
		SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0,
			(LPARAM)0);

		// Add "Machine Root" entry
		SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)FOLDER_MACHINEROOT);

		// Add "Up One Level" entry
		SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)FOLDER_PARENT);

		// create number format
		memset(&numfmtItem, 0, sizeof(numfmtItem));
		numfmtItem.lpThousandSep = THOUSANDS_SEPARATOR;
		numfmtItem.lpDecimalSep = DECIMAL_SEPARATOR;
		numfmtItem.Grouping = 3;

		// Get longest file object name
		for(long lcv = 0L; lcv < pllstOutput->getLength(); lcv++)
		{
			// Get current file object's information
			pwfdItem = pllstOutput->getEntry(lcv)->pwfdFileInfo;

			if(pwfdItem->cFileName)
			{
				// check object's name
				if(lstrlen(pwfdItem->cFileName) > lLongestFileObjectName)
					lLongestFileObjectName = lstrlen(pwfdItem->cFileName);
			}
		}

		if(m_pllstActiveFileManager != NULL)
		{
			// Determine what sort to perform
			switch(m_aseActiveSort)
			{
			case aseCustom:
				bSortSucceeded = sortFileListByCustom(0, 
					m_pllstActiveFileManager->getLength());
			case aseNameAsc:
			case aseNameDesc:
				bSortSucceeded = sortFileListByName(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseTypeAsc:
			case aseTypeDesc:
				bSortSucceeded = sortFileListByType(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseSizeAsc:
			case aseSizeDesc:
				bSortSucceeded = sortFileListBySize(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseDateAsc:
			case aseDateDesc:
				bSortSucceeded = sortFileListByDate(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseExtensionAsc:
			case aseExtensionDesc:
				bSortSucceeded = sortFileListByExtension(0,
					m_pllstActiveFileManager->getLength());
				break;

			default:
				break;
			}

			// check sort 
			if(!bSortSucceeded)
				WrappedMessageBox(
				_T("Could not sort the active file list."),
				MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
		}
		// iterate through file objects
		for(long lcv = 0L; lcv < pllstOutput->getLength(); lcv++)
		{
			// Get current file object's information
			pwfdItem = pllstOutput->getEntry(lcv)->pwfdFileInfo;

			// Get current file object's rights
			pacerightsItem = pllstOutput->getEntry(lcv)->paceFileRights;

			// Make sure this isn't the parent / current directory
			if(pwfdItem != NULL)
			{
				//	 Convert date modified time
				//		For read-only file systems, the last accessed time
				//			will be zero (not present). Use the date created
				//			instead (this is what Windows(r) actually does).
				if(pwfdItem->ftLastAccessTime.dwLowDateTime &&
					pwfdItem->ftLastAccessTime.dwHighDateTime)
					FileTimeToLocalFileTime(&pwfdItem->ftLastAccessTime, &ftLocal);
				else
					FileTimeToLocalFileTime(&pwfdItem->ftCreationTime, &ftLocal);

				//	Convert to system time
				FileTimeToSystemTime(&ftLocal, &systLocal);

				// Create attributes string
				//	 Content Not Indexed
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
					strAttributes = _T("N");
				else
					strAttributes = _T("_");
				//	 Archive
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
					strAttributes += _T("A");
				else
					strAttributes += _T("_");
				//	 Read-only
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					strAttributes += _T("R");
				else
					strAttributes += _T("_");
				//	 Hidden
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
					strAttributes += _T("H");
				else
					strAttributes += _T("_");
				//	 System
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
					strAttributes += _T("S");
				else
					strAttributes += _T("_");
				//	 Compressed
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
					strAttributes += _T("C");
				else
					strAttributes += _T("_");
				//	 Encrypted
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
					strAttributes += _T("E");
				else
					strAttributes += _T("_");

				// Create permissions string
				strPermissions = pacerightsItem->toString();

				//	 Get format for type
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					//	 create output string
					_stprintf(tstrBuffer, FORMAT_DIRECTORY, lLongestFileObjectName,
						pwfdItem->cFileName,
						systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
						systLocal.wHour, systLocal.wMinute,
						strAttributes.c_str(),
						strPermissions.c_str());
				}
				else
				{
					// Format file size
					_stprintf(tstrNumber, _T("%u"), pwfdItem->nFileSizeLow);
					GetNumberFormat(LOCALE_USER_DEFAULT, 0, tstrNumber,
						&numfmtItem, tstrBuffer, sizeof(tstrBuffer));
					lstrcpy(tstrNumber, tstrBuffer);

					// create output string
					if(lstrlen(pwfdItem->cFileName))
						_stprintf(tstrBuffer, FORMAT_FILE, tstrNumber, 
							lLongestFileObjectName, pwfdItem->cFileName,
							systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
							systLocal.wHour, systLocal.wMinute,
							strAttributes.c_str(),
							strPermissions.c_str());
				}
				
				//	 display string
				SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
					(LPARAM)tstrBuffer);
			}
		}

		// If we made it here, return success
		bReturn = TRUE;
	
		// Compute gestimate for horizontal scrollbar width
		HDC hdcTemp = GetDC(hwndOutputControl);
		if(hdcTemp)
		{
			RECT rctTemp;
			SIZE sszText;
			TCHAR tstrBuffer[MAX_PATH * 2];
			long lScrollRange = 0L;

			// get RECT for active File Manager
			GetWindowRect(hwndOutputControl, &rctTemp);

			// Get text for first "normalized" item
			SendMessage(hwndOutputControl, LB_GETTEXT, 
				(WPARAM)INDEX_PARENTFOLDER + 1, (LPARAM)tstrBuffer);

			// get text width
			GetTextExtentPoint32(hdcTemp, tstrBuffer, lstrlen(tstrBuffer),
				&sszText);

			// Calc scroll range
			lScrollRange = sszText.cx + abs(sszText.cx - (rctTemp.right - rctTemp.left));

			// Set scrollbar's range to total required - control's width
			SendMessage(hwndOutputControl, LB_SETHORIZONTALEXTENT, (WPARAM)lScrollRange, 
				0L);

			// Release DC
			ReleaseDC(hwndOutputControl, hdcTemp);
		}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to display the directory listing, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// clear "working" flag
	bDisplayingDirectoryList = FALSE;

    // return success / fail val
    return bReturn;
}

/**
 * Sorts and re-displays the directory listing by calling 
 * displayDirectoryListing(). This method should be used when the calling
 * function or method needs to initiate a sort but doesn't have (or doesn't 
 * need) access to which file manager is active and which file list should be 
 * used.
 *
 * @return TRUE if no errrs occur, otherwise FALSE
 */
BOOL CMainWindow::sortDirectoryListing()
{
	BOOL bReturn = FALSE;

	try
	{
		LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;
		TCHAR tstrBuffer[100] = EMPTY_STRING;
		int iCtrlID = 0;

		// validate active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return bReturn;

		// get active File Manager's ID
		iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);
		
		// Determine which file list should be used
		if(iCtrlID == IDC_LSTFILEMANAGER1)
			pllstActive = m_pllstFileManager1;
		else
			pllstActive = m_pllstFileManager2;

		// validate list
		if(pllstActive == NULL)
			return bReturn;
		if(pllstActive->getLength() == 0)
			return TRUE;

		// Clear contents
		SendMessage(m_hwndActiveFileManager, LB_RESETCONTENT, (WPARAM)0,
			(LPARAM)0);

		// Add wait message... the order in which the following steps
		//	 are executed is of the UPTMOST importance!
		lstrcpy(tstrBuffer, _T("Sorting File List..."));
		SendMessage(m_hwndActiveFileManager, LB_ADDSTRING, (WPARAM)0,
			(LPARAM)tstrBuffer);

		// Allow wait message to repaint
		doEvents();

		// Suspend drawing to speed things up
		SendMessage(m_hwndActiveFileManager, WM_SETREDRAW, (WPARAM)FALSE,
			0L);

		// Clear wait message
		SendMessage(m_hwndActiveFileManager, LB_RESETCONTENT, (WPARAM)0,
			(LPARAM)0);

		// Add "Machine Root" entry
		SendMessage(m_hwndActiveFileManager, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)FOLDER_MACHINEROOT);

		// Add "Up One Level" entry
		SendMessage(m_hwndActiveFileManager, LB_ADDSTRING, (WPARAM)0, 
			(LPARAM)FOLDER_PARENT);

		// call displayDirectoryListing()
		bReturn = displayDirectoryListing(m_hwndActiveFileManager, pllstActive);

		// Enable drawing
		SendMessage(m_hwndActiveFileManager, WM_SETREDRAW, (WPARAM)TRUE, 0L);

		// set index to parent folder
		SendMessage(m_hwndActiveFileManager, LB_SETSEL, 
			(WPARAM)TRUE, INDEX_PARENTFOLDER);
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to sort the directory listing, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Subclasses all appropriate controls.
 *
 * @return TRUE if all controls are subclassed, otherwise FALSE.
 */
BOOL CMainWindow::subclassControls()
{
	BOOL bReturn = FALSE;	// default to failure val

	try
	{
		HWND hwndTemp = NULL;
		LONG_PTR lptrProc = 0L;

		// validate this object's handle
		if(m_hwndThis == NULL)
			return bReturn;
		
		//// File Manager 1

		//	 attempt to get handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER1);

		//	 validate, continue
		if(hwndTemp == NULL)
			return bReturn;

		//	 set WindowProc
		m_wndprocPreviousFileManager1 = (WNDPROC)SetWindowLongPtr(hwndTemp, 
											GWLP_WNDPROC, 
											(LONG)&FileManager1WindowProc);
		if((LONG)m_wndprocPreviousFileManager1 == 0)
			return bReturn;

		//Parth Software Solution
		//// Treeview explorer File Manager1

		//	 attempt to get handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER1);

		//	 validate, continue
		if(hwndTemp == NULL)
			return bReturn;

		//	 set WindowProc
		m_wndprocPreviousFileManager3 = (WNDPROC)SetWindowLongPtr(hwndTemp, 
											GWLP_WNDPROC, 
											(LONG)&FileManager3WindowProc);
		if((LONG)m_wndprocPreviousFileManager3 == 0)
			return bReturn;



		//// Treeview explorer File Manager2

		//	 attempt to get handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER2);

		//	 validate, continue
		if(hwndTemp == NULL)
			return bReturn;

		//	 set WindowProc
		m_wndprocPreviousFileManager4 = (WNDPROC)SetWindowLongPtr(hwndTemp, 
											GWLP_WNDPROC, 
											(LONG)&FileManager4WindowProc);
		if((LONG)m_wndprocPreviousFileManager4 == 0)
			return bReturn;
		//Parth Software Solution

		//// File Manager 2

		//	 attempt to get handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER2);

		//	 validate, continue
		if(hwndTemp == NULL)
			return bReturn;

		//	 set WindowProc
		m_wndprocPreviousFileManager2 = (WNDPROC)SetWindowLongPtr(hwndTemp, 
											GWLP_WNDPROC, 
											(LONG)&FileManager2WindowProc);
		if((LONG)m_wndprocPreviousFileManager2 == 0)
			return bReturn;

		//// Command Prompt

		//	 attempt to get handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPT);

		//	 validate, continue
		if(hwndTemp == NULL)
			return bReturn;

		//	 set WindowProc
		m_wndprocPreviousCommandPrompt = (WNDPROC)SetWindowLongPtr(hwndTemp, 
											GWLP_WNDPROC, 
											(LONG)&CommandPromptWindowProc);
		if((LONG)m_wndprocPreviousCommandPrompt == 0)
			return bReturn;		

		//// Command Prompt
		//	 attempt to get handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		//	 validate, continue
		if(hwndTemp == NULL)
			return bReturn;
		//	 set WindowProc
		m_wndprocPreviousCommandConsolePrompt = (WNDPROC)SetWindowLongPtr(hwndTemp, 
											GWLP_WNDPROC, 
											(LONG)&CommandPromptConsoleWindowProc);
		if((LONG)m_wndprocPreviousCommandConsolePrompt == 0)
			return bReturn;		

    ////// Product Name

		////	 attempt to get handle
		//hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLPRODUCTNAME);

		////	 validate, continue
		//if(hwndTemp == NULL)
		//	return bReturn;

		////	 set WindowProc
		//m_wndprocPreviousProductName = (WNDPROC)SetWindowLongPtr(hwndTemp, 
		//									GWLP_WNDPROC, 
		//									(LONG)&ProductNameWindowProc);
		//if((LONG)m_wndprocPreviousProductName == 0)
		//	return bReturn;		

		////// Trial Version

		////	 attempt to get handle
		//hwndTemp = GetDlgItem(m_hwndThis, IDC_LBLTRIALVERSION);

		////	 validate, continue
		//if(hwndTemp == NULL)
		//	return bReturn;

		////	 set WindowProc
		//m_wndprocPreviousTrialVersion = (WNDPROC)SetWindowLongPtr(hwndTemp, 
		//									GWLP_WNDPROC, 
		//									(LONG)&TrialVersionWindowProc);
		//if((LONG)m_wndprocPreviousTrialVersion == 0)
		//	return bReturn;	

		// set success return val
		bReturn = TRUE;
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to get the subclass the controls, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Changes from full screen to windowed or windowed to full screen "window
 * modes."
 *
 * @param bFullScreen
 *
 * @return
 */
BOOL CMainWindow::changeWindowMode(BOOL bFullScreen)
{
	BOOL bReturn = TRUE,
		 bOriginalAlwaysLaunchFullScreen = FALSE;

	try
	{
		tstring strOpenDrawing = EMPTY_STRING;

		// validate handle to GUI... if it isn't good, then
		//	 there's nothing we can do
		if(m_hwndThis == NULL)
			return FALSE;
		
		// get the fullpath of the currently open drawing file, if applicable
		if(m_cdwgengThis)
		{
			if(m_cdwgengThis->hasActiveDrawing())
				strOpenDrawing = m_cdwgengThis->getFilename();
		}

		// store original always launch full screen setting
		bOriginalAlwaysLaunchFullScreen = g_csetApplication.alwaysLaunchFullScreen();

		// Set "don't shutdown the app" flag
		m_bIsChangingWindowMode = TRUE;

		// set always launch full screen setting to whatever the requested mode is
		g_csetApplication.alwaysLaunchFullScreen(bFullScreen);

		// Destroy the current window
		if(DestroyWindow(m_hwndThis))
		{
			// Call show with whatever the desired mode is
			bReturn = show(bFullScreen);

			// Reset "don't shutdown the app" flag
			m_bIsChangingWindowMode = FALSE;

			// store flag which indicates what mode we're in
			m_bInFullScreenMode = bFullScreen;

			// restore active drawing
			if(m_cdwgengThis && strOpenDrawing.length())
			{
				// attempt to set active drawing
				if(m_cdwgengThis->setFilename((TCHAR *)strOpenDrawing.c_str()))
				{
					// order redraw
					m_cdwgengThis->renderDrawing();
				}
			}
		}
		else
			bReturn = FALSE;
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to change window modes, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;

		// check app shutdown flag... if we made it here and this wasn't reset, then
		//	 something BAD happened.
		if(m_bIsChangingWindowMode)
		{
			// error message

			// shutdown the app
			PostQuitMessage(0);
		}
    }

	// restore original always launch full screen setting
	g_csetApplication.alwaysLaunchFullScreen(bOriginalAlwaysLaunchFullScreen);

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

///**
// * Checks to see if the specified filename has a valid AutoCad drawing
// * file extension.
// *
// * @param tstr
// *
// * @return TRUE if the file has an AutoCad extension and no errors occur, 
// * otherwise FALSE.
// */
//BOOL CMainWindow::hasAutoCadExtension(TCHAR *tstrFilename)
//{
//
//}

/**
 * Opens the currently selected file in AutoCad. If the currently selected
 * file is NOT a DWG, then the user is prompted as to whether or not they
 * want to open the file using the default application associated with
 * the file.
 *
 * @return TRUE if no errors occur, otherwise FALSE.
 */
BOOL CMainWindow::openInAutoCad()
{
    BOOL bReturn = FALSE;	// default to optimistic return val

    try
	{
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;
		BOOL bIsDirectory = FALSE;
		int iIndex = -1;

		// validate active File Manager's handle
		if(m_hwndActiveFileManager == NULL)
			return bReturn;

		//if there is no checked item
		iIndex = GetSelectedItemsCount(m_hwndActiveFileManager);
		if(iIndex == 0)
		{
			WrappedMessageBox("Please select atleast single item/s", 			MAINWINDOW_TITLE,
				MB_OK | MB_ICONINFORMATION);
			return bReturn;

		}
		// get currently selected item
		if(iIndex != 1)
		{
			WrappedMessageBox("To execute should have single item at one time", 			MAINWINDOW_TITLE,
				MB_OK | MB_ICONINFORMATION);
			CWin32TreeView::TreeView_SelectAll(m_hwndActiveFileManager, false);
			return bReturn;
		}

		CWin32TreeView::GetWin32SelectedItemText(m_hwndActiveFileManager, tstrBuffer, _countof(tstrBuffer));

		//// get text for currently selected item
		//SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)iIndex,
		//	(LPARAM)tstrBuffer);

		// validate, continue
		if(lstrlen(tstrBuffer))
		{
			int iCtrlID = 0;
			tstring strFullpath = EMPTY_STRING;
			TCHAR *ptcExtension = NULL;
			BOOL bProceed = TRUE;

			// get which File Manager is active
			iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);
			
			// Get currently selected item's name
			if(_tcsstr(tstrBuffer, SEARCHKEY_DIRECTORY))
			{
				// get directory's name
				getDirectoryNameFromItem(tstrBuffer, tstrBuffer);

				// set flag indicating this is a directory
				bIsDirectory = TRUE;
			}
			else
				getFilenameFromItem(tstrBuffer, tstrBuffer);

			TCHAR szFullPath[MAX_PATH] = {0};
			// Create full path...
			//	 Get base path
			//HTREEITEM hTreeItem = TreeView_GetSelection(m_hwndActiveFileManager);
			HTREEITEM hTreeItem = CWin32TreeView::GetCurrentSelectedItem(m_hwndActiveFileManager);
			if(!CWin32TreeView::GetSelectedItemParentPath(m_hwndActiveFileManager, hTreeItem, szFullPath))
			{
				return false;
			}
			strFullpath = szFullPath;

			//if(iCtrlID == IDC_TVFILEMANAGER1)
			//	strFullpath = g_csetApplication.lastFolderFileManager1();
			//else
			//	strFullpath = g_csetApplication.lastFolderFileManager2();

			//	 Check trailing backslash
			if(strFullpath[strFullpath.length() - 1] != _T('\\'))
				strFullpath += _T("\\");
			//	 Append filename (OR directory name)
			strFullpath += tstrBuffer;

			// check and see if this is a DWG file...
			if(strFullpath.length() > 3)
				ptcExtension = &strFullpath[strFullpath.length() - 3];

			// check extensions...
			if(_tcsstr(FILEEXTENSIONS_AUTOCAD_ALL, ptcExtension) == NULL)
			{
				if(!bIsDirectory)
				{
					bProceed = (WrappedMessageBox(
						_T("NOTE: This is NOT an AutoCad Drawing File.\n\nDo you wish to continue and open this file using\nthe default application associated with it?"),
									MAINWINDOW_TITLE,
									MB_YESNO | MB_ICONQUESTION) == IDYES);
				}
				else
				{
					bProceed = (WrappedMessageBox(
						_T("NOTE: This is a directory.\n\nDo you wish to continue\nand open this file in Windows(r) Explorer?"),
									MAINWINDOW_TITLE,
									MB_YESNO | MB_ICONQUESTION) == IDYES);
				}
			}
			else
			{
				tstring strFilename;
				TCHAR *ptcExtension = NULL;
				strFilename = strFullpath;
				m_strFullFilepath = strFullpath;
				// get extension
				if(strFilename.length() > 3)
					ptcExtension = &strFilename[strFilename.length() - 3];

				// validate, continue
				if(ptcExtension)
				{
					if(_tcsstr(FILEEXTENSIONS_AUTOCAD_ALL, 
						ptcExtension) != NULL)
					{
						bProceed = false;
						// attempt to open and render drawing
						if(m_cdwgengThis)
						{
							m_cdwgengThis->setFilename(
								(TCHAR *)strFilename.data());

							// order redraw
							m_cdwgengThis->renderDrawing();
						}
						// order redraw...
						//InvalidateRect(m_hwndThis, NULL, TRUE);
					}
					else
					{
						// do nothing for now
					}
				}

			}

			// Open file / directory if user agreed
			if(bProceed)
			{
				// ask Windows to open the "file" using the associated application...
				//   SO simple... :D
				ShellExecute(m_hwndThis, _T("OPEN"), strFullpath.c_str(),
					NULL, NULL, SW_NORMAL);				
			}

			// return success
			bReturn = TRUE;
		}
    }
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to open the active DWG in AutoCad.");
        
        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Displays information about the active DWG. The "active DWG" is the one 
 * being viewed.
 */
BOOL CMainWindow::getDWGInformation()
{
	CDWGInformationDialog *pcdwginfTemp = NULL;
	TCHAR *ptcDWGInformation = NULL;
    BOOL bReturn = FALSE;	// default to optimistic return val

    try
	{
		// validate render engine
		if(m_cdwgengThis == NULL)
			return FALSE;
		if(lstrlen(m_cdwgengThis->getFilename()) == 0)
			return FALSE;

		// validate application instance
		if(m_hinstApplication == NULL)
			return FALSE;

		// attempt to get help text
		if(m_cdwgengThis->getDWGInformation(ptcDWGInformation))
		{
			// attempt to create dialog used to display drawing information
			pcdwginfTemp = new CDWGInformationDialog(m_hinstApplication, 
									ptcDWGInformation);

			// validate, continue
			if(pcdwginfTemp)
			{
				// display (modal)
				pcdwginfTemp->show();

				// set success return val
				bReturn = TRUE;
			}
		}
    }
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to retrieve the information for the active DWG.");
        
        // set fail value
        bReturn = FALSE;
    }

	// garbage collect
	if(pcdwginfTemp)
	{
		delete pcdwginfTemp;
		pcdwginfTemp = NULL;
	}
	if(ptcDWGInformation)
	{
		delete[] ptcDWGInformation;
		ptcDWGInformation = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Selects all files and directories in the currently active File Manager. 
 * Returns the number of file objects selected.
 *
 * @return the number of items selected if successful, otherwise 0.
 */
long CMainWindow::selectAllFileObjects()
{
    long lReturn = 0L;

    try
	{	
		long lCount = 0L;

		// validate active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return lReturn;

		static bool bSelected = false;
		bSelected = !bSelected;
		HTREEITEM hTreeItem = GetSelectedItem();
		CWin32TreeView::TreeView_SelectAll(m_hwndActiveFileManager, bSelected, hTreeItem);

		//// Get item count
		//lCount = SendMessage(m_hwndActiveFileManager, LB_GETCOUNT, (WPARAM)0, 0L);

		//// Clear the current selection
		//SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)FALSE, -1L);

		// validate, continue
		//if(lCount != LB_ERR)
		//{
			//TCHAR tstrBuffer[MAX_PATH];

			//for(long lcv = 0L; lcv < lCount; lcv++)
			//{
			//	// Changed 03-29-2011 to select ALL items which are neither the parent
			//	//	 directory nor the machine root.
			//	// get current item
			//	SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)lcv, 
			//		(LPARAM)tstrBuffer);

			//	// check current item for search key (in this case make sure it
			//	//	 isn't the parent directory or the machine root)
			//	if(_tcsstr(tstrBuffer, FOLDER_MACHINEROOT) == NULL && 
			//	   lstrcmp(tstrBuffer, FOLDER_PARENT))
			//		SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)TRUE, (LPARAM)lcv);
			//}

			// Much better way to do this...

			// Select all
		//	SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)TRUE, -1L);

		//	// Remove parent folder and machine root
		//	SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)FALSE, 
		//		INDEX_MACHINEROOT);
		//	SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)FALSE,
		//		INDEX_PARENTFOLDER);
		//}
    }
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to select all files.");
        
        // set fail value
        lReturn = 0L;
    }

    // return success / fail val
    return lReturn;
}

/**
 * Executes whatever the appropriate, double-click action is for the item 
 * in the active File Manager.
 *
 * @return
 */
BOOL CMainWindow::executeSelectedItem_TV(int iID)
{
    BOOL bReturn = FALSE;

    try
	{
		long lIndex = -1L;
		//int iID = -1;

		// validate active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return bReturn;
				
		//m_hwndActiveFileManager = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER1);	
		m_hwndActiveFileManager = GetDlgItem(m_hwndThis, iID);		
		HWND hTreectrl = m_hwndActiveFileManager;//GetDlgItem(m_hwndThis,IDC_TVFILEMANAGER1);
		// get currently selected item
		TV_ITEM tvi;
		char Text[255]="";
		memset(&tvi,0,sizeof(tvi));

		Selected=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iID,TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)Selected);
		
		if(Selected==NULL)
		{
			MessageBox(m_hwndThis,"No Items in TreeView","Error",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}

		// check if user has selected root node
		HTREEITEM hRoot = TreeView_GetRoot(hTreectrl);
		if(hRoot == Selected)
			return FALSE;		
		
		TreeView_EnsureVisible(hTreectrl,Selected);
	    SendDlgItemMessage(m_hwndThis,iID,TVM_SELECTITEM,TVGN_CARET,(LPARAM)Selected);
		flagSelected=true;
		
        tvi.mask=TVIF_TEXT;
		tvi.pszText=Text;
		tvi.cchTextMax=256;
		tvi.hItem=Selected;
		
		// get parent text till root
		HTREEITEM hcurrItem = NULL;
		//BOOL IsRoot = FALSE;
		std::vector<std::string> vPath;
		hcurrItem = Selected;
		do
		{
			
			char Text1[255]="";
			TV_ITEM tv1; 
			tv1.mask=TVIF_TEXT;
			tv1.pszText=Text1;
			tv1.cchTextMax=256;
				
			//hcurrItem = (HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_GETITEM,TVGN_PARENT,(LPARAM)&tv1);
			hcurrItem = TreeView_GetParent(hTreectrl,hcurrItem);  

			if(hcurrItem)
			{
				tv1.hItem=hcurrItem;
				bReturn = TreeView_GetItem(hTreectrl,&tv1);
				if(strcmp(tv1.pszText,TVFOLDER_ROOT) != 0)
				{
					// Parth Software Solution buf fixing
					std::string fullfilename = tv1.pszText;
					int found=fullfilename.find(SEARCHKEY_DIRECTORY);
					if (found!= tstring::npos)
					{					
						int found1 = fullfilename.find("|");
						tstring tmpfile = fullfilename.substr(found1 + 1);
						int found2 = tmpfile.find("|");
						tmpfile = fullfilename.substr(found1 + 1,found2-1);

						StringTrim(tmpfile);
						TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
						_tcscpy(tv1.pszText,tmp1);

					}
					// it is file
					else
					{
						if (found!= tstring::npos)
						{
							int found1 = fullfilename.find("|");
							tstring tmpfile = fullfilename.substr(found1 + 1);
							int found2 = tmpfile.find("|");
							tmpfile = fullfilename.substr(found1 + 1,found2-1);
							
							StringTrim(tmpfile);
						
							TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
							_tcscpy(tv1.pszText,tmp1);
							
						}
					}
					//
					vPath.push_back(tv1.pszText);
				}
				//IsRoot = FALSE;
			}
		}while(hcurrItem != NULL);
		//lIndex = SendMessage(m_hwndActiveFileManager, LB_GETCURSEL, (WPARAM)0, 
		//			0L);

		// Get ID for current File Manager - i.e. determine whether this is
		//	 File Manager 1 or 2.
		//iID = GetDlgCtrlID(m_hwndActiveFileManager);

		// validate selected item, continue
		//if(lIndex > -1L)//ntf
		{
			TCHAR tstrBuffer[MAX_PATH];

			// get selected item
			/*if(SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)lIndex,
			   (LPARAM)tstrBuffer) != LB_ERR)*/
			if(SendDlgItemMessage(m_hwndThis,iID,TVM_GETITEM,TVGN_CARET,(LPARAM)&tvi))
			{
				// Get base folder for selected item
				tstring strTemp = EMPTY_STRING;
				LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;

				pllstActive = m_pllstFileManager1;//shri
				strTemp = g_csetApplication.lastFolderFileManager1();				
				{
					//
					std::vector<std::string>::const_iterator itr; 
					_tcscpy(tstrBuffer , "");
					  //for (itr=vPath.end(); itr!=vPath.begin(); itr++) 
					  for(int i=vPath.size()-1;i >= 0;i--)
					  { 
						  std::string str = vPath[i];//(*itr);
						 _tcscat(tstrBuffer , str.c_str());
						_tcscat(tstrBuffer , "\\");
					  } 
					//
					//memcpy(tstrBuffer,"",260);
					//_tcscpy( tstrBuffer , strTemp.c_str());
					  //Parth Software Solution bug fixing
					  BOOL IsFile = FALSE;
					  if(_tcscmp(tstrBuffer,"") != 0)
					  {
						  GetSelectedItemText(m_hwndActiveFileManager, tvi.pszText, _countof(tstrBuffer),&IsFile);						
					  }
					  // Parth Software Solution
					_tcscat(tstrBuffer , tvi.pszText);
					if(IsFile)
						return TRUE;
					//_tcscat(tstrBuffer , "\\");
				}
				
				if(FILE_ATTRIBUTE_DIRECTORY != GetFileAttributes((LPCTSTR)tstrBuffer))
				{
					int lcv = _tcslen(tstrBuffer);
					if(tstrBuffer[lcv - 1] != _T(':'))
					{
						// store folder
						return FALSE;
					}
				}

				// Succeeded, store in settings
				if(iID == IDC_TVFILEMANAGER1)
					g_csetApplication.lastFolderFileManager1(tstrBuffer);
				else
					g_csetApplication.lastFolderFileManager2(tstrBuffer);					

				if(hTreectrl)
					DeleteAllChildItems(hTreectrl, Selected);

				// get directory listing
				bReturn = getDirectoryListing_TV(tstrBuffer, m_hwndActiveFileManager,
							pllstActive);

				// Copy to lookup list
				copyFileListToLookupList();

				// Select parent folder in list
				/*SendMessage(m_hwndActiveFileManager, LB_SETSEL, 
					(WPARAM)TRUE, (LPARAM)INDEX_PARENTFOLDER);*/

				//pllstActive = m_pllstFileManager1;
			}
		}

		//Parth Software Solution
		TreeView_SelectItem(m_hwndActiveFileManager,Selected);
		TreeView_Expand(m_hwndActiveFileManager,Selected, TVM_EXPAND);
		//Parth Software Solution
	}
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to execute the selected item.");
        
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
 * Executes whatever the appropriate, double-click action is for the item 
 * in the active File Manager.
 *
 * @return
 */
BOOL CMainWindow::executeSelectedItem()
{
    BOOL bReturn = FALSE;

    try
	{
		long lIndex = -1L;
		int iID = -1;

		// validate active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return bReturn;
		
		// get currently selected item
		lIndex = SendMessage(m_hwndActiveFileManager, LB_GETCURSEL, (WPARAM)0, 
					0L);

		// Get ID for current File Manager - i.e. determine whether this is
		//	 File Manager 1 or 2.
		iID = GetDlgCtrlID(m_hwndActiveFileManager);

		// validate selected item, continue
		if(lIndex > -1L)
		{
			TCHAR tstrBuffer[MAX_PATH];

			// get selected item
			if(SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)lIndex,
			   (LPARAM)tstrBuffer) != LB_ERR)
			{
				tstring strTemp = EMPTY_STRING;
				LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;

				// Get base folder for selected item
				if(iID == IDC_LSTFILEMANAGER1)
				{
					strTemp = g_csetApplication.lastFolderFileManager1();
					pllstActive = m_pllstFileManager1;
				}
				else
				{
					strTemp = g_csetApplication.lastFolderFileManager2();
					pllstActive = m_pllstFileManager2;
				}

				// check and see what "action," if any, should be performed
				if(lstrcmp(tstrBuffer, FOLDER_PARENT) == 0)
				{
					// store
					lstrcpy(tstrBuffer, (TCHAR *)strTemp.data());

					// move up to parent folder
					for(long lcv = strTemp.length() - 2; lcv > -1; lcv--)
					{
						if(tstrBuffer[lcv] == _T('\\'))
						{
							// store folder
							tstrBuffer[++lcv] = 0;

							// exit loop
							break;
						}
					}

					// Get Directory Listing
					if(getDirectoryListing(tstrBuffer, 
					   m_hwndActiveFileManager, pllstActive))
					{
						// Succeeded, store in settings
						if(iID == IDC_LSTFILEMANAGER1)
							g_csetApplication.lastFolderFileManager1(
							tstrBuffer);
						else
							g_csetApplication.lastFolderFileManager2(
								tstrBuffer);

						// Select parent folder in list
						SendMessage(m_hwndActiveFileManager, LB_SETSEL, 
							(WPARAM)TRUE, (LPARAM)INDEX_PARENTFOLDER);

						// Copy to lookup list
						copyFileListToLookupList();

						// set success return val
						bReturn = TRUE;
					}
				}
				else if(_tcsstr(tstrBuffer, SEARCHKEY_DIRECTORY) != NULL)
				{
					// Folder... append folder name to current, store, then get
					//	 new folder listing
					
					// Append...
					if(strTemp[strTemp.length() - 1] != _T('\\'))
						strTemp += _T("\\");
					
					// Attempt to get directory name and append
					if(getDirectoryNameFromItem(tstrBuffer, tstrBuffer))
					{
						// Store
						strTemp += tstrBuffer;
						strTemp += _T('\\');

						// Get Directory Listing
						if(getDirectoryListing((TCHAR *)strTemp.data(), 
						   m_hwndActiveFileManager, pllstActive))
						{
							// Succeeded, store in settings
							if(iID == IDC_LSTFILEMANAGER1)
								g_csetApplication.lastFolderFileManager1(
								(TCHAR *)strTemp.data());
							else
								g_csetApplication.lastFolderFileManager2(
									(TCHAR *)strTemp.data());

							// Select parent folder in list
							SendMessage(m_hwndActiveFileManager, LB_SETSEL, 
								(WPARAM)TRUE, (LPARAM)INDEX_PARENTFOLDER);

							// Copy to lookup list
							copyFileListToLookupList();

							// set success return val
							bReturn = TRUE;
						}
					}
				}
				else if(lstrcmp(tstrBuffer, FOLDER_MACHINEROOT) == 0)
				{
					// Display drive list
					bReturn = getDriveListing(m_hwndActiveFileManager);

					// Succeeded, store in settings
					if(iID == IDC_LSTFILEMANAGER1)
						g_csetApplication.lastFolderFileManager1(FOLDER_ROOT);
					else
						g_csetApplication.lastFolderFileManager2(FOLDER_ROOT);

					// Copy to lookup list
					copyFileListToLookupList();

					// Select first folder in list
					SendMessage(m_hwndActiveFileManager, LB_SETSEL, 
						(WPARAM)TRUE, (LPARAM)0);
				}
				else if(_tcsstr(tstrBuffer, SEARCHKEY_DRIVE))
				{
					// Drive
					if(getDriveNameFromItem(tstrBuffer, tstrBuffer))
					{
						// Succeeded, store in settings
						if(iID == IDC_TVFILEMANAGER1)
							g_csetApplication.lastFolderFileManager1(tstrBuffer);
						else
							g_csetApplication.lastFolderFileManager2(tstrBuffer);					

						// get directory listing
						bReturn = getDirectoryListing(tstrBuffer, m_hwndActiveFileManager,
									pllstActive);

						// Copy to lookup list
						copyFileListToLookupList();

						// Select parent folder in list
						SendMessage(m_hwndActiveFileManager, LB_SETSEL, 
							(WPARAM)TRUE, (LPARAM)INDEX_PARENTFOLDER);
					}
				}
				else
				{
					// File
					if(getFilenameFromItem(tstrBuffer, tstrBuffer))
					{
						tstring strFilename = EMPTY_STRING;
						TCHAR *ptcExtension = NULL;

						// construct fullpath
						strFilename = strTemp;
						if(strFilename[strFilename.length() - 1] != _T('\\'))
							strFilename += _T("\\");
						strFilename += tstrBuffer;

						// get extension
						if(strFilename.length() > 3)
							ptcExtension = &strFilename[strFilename.length() - 3];
						
						// validate, continue
						if(ptcExtension)
						{
							if(_tcsstr(FILEEXTENSIONS_AUTOCAD_ALL, 
									   ptcExtension) != NULL)
							{
								// attempt to open and render drawing
								if(m_cdwgengThis)
									m_cdwgengThis->setFilename(
										(TCHAR *)strFilename.data());

								// order redraw...
								InvalidateRect(m_hwndThis, NULL, TRUE);
							}
							else
							{
								// do nothing for now
							}
						}
					}
				}
			}
		}
	}
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to execute the selected item.");
        
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
 * Given a formatted "File Manager" string, returns the name of the 
 * directory, if any.
 *
 * @note Because of the way this method handles the locating of the
 * actual file system object's name, it is SAFE to use the same
 * buffer for both input and output parameters.
 *
 * @param tstrItem
 *
 * @param tstrOutDirectoryName
 *
 * @return
 */
BOOL CMainWindow::getDirectoryNameFromItem(TCHAR *tstrItem, 
	TCHAR *tstrOutDirectoryName)
{
	BOOL bReturn = FALSE;

	try
	{
		WIN32_FIND_DATA *pwfdItem = NULL;
		TCHAR *ptcTemp = NULL;
		int iIndex = -1;

		// validate item
		if(lstrlen(tstrItem) == 0)
			return bReturn;
		if(lstrcmp(tstrItem, FOLDER_PARENT) == 0)
			return bReturn;
		
		// validate out param
		if(tstrOutDirectoryName == NULL)
			return bReturn;

		// validate active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return bReturn;

		// validate active File Manager's file list
		if(m_pllstActiveFileManager == NULL)
			return bReturn;
		if(m_pllstActiveFileManager->getLength() == 0)
			return bReturn;
		
		// attempt to locate directory indicator
		ptcTemp = _tcsstr(tstrItem, SEARCHKEY_DIRECTORY);
		if(ptcTemp == NULL)	
			return bReturn;

		// get index of item
		iIndex = SendMessage(m_hwndActiveFileManager, LB_FINDSTRINGEXACT,
					(WPARAM)-1, (LPARAM)tstrItem);

		// validate
		if(iIndex == LB_ERR)
			return bReturn;
		else
			// adjust index by two for the entries which are always added
			iIndex -= 2;

    if (FILE_INFORMATION* fi = m_pllstActiveFileManager->getEntry(iIndex))
    {
		  // get item, set filename
		  pwfdItem = fi->pwfdFileInfo;

		  //// move past directory indicator
		  //ptcTemp += (lstrlen(SEARCHKEY_DIRECTORY) * sizeof(TCHAR));
		  //
		  //// move to next non-whitespace character
		  //for(; ptcTemp; ptcTemp++)
		  //{
		  //	if(*ptcTemp != _T(' '))
		  //		break;
		  //}

		  // check current pointer
		  if(pwfdItem)
		  {
			  // copy to output param
			  lstrcpy(tstrOutDirectoryName, pwfdItem->cFileName);

			  // if we made it here, return success
			  bReturn = TRUE;
		  }
    }
	}
	catch(...)
	{
		// set last error
		m_strLastError = EMPTY_STRING;

		// reset fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Given a formatted "File Manager" string, returns the name of the 
 * file contained in it, if any.
 *
 * @note Because of the way this method handles the locating of the
 * actual file system object's name, it is SAFE to use the same
 * buffer for both input and output parameters.
 *
 * @param tstrItem
 *
 * @param tstrOutFilename
 *
 * @return
 */
BOOL CMainWindow::getFilenameFromItem(TCHAR *tstrItem, 
	TCHAR *tstrOutFilename)
{
	BOOL bReturn = FALSE;

	try
	{
		WIN32_FIND_DATA *pwfdItem = NULL;
		TCHAR *ptcTemp = NULL;
		int iIndex = -1;

		// validate item
		if(lstrlen(tstrItem) == 0)
			return bReturn;
		if(lstrcmp(tstrItem, FOLDER_PARENT) == 0)
			return bReturn;

		// validate out param
		if(tstrOutFilename == NULL)
			return bReturn;

		// validate active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return bReturn;

		// validate active File Manager's file list
		if(m_pllstActiveFileManager == NULL)
			return bReturn;
		if(m_pllstActiveFileManager->getLength() == 0)
			return bReturn;

		// make sure this isn't a directory
		if(_tcsstr(tstrItem, SEARCHKEY_DIRECTORY) != NULL)
			return bReturn;

		
		std::string fullfilename(tstrItem);
		int found1 = fullfilename.find("|");
		tstring tmpfile = fullfilename.substr(found1 + 1);
		int found2 = tmpfile.find("|");
		tmpfile = fullfilename.substr(found1 + 1,found2-1);
		
		StringTrim(tmpfile);

		_tcscpy(tstrOutFilename, tmpfile.c_str());
		

		//// get index of item
		//iIndex = SendMessage(m_hwndActiveFileManager, LB_FINDSTRINGEXACT,
		//	(WPARAM)-1, (LPARAM)tstrItem);

		//// validate
		//if(iIndex == LB_ERR)
		//	return bReturn;
		//else
		//	// adjust index by two for entries which are always added
		//	iIndex -= 2;

		//// get item, set filename
		//if (FILE_INFORMATION* fi = m_pllstActiveFileManager->getEntry(iIndex))
		//{
		//	pwfdItem = fi->pwfdFileInfo;

		//	//// position pointer at what should be the start of the filename
		//	//ptcTemp = &tstrItem[SEARCHKEY_STARTPOS_FILENAME];

		//	// check current pointer
		//	if(pwfdItem)
		//	{
		//		// copy to output param
		//		lstrcpy(tstrOutFilename, pwfdItem->cFileName);

		//		// if we made it here, return success
		//		bReturn = TRUE;
		//	}
		//}
	}
	catch(...)
	{
		// set last error
		m_strLastError = EMPTY_STRING;

		// reset fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Given a formatted "File Manager" string, returns the name of the 
 * drive contained in it, if any.
 *
 * @note Because of the way this method handles the locating of the
 * actual file system object's name, it is NOTE SAFE to use the same
 * buffer for both input and output parameters. This is DIFFERENT
 * than the other two similar methods.
 *
 * @param tstrItem
 *
 * @param tstrOutDriveName
 *
 * @return
 */
BOOL CMainWindow::getDriveNameFromItem(TCHAR *tstrItem, 
	TCHAR *tstrOutDriveName)
{
	BOOL bReturn = FALSE;

	try
	{
		TCHAR *ptcTemp = NULL,
			  tstrBuffer[MAX_PATH] = EMPTY_STRING;
		tstring strTemp = EMPTY_STRING;

		// validate item
		if(lstrlen(tstrItem) == 0)
			return bReturn;
		
		// validate out param
		if(tstrOutDriveName == NULL)
			return bReturn;

		// copy item to local buffer
		_tcscpy(tstrBuffer, tstrItem);

		// attempt to locate directory indicator
		ptcTemp = _tcsstr(tstrBuffer, SEARCHKEY_DRIVE);
		if(ptcTemp == NULL)	
			return bReturn;

		// End string where drive search key starts
		*ptcTemp = 0;

		// store string...
		strTemp = tstrBuffer;
		
		// trim and replace trailing space with a backslash
		strTemp[strTemp.length() - 1] = _T('\\');

		// copy to output string
		_tcscpy(tstrOutDriveName, (TCHAR *)strTemp.data());

		// one last sanity check
		if(lstrlen(tstrOutDriveName))
			bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = EMPTY_STRING;

		// reset fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Using the active File Manager, creates a new directory within the current
 * folder.
 *
 * @return TRUE if the user specified directory is created successfully,
 * otherwise FALSE.
 */
BOOL CMainWindow::createNewDirectory()
{
	CCreateDirectoryDialog *pcddlgThis = NULL;
	BOOL bReturn = FALSE;

	try
	{
		int iReturn = 0;

		// check active File Manager
		if(m_hwndActiveFileManager == NULL)
		{
			// set last error
			m_strLastError = _T("The active File Manager is invalid.");

			// return fail val
			return bReturn;
		}
		
		// Attempt to create new create directory dialog instance
		pcddlgThis = new CCreateDirectoryDialog(m_hinstApplication);
		
		// validate, continue
		if(pcddlgThis == NULL)
		{
			// set last error
			m_strLastError = _T("Could not create instance of new directory dialog.");

			// return fail val
			return bReturn;
		}
		
		// Show dialog (show modally)
		iReturn = pcddlgThis->show();
		
		// check modal return value
		if(iReturn == IDOK)
		{
			tstring strDirectoryName = EMPTY_STRING,
						strDirectoryFullpath = EMPTY_STRING;

			TCHAR szFullPath[MAX_PATH] = {0};
			TCHAR tstrItem[MAX_PATH] = {0};

			// get new directory name
			strDirectoryName = pcddlgThis->getDirectoryName();

			// check name
			if(strDirectoryName.length() != 0)
			{
				int iFileManagerID = 0;
				long lReturn = 0L;
				
				// get which File Manager
				iFileManagerID = GetDlgCtrlID(m_hwndActiveFileManager);
				
				// create fullpath to directory
				//	 Select base path depending on which File Manager this is
				//if(iFileManagerID == IDC_TVFILEMANAGER1)
				//	strDirectoryFullpath = g_csetApplication.lastFolderFileManager1();
				//else
				//	strDirectoryFullpath = g_csetApplication.lastFolderFileManager2();

				//if there is no checked item
				int iIndex = GetSelectedItemsCount(m_hwndActiveFileManager);
				if(iIndex == 0)
				{
					WrappedMessageBox("Please select atleast single item/s", 			MAINWINDOW_TITLE,
							MB_OK | MB_ICONINFORMATION);
					CWin32TreeView::TreeView_SelectAll(m_hwndActiveFileManager, false);
					return bReturn;
				}
				if(iIndex > 1)
				{
					WrappedMessageBox("Selection of Destination folder should be only one", MAINWINDOW_TITLE,
						MB_OK | MB_ICONINFORMATION);
					CWin32TreeView::TreeView_SelectAll(m_hwndActiveFileManager, false);
					return bReturn;
				}

				HTREEITEM hTreeItem = CWin32TreeView::GetCurrentSelectedItem(m_hwndActiveFileManager);
				if(!CWin32TreeView::GetSelectedItemParentPath(m_hwndActiveFileManager, hTreeItem, szFullPath))
				{
					return false;
				}

				strDirectoryFullpath = szFullPath;
				CWin32TreeView::GetItemText(m_hwndActiveFileManager, hTreeItem, tstrItem, _countof(tstrItem));
				strDirectoryFullpath += tstrItem;

				//	 Append backslash if necessary
				if(strDirectoryFullpath[strDirectoryFullpath.length() - 1] != _T('\\'))
					strDirectoryFullpath += _T("\\");
				//	 Append directory name
				strDirectoryFullpath += strDirectoryName;
				
				// Attempt to create directory
				lReturn = CreateDirectory((TCHAR *)strDirectoryFullpath.data(), NULL);
				
				// check and see if the create failed if the folder already existed...
				if(lReturn == 0L && GetLastError() == ERROR_ALREADY_EXISTS)
					lReturn = 2L;

				int ifound = strDirectoryFullpath.rfind("\\");
				strDirectoryFullpath[ifound+1] = '\0';

				//// Clear current selection
				//SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)FALSE, (LPARAM)-1);
				g_csetApplication.lastFolderFileManager1((TCHAR *)strDirectoryFullpath.c_str());
				

				// check return val, update corresponding File Manager
				if(lReturn)
				{
					// Update folder listing...
					if(lReturn != 2L)
					{
						if(iFileManagerID == IDC_TVFILEMANAGER1)
						{
							if(!getDirectoryListing_TV(g_csetApplication.lastFolderFileManager1(),
								m_hwndActiveFileManager, m_pllstFileManager1))
								displayDirectoryListing_TV( m_hwndActiveFileManager, m_pllstFileManager1 ); 
						}
						else
						{
							if(!getDirectoryListing_TV(g_csetApplication.lastFolderFileManager2(),
								m_hwndActiveFileManager, m_pllstFileManager2))
								displayDirectoryListing_TV( m_hwndActiveFileManager, m_pllstFileManager2 ); 
						}
					}

					// Select new item
					selectFileOrDirectory((TCHAR *)strDirectoryName.data(), TRUE);

					// set success val
					bReturn = TRUE;
				}
			}
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to create the directory specified, an unexpected error occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// garbage collect
	if(pcddlgThis != NULL)
	{
		delete pcddlgThis;
		pcddlgThis = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Selects, depending on the is directory flag, a file or folder in the 
 * currently active File Manager whose name matches the on specified.
 * 
 * @param tstrFileSystemObjectName
 *
 * @param bIsDirectory
 *
 * @return
 */
int CMainWindow::selectFileOrDirectory(TCHAR *tstrFileSystemObjectName, 
	BOOL bIsDirectory)
{
	int iReturn = LB_ERR;

	try
	{
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;
		long lCount = 0L;

		// check active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return iReturn;

		// validate file system object name
		if(lstrlen(tstrFileSystemObjectName) == 0)
			return iReturn;
		
		// attempt to get item count
		lCount = SendMessage(m_hwndActiveFileManager, LB_GETCOUNT, (WPARAM)0, 0L);
		if(lCount == 0)
			return iReturn;

		// attempt to locate item... NOTE: this loop is optimized for clarity
		for(long lcv = 0L; lcv < lCount; lcv++)
		{
			// get current item
			SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)lcv, 
				(LPARAM)tstrBuffer);
			
			// get file system object name
			if(bIsDirectory)
			{
				if(getDirectoryNameFromItem(tstrBuffer, tstrBuffer))
				{
					if(lstrcmp(tstrBuffer, tstrFileSystemObjectName) == 0)
					{
						// return index
						iReturn = (int)lcv;

						// we're done, exit
						break;
					}
				}
			}
			else
			{
				if(getFilenameFromItem(tstrBuffer, tstrBuffer))
				{
					if(lstrcmp(tstrBuffer, tstrFileSystemObjectName) == 0)
					{
						// return index
						iReturn = (int)lcv;

						// we're done, exit
						break;
					}
				}
			}
		}
		
		// Select item
		if(iReturn != LB_ERR)
			SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)TRUE, 
				iReturn);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to select the specified object, an unexpected error occurred.");

		// reset fail val
		iReturn = LB_ERR;
	}

	// reset last error, if applicable
	if(iReturn != LB_ERR)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return iReturn;
}

/**
 * Displays the settings dialog so the user can choose a new resolution / 
 * display settings.
 *
 * @note The settings dialog can ONLY be displayed when the application is
 * running in windowed mode. This prevents us from needing multiple versions
 * of this dialog.
 *
 * @return TRUE if the settings dialog is loaded and display successfully,
 * otherwise FALSE. NOTE: whether the user chooses OK or Cancel from the
 * settings dialog is not reflected in the return value.
 */
BOOL CMainWindow::displaySettings()
{
	CSettingsDialog *pcsetdlgThis = NULL;
	BOOL bReturn = FALSE;
	
	try
	{
		int iReturn = IDCANCEL;
		tstring strGraphicsModeBefore = EMPTY_STRING,
					strGraphicsModeAfter = EMPTY_STRING;

		// validate application instance
		if(m_hinstApplication == NULL)
			return bReturn;

		// make sure we are not in full screen mode...
		if(m_bInFullScreenMode)
		{
			// Inform user they will need to revert to windowed mode to use this
			MessageBox(m_hwndThis, _T("The settings dialog can only be accessed when running\nin windowed mode. Please exit full screen mode."), 
				MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

			// return fail val
			return bReturn;
		}

		// Attempt to create settings dialog
		pcsetdlgThis = new CSettingsDialog(m_hinstApplication, m_hwndThis);

		// validate, continue
		if(pcsetdlgThis == NULL)
		{
			// set last error
			m_strLastError = _T("An error occurred while attempting to load/show the settings dialog.");

			// return fail val
			return bReturn;
		}

		// get current graphics mode
		strGraphicsModeBefore = g_csetApplication.graphicsMode();

		// Display, capture modal return val
		iReturn = pcsetdlgThis->show();

		// Check return val
		if(iReturn == IDOK)
		{
			// reload settings for this object...
			
			// get selected graphics mode
			strGraphicsModeAfter = g_csetApplication.graphicsMode();

			// compare
			if(strGraphicsModeBefore != strGraphicsModeAfter && m_bInFullScreenMode)
			{
				// change mode
				changeWindowMode(m_bInFullScreenMode);
			}
		}

		// If we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to display the application's settings, an unexpected error occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// garbage collect
	if(pcsetdlgThis != NULL)
	{
		delete pcsetdlgThis;
		pcsetdlgThis = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Displays the options dialog. NOTE: The options dialog currently contains
 * no options and is reserved for future use.
 *
 *
 * @return TRUE if the options dialog is loaded and display successfully,
 * otherwise FALSE. NOTE: whether the user chooses OK or Cancel from the
 * options dialog is not reflected in the return value.
 */
BOOL CMainWindow::displayOptions()
{
	COptionsDialog *pcopdlgThis = NULL;
	BOOL bReturn = FALSE;
	
	try
	{
		int iReturn = IDCANCEL;
		tstring strGraphicsModeBefore = EMPTY_STRING,
					strGraphicsModeAfter = EMPTY_STRING;

		// validate application instance
		if(m_hinstApplication == NULL)
			return bReturn;

		// make sure we are not in full screen mode...
		if(m_bInFullScreenMode)
		{
			// Inform user they will need to revert to windowed mode to use this
			MessageBox(m_hwndThis, _T("The options dialog can only be accessed when running\nin windowed mode. Please exit full screen mode."), 
				MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

			// return fail val
			return bReturn;
		}

		// Attempt to create settings dialog
		pcopdlgThis = new COptionsDialog(m_hinstApplication);

		// validate, continue
		if(pcopdlgThis == NULL)
		{
			// set last error
			m_strLastError = _T("An error occurred while attempting to load/show the options dialog.");

			// return fail val
			return bReturn;
		}

		// Display, capture modal return val
		iReturn = pcopdlgThis->show();

		// Check return val
		if(iReturn == IDOK)
		{
			// While not ideal, we must destroy the command prompt's background
			//	 color brush here so it will be updated when the repaint occurs.
			DeleteObject(m_hbrCommandPromptBackground);
			m_hbrCommandPromptBackground = NULL;

			EditsBkColor();

			// Order redraw to update any color changes
			InvalidateRect(m_hwndThis, NULL, TRUE);
		}
		// If we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to display the application's options, an unexpected error occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// garbage collect
	if(pcopdlgThis != NULL)
	{
		delete pcopdlgThis;
		pcopdlgThis = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

void CMainWindow::SetSelectedTextAndBand()
{

	//HTREEITEM hItem = TreeView_GetSelection(m_hwndActiveFileManager);
	//CWin32TreeView::SetItemBold(m_hwndActiveFileManager, hItem, TRUE);

	return;

	HWND tp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TVFILEMANAGER2);
	COLORREF tpcr = g_csetApplication.Color(CSettings::FileManager1, CSettings::SelectedText);
	//SendMessage(							 // returns LRESULT in lResult
	//	tp,							 // (HWND) handle to destination control
	//	TVM_SETBKCOLOR,					 // (UINT) message ID
	//	0,								 // = 0; not used, must be zero 
	//	(LPARAM)tpcr                 // = (LPARAM)(COLORREF) clrBk;
	//	);

	//tpcr = g_csetApplication.Color(CSettings::FileManager1, CSettings::SelectedText);
	SendMessage(							 // returns LRESULT in lResult
		tp,							 // (HWND) handle to destination control
		TVM_SETTEXTCOLOR,					 // (UINT) message ID
		0,								 // = 0; not used, must be zero 
		(LPARAM)tpcr                 // = (LPARAM)(COLORREF) clrBk;
		);

	// bug fix for text color
	tpcr = g_csetApplication.Color(CSettings::FileManager2, CSettings::SelectedText);
	SendMessage(							 // returns LRESULT in lResult
		tp,							 // (HWND) handle to destination control
		TVM_SETTEXTCOLOR,					 // (UINT) message ID
		0,								 // = 0; not used, must be zero 
		(LPARAM)tpcr                 // = (LPARAM)(COLORREF) clrBk;
		);
	//Parth Software Solution
}

void CMainWindow::EditsBkColor()
{
	HWND tp = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPT);
	COLORREF tpcr = g_csetApplication.Color(CSettings::CommandlineInput, CSettings::Background);
	SendMessage(tp, EM_SETBKGNDCOLOR, 0, tpcr);

	tp = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
	tpcr = g_csetApplication.Color(CSettings::CommandlineOutput, CSettings::Background);
	SendMessage(tp, EM_SETBKGNDCOLOR, 0, tpcr);

	tp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TVFILEMANAGER1);
	tpcr = g_csetApplication.Color(CSettings::FileManager1, CSettings::Background);
	SendMessage(							 // returns LRESULT in lResult
		tp,							 // (HWND) handle to destination control
		TVM_SETBKCOLOR,					 // (UINT) message ID
		0,								 // = 0; not used, must be zero 
		(LPARAM)tpcr                 // = (LPARAM)(COLORREF) clrBk;
		);

	// bug fix for text color
	tpcr = g_csetApplication.Color(CSettings::FileManager1, CSettings::ForegroundText);
	SendMessage(							 // returns LRESULT in lResult
		tp,							 // (HWND) handle to destination control
		TVM_SETTEXTCOLOR,					 // (UINT) message ID
		0,								 // = 0; not used, must be zero 
		(LPARAM)tpcr                 // = (LPARAM)(COLORREF) clrBk;
		);

	//Parth Software Solution
		
	tp = GetDlgItem(pcmwndThis->m_hwndThis, IDC_TVFILEMANAGER2);
	tpcr = g_csetApplication.Color(CSettings::FileManager2, CSettings::Background);
	SendMessage(							 // returns LRESULT in lResult
		tp,							 // (HWND) handle to destination control
		TVM_SETBKCOLOR,					 // (UINT) message ID
		0,								 // = 0; not used, must be zero 
		(LPARAM)tpcr                 // = (LPARAM)(COLORREF) clrBk;
		);

	// bug fix for text color
	tpcr = g_csetApplication.Color(CSettings::FileManager2, CSettings::ForegroundText);
	SendMessage(							 // returns LRESULT in lResult
		tp,							 // (HWND) handle to destination control
		TVM_SETTEXTCOLOR,					 // (UINT) message ID
		0,								 // = 0; not used, must be zero 
		(LPARAM)tpcr                 // = (LPARAM)(COLORREF) clrBk;
		);
	//Parth Software Solution
		
}

//Parth Software Solution
// sort bttons functions

BOOL CMainWindow::SortTV1AsPerName1(int iID)
{
	m_aseActiveSort = m_ActNameSort;//aseNameDesc; //aseSizeDesc;
	BOOL retval = SortExpandedItems_TV(iID);
	if(m_ActNameSort == aseNameAsc)
		m_ActNameSort = aseNameDesc;
	else
		m_ActNameSort = aseNameAsc;

	if(!retval)
		return FALSE;
	m_aseActiveSort = aseNone;
	return TRUE;
}

BOOL CMainWindow::SortTV1AsPerExt1(int iID)
{
	m_aseActiveSort = m_ActExtSort; //aseExtensionAsc; //aseSizeDesc;
	BOOL retval = SortExpandedItems_TV(iID);
	if(m_ActExtSort == aseExtensionAsc)
		m_ActExtSort = aseExtensionDesc;
	else
		m_ActExtSort = aseExtensionAsc;

	if(!retval)
		return FALSE;
	m_aseActiveSort = aseNone;
	return TRUE;
}

BOOL CMainWindow::SortTV1AsPerSize1(int iID)
{	
	m_aseActiveSort = m_ActSizeSort; //aseSizeDesc; //aseSizeDesc;
	BOOL retval = SortExpandedItems_TV(iID);
	if(m_ActSizeSort == aseSizeAsc)
		m_ActSizeSort = aseSizeDesc;
	else
		m_ActSizeSort = aseSizeAsc;

	if(!retval)
		return FALSE;
	m_aseActiveSort = aseNone;
	return TRUE;
}

BOOL CMainWindow::SortTV1AsPerDate1(int iID)
{	
	m_aseActiveSort = m_ActDateSort; //aseDateDesc; //aseDateDesc;
	BOOL retval = SortExpandedItems_TV(iID);
	if(m_ActDateSort == aseDateAsc)
		m_ActDateSort = aseDateDesc;
	else
		m_ActDateSort = aseDateAsc;
	if(!retval)
		return FALSE;
	m_aseActiveSort = aseNone;
	return TRUE;
}

//Parth Software Solution sort for dir
BOOL CMainWindow::SortTVAsPerDir(int iID)
{	
	m_aseActiveSort = m_ActDirSort;
	BOOL retval = SortExpandedItems_TV(iID);
	if(m_ActDirSort == aseDirAsc)
		m_ActDirSort = aseDirDesc;
	else
		m_ActDirSort = aseDirAsc;
	if(!retval)
		return FALSE;
	m_aseActiveSort = aseNone;
	return TRUE;
}
//Parth Software Solution
BOOL CMainWindow::SortTV1AsPerPermission1(int iID)
{	
	m_aseActiveSort = aseExtensionDesc; //aseExtensionDesc;
	BOOL retval = SortExpandedItems_TV(iID);
	if(!retval)
		return FALSE;
	m_aseActiveSort = aseNone;
	return TRUE;
}
BOOL CMainWindow::SortTV1AsPerAlt1(int iID)
{	
	m_aseActiveSort = aseExtensionDesc; //aseExtensionDesc;
	BOOL retval = SortExpandedItems_TV(iID);
	if(!retval)
		return FALSE;
	m_aseActiveSort = aseNone;
	return TRUE;
}

//Parth Software Solution
/**
 * Displays the help dialog.
 *
 *
 * @return TRUE if the options dialog is loaded and display successfully,
 * otherwise FALSE. NOTE: whether the user chooses OK or Cancel from the
 * options dialog is not reflected in the return value.
 */
BOOL CMainWindow::displayHelp()
{
	char szSample[100] = {0};
	strcpy(szSample, "From Client");
	XLV_PIPE_DATA_REG szdata = {0};
	szdata.eMessageInfo = 100;
	memcpy(&szdata.bData, szSample, sizeof(szdata.bData));
	CXlvCommunicator objXlvCommunicator(_NAMED_PIPE_SERVER, true);
	objXlvCommunicator.SendData(&szdata, sizeof(XLV_PIPE_DATA_REG));

	return TRUE;
	//HANDLE hFile;
	//BOOL flg;
	//DWORD dwWrite;
	//char szPipeUpdate[200];
	//hFile = CreateFile("\\\\.\\pipe\\UIServer", GENERIC_WRITE,
	//	0, NULL, OPEN_EXISTING,
	//	0, NULL);

	//strcpy(szPipeUpdate,"Data from Named Pipe client for createnamedpipe");
	//if(hFile == INVALID_HANDLE_VALUE)
	//{ 
	//	DWORD dw = GetLastError();
	//	printf("CreateFile failed for Named Pipe client\n:" );
	//}
	//else
	//{
	//	flg = WriteFile(hFile, szPipeUpdate, strlen(szPipeUpdate), &dwWrite, NULL);
	//	if (FALSE == flg)
	//	{
	//		printf("WriteFile failed for Named Pipe client\n");
	//	}
	//	else
	//	{
	//		printf("WriteFile succeeded for Named Pipe client\n");
	//	}
	//	CloseHandle(hFile);
	//}

	//return TRUE;

	CHelpDialog *pchlpdlgThis = NULL;
	BOOL bReturn = FALSE;

	try
	{
		int iReturn = IDCANCEL;
		tstring strGraphicsModeBefore = EMPTY_STRING,
			strGraphicsModeAfter = EMPTY_STRING;

		// validate application instance
		if(m_hinstApplication == NULL)
			return bReturn;

		// Attempt to create settings dialog
		pchlpdlgThis = new CHelpDialog(m_hinstApplication);

		// validate, continue
		if(pchlpdlgThis == NULL)
		{
			// set last error
			m_strLastError = _T("An error occurred while attempting to load/show the help dialog.");

			// return fail val
			return bReturn;
		}

		// Display, capture modal return val
		iReturn = pchlpdlgThis->show();

		// If we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to display the application's help, an unexpected error occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// garbage collect
	if(pchlpdlgThis != NULL)
	{
		delete pchlpdlgThis;
		pchlpdlgThis = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Displays the layer control dialog.
 *
 * @return TRUE if the options dialog is loaded and display successfully,
 * otherwise FALSE. NOTE: whether the user chooses OK or Cancel from the
 * options dialog is not reflected in the return value.
 */
BOOL CMainWindow::displayLayerControl()
{
	CLayerControlDialog *pclctldlgThis = NULL;
	BOOL bReturn = FALSE;
	
	try
	{
		int iReturn = IDCANCEL;

		// validate application instance
		if(m_hinstApplication == NULL)
			return bReturn;

		// Attempt to create settings dialog
		pclctldlgThis = new CLayerControlDialog(m_hinstApplication,
								m_cdwgengThis->getLayers());

		// validate, continue
		if(pclctldlgThis == NULL)
		{
			// set last error
			m_strLastError = _T("An error occurred while attempting to load/show the help dialog.");

			// return fail val
			return bReturn;
		}

		// Display, capture modal return val
		iReturn = pclctldlgThis->show();

		// Order a redraw of the active drawing.
		InvalidateRect(m_hwndThis, NULL, TRUE);

		// If we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to display the application's help, an unexpected error occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// garbage collect
	if(pclctldlgThis != NULL)
	{
		delete pclctldlgThis;
		pclctldlgThis = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Displays the file attributes and permissions for the currently selected
 * file in the active File Manager.
 *
 *
 * @return
 */
BOOL CMainWindow::displayFileAttributesAndPermissions()
{
	CFileAttributesDialog *pcfadlgThis = NULL;
	LinkedListEx<FILE_INFORMATION> *pllstLocal = NULL;

	int *ariSelected = NULL;
	BOOL bReturn = FALSE;

	try
	{
		tstring strBasePath = EMPTY_STRING;
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;
		int iIndex = -1;
		long //lCount,
			lSelCount;

		// validate application instance
		if(m_hinstApplication == NULL)
			return FALSE;

		// validate File Manager's handle
		if(m_hwndActiveFileManager == NULL)
			return FALSE;

		// validate File Manager's list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate this object's handle
		if(m_hwndThis == NULL)
			return FALSE;

		// attempt to create temporary, local list
		pllstLocal = new LinkedListEx<FILE_INFORMATION>();
		if(pllstLocal == NULL)
			return FALSE;

		// set auto-destroy to FALSE since storage is owned by the active File
		//	 Manager's list object
		pllstLocal->setAutoDestroy(FALSE);

		// Get count and selected item count
		//lCount = SendMessage(m_hwndActiveFileManager, LB_GETCOUNT, (WPARAM)0, 
		//				0L);
		//lSelCount = SendMessage(m_hwndActiveFileManager, LB_GETSELCOUNT, 
		//				(WPARAM)0,  0L);

		bool bIsMultipleFileSelected = false;

		lSelCount = GetSelectedItemsCount(m_hwndActiveFileManager);
		if(lSelCount == 0)
		{
			WrappedMessageBox("Please select atleast single item/s", 			MAINWINDOW_TITLE,
				MB_OK | MB_ICONINFORMATION);
			CWin32TreeView::TreeView_SelectAll(m_hwndActiveFileManager, false);
			return bReturn;
		}
		if(lSelCount > 1)
		{
			bIsMultipleFileSelected = true;
		}

		//Parth Software Solution
		int iID = GetDlgCtrlID(m_hwndActiveFileManager);
		HTREEITEM Selected = NULL;
		Selected = CWin32TreeView::GetCurrentSelectedItem(m_hwndActiveFileManager);

		if(Selected==NULL)
		{
			MessageBox(m_hwndThis,"No Items in TreeView","Error",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}

		if(/*lCount == 0L || */lSelCount == 0L)
			return FALSE;

		int iCtrlID = 0;
		iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);
		HWND hWnd1 = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER1);
		HWND hWnd2 = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER2);

		TCHAR szFullPath[MAX_PATH] = {0};
		TCHAR szItem[MAX_PATH] = {0};
		HWND hTreeCntrl = NULL;
		std::vector<HTREEITEM> vSelectedPaths;
		if(iCtrlID == IDC_TVFILEMANAGER2)//Get here reverse file manager handle
		{
			hTreeCntrl = hWnd2;
		}
		else
		{
			hTreeCntrl = hWnd1;
		}

		CWin32TreeView::TreeView_GetAllSelectedItems(hTreeCntrl, vSelectedPaths);

		std::vector<HTREEITEM>::const_iterator it;
		for( it = vSelectedPaths.begin(); it !=  vSelectedPaths.end(); it++)
		{
			HTREEITEM hTreeItem = *it;
			//Source
			if(!CWin32TreeView::GetSelectedItemParentPath(hWnd1, hTreeItem, szFullPath))
			{
				return false;
			}

			CWin32TreeView::GetItemText(hWnd1, hTreeItem, szItem, MAX_PATH);
			strBasePath = szFullPath;
			strBasePath += szItem;

			LinkedListEx<FILE_INFORMATION> *pllstFileMgr = new LinkedListEx<FILE_INFORMATION>;

			// validate, make call
			getFileInformation((TCHAR *)strBasePath.data(),
				m_hwndActiveFileManager, 
				pllstFileMgr);

			FILE_INFORMATION *pfiTemp = new FILE_INFORMATION;

			//// Attempt to get selected items
			for(long lcv = 0L; lcv < pllstFileMgr->getLength(); lcv++)
			{
				// get item... The "- 2" accounts for the machine root and
				//	 parent folder entries in the list.
				pfiTemp = pllstFileMgr->getEntry(lcv);

				// validate, continue
				if(pfiTemp != NULL && 
					(strcmp(pfiTemp->pwfdFileInfo->cFileName,szItem) == 0))
				{
					// add to temp, local list
					pllstLocal->add(pfiTemp);
				}
			}
		}

		int iFound = strBasePath.rfind(_T("\\"));
		strBasePath[iFound+1] = '\0';
		
		pcfadlgThis = new CFileAttributesDialog(m_hinstApplication, m_hwndThis,
			(TCHAR *)strBasePath.data(), pllstLocal);
		if(pcfadlgThis)
		{
			tstring strFullpath = EMPTY_STRING;
			int iReturn = IDCANCEL,
				iCtrlID = 0;

			// display file attributes
			iReturn = pcfadlgThis->show();

			// Check return value
			if(iReturn == IDOK)
			{
				// get active File Manager's ID
				iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);

				// Get information for active File Manager
				if(iCtrlID == IDC_TVFILEMANAGER1)
				{
					// set path
					strFullpath = 
						g_csetApplication.lastFolderFileManager1();
				}
				else if(iCtrlID == IDC_TVFILEMANAGER2)
				{
					// set path
					strFullpath = 
						g_csetApplication.lastFolderFileManager2();
				}

				//// validate, make call
				//getDirectoryListing((TCHAR *)strFullpath.data(),
				//			m_hwndActiveFileManager, 
				//			m_pllstActiveFileManager);
			}

			// if we made it here, return success
			bReturn = TRUE;
		}

		///////////////////////////////////////////////////////////////////////
		// Original code
		///////////////////////////////////////////////////////////////////////

		//// attempt to get selected item's index
		//iIndex = SendMessage(m_hwndActiveFileManager, LB_GETCURSEL, (WPARAM)0, 0L);
		//if(iIndex == LB_ERR/* -1 */)
		//	return bReturn;

		//// get text for item
		//SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)iIndex, 
		//	(LPARAM)tstrBuffer);

		//// make sure there is something to do
		//if(lstrlen(tstrBuffer))
		//{
		//	BOOL bSuccess = FALSE;

		//	// Determine type 
		//	if(_tcsstr(tstrBuffer, SEARCHKEY_DIRECTORY) == NULL)
		//		bSuccess = getFilenameFromItem(tstrBuffer, tstrBuffer);
		//	else
		//	{
		//		// attempt to get directory name
		//		bSuccess = getDirectoryNameFromItem(tstrBuffer, tstrBuffer);

		//		// if sucessfull, go ahead and add trailing backslash
		//		if(bSuccess)
		//			_tcscat(tstrBuffer, _T("\\"));
		//	}

		//	// get file system object's name
		//	if(bSuccess)
		//	{
		//		std::string strFullpath = EMPTY_STRING;
		//		int iCtrlID = 0;

		//		// get which File Manager
		//		iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);

		//		// construct fullpath to file
		//		//	 get base path
		//		if(iCtrlID == IDC_LSTFILEMANAGER1)
		//			strFullpath = g_csetApplication.lastFolderFileManager1();
		//		else
		//			strFullpath = g_csetApplication.lastFolderFileManager2();
		//		//	 append backslash if necessary
		//		if(strFullpath[strFullpath.length() - 1] != _T('\\'))
		//			strFullpath += _T("\\");
		//		//	 append filename
		//		strFullpath += tstrBuffer;

		//		// Attempt to create file attributes dialog
		//		pcfadlgThis = new CFileAttributesDialog(m_hinstApplication, m_hwndThis,
		//							(TCHAR *)strFullpath.data());
		//		if(pcfadlgThis)
		//		{
		//			int iReturn = IDCANCEL;
		//			
		//			// display file attributes
		//			iReturn = pcfadlgThis->show();

		//			// Check return value
		//			if(iReturn == IDOK)
		//			{
		//				LinkedListEx<FILE_INFORMATION> *pllstOutput = NULL;
		//				
		//				// Get information for active File Manager
		//				if(iCtrlID == IDC_LSTFILEMANAGER1)
		//				{
		//					// set file list
		//					pllstOutput = m_pllstFileManager1;
		//					
		//					// set path
		//					strFullpath = 
		//						g_csetApplication.lastFolderFileManager1();
		//				}
		//				else if(iCtrlID == IDC_LSTFILEMANAGER2)
		//				{
		//					// set file list
		//					pllstOutput = m_pllstFileManager2;
		//					
		//					// set path
		//					strFullpath = 
		//						g_csetApplication.lastFolderFileManager2();
		//				}

		//				// validate, make call
		//				if(pllstOutput != NULL)
		//					getDirectoryListing((TCHAR *)strFullpath.data(),
		//								m_hwndActiveFileManager, pllstOutput);
		//			}

		//			// if we made it here, return success
		//			bReturn = TRUE;
		//		}
		//	}
		//}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to display the attributes and permissions for the selected file, an unexpected error occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// garbage collect
	if(pcfadlgThis != NULL)
	{
		delete pcfadlgThis;
		pcfadlgThis = NULL;
	}
	if(pllstLocal != NULL)
	{
		delete pllstLocal;
		pllstLocal = NULL;
	}
	if(ariSelected != NULL)
	{
		delete[] ariSelected;
		ariSelected = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Prompts the user for a file mask, then applies the mask selecting all
 * files which meet the criteria.
 */
long CMainWindow::selectFilesByMask()
{
	CSelectFilesDialog *pcsfdlgThis = NULL;
	LinkedListEx<FILE_INFORMATION> *pllstTemp = NULL;
	BOOL bReturn = FALSE;

	try
	{
		HWND hwndTemp = NULL;

		// validate application instance
		if(m_hinstApplication == NULL)
			return bReturn;

		// validate File Manager's handle
		if(m_hwndActiveFileManager == NULL)
			return bReturn;

		// validate this object's handle
		if(m_hwndThis == NULL)
			return bReturn;

		// make sure we can get the temp File Manager's handle
		hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGERTEMP);
		if(hwndTemp == NULL)
			return bReturn;

		// attempt to create dialog
		pcsfdlgThis = new CSelectFilesDialog(m_hinstApplication);
		if(pcsfdlgThis)
		{
			int iReturn = IDCANCEL,
				iFileManagerID = 0;

			// display dialog
			iReturn = pcsfdlgThis->show();

			// check modal return value
			if(iReturn == IDOK)
			{
				tstring strFullpath = EMPTY_STRING;
				long lCount = 0L;
				
				// Perform a preemptive check of the mask...
				if(lstrlen(pcsfdlgThis->getFileMask()) == 0)
				{
					// set last error

					// return fail val
					return FALSE;
				}
				if(_tcscmp(pcsfdlgThis->getFileMask(), MASK_ALLFILESDIRECTORIES) == 0
					||
				   _tcscmp(pcsfdlgThis->getFileMask(), MASK_ALLFILES) == 0)
				{
					// select all file system objects
					bReturn = selectAllFileObjects();

					// clear last error, if applicable
					if(bReturn)
						m_strLastError = EMPTY_STRING;

					// return success
					return bReturn;
				}

				// get which file manager
				iFileManagerID = GetDlgCtrlID(m_hwndActiveFileManager);

				// construct fullpath depending on active file manager
				//	 get base path
				if(iFileManagerID == IDC_LSTFILEMANAGER1)
					strFullpath = g_csetApplication.lastFolderFileManager1();
				else
					strFullpath = g_csetApplication.lastFolderFileManager2();
				//	 append backslash if necessary
				if(strFullpath[strFullpath.length() - 1] != _T('\\'))
					strFullpath += _T("\\");
				//	 Append file spec
				strFullpath += pcsfdlgThis->getFileMask();

				// load file list
				pllstTemp = new LinkedListEx<FILE_INFORMATION>();
				if(pllstTemp == NULL)
				{
					// set last error
					m_strLastError = _T("Could not create the temporary file list while attempting to select files by mask.");

					// return fail
					return FALSE;
				}
				
				// get the directory listing
				getDirectoryListing((TCHAR *)strFullpath.data(), hwndTemp,
					pllstTemp);

				// remove parent folder and machine root
				SendMessage(hwndTemp, LB_DELETESTRING, (WPARAM)0, 0L);
				SendMessage(hwndTemp, LB_DELETESTRING, (WPARAM)0, 0L);

				//SendMessage(hwndTemp, LB_RESETCONTENT, (WPARAM)0, 0L);
				//SendMessage(hwndTemp, LB_DIR, 
				//	(WPARAM)DDL_ARCHIVE | DDL_READONLY | DDL_HIDDEN 
				//			| DDL_SYSTEM | DDL_READWRITE, 
				//	(LPARAM)(TCHAR *)strFullpath.data());

				// get file count
				lCount = SendMessage(hwndTemp, LB_GETCOUNT, (WPARAM)0, 0L);
				if(lCount)
				{
					WIN32_FIND_DATA *pwfdMask = NULL,
									*pwfdActive = NULL;
					int iIndex = 0,
						iTopIndex = -1;

					// Clear current selection
					SendMessage(m_hwndActiveFileManager, LB_SETSEL, 
						(WPARAM)FALSE, -1L);
					
					// Shut-off redraw
					SendMessage(m_hwndActiveFileManager, WM_SETREDRAW, 
						(WPARAM)FALSE, 0L);

					// Get current top index
					iTopIndex = SendMessage(m_hwndActiveFileManager, LB_GETTOPINDEX,
									(WPARAM)0, 0L);

					// iterate through list and attempt to locate in active File
					//	 Manager
					for(long lcv = 0L; lcv < lCount; lcv++)
					{
						// get current item
						pwfdMask = pllstTemp->getEntry((int)lcv)->pwfdFileInfo;

						// attempt to locate in File Manager
						for(; iIndex < m_pllstActiveFileManager->getLength(); )
						{
							pwfdActive = m_pllstActiveFileManager->getEntry(iIndex)->pwfdFileInfo;
							
							if(_tcscmp(pwfdMask->cFileName, 
								pwfdActive->cFileName) == 0)
							{
								SendMessage(m_hwndActiveFileManager, LB_SETSEL,
									(WPARAM)TRUE, iIndex + 2);
								break;
							}
							else
								iIndex++;
						}
					}
					
					// Restore top index
					SendMessage(m_hwndActiveFileManager, LB_SETTOPINDEX, 
						(WPARAM)iTopIndex, 0L);

					// turn redraw back on
					SendMessage(m_hwndActiveFileManager, WM_SETREDRAW, 
						(WPARAM)TRUE, 0L);
				}
			}
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to display the select files by mask dialog, an unexpected error occurred.");

		// reset fail val
		bReturn = FALSE;
	}

	// garbage collect
	if(pcsfdlgThis != NULL)
	{
		delete pcsfdlgThis;
		pcsfdlgThis = NULL;
	}
	if(pllstTemp != NULL)
	{
		pllstTemp->clear();
		delete pllstTemp;
		pllstTemp = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}



/**
 * Populates the active File Manager with the "Machine Root" (or drive 
 * listing).
 *
 * @return TRUE if the drive listing is retrieved and no errors occur,
 * otherwise FALSE.
 */
BOOL CMainWindow::getDriveListing_TV(HWND hwndOutputControl)
{
	CAttachedDrives *pcadrvTemp = NULL;
    BOOL bReturn = FALSE;

    try
	{
		LPDRIVEINFORMATION lpdrvinfTemp = NULL;
		int iCDriveIndex = -1,
			iLongestTypeName = 0,
			iTempLength = 0;

		// make sure the output control's handle is good
		if(hwndOutputControl == NULL)
			return FALSE;
		
		int iCntlID = GetDlgCtrlID(hwndOutputControl);

		// attempt to create the attached drives object
		pcadrvTemp = new CAttachedDrives();
		if(pcadrvTemp == NULL)
		{
			// last error
			m_strLastError = _T("Could not create the attached drives object.");

			// return fail val
			return FALSE;
		}

		// clear contents
		//SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0, 0L);
		TreeView_DeleteAllItems(hwndOutputControl);
		TVINSERTSTRUCT tvinsert;//TV_INSERTSTRUCT
		//Root = NULL;
		HTREEITEM currRoot = NULL;//Parth Software Solution bug fix for node 0000 double in 2nd tv 
		BOOL retval = FALSE;
		//if(tvinsert.hParent == NULL)
		{
			// creating image list and put it into the tree control
			//====================================================//
			/*
			HWND hTree = GetDlgItem(m_hwndThis,IDC_TVFILEMANAGER1);
			hImageList=ImageList_Create(16,16,ILC_COLOR16,2,10);					  // Macro: 16x16:16bit with 2 pics [array]
			hBitMap=LoadBitmap(m_hinstApplication,MAKEINTRESOURCE(IDB_TREE));					  // load the picture from the resource
			ImageList_Add(hImageList,hBitMap,NULL);								      // Macro: Attach the image, to the image list
			DeleteObject(hBitMap);													  // no need it after loading the bitmap
		    SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control
			*/

			tvinsert.hParent=NULL;			// top most level no need handle
			tvinsert.hInsertAfter=TVI_ROOT; // work as root level
			tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVM_SORTCHILDREN;
			tvinsert.item.pszText=TVFOLDER_ROOT;
			tvinsert.item.iImage=0;
			tvinsert.item.iSelectedImage=1;
			Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iCntlID,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
			//Parth Software Solution
			int iID1 = GetDlgCtrlID(hwndOutputControl);
			if(iID1 == IDC_TVFILEMANAGER1)
			{
				Root=Parent;
				currRoot=Root;
				
			}
			
			if(iID1 == IDC_TVFILEMANAGER2)
			{
				Root2=Parent;
				currRoot=Root2;
				
			}
			//Parth Software Solution
			//Root=Parent;//old
			Before=Parent;   
			//retval = TreeView_SortChildren(  hTree ,  Root,  TRUE);
		}
		
		// iterate through drives, calculate longest type name
		for(int i = 0; i < pcadrvTemp->attachedDriveCount(); i++)
		{
			// get current drive
			lpdrvinfTemp = pcadrvTemp->driveInformation(i);

			// validate, continue
			if(lpdrvinfTemp != NULL)
			{
				// check type length

				// get length of type name
				iTempLength = lstrlen(lpdrvinfTemp->tstrDriveType);

				// check against "running total"
				if(iTempLength > iLongestTypeName)
					iLongestTypeName = iTempLength;
			}
		}

		// iterate through attached drives and display
		for(int i = 0; i < pcadrvTemp->attachedDriveCount(); i++)
		{
			// get current drive
			lpdrvinfTemp = pcadrvTemp->driveInformation(i);
			
			// validate, continue
			if(lpdrvinfTemp != NULL)
			{
				TCHAR tstrBuffer[MAX_PATH];
				
				// is this the C drive?
				if(lpdrvinfTemp->tchDriveLetter == _T('C'))
					iCDriveIndex = i;

				// create output string
				/*_stprintf(tstrBuffer, FORMAT_DRIVE, lpdrvinfTemp->tchDriveLetter,
					iLongestTypeName, lpdrvinfTemp->tstrDriveType,
					lpdrvinfTemp->tstrFileSystem);*/
				_stprintf(tstrBuffer, FORMAT_DRIVE1, lpdrvinfTemp->tchDriveLetter);					

				// output to File Manager
				//SendMessage(hwndOutputControl, LB_ADDSTRING, 
				//	(WPARAM)0, (LPARAM)tstrBuffer);	
				//if(Root)
				if(currRoot)
				{
					tvinsert.hParent=currRoot; 
					tvinsert.hInsertAfter=TVI_LAST;  // below parent
					tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
					tvinsert.item.iImage=0;
					tvinsert.item.iSelectedImage=1;
					tvinsert.item.pszText=tstrBuffer;
					Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iCntlID,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
				}
				

			}
		}

		// if we made it here, return success
		bReturn = TRUE;

		// Select C drive for the default
		//SendMessage(hwndOutputControl, LB_SETSEL, (WPARAM)TRUE, iCDriveIndex);
		//ntf
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While retrieving the drive listing, an unexpected error occurred.");
		
		// set fail val
		bReturn = FALSE;
	}
	
	// garbage collect
	if(pcadrvTemp)
	{
		delete pcadrvTemp;
		pcadrvTemp = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}


////Parth Software Solution
///**
// * Populates the active File Manager with the "Machine Root" (or drive 
// * listing).
// *
// * @return TRUE if the drive listing is retrieved and no errors occur,
// * otherwise FALSE.
// */
//BOOL CMainWindow::getDriveListing_TV(HWND hwndOutputControl)
//{
//	CAttachedDrives *pcadrvTemp = NULL;
//    BOOL bReturn = FALSE;
//
//    try
//	{
//		LPDRIVEINFORMATION lpdrvinfTemp = NULL;
//		int iCDriveIndex = -1,
//			iLongestTypeName = 0,
//			iTempLength = 0;
//
//		// make sure the output control's handle is good
//		if(hwndOutputControl == NULL)
//			return FALSE;
//		
//		// attempt to create the attached drives object
//		pcadrvTemp = new CAttachedDrives();
//		if(pcadrvTemp == NULL)
//		{
//			// last error
//			m_strLastError = _T("Could not create the attached drives object.");
//
//			// return fail val
//			return FALSE;
//		}
//
//		// clear contents
//		//SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0, 0L);
//		TreeView_DeleteAllItems(hwndOutputControl);
//
//		// iterate through drives, calculate longest type name
//		for(int i = 0; i < pcadrvTemp->attachedDriveCount(); i++)
//		{
//			// get current drive
//			lpdrvinfTemp = pcadrvTemp->driveInformation(i);
//
//			// validate, continue
//			if(lpdrvinfTemp != NULL)
//			{
//				// check type length
//
//				// get length of type name
//				iTempLength = lstrlen(lpdrvinfTemp->tstrDriveType);
//
//				// check against "running total"
//				if(iTempLength > iLongestTypeName)
//					iLongestTypeName = iTempLength;
//			}
//		}
//
//		TVINSERTSTRUCT tvinsert;//TV_INSERTSTRUCT
//		tvinsert.hParent=NULL;			// top most level no need handle
//		tvinsert.hInsertAfter=TVI_ROOT; // work as root level
//        tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
//        tvinsert.item.pszText=TVFOLDER_ROOT;
//		tvinsert.item.iImage=0;
//		tvinsert.item.iSelectedImage=1;
//		Parent=(HTREEITEM)SendMessage(hwndOutputControl,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
//		Root=Parent;
//		Before=Parent;                   // handle of the before root
//		tvinsert.hParent=Parent; 
//
//		// iterate through attached drives and display
//		for(int i = 0; i < pcadrvTemp->attachedDriveCount(); i++)
//		{
//			// get current drive
//			lpdrvinfTemp = pcadrvTemp->driveInformation(i);
//			
//			// validate, continue
//			if(lpdrvinfTemp != NULL)
//			{
//				TCHAR tstrBuffer[MAX_PATH];
//				
//				// is this the C drive?
//				if(lpdrvinfTemp->tchDriveLetter == _T('C'))
//					iCDriveIndex = i;
//
//				// create output string
//				_stprintf(tstrBuffer, FORMAT_DRIVE, lpdrvinfTemp->tchDriveLetter,
//					iLongestTypeName, lpdrvinfTemp->tstrDriveType,
//					lpdrvinfTemp->tstrFileSystem);
//
//				// output to File Manager
//				/*SendMessage(hwndOutputControl, LB_ADDSTRING, 
//					(WPARAM)0, (LPARAM)tstrBuffer);*/
//				tvinsert.hInsertAfter=TVI_LAST; 
//				tvinsert.item.pszText=tstrBuffer;
//				Parent=(HTREEITEM)SendMessage(hwndOutputControl,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
//			}
//		}
//
//		// if we made it here, return success
//		bReturn = TRUE;
//
//		// Select C drive for the default
//		SendMessage(hwndOutputControl, LB_SETSEL, (WPARAM)TRUE, iCDriveIndex);
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("While retrieving the drive listing, an unexpected error occurred.");
//		
//		// set fail val
//		bReturn = FALSE;
//	}
//	
//	// garbage collect
//	if(pcadrvTemp)
//	{
//		delete pcadrvTemp;
//		pcadrvTemp = NULL;
//	}
//
//	// reset last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//    // return success / fail val
//    return bReturn;
//}
////Parth Software Solution


/**
 * Populates the active File Manager with the "Machine Root" (or drive 
 * listing).
 *
 * @return TRUE if the drive listing is retrieved and no errors occur,
 * otherwise FALSE.
 */
BOOL CMainWindow::getDriveListing(HWND hwndOutputControl)
{
	CAttachedDrives *pcadrvTemp = NULL;
    BOOL bReturn = FALSE;

    try
	{
		LPDRIVEINFORMATION lpdrvinfTemp = NULL;
		int iCDriveIndex = -1,
			iLongestTypeName = 0,
			iTempLength = 0;

		// make sure the output control's handle is good
		if(hwndOutputControl == NULL)
			return FALSE;
		
		// attempt to create the attached drives object
		pcadrvTemp = new CAttachedDrives();
		if(pcadrvTemp == NULL)
		{
			// last error
			m_strLastError = _T("Could not create the attached drives object.");

			// return fail val
			return FALSE;
		}

		// clear contents
		SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0, 0L);

		// iterate through drives, calculate longest type name
		for(int i = 0; i < pcadrvTemp->attachedDriveCount(); i++)
		{
			// get current drive
			lpdrvinfTemp = pcadrvTemp->driveInformation(i);

			// validate, continue
			if(lpdrvinfTemp != NULL)
			{
				// check type length

				// get length of type name
				iTempLength = lstrlen(lpdrvinfTemp->tstrDriveType);

				// check against "running total"
				if(iTempLength > iLongestTypeName)
					iLongestTypeName = iTempLength;
			}
		}

		// iterate through attached drives and display
		for(int i = 0; i < pcadrvTemp->attachedDriveCount(); i++)
		{
			// get current drive
			lpdrvinfTemp = pcadrvTemp->driveInformation(i);
			
			// validate, continue
			if(lpdrvinfTemp != NULL)
			{
				TCHAR tstrBuffer[MAX_PATH];
				
				// is this the C drive?
				if(lpdrvinfTemp->tchDriveLetter == _T('C'))
					iCDriveIndex = i;

				// create output string
				_stprintf(tstrBuffer, FORMAT_DRIVE, lpdrvinfTemp->tchDriveLetter,
					iLongestTypeName, lpdrvinfTemp->tstrDriveType,
					lpdrvinfTemp->tstrFileSystem);

				// output to File Manager
				SendMessage(hwndOutputControl, LB_ADDSTRING, 
					(WPARAM)0, (LPARAM)tstrBuffer);
			}
		}

		// if we made it here, return success
		bReturn = TRUE;

		// Select C drive for the default
		SendMessage(hwndOutputControl, LB_SETSEL, (WPARAM)TRUE, iCDriveIndex);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While retrieving the drive listing, an unexpected error occurred.");
		
		// set fail val
		bReturn = FALSE;
	}
	
	// garbage collect
	if(pcadrvTemp)
	{
		delete pcadrvTemp;
		pcadrvTemp = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
  * Performing operation on file system objects from between File Managers
  */
BOOL CMainWindow::operateFSOFromTo(UINT iOperation)
{
	BOOL bReturn = TRUE;	// default to optimistic return val

 	std::map<UINT, tstring> operation_strings[2];
	enum
	{
		eNameString = 0, eProcessString
	};
	operation_strings[eNameString][FO_COPY] = _T("copy");
	operation_strings[eNameString][FO_MOVE] = _T("move");
	operation_strings[eProcessString][FO_COPY] = _T("copying");
	operation_strings[eProcessString][FO_MOVE] = _T("moving");

	try
	{
		tstring strSourceBase = EMPTY_STRING,
			strDestBase = EMPTY_STRING,
			strSource = EMPTY_STRING,
			strDest = EMPTY_STRING,
			strTemp = EMPTY_STRING;
		SHFILEOPSTRUCT shfopCopy;
		HWND /*hwndProgress = NULL,*/
			hwndTemp = NULL;
		TCHAR tstrItem[MAX_PATH + 1] = EMPTY_STRING,
			tstrSrcBuffer[MAX_PATH + 1] = EMPTY_STRING,
			tstrDestBuffer[MAX_PATH + 1] = EMPTY_STRING;
		int iCtrlID = 0;
		long lCount = 0L,
			lSelCount = 0L,
			lCurSel = 0L;
		BOOL bIsFile = FALSE;

		// validate active file manager
		if(m_hwndActiveFileManager == NULL)
		{
			// set last error
			m_strLastError = _T("No active File Manager.");

			// return fail
			return FALSE;
		}

		// Attempt to get which File Manager is active
		iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);
		HWND hWnd1 = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER1);
		HWND hWnd2 = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER2);

		HWND m_hwndSndSeltdFileMan = NULL;
		if(iCtrlID == IDC_TVFILEMANAGER1)
		{
			m_hwndSndSeltdFileMan = hWnd2;
		}
		else
		{
			m_hwndSndSeltdFileMan = hWnd1;
		}
		// Make sure there is something to do... nothing to do DOES NOT indicate
		//	 an error condition
		///lCount = SendMessage(m_hwndActiveFileManager, LB_GETCOUNT, (WPARAM)0, 0L);
		//lSelCount = SendMessage(m_hwndActiveFileManager, LB_GETSELCOUNT, (WPARAM)0, 
		//				0L);
		lSelCount = GetSelectedItemsCount(m_hwndSndSeltdFileMan);
		if(lSelCount == 0)
		{
			WrappedMessageBox("Please select atleast single item/s", 			MAINWINDOW_TITLE,
					MB_OK | MB_ICONINFORMATION);
			CWin32TreeView::TreeView_SelectAll(m_hwndSndSeltdFileMan, false);
			return bReturn;
		}
		if(lSelCount > 1)
		{
			WrappedMessageBox("Selection of Destination folder should be only one", MAINWINDOW_TITLE,
				MB_OK | MB_ICONINFORMATION);
			CWin32TreeView::TreeView_SelectAll(m_hwndSndSeltdFileMan, false);
			return bReturn;
		}

		//lSelCount = TreeView_GetSelectedCount(m_hwndActiveFileManager);

		//if(lCount == 0L || lSelCount == 0L)
		//	return TRUE;

		// If Sel count is 1, check and see if the machine root or parent folder is
		//	 what is selected
		if(lSelCount == 1)
		{
			// get current item
			//lCurSel = SendMessage(m_hwndActiveFileManager, LB_GETCURSEL, 
			//			(WPARAM)0, 0L);

			CWin32TreeView::GetWin32SelectedItemText(m_hwndSndSeltdFileMan, tstrItem, _countof(tstrItem));

			//// get text for item
			//SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)lCurSel,
			//	(LPARAM)tstrItem);

			// assign for easier comparison
			strTemp = tstrItem;

			// Check and see if the parent folder is selected or if we are in the
			//	 machine root...
			if(strTemp == FOLDER_ROOT)
			{
				tstring msg = _T("You cannot ");
				msg += operation_strings[eNameString][iOperation];
				msg += _T("a drive. Please navigate to a directory\nand select a file or directory.");

				WrappedMessageBox(
					msg.c_str(),
					MAINWINDOW_TITLE,
					MB_OK | MB_ICONINFORMATION);

				// return success (no error occurred)
				return TRUE;
			}
			else if(strTemp == FOLDER_PARENT)
			{
				tstring msg = _T("You cannot perform a ");
				msg += operation_strings[eNameString][iOperation];
				msg += _T(" on the parent folder. Please\nselect another file or directory.");

				WrappedMessageBox(
					msg.c_str(),
					MAINWINDOW_TITLE,
					MB_OK | MB_ICONINFORMATION);

				// return success (no error occurred)
				return TRUE;
			}
		}

		HTREEITEM hTreeItem1 = CWin32TreeView::GetCurrentSelectedItem(hWnd1);
		HTREEITEM hTreeItem2 = CWin32TreeView::GetCurrentSelectedItem(hWnd2);			

		TCHAR szFullPath[MAX_PATH] = {0};
		TCHAR szItem[MAX_PATH] = {0};
		// Get base folders
		if(iCtrlID == IDC_TVFILEMANAGER1)
		{
			//Source
			if(!CWin32TreeView::GetSelectedItemParentPath(hWnd1, hTreeItem1, szFullPath))
			{
				return false;
			}

			CWin32TreeView::GetItemText(hWnd1, hTreeItem1, szItem, MAX_PATH);
			strSourceBase = szFullPath;
			strSourceBase += szItem;

			//Dest
			memset(szFullPath, 0, MAX_PATH);
			memset(szItem, 0, MAX_PATH);
			if(!CWin32TreeView::GetSelectedItemParentPath(hWnd2, hTreeItem2, szFullPath))
			{
				return false;
			}

			CWin32TreeView::GetItemText(hWnd2, hTreeItem2, szItem, MAX_PATH);
			strDestBase = szFullPath;
			strDestBase += szItem;
		}
		else
		{
			//Source
			if(!CWin32TreeView::GetSelectedItemParentPath(hWnd2, hTreeItem2, szFullPath))
			{
				return false;
			}

			CWin32TreeView::GetItemText(hWnd2, hTreeItem2, szItem, MAX_PATH);
			strSourceBase = szFullPath;
			strSourceBase += szItem;

			//Dest
			memset(szFullPath, 0, MAX_PATH);
			memset(szItem, 0, MAX_PATH);
			if(!CWin32TreeView::GetSelectedItemParentPath(hWnd1, hTreeItem1, szFullPath))
			{
				return false;
			}

			CWin32TreeView::GetItemText(hWnd1, hTreeItem1, szItem, MAX_PATH);
			strDestBase = szFullPath;
			strDestBase += szItem;
		}

		SetPercentage(0);
		{
			tstring msg = _T("WARNING: You are about to ");
			msg += operation_strings[eNameString][iOperation];
			msg += _T(" the selected files and/or folders.\nThis CANNOT be undone.\n\nDo you wish to continue?");

			// Display warning message
			if(WrappedMessageBox( 
				msg.c_str(),
				MAINWINDOW_TITLE, MB_YESNO | MB_ICONQUESTION) == IDNO)
				return TRUE;
		}

		//To check if Destination directory is a valid directory else return.
		if(FILE_ATTRIBUTE_DIRECTORY != GetFileAttributes(strDestBase.c_str()))
		{
			int lcv = _tcslen(strDestBase.c_str());
			if(strDestBase.c_str()[lcv - 1] != _T(':'))
			{
				// Not a drive or a folder means not a valid destination folder return.
				m_strLastError = _T("Not a valid destination folder\n");
				m_strLastError += strDestBase;
				m_strLastError += _T("\nThe copy failed.");

				// display this one...
				WrappedMessageBox( m_strLastError.c_str(),
					MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
				return FALSE;

			}
		}

		HWND hTreeCntrl = NULL;
		std::vector<HTREEITEM> vSelectedPaths;
		if(iCtrlID == IDC_TVFILEMANAGER2)//Get here reverse file manager handle
		{
			hTreeCntrl = hWnd2;
			hTreeItem2 = CWin32TreeView::GetCurrentSelectedItem(hWnd1);
		}
		else
		{
			hTreeCntrl = hWnd1;
			hTreeItem2 = CWin32TreeView::GetCurrentSelectedItem(hWnd2);
		}

		CWin32TreeView::TreeView_GetAllSelectedItems(hTreeCntrl, vSelectedPaths);

		int iSelectedCount = CWin32TreeView::GetSelectedCount(hTreeCntrl);
		SetPercentage(0);
		SetPercentageRange(iSelectedCount);

		std::vector<HTREEITEM>::const_iterator it;
		for( it = vSelectedPaths.begin(); it !=  vSelectedPaths.end(); it++)
		{
			HTREEITEM hTreeItem = *it;

			//Source
			if(!CWin32TreeView::GetSelectedItemParentPath(hWnd1, hTreeItem, szFullPath))
			{
				return false;
			}

			CWin32TreeView::GetItemText(hWnd1, hTreeItem, szItem, MAX_PATH);
			strSourceBase = szFullPath;
			strSourceBase += szItem;

			//Dest
			memset(szFullPath, 0, MAX_PATH);
			memset(szItem, 0, MAX_PATH);
			if(!CWin32TreeView::GetSelectedItemParentPath(hWnd2, hTreeItem2, szFullPath))
			{
				return false;
			}

			CWin32TreeView::GetItemText(hWnd2, hTreeItem2, szItem, MAX_PATH);
			strDestBase = szFullPath;
			strDestBase += szItem;

			//// get current item
			CWin32TreeView::GetItemText(hTreeCntrl, hTreeItem, tstrItem, _countof(tstrItem));

			// validate, continue
			if(lstrlen(tstrItem))
			{
				// Initialize source and dest...
				//	 Source
				strSource = strSourceBase;

				if(FILE_ATTRIBUTE_DIRECTORY == GetFileAttributes(strSource.c_str()))
				{
					getDirectoryNameFromItem(tstrItem, tstrItem);

					// Un-set this is a file flag
					bIsFile = FALSE;
				}
				else
				{
					// File
					getFilenameFromItem(tstrItem, tstrItem);

					// Set this is a file flag
					bIsFile = TRUE;
				}

				//	 If source is a folder, add wildcards
				if(!bIsFile)
				{
					strSource += _T("\\*.*");
				}
				//	 Dest
				strDest = strDestBase;
				if(strDest[strDest.length() - 1] != _T('\\'))
					strDest += _T("\\");
				strDest += tstrItem;

				// Source and Destination must be dual-terminated, so store in 
				//	 temp buffers
				//	 Source
				memset(tstrSrcBuffer, 0, sizeof(tstrSrcBuffer));
				_tcscpy(tstrSrcBuffer, strSource.c_str());
				//	 Destination
				memset(tstrDestBuffer, 0, sizeof(tstrDestBuffer));
				_tcscpy(tstrDestBuffer, strDest.c_str());

				// initialize struct
				memset(&shfopCopy, 0, sizeof(shfopCopy));
				shfopCopy.wFunc = iOperation;
				shfopCopy.pFrom = tstrSrcBuffer;
				shfopCopy.pTo = tstrDestBuffer;
				// Parth Software Solution bug fixing
				shfopCopy.fFlags = FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION |FOF_SIMPLEPROGRESS;

				// Verify that if this is a directory, the directoy is NOT
				//	 empty. If it is, then perform a create ONLY as this
				//	 can result in an error on some XP systems.
				if(!bIsFile)
				{
					HWND hwndTemp = NULL;

					// get handle to "temp" File Manager
					hwndTemp = GetDlgItem(m_hwndThis, IDC_FILELISTTEMP);

					// validate, continue
					if(hwndTemp != NULL)
					{
						// clear
						SendMessage(hwndTemp, LB_RESETCONTENT, (WPARAM)0, 0L);

						// get files
						SendMessage(hwndTemp, LB_DIR, (WPARAM)FILE_ATTRIBUTE_NORMAL
							| FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM
							| FILE_ATTRIBUTE_HIDDEN, (LPARAM)tstrSrcBuffer);

						// check count
						if(SendMessage(hwndTemp, LB_GETCOUNT, (WPARAM)0, 0L) == 0)
						{
							// create directory
							if(!CreateDirectoryStructure(tstrDestBuffer, TRUE))
							{
								// Provide general, more readable message per client request.
								m_strLastError = _T("An error occurred while copying the file system object: ");
								m_strLastError += tstrSrcBuffer;
								m_strLastError += _T(". The copy failed.");

								// display this one...
								WrappedMessageBox( m_strLastError.c_str(),
									MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

								// set fail val
								bReturn = FALSE;
							}

							// increment count
							lCurSel++;

							// let everything paint
							doEvents();

							// next, please
							hTreeItem = TreeView_GetNextItem(m_hwndSndSeltdFileMan, &hTreeItem, TVGN_NEXT);
							
						}
					}
				}

				int iReturn = SHFileOperation(&shfopCopy);
				// Attempt the copy
				if(iReturn > 0)
				{
					// Provide general, more readable message per client request.
					m_strLastError = _T("An error : ");
					m_strLastError += CheckErrorGivePromptMessage(iReturn);
					m_strLastError += _T(" occurred while ");
					m_strLastError += operation_strings[eProcessString][iOperation];
					m_strLastError += _T(" the file system object: ");
					m_strLastError += tstrSrcBuffer;
					m_strLastError += _T(". The ");
					m_strLastError += operation_strings[eNameString][iOperation];
					m_strLastError += _T(" failed.");

					// display this one...
					WrappedMessageBox( m_strLastError.c_str(),
						MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

					// set fail val
					bReturn = FALSE;
				}
				else
					lCurSel++;

				//// increment progressbar
				//SendMessage(progress_bar_handle(), PBM_SETPOS, (WPARAM)lCurSel, 0L);

				//char szBuffer[30] = {0};
				//sprintf(szBuffer, "%d%%", (int)lCurSel);
				//SetDlgItemText(m_hwndThis, IDC_STATIC_PROGRESS_TEXT, szBuffer);

				// let progressbar redraw...
				doEvents();
				SetPercentage();
			}
		}

		if(strSourceBase.length() > 0 && iOperation == FO_MOVE)
		{
			//remove directory that remain at source after moving.
			::RemoveDirectory(strSourceBase.c_str());
		}

		SetPercentage(100);//Set Percentage to 100

		HTREEITEM hTreeItem;
		//if(iCtrlID == IDC_TVFILEMANAGER1)
		//{
		//	Selected = hTreeItem = TreeView_GetParent(hWnd1, hTreeItem1);  		
		//	TreeView_SelectItem(hWnd1, hTreeItem);

		//	if(!getDirectoryListing_TV(g_csetApplication.lastFolderFileManager1(),
		//		hWnd1, m_pllstFileManager1))
		//		displayDirectoryListing_TV( hWnd1, m_pllstFileManager1 ); 

		//	if(!getDirectoryListing_TV(g_csetApplication.lastFolderFileManager2(),
		//		hWnd2, m_pllstFileManager2))
		//		displayDirectoryListing_TV( hWnd2, m_pllstFileManager2 ); 
		//}
		//else
		//{

			if(iCtrlID == IDC_TVFILEMANAGER1)
			{
				hTreeItem = TreeView_GetParent(hWnd1, hTreeItem1);  		
				if(hTreeItem)
				{
					TreeView_SelectItem(hWnd1, hTreeItem);
					executeCheckedItem_TV(IDC_TVFILEMANAGER2);
				}
			}
			else
			{
				hTreeItem = TreeView_GetParent(hWnd2, hTreeItem2);  		
				if(hTreeItem)
				{
					TreeView_SelectItem(hWnd2, hTreeItem);
					executeCheckedItem_TV(IDC_TVFILEMANAGER1);
				}

			}

			if(!getDirectoryListing_TV(g_csetApplication.lastFolderFileManager2(),
				hWnd2, m_pllstFileManager2))
				displayDirectoryListing_TV( hWnd2, m_pllstFileManager2 ); 

			if(!getDirectoryListing_TV(g_csetApplication.lastFolderFileManager1(),
				hWnd1, m_pllstFileManager1))
				displayDirectoryListing_TV( hWnd1, m_pllstFileManager1 ); 
		//}

		//SetDlgItemText(m_hwndThis, IDC_STATIC_PROGRESS_TEXT, "100%");

		//// Refresh destination folder
		////	 get handle for destination File Manager
		//if(iCtrlID == IDC_LSTFILEMANAGER1)
		//{
		//	hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER2);
		//	pllstTemp = m_pllstFileManager2;
		//}
		//else
		//{
		//	hwndTemp = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGER1);
		//	pllstTemp = m_pllstFileManager1;
		//}
		////   Clear existing selection
		//SendMessage(hwndTemp, LB_SETSEL, (WPARAM)FALSE, -1L);
		////	 get directory listing
		//getDirectoryListing(strDestBase.c_str(), hwndTemp, 
		//	pllstTemp);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While ");
		m_strLastError += operation_strings[eProcessString][iOperation];
		m_strLastError += _T(" files / directories, an unexpected error occurred.");

		// set fail value
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Copies files and directories between File Managers.
 *
 *
 */
BOOL CMainWindow::copyFilesAndDirectories()
{
  return operateFSOFromTo(FO_COPY);
}

/**
 * Deletes files and directories in the active File Manager.
 *
 *
 */
BOOL CMainWindow::deleteFilesAndDirectories()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
	{
		tstring strSourceBase = EMPTY_STRING,
			strSource = EMPTY_STRING;
		LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;
		SHFILEOPSTRUCT shfopDelete;
		//HWND hwndProgress = NULL;
		TCHAR tstrItem[MAX_PATH] = EMPTY_STRING,
			tstrSrcBuffer[MAX_PATH] = EMPTY_STRING,
			tstrDestBuffer[MAX_PATH] = EMPTY_STRING;
		int iCtrlID = 0;
		long lCount = 0L,
			lSelCount = 0L,
			lCurSel = 0L;
		BOOL bIsFile = FALSE;

		// validate active file manager
		if(m_hwndActiveFileManager == NULL)
		{
			// set last error
			m_strLastError = _T("No active File Manager.");

			// return fail
			return FALSE;
		}

		SetPercentage(0);

		// Display warning message
		if(WrappedMessageBox( 
			_T("WARNING: You are about to permanently delete the selected files and/or folders.\nThis CANNOT be undone.\n\nDo you wish to continue?"),
			MAINWINDOW_TITLE, MB_YESNO | MB_ICONQUESTION) == IDNO)
			return TRUE;

		// Make sure there is something to do... nothing to do DOES NOT indicate
		//	 an error condition
		//lCount = SendMessage(m_hwndActiveFileManager, LB_GETCOUNT, (WPARAM)0, 0L);

		//lSelCount = SendMessage(m_hwndActiveFileManager, LB_GETSELCOUNT, (WPARAM)0, 0L);

		//lSelCount = TreeView_GetSelectedCount(m_hwndActiveFileManager);
		lSelCount = GetSelectedItemsCount(m_hwndActiveFileManager);
		if(lSelCount == 0)
		{
			WrappedMessageBox("Please select atleast single item/s", 			MAINWINDOW_TITLE,
					MB_OK | MB_ICONINFORMATION);
		}


		if(/*lCount == 0L || */lSelCount == 0L)
			return TRUE;

		// Setup progressbar
		HWND hwndProgress = progress_bar_handle();
		if(hwndProgress)
		{
			// set range
			SendMessage(hwndProgress, PBM_SETRANGE32, (WPARAM)0, 
				(LPARAM)lSelCount);
			// set initial increment
			SendMessage(hwndProgress, PBM_SETSTEP, (WPARAM)1, 0L);
			// set initial value
			SendMessage(hwndProgress, PBM_SETPOS, (WPARAM)0, 0L);
		}

		// Attempt to get which File Manager is active
		iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);

		// Get base folders
		if(iCtrlID == IDC_TVFILEMANAGER1)
		{
			// File List
			pllstActive = m_pllstFileManager1;
		}
		else
		{
			// File List
			pllstActive = m_pllstFileManager2;
		}

		// Iterate through selected items and perform copy for each
		HTREEITEM hFirstCheckItem;
		hFirstCheckItem = CWin32TreeView::GetCurrentSelectedItem(m_hwndActiveFileManager);			

		std::vector<HTREEITEM> vSelectedPaths;
		CWin32TreeView::TreeView_GetAllSelectedItems(m_hwndActiveFileManager, vSelectedPaths);

		int iSelectedCount = CWin32TreeView::GetSelectedCount(m_hwndActiveFileManager);
		SetPercentageRange(iSelectedCount);

		std::vector<HTREEITEM>::const_iterator it;
		for( it = vSelectedPaths.begin(); it !=  vSelectedPaths.end(); it++)
		{		
			HTREEITEM hTreeItem = (HTREEITEM)*it; 
			TCHAR szFilePath[MAX_PATH] = {0};
			if(!CWin32TreeView::GetSelectedItemParentPath(m_hwndActiveFileManager, hTreeItem, szFilePath))
			{
				continue;
			}

			strSourceBase = szFilePath;

			CWin32TreeView::GetItemText(m_hwndActiveFileManager, hTreeItem, tstrItem, _countof(tstrItem));
			// validate, continue
			if(lstrlen(tstrItem))
			{
				// Initialize source and dest...
				//	 Source
				strSource = strSourceBase;
				if(strSource[strSource.length() - 1] != _T('\\'))
					strSource += _T("\\");
				strSource += tstrItem;

				// get item name
				if(FILE_ATTRIBUTE_DIRECTORY == GetFileAttributes(strSource.c_str()))
				{
					getDirectoryNameFromItem(tstrItem, tstrItem);

					// Un-set this is a file flag
					bIsFile = FALSE;
				}
				else
				{
					// File
					getFilenameFromItem(tstrItem, tstrItem);

					// Set this is a file flag
					bIsFile = TRUE;
				}

				//	 If source is a folder, add wildcards
				//if(!bIsFile)
					//strSource += _T("*");

				//// get item name
				//if(_tcsstr(tstrItem, SEARCHKEY_DIRECTORY))
				//{
				//	// Directory
				//}
				//else if(_tcsstr(tstrItem, SEARCHKEY_DRIVE) 
				//	|| _tcsstr(tstrItem, FOLDER_MACHINEROOT)
				//	|| _tcsstr(tstrItem, FOLDER_PARENT))
				//{
				//	// do nothing, next please
				//	continue;
				//}

				// Source and Destination MUST BE dual-terminated, so store in 
				//	 temp buffers
				//	 Source
				memset(tstrSrcBuffer, 0, sizeof(tstrSrcBuffer));
				_tcscpy(tstrSrcBuffer, (TCHAR *)strSource.data());
				//	 Destination
				memset(tstrDestBuffer, 0, sizeof(tstrDestBuffer));

				// initialize struct
				memset(&shfopDelete, 0, sizeof(shfopDelete));
				shfopDelete.wFunc = FO_DELETE;
				shfopDelete.pFrom = tstrSrcBuffer;
				shfopDelete.fFlags = FOF_NOCONFIRMATION;

				// Attempt the delete
				int iReturn = SHFileOperation(&shfopDelete);
				// Attempt the copy
				if(iReturn > 0)
				{
					m_strLastError = _T("An error : ");
					m_strLastError += CheckErrorGivePromptMessage(iReturn);
					m_strLastError += _T(" occurred while deleting the file system object:  ");
					m_strLastError += tstrSrcBuffer;
					m_strLastError += _T(". The deleting failed.");

					// display this one...
					WrappedMessageBox( (TCHAR *)m_strLastError.data(),
						MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

					// set fail val
					bReturn = FALSE;
				}
				else
				{
					// If this was a directory, its contents have been removed. However,
					//	 the directory itself still remains. Remove the directory.
					if(!bIsFile)
					{
						// reconstruct source less wildcard
						//strSource = strSourceBase;
						//strSource += tstrItem;

						// remove the directory
						if(!RemoveDirectory((TCHAR *)strSource.data()))
							bReturn = FALSE;
					}

					// increment current selection count
					lCurSel++;
				}

				// increment progressbar
				//if(hwndProgress)
				//SendMessage(hwndProgress, PBM_SETPOS, (WPARAM)lCurSel, 
				//	0L);
				//	char szBuffer[30] = {0};
				//	sprintf(szBuffer, "%d%%", (int)lCurSel);
				//SetDlgItemText(m_hwndThis, IDC_STATIC_PROGRESS_TEXT, szBuffer);
				SetPercentage();

				// allow progressbar to redraw
				doEvents();
			}
		}

		SetPercentage(100);
		HTREEITEM hTreeitem = TreeView_GetParent(m_hwndActiveFileManager, hFirstCheckItem);  		
		if(hTreeitem)
			TreeView_SelectItem(m_hwndActiveFileManager, hTreeitem);

		// If we made it here, check the return val. If everything went ok, then
		//	 refresh directory listing
		getDirectoryListing_TV((TCHAR *)strSourceBase.data(), m_hwndActiveFileManager, 
			pllstActive);

		//// Set parent folder as the selected item
		//SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)TRUE, (LPARAM)INDEX_PARENTFOLDER);
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While deleting files / directories, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}


BOOL CMainWindow::switchFileManagers()
{
		// Attempt to get which File Manager is active
		int iCtrlID = 0;
		iCtrlID = GetDlgCtrlID(pcmwndThis->m_hwndActiveFileManager);

    int nDlgItem = 0;

	if(iCtrlID == IDC_TVFILEMANAGER1)
		{
      nDlgItem = IDC_LSTFILEMANAGER2;
			pcmwndThis->m_pllstActiveFileManager =
				pcmwndThis->m_pllstFileManager2;
    }
    else if(iCtrlID == IDC_LSTFILEMANAGER2)
    {
      nDlgItem = IDC_TVFILEMANAGER1;
			pcmwndThis->m_pllstActiveFileManager =
				pcmwndThis->m_pllstFileManager1;
    }

		HWND hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, nDlgItem);
		if(hwndTemp)
		{
			// move input focus
			SetFocus(hwndTemp);

			// make the active File Manager
			pcmwndThis->m_hwndActiveFileManager = hwndTemp;
      return TRUE;
    }

    return FALSE;
}

/**
 * Renames files and directories in the active File Manager.
 *
 *
 */
BOOL CMainWindow::renameFilesAndDirectories()
{
	CRenameFileDirectoryDialog *pcrfddlgRename = NULL;
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		tstring strSourceBase = EMPTY_STRING,
					strSource = EMPTY_STRING,
					strDestBase = EMPTY_STRING,
					strDest = EMPTY_STRING;
		LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;
		SHFILEOPSTRUCT shfopRename;
		//HWND hwndProgress = NULL;
		TCHAR tstrItem[MAX_PATH] = EMPTY_STRING,
			  tstrSrcBuffer[MAX_PATH] = EMPTY_STRING,
			  tstrDestBuffer[MAX_PATH] = EMPTY_STRING;
		int iCtrlID = 0;
		long lCount = 0L,
			 lSelCount = 0L,
			 lCurSel = 0L;
		BOOL bIsFile = FALSE;

		// validate active file manager
		if(m_hwndActiveFileManager == NULL)
		{
			// set last error
			m_strLastError = _T("No active File Manager.");

			// return fail
			return FALSE;
		}

		SetPercentage(0);

		// Display warning message
		if(WrappedMessageBox( 
		   _T("WARNING: You are about to rename the selected files and/or folders.\nThis CANNOT be undone.\n\nDo you wish to continue?"),
		   MAINWINDOW_TITLE, MB_YESNO | MB_ICONQUESTION) == IDNO)
			return TRUE;

		// Make sure there is something to do... nothing to do DOES NOT indicate
		//	 an error condition
		lCount = SendMessage(m_hwndActiveFileManager, LB_GETCOUNT, (WPARAM)0, 0L);
		lSelCount = SendMessage(m_hwndActiveFileManager, LB_GETSELCOUNT, (WPARAM)0, 
						0L);
		if(lCount == 0L || lSelCount == 0L)
			return TRUE;

		// Setup progressbar
    HWND hwndProgress = progress_bar_handle();
		if(hwndProgress)
		{
			// set range
			//SendMessage(hwndProgress, PBM_SETRANGE32, (WPARAM)0, 
			//	(LPARAM)lSelCount);
			SetPercentage(0);
			SetPercentageRange(lSelCount);
			SetPercentage();
			// set initial increment
			//SendMessage(hwndProgress, PBM_SETSTEP, (WPARAM)1, 0L);

			//// set initial value
			//SendMessage(hwndProgress, PBM_SETPOS, (WPARAM)0, 0L);
		}

		// Attempt to get which File Manager is active
		iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);

		// Get base folders
		if(iCtrlID == IDC_TVFILEMANAGER1)
		{
			// Source
			strSourceBase = g_csetApplication.lastFolderFileManager1();

			// File List
			pllstActive = m_pllstFileManager1;
		}
		else
		{
			// Source
			strSourceBase = g_csetApplication.lastFolderFileManager2();

			// File List
			pllstActive = m_pllstFileManager2;
		}

		// Dest
		strDestBase = strSourceBase;

		// Iterate through selected items and perform copy for each
		for(long lcv = 0L; lcv < lCount; lcv++)
		{
			// get sel state for current item
			if(SendMessage(m_hwndActiveFileManager, LB_GETSEL, (WPARAM)lcv, 0L) > 0)
			{
				// get current item
				SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)lcv,
					(LPARAM)tstrItem);

				// validate, continue
				if(lstrlen(tstrItem))
				{
					int iReturn = 0;

					// get item name
					if(_tcsstr(tstrItem, SEARCHKEY_DIRECTORY))
					{
						// Directory
						getDirectoryNameFromItem(tstrItem, tstrItem);

						// Un-set this is a file flag
						bIsFile = FALSE;
					}
					else if(_tcsstr(tstrItem, SEARCHKEY_DRIVE) 
							|| _tcsstr(tstrItem, FOLDER_MACHINEROOT)
							|| _tcsstr(tstrItem, FOLDER_PARENT))
					{
						// do nothing, next please
						continue;
					}
					else
					{
						// File
						getFilenameFromItem(tstrItem, tstrItem);

						// Set this is a file flag
						bIsFile = TRUE;
					}
					
					// create rename dialog if it isn't already created
					if(pcrfddlgRename == NULL)
						pcrfddlgRename = new CRenameFileDirectoryDialog(
												m_hinstApplication,
												tstrItem);

					// display dialog
					iReturn = pcrfddlgRename->show();
					
					if(iReturn == IDOK)
					{
						// Initialize source and dest...
						//	 Source
						strSource = strSourceBase;
						if(strSource[strSource.length() - 1] != _T('\\'))
							strSource += _T("\\");
						strSource += tstrItem;
						//	 Dest
						strDest = strDestBase;
						if(strDest[strDest.length() - 1] != _T('\\'))
							strDest += _T("\\");
						strDest += pcrfddlgRename->getNewObjectName();

						// Source and Destination MUST BE dual-terminated, so store in 
						//	 temp buffers
						//	 Source
						memset(tstrSrcBuffer, 0, sizeof(tstrSrcBuffer));
						_tcscpy(tstrSrcBuffer, (TCHAR *)strSource.data());
						//	 Destination
						memset(tstrDestBuffer, 0, sizeof(tstrDestBuffer));
						_tcscpy(tstrDestBuffer, (TCHAR *)strDest.data());

						// initialize struct
						memset(&shfopRename, 0, sizeof(shfopRename));
						shfopRename.wFunc = FO_RENAME;
						shfopRename.pFrom = tstrSrcBuffer;
						shfopRename.pTo = tstrDestBuffer;
						shfopRename.fFlags = FOF_NOCONFIRMATION;

						// Attempt the rename
						int iReturn = SHFileOperation(&shfopRename);
						// Attempt the copy
						if(iReturn > 0)
						{
							m_strLastError = _T("An error : ");
							m_strLastError += CheckErrorGivePromptMessage(iReturn);
							m_strLastError += _T(" occurred while renaming the file system object:  ");
							m_strLastError += tstrSrcBuffer;
							m_strLastError += _T(". The rename failed.");

							// display this one...
							WrappedMessageBox( (TCHAR *)m_strLastError.data(),
								MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

							// set fail val
							bReturn = FALSE;
						}
						else
						{
							// If this was a directory, its contents have been removed. However,
							//	 the directory itself still remains. Remove the directory.
							if(!bIsFile)
							{
								// reconstruct source less wildcard
								strSource = strSourceBase;
								strSource += tstrItem;

								// remove the directory
								if(!RemoveDirectory((TCHAR *)strSource.data()))
									bReturn = FALSE;
							}
							
							// increment current selection count
							lCurSel++;
						}

						//// increment progressbar
						//if(hwndProgress)
						//	SendMessage(hwndProgress, PBM_SETPOS, (WPARAM)lCurSel, 
						//		0L);

						// allow progressbar to redraw
						doEvents();
					}
				}
			}
		}

		// If we made it here, check the return val. If everything went ok, then
		//	 refresh directory listing
		getDirectoryListing((TCHAR *)strSourceBase.data(), m_hwndActiveFileManager,
			pllstActive);
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While renaming files / directories, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// garbage collect
	if(pcrfddlgRename)
	{
		delete pcrfddlgRename;
		pcrfddlgRename = NULL;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Initializes the DWG rendering engine.
 *
 * @return TRUE if the DWG rendering engine is intiailized and no errors
 * occur, otherwise FALSE.
 */
BOOL CMainWindow::initializeDWGRenderEngine()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		HWND hwndTemp = NULL;

		// make sure the CAD importer library exists or can be extracted
		if(!extractCADImporterLibrary())
		{
			// display error from load/extract attempt
			WrappedMessageBox( (TCHAR *)m_strLastError.c_str(),
				MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

			// return fail
			return FALSE;
		}

		// Attempt to create DWG render engine object
		m_cdwgengThis = new CDWGRenderEngine();
		if(m_cdwgengThis == NULL)
		{
			// set last error
			m_strLastError = _T("Could not create the DWG rendering engine.");

			// return fail
			return FALSE;
		}
		if(lstrlen(m_cdwgengThis->getLastError()))
		{
			// set last error
			m_strLastError = m_cdwgengThis->getLastError();

			// return fail
			return FALSE;
		}

		// validate *this* object's window handle
		if(m_hwndThis == NULL)
		{
			// set last error
			m_strLastError = _T("The main window's handle is invalid.");

			// return fail val
			return FALSE;
		}

		// attach input / output to picture box
		hwndTemp = GetDlgItem(m_hwndThis, IDC_RICHEDIT_VIEW);
		if(hwndTemp == NULL)
		{
			// set last error
			m_strLastError = _T("Could not retrieve the window handle for the viewer.");

			// return fail val
			return FALSE;
		}

		// set output to picture box
		m_cdwgengThis->setOutputControl(hwndTemp);

		// attach progress control to app's progressbar
		hwndTemp = GetDlgItem(m_hwndThis, IDC_PRGBRMAIN);
		if(hwndTemp == NULL)
		{
			// set last error
			m_strLastError = _T("Could not retrieve the window handle for the progressbar.");

			// return fail val
			return FALSE;
		}

		// set progressbar to internal
		m_cdwgengThis->setProgressbarControl(hwndTemp);
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While initializing the DWG rendering engine, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Initializes the captured command prompt.
 *
 * @return TRUE if the captured command prompt is intiailized and no errors
 * occur, otherwise FALSE.
 */
BOOL CMainWindow::initializeCommandPrompt()
{
    BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		// Attempt to create command prompt object
		m_ccapcmdThis = new CCapturedCommandPrompt();
		if(m_ccapcmdThis == NULL)
		{
			// set last error
			m_strLastError = _T("Could not create command prompt object.");

			// return fail
			return FALSE;
		}
		if(lstrlen(m_ccapcmdThis->getLastError()))
		{
			// set last error
			m_strLastError = m_ccapcmdThis->getLastError();

			// return fail
			return FALSE;
		}

		// attach input / output to textbox
    }
    catch(...)
    {
        // set last error
        m_strLastError = _T("While initializing the command prompt, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

    // return success / fail val
    return bReturn;
}

/**
 * Toggles the selected state of the currently selected file object in the
 * active File Manager.
 *
 * @return 
 */
BOOL CMainWindow::toggleSelectedFileObject()
{
	BOOL bReturn = TRUE;

	try
	{
		int iIndex = -1;
		BOOL bSelected = FALSE;

		// validate active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return FALSE;
		
		// get currently selected item
		iIndex = SendMessage(m_hwndActiveFileManager, LB_GETCARETINDEX, (WPARAM)0, 0L);
		if(iIndex != LB_ERR)
		{
			// get current selection state
			bSelected = SendMessage(m_hwndActiveFileManager, LB_GETSEL, (WPARAM)iIndex,
							0L);

			// Toggle state
			bSelected = !bSelected;

			// Set new state
			SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)bSelected,
				(LPARAM)iIndex);
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While toggling the state of the active file object, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

///**
// * Draws the command buttons within the "command button bar."
// *
// * @return 
// */
//BOOL CMainWindow::drawCommandButtons(HWND hwndBar, HDC hdcBar, RECT rctBar)
//{
//	HPEN hpenButton = NULL,
//		 hpenPrevious = NULL;
//	HBRUSH hbrBackground = NULL,
//		   hbrPrevious = NULL;
//	HFONT hfontPrevious = NULL;
//	HDC hdcBarLocal = NULL,
//		hdcButtonImage = NULL;
//	BOOL bReturn = TRUE,
//		 bCreatedDC = FALSE;
//
//	try
//	{
//		RECT rctButton;
//		TCHAR tstrCaption[10] = EMPTY_STRING;
//		float fBarWidth = 0.0f,
//			  fBarHeight = 0.0f,
//			  fCalcdValue = 0.0f;
//		int iButtonWidth = 0;
//		
//		// validate handle and DC
//		if(hwndBar == NULL)
//			return FALSE;
//		if(hdcBar == NULL)
//		{
//			// get HDC
//			hdcBarLocal = GetDC(hwndBar);
//
//			// set flag indicating this method created it (so it can be released)
//			bCreatedDC = TRUE;
//		}
//		else
//			hdcBarLocal = hdcBar;
//
//		// Get width and height for calculations
//		fBarWidth = (float)(rctBar.right - rctBar.left);
//		fBarHeight = (float)(rctBar.bottom - rctBar.top);
//
//		// Calculate width of one command button
//		//	 Store actual real number size of button
//		fCalcdValue = (fBarWidth - ((float)LAYOUT_COUNT_COMMANDBUTTONS - 1.0f)) / (float)LAYOUT_COUNT_COMMANDBUTTONS;
//		//	 Round "button width"
//		fCalcdValue = floor(fCalcdValue);
//		iButtonWidth = (long)fCalcdValue;
//		//	 Calculate "bar width"
//		fCalcdValue *= LAYOUT_COUNT_COMMANDBUTTONS;
//		//	 Find difference between real and integer values...
//		//	   Use this remainder.
//		fCalcdValue -= fBarWidth;
//
//		// initialize button rect
//		memcpy(&rctButton, &rctBar, sizeof(rctBar));
//		rctButton.left = 1;
//		rctButton.right = rctButton.left + iButtonWidth;
//		rctButton.top = 1;
//		rctButton.bottom--;
//
//		// create pen
//		hpenButton = (HPEN)GetStockObject(WHITE_PEN);
//		if(hpenButton)
//			hpenPrevious = (HPEN)SelectObject(hdcBar, hpenButton);
//
//		// create brush
//		hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
//		if(hbrBackground)
//			hbrPrevious = (HBRUSH)SelectObject(hdcBar, hbrBackground);
//
//		// Set text back and fore colors
//		SetTextColor(hdcBarLocal, RGB(255, 255, 255));
//		SetBkColor(hdcBarLocal, RGB(0, 0, 0));
//
//		// Set font
//		hfontPrevious = (HFONT)SelectObject(hdcBarLocal, m_hfontControls);
//
//		// Create command button RECT array if it doesn't exist already
//		if(m_arrctCommandButtons == NULL)
//		{
//			// create RECT array
//			m_arrctCommandButtons = new RECT*[(UINT)LAYOUT_COUNT_COMMANDBUTTONS];
//
//			// initialize
//			if(m_arrctCommandButtons)
//			{
//				for(int i = 0; i < LAYOUT_COUNT_COMMANDBUTTONS; i++)
//				{
//					// initialize element to new RECT
//					m_arrctCommandButtons[i] = new RECT();
//
//					// clear new RECT
//					memset(m_arrctCommandButtons[i], 0, sizeof(RECT));
//				}
//			}
//
//			// initialize HBITMAP array
//			for(int i = 0; i < LAYOUT_COUNT_BUTTONSALLSTATES; i++)
//			{
//				// attempt to load
//				m_arbmpCommandButtons[i] = 
//					LoadBitmap(m_hinstApplication, 
//								MAKEINTRESOURCE(i + IDB_BITMAP1));
//			}
//		}
//
//		// Attempt to create bitmap DC
//		hdcButtonImage = CreateCompatibleDC(hdcBarLocal);
//
//		// Draw buttons
//		for(int iButton = 0; iButton < (int)LAYOUT_COUNT_COMMANDBUTTONS; iButton++)
//		{
//			//// draw current button
//			//RoundRect(hdcBarLocal, rctButton.left, rctButton.top, rctButton.right, 
//			//	rctButton.bottom, LAYOUT_RADIUS_COMMANDBUTTONS, 
//			//	LAYOUT_RADIUS_COMMANDBUTTONS);
//
//			//// Create caption
//			//_stprintf(tstrCaption, "F%d", iButton + 1);
//
//			//// output caption
//			//DrawText(hdcBarLocal, tstrCaption, lstrlen(tstrCaption),
//			//	&rctButton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//
//			// Get current button into compatible DC
//			SelectObject(hdcButtonImage, m_arbmpCommandButtons[iButton * 2]);
//
//			// Draw current button
//			StretchBlt(hdcBarLocal, rctButton.left, rctButton.top,
//				rctButton.right - rctButton.left, 
//				rctButton.bottom - rctButton.top,
//				hdcButtonImage, 20, 5, 84, 20, SRCCOPY);
//
//			// Store button's RECT
//			CopyRect(m_arrctCommandButtons[iButton], &rctButton);
//
//			// increment button rect
//			rctButton.left = rctButton.right + 1;
//			rctButton.right = rctButton.left + iButtonWidth;
//		}
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		bReturn = FALSE;
//	}
//
//	// clear created GDI objects
//	if(hpenButton)
//	{
//		// restore original pen
//		if(hpenPrevious)
//			SelectObject(hdcBarLocal, hpenPrevious);
//		
//		// destroy pen
//		DeleteObject(hpenButton);
//	}
//	if(hbrBackground)
//	{
//		// restore original pen
//		if(hbrPrevious)
//			SelectObject(hdcBarLocal, hbrPrevious);
//		
//		// destroy pen
//		DeleteObject(hbrBackground);
//	}
//	if(hfontPrevious)
//	{
//		// restore original font
//		SelectObject(hdcBarLocal, hfontPrevious);
//
//		// Controls font object is owned by this object, DO NOT destroy
//	}
//
//	// If the DC had to be created, release it
//	if(bCreatedDC)
//	{
//		// release DC
//		ReleaseDC(hwndBar, hdcBarLocal);
//		
//		// de-ref
//		hdcBarLocal = NULL;
//	}
//	
//	// Destroy created compatible DC
//	if(hdcButtonImage)
//	{
//		// release DC
//		DeleteDC(hdcButtonImage);
//
//		// de-ref
//		hdcButtonImage = NULL;
//	}
//
//	// reset last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// return success / fail val
//	return bReturn;
//}

/**
 * Sorts the active file list by name either in ascending or descending
 * order depending on what the active sort is.
 */
BOOL CMainWindow::sortFileListByName(long lStart, long lLength)
{
	BOOL bReturn = TRUE;

	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		long lcv,
			 lPass = lLength - 1;

		// validate active file list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate sort type
		//if(m_aseActiveSort != aseCustom && m_aseActiveSort != aseNameAsc && m_aseActiveSort != aseNameDesc)
		//	return FALSE;

		// validate current pass
		if(lPass < 0)
			return TRUE;
		
		if(m_aseActiveSort == aseNameAsc || m_aseActiveSort == aseCustom ||
			m_aseActiveSort == aseDirAsc)
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				if(lstrcmp(pwfdItemOne->cFileName, pwfdItemTwo->cFileName) > 0)
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);

				//pcmwndThis->SetPercentage();
			}
		}
		else
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				if(lstrcmp(pwfdItemOne->cFileName, pwfdItemTwo->cFileName) < 0)
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);

				//pcmwndThis->SetPercentage();
			}
		}
		// call until stop condition is met
		bReturn = sortFileListByName(lStart, lPass);

	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

BOOL CMainWindow::sortFileListByCustom(long lStart, long lLength)
{
	BOOL bReturn = TRUE ,bReturn1 = TRUE;

	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
			*pwfdItemTwo = NULL;
		long lcv,
			lPass = lLength - 1;

		// validate active file list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate sort type
		if(m_aseActiveSort != aseCustom)
			return FALSE;

		// validate current pass
		if(lPass < 0)
			return TRUE;

		sortFileListByDirOrFileFirst(0, m_pllstActiveFileManager->getLength());

		for(lcv = lStart; lcv < lPass; lcv++)
		{
			// get items
			pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
			pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;

			if(pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
				&&
				!(pwfdItemTwo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				break;		
		}
		long lPivot = lcv + 1;
		// call until stop condition is met
		bReturn = sortFileListByName(lStart, lPivot);
		bReturn1 = sortFileListByExtension(lPivot, lPass+1);
		if(bReturn && bReturn1)
			bReturn = TRUE;
		else
			bReturn = FALSE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}


/**
 * Sorts the active file list by type either in ascending or descending
 * order depending on what the active sort is.
 */
BOOL CMainWindow::sortFileListByType(long lStart, long lLength)
{
	BOOL bReturn = TRUE;

	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		long lcv,
			 lPass = lLength - 1;

		// validate active file list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate sort type
		if(m_aseActiveSort != aseCustom && m_aseActiveSort != aseTypeAsc && m_aseActiveSort != aseTypeDesc)
			return FALSE;

		// validate current pass
		if(lPass < 0)
			return TRUE;
		
		if(m_aseActiveSort == aseTypeAsc || m_aseActiveSort == aseCustom)
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				if(pwfdItemTwo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
					&&
					!(pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);
				
				//pcmwndThis->SetPercentage();

			}
		}
		else
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				if(pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
					&&
					!(pwfdItemTwo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);

				//pcmwndThis->SetPercentage();
			}
		}

		// call until stop condition is met
		bReturn = sortFileListByType(lStart, lPass);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Sorts the active file list by size either in ascending or descending
 * order depending on what the active sort is.
 */
BOOL CMainWindow::sortFileListBySize(long lStart, long lLength)
{
	BOOL bReturn = TRUE;

	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		long lcv,
			 lPass = lLength - 1;

		// validate active file list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate sort type
		if(m_aseActiveSort != aseCustom && m_aseActiveSort != aseSizeAsc && m_aseActiveSort != aseSizeDesc)
			return FALSE;

		// validate current pass
		if(lPass < 0)
			return TRUE;
		
		if(m_aseActiveSort == aseSizeAsc || m_aseActiveSort == aseCustom)
		{ 
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				if(pwfdItemOne->nFileSizeLow > pwfdItemTwo->nFileSizeLow)
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);

				//pcmwndThis->SetPercentage();

			}
		}
		else
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				if(pwfdItemTwo->nFileSizeLow > pwfdItemOne->nFileSizeLow)
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);
					//pcmwndThis->SetPercentage();
			}
		}

		// call until stop condition is met
		bReturn = sortFileListBySize(lStart, lPass);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Sorts the active file list by date either in ascending or descending
 * order depending on what the active sort is.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CMainWindow::sortFileListByDate(long lStart, long lLength)
{
	BOOL bReturn = TRUE;

	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		long lcv,
			 lPass = lLength - 1;

		// validate active file list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate sort type
		if(m_aseActiveSort != aseDateAsc && m_aseActiveSort != aseDateDesc)
			return FALSE;

		// validate current pass
		if(lPass < 0)
			return TRUE;
		
		if(m_aseActiveSort == aseDateAsc)
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				if(CompareFileTime(&pwfdItemOne->ftLastAccessTime,
					&pwfdItemTwo->ftLastAccessTime) > 0)
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);
					//pcmwndThis->SetPercentage();
			}
		}
		else
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				if(CompareFileTime(&pwfdItemOne->ftLastAccessTime,
					&pwfdItemTwo->ftLastAccessTime) < 0)
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);
					//pcmwndThis->SetPercentage();
			}
		}

		// call until stop condition is met
		bReturn = sortFileListByDate(lStart, lPass);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}


/**
 * Sorts the active file list by extension either in ascending or 
 * descending order depending on what the active sort is.
 *
 * @note this method uses a bubble sort
 *
 * @param lStart index where sorting should begin
 *
 * @param lLength length of data to be sorted
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CMainWindow::sortFileListByExtension(long lStart, long lLength)
{
	BOOL bReturn = TRUE;

	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		tstring strTemp = EMPTY_STRING;
		TCHAR *ptcItemOneExt = NULL,
			  *ptcItemTwoExt = NULL;
		long lcv,
			 lPass = lLength - 1,
			 lExtIndex = 0;

		// validate active file list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate sort type
		if( m_aseActiveSort != aseCustom && m_aseActiveSort != aseExtensionAsc && m_aseActiveSort != aseExtensionDesc)
			return FALSE;

		// validate current pass
		if(lPass < 0)
			return TRUE;
		
		if(m_aseActiveSort == aseExtensionAsc || m_aseActiveSort == aseCustom)
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;

				// get extensions

				// Item 1
				strTemp = pwfdItemOne->cFileName;
				lExtIndex = strTemp.find_last_of(_T("."));
				if(lExtIndex != -1)
					ptcItemOneExt = (TCHAR *)&pwfdItemOne->cFileName[lExtIndex];
				else
					ptcItemOneExt = (TCHAR *)&pwfdItemOne->cFileName[0];
					
				// Item 2
				strTemp = pwfdItemTwo->cFileName;
				lExtIndex = strTemp.find_last_of(_T("."));
				if(lExtIndex != -1)
					ptcItemTwoExt = (TCHAR *)&pwfdItemTwo->cFileName[lExtIndex];
				else
					ptcItemTwoExt = (TCHAR *)&pwfdItemTwo->cFileName[0];

				// compare extensions
				if(lstrcmp(ptcItemOneExt, ptcItemTwoExt) > 0)
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);

					//pcmwndThis->SetPercentage();

			}
		}
		else
		{
			for(lcv = lStart; lcv < lPass; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
					
				// get extensions

				// Item 1
				strTemp = pwfdItemOne->cFileName;
				lExtIndex = strTemp.find_last_of(_T("."));
				if(lExtIndex != -1)
					ptcItemOneExt = (TCHAR *)&pwfdItemOne->cFileName[lExtIndex];
				else
					ptcItemOneExt = (TCHAR *)&pwfdItemOne->cFileName[0];
					
				// Item 2
				strTemp = pwfdItemTwo->cFileName;
				lExtIndex = strTemp.find_last_of(_T("."));
				if(lExtIndex != -1)
					ptcItemTwoExt = (TCHAR *)&pwfdItemTwo->cFileName[lExtIndex];
				else
					ptcItemTwoExt = (TCHAR *)&pwfdItemTwo->cFileName[0];

				// compare extensions
				if(lstrcmp(ptcItemOneExt, ptcItemTwoExt) < 0)
					// perform swap
					m_pllstActiveFileManager->swap(lcv, lcv + 1);
				
				//pcmwndThis->SetPercentage();
			}
		}

		// call until stop condition is met
		bReturn = sortFileListByExtension(lStart, lPass);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;	
}


// Parth Software Solution
// sort func As per dir Or files
BOOL CMainWindow::SortFilesInDirs(int lStart,int lPass)
{
	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		for(int i = lStart; i < lPass; i++)
			{
				for(int lcv = lStart; lcv < lPass - i; lcv++)
				{
					// get items
					pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
					pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;

					// check whether items are of type dir
					if(!(pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						&&
						!(pwfdItemTwo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{
					// first swap the directories as per theirnames
						if(lstrcmp(pwfdItemTwo->cFileName, pwfdItemOne->cFileName) > 0)
						{
							// perform swap
							m_pllstActiveFileManager->swap(lcv, lcv + 1);
						}
					}
				}

			}
		return true;



	}
	catch(...)
	{
		return false;
	}
}
BOOL CMainWindow::SortDirsinDirs(int lStart,int NumOfDirs)
{
	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;

		for(int i = lStart; i < NumOfDirs; i++)
			{
				for(int lcv = lStart; lcv < NumOfDirs - i; lcv++)
				{
					if(m_pllstActiveFileManager->getEntry(lcv + 1))// Check if item is present or not :: Handled for crash
					{
						// get items
						pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
						pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;

						// check whether items are of type dir
						if((pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
							&&
							(pwfdItemTwo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
						{
							// first swap the directories as per theirnames
							if(lstrcmp(pwfdItemOne->cFileName, pwfdItemTwo->cFileName) > 0)
							{
								// perform swap
								m_pllstActiveFileManager->swap(lcv, lcv + 1);
							}
						}
					}
				}

			}
		return true;
	}
	catch(...)
	{
		return false;
	}
}

TCHAR * CMainWindow::GetExtension(WIN32_FIND_DATA *pwfdItemOne)
{
	TCHAR *ptcItemExt = NULL;
	long lExtIndex = 0;
	tstring strTemp = EMPTY_STRING;
	
	strTemp = pwfdItemOne->cFileName;
	lExtIndex = strTemp.find_last_of(_T("."));
	if(lExtIndex != -1)
		ptcItemExt = (TCHAR *)&pwfdItemOne->cFileName[lExtIndex];
	else
		ptcItemExt = (TCHAR *)&pwfdItemOne->cFileName[0];
				
	return ptcItemExt;
}

BOOL CMainWindow::SortUsingCriteria(ACTIVESORTENUM ActiveSortType,WIN32_FIND_DATA *pwfdItemOne ,WIN32_FIND_DATA *pwfdItemTwo,long lcv)
{
	BOOL bReturn = TRUE;
	try
	{
		switch(ActiveSortType)
		{
			case aseNone:
							sortFileListByCustom(0,m_pllstActiveFileManager->getLength());
							break;
			case aseNameAsc:
							if(lstrcmp(pwfdItemOne->cFileName, pwfdItemTwo->cFileName) > 0)
										m_pllstActiveFileManager->swap(lcv, lcv + 1);
							break;
			case aseNameDesc:
							if(lstrcmp(pwfdItemOne->cFileName, pwfdItemTwo->cFileName) < 0)
										m_pllstActiveFileManager->swap(lcv, lcv + 1);
							break;
			case aseExtensionAsc:
							if(lstrcmp(GetExtension(pwfdItemOne), GetExtension(pwfdItemTwo)) >0)
										m_pllstActiveFileManager->swap(lcv, lcv + 1);
							break;
			case aseExtensionDesc:
							if(lstrcmp(GetExtension(pwfdItemOne), GetExtension(pwfdItemTwo)) <0)
										m_pllstActiveFileManager->swap(lcv, lcv + 1);
							break;
			case aseSizeAsc:
							if(pwfdItemOne->nFileSizeLow > pwfdItemTwo->nFileSizeLow )
										m_pllstActiveFileManager->swap(lcv, lcv + 1);
							break;
			case aseSizeDesc:
							if(pwfdItemOne->nFileSizeLow < pwfdItemTwo->nFileSizeLow )
										m_pllstActiveFileManager->swap(lcv, lcv + 1);
							break;
			case aseDateAsc:
							if(CompareFileTime(&pwfdItemOne->ftLastAccessTime,&pwfdItemTwo->ftLastAccessTime) < 0)
								m_pllstActiveFileManager->swap(lcv, lcv + 1);
							break;
			case aseDateDesc:
							if(CompareFileTime(&pwfdItemOne->ftLastAccessTime,&pwfdItemTwo->ftLastAccessTime) > 0)
								m_pllstActiveFileManager->swap(lcv, lcv + 1);
							break;
		}
	}
	catch(...)
	{
		m_strLastError = _T("");
		bReturn = FALSE;
	}
	return bReturn;
}

void CMainWindow::ArrangeFoldersAndFiles()
{
	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		long lcv, lStart =0, 
			 lPass = m_pllstActiveFileManager->getLength() - 1,
			 lExtIndex = 0;

		// validate current pass
		if(lPass < 0)
			return;
	
		for(int i = lStart; i < lPass; i++)
		{
			for(lcv = lStart; lcv < lPass - i; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;

				// check whether items are of type dir
				BOOL bdir = pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
				if(bdir)	
				{
					m_pllstActiveFileManager->swap(lcv, lcv + 1);						
				}	
			}
		}
		for(int i = lStart; i < lPass; i++)
		{
			for(lcv = lStart; lcv < lPass - i; lcv++)
			{
				// get items
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;

				// check whether items are of type files
				BOOL bdir = pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
				if(!bdir)	
				{
					m_pllstActiveFileManager->swap(lcv, lcv + 1);						
				}	
			}
		}
	}
	catch(...)
	{
		m_strLastError = _T("");
	}
}

BOOL CMainWindow::SortUsingCriteria(long lStart, long lLength, ACTIVESORTENUM ActiveSortType)
{
	BOOL bReturn = TRUE;

	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		long lcv,
			 lPass = lLength - 1,
			 lExtIndex = 0;

		// validate current pass
		if(lPass < 0)
			return TRUE;

		ArrangeFoldersAndFiles();
		
		// Sort Dir 
		for(int i = lStart; i < lPass; i++)
		{
			for(lcv = lStart; lcv < lPass - i; lcv++)
			{
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
				
				if( (pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					(pwfdItemTwo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
					bReturn = SortUsingCriteria(ActiveSortType,pwfdItemOne,pwfdItemTwo,lcv);
				}

			}
		}

		// Sort Files 
		for(int i = lStart; i < lPass; i++)
		{
			for(lcv = lStart; lcv < lPass - i; lcv++)
			{
				pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
				pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;
				
				if( !(pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					!(pwfdItemTwo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
					bReturn = SortUsingCriteria(ActiveSortType,pwfdItemOne,pwfdItemTwo,lcv);
				}

			}
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;	
}


BOOL CMainWindow::sortFileListByDirOrFileFirst(long lStart, long lLength)
{
	BOOL bReturn = TRUE;

	try
	{
		WIN32_FIND_DATA *pwfdItemOne = NULL,
						*pwfdItemTwo = NULL;
		tstring strTemp = EMPTY_STRING;
		TCHAR *ptcItemOneExt = NULL,
			  *ptcItemTwoExt = NULL;
		long lcv,
			 lPass = lLength - 1,
			 lExtIndex = 0;

		// validate active file list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate sort type
		if(m_aseActiveSort != aseDirAsc && m_aseActiveSort != aseDirDesc && m_aseActiveSort != aseCustom )
			return FALSE;

		// validate current pass
		if(lPass < 0)
			return TRUE;
		
		if(m_aseActiveSort == aseDirAsc || m_aseActiveSort == aseCustom)
		{
			// 1. first get the no of dirs aailable in the parent dir
			int NumOfDirs = 0;
			for(int i = lStart; i < lLength; i++)
			{
				pwfdItemOne = m_pllstActiveFileManager->getEntry(i)->pwfdFileInfo;
				if(pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					NumOfDirs++;
			}
			// 2. move all dirs up
			if(NumOfDirs)
			{
				for(int i = lStart; i < lPass; i++)
				{
					for(lcv = lStart; lcv < lPass - i; lcv++)
					{
						// get items
						pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
						pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;

						// check whether items are of type dir
						BOOL bdir = pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
						if(!bdir)	
						{
							// perform swap
							m_pllstActiveFileManager->swap(lcv, lcv + 1);						
						}	
					}

				}
				// 3. sort dirs
				sortFileListByName(lStart,NumOfDirs);
				//SortDirsinDirs(lStart,NumOfDirs);
			}
			// Now sort the files
			//sortFileListByName(NumOfDirs,lPass+1);
			//SortFilesInDirs(NumOfDirs,lPass);
		}
		else
		{
			// 1. first get the no of Files aailable in the parent dir
			int NumOfFiles = 0;
			for(int i = lStart; i < lLength; i++)
			{
				pwfdItemOne = m_pllstActiveFileManager->getEntry(i)->pwfdFileInfo;
				if(!(pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					NumOfFiles++;
			}

			if(NumOfFiles)
			{
				// 2. move all files up
				for(int i = lStart; i < lPass; i++)
				{
					for(lcv = lStart; lcv < lPass - i; lcv++)
					{
						// get items
						pwfdItemOne = m_pllstActiveFileManager->getEntry(lcv)->pwfdFileInfo;
						pwfdItemTwo = m_pllstActiveFileManager->getEntry(lcv + 1)->pwfdFileInfo;

						// check whether items are of type dir
						BOOL bdir = pwfdItemOne->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
						if(bdir)	
						{
							// perform swap
							m_pllstActiveFileManager->swap(lcv, lcv + 1);						
						}	
						//pcmwndThis->SetPercentage();
					}

				}

				// sort the files first
				sortFileListByName(lStart,NumOfFiles);
				//SortFilesInDirs(lStart,NumOfFiles);
			}
			//Now sort dirs
			//sortFileListByName(NumOfFiles+1,lPass);
			//SortDirsinDirs(NumOfFiles+1,lPass);
		}

		// call until stop condition is met
		//bReturn = sortFileListByDirOrFileFirst(lStart, lPass);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;	
}
// Parth Software Solution
/**
 * Gets the currently selected items for the active File Manager and 
 * stores it in the appropriate selection array (member field).
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CMainWindow::getCurrentSelection(HWND hwndLastActive)
{
	BOOL bReturn = TRUE;

	try
	{
		int iCtrlID = 0,
			*ariSelection = NULL;
		long lReturn = 0L;

		// validate last active File Manager handle
		if(hwndLastActive == NULL)
			return FALSE;

		// get number of items
		lReturn = SendMessage(hwndLastActive, LB_GETSELCOUNT, 
						(WPARAM)0, 0L);

		// create array if applicable
		if(lReturn > LB_ERR)
		{
			// dimension array
			ariSelection = new int[lReturn];

			// validate, continue
			if(ariSelection == NULL)
				bReturn = FALSE;
			else
			{
				// Get items
				lReturn = SendMessage(hwndLastActive, LB_GETSELITEMS, 
							(WPARAM)lReturn, (LPARAM)ariSelection);
			}
		}
		
		// get Ctrl ID
		iCtrlID = GetDlgCtrlID(hwndLastActive);

		// determine array from ctrl ID
		if(iCtrlID == IDC_TVFILEMANAGER1)
		{
			// destroy existing array
			if(m_ariFileManager1Selection)
				delete[] m_ariFileManager1Selection;

			// assign new (OR empty) array
			m_ariFileManager1Selection = ariSelection;

			// set count
			m_iCurrentSelectionCountFileManager1 = lReturn;
		}
		else if(iCtrlID == IDC_LSTFILEMANAGER2)
		{
			// destroy existing array
			if(m_ariFileManager2Selection)
				delete[] m_ariFileManager2Selection;

			// assign new (OR empty) array
			m_ariFileManager2Selection = ariSelection;

			// set count
			m_iCurrentSelectionCountFileManager2 = lReturn;
		}

//#ifdef _DEBUG
//
//		std::ofstream ofs("F:\\Downloads\\Output.txt", std::ios_base::binary | std::ios_base::out | std::ios_base::app);
//		if(iCtrlID == IDC_LSTFILEMANAGER1)
//			ofs << "File Manager 1\r\n";
//		else
//			ofs << "File Manager 2\r\n";
//		for(long lcv = 0L; lcv < *piSelectionCount; lcv++)
//		{
//			ofs << ariSelection[lcv] << "\r\n";
//		}
//		ofs << "\r\n";
//
//		ofs.close();
//
//#endif

		// set return value
		if(lReturn == LB_ERR || lReturn == 0L)
			bReturn = FALSE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("An unexpected error occurred while attempting to retrieve the active File Manager's selection.");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Sets the currently selected items for the active File Manager.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
BOOL CMainWindow::setCurrentSelection()
{
	BOOL bReturn = TRUE;

	try
	{
		int iCtrlID = 0,
			*ariSelection = NULL,
			*piSelectionCount = NULL;

		// validate last active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return FALSE;

		// get Ctrl ID
		iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);

		// determine array from ctrl ID
		if(iCtrlID == IDC_TVFILEMANAGER1)
		{
			// set array
			ariSelection = m_ariFileManager1Selection;

			// set count
			piSelectionCount = &m_iCurrentSelectionCountFileManager1;
		}
		else if(iCtrlID == IDC_LSTFILEMANAGER2)
		{
			// set array
			ariSelection = m_ariFileManager2Selection;

			// set count
			piSelectionCount = &m_iCurrentSelectionCountFileManager2;
		}

		// validate array... if there's nothing to do then exit
		if(piSelectionCount == NULL || *piSelectionCount == 0L)
		{
			// set to parent folder index (the default)
			SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)TRUE,
				(LPARAM)INDEX_PARENTFOLDER);

			// return success
			return TRUE;
		}

		// validate array pointer, jic...
		if(ariSelection == NULL)
			return FALSE;

		// Set items... not sure why the two API messages for this are
		//	 different
		for(long lcv = 0L; lcv < *piSelectionCount; lcv++)
		{
			// set selection state for item
			SendMessage(m_hwndActiveFileManager, LB_SETSEL, (WPARAM)TRUE,
				ariSelection[lcv]);
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Determines if the specified point is "inside" a command button in the
 * "button bar." This method returns the Ctrl ID of the command button.
 *
 * @return The control ID if successful, otherwise 0 (zero).
 */
long CMainWindow::getCommandButtonAtPoint(POINT ptTest)
{
	long lReturn = 0L;

	try
	{
		RECT *prctTemp = NULL;

		// validate command button RECT array
		if(m_arrctCommandButtons == NULL)
			return 0L;

		// iterate through command buttons and locate POINT, if applicable
		for(int i = 0; i < LAYOUT_COUNT_COMMANDBUTTONS; i++)
		{
			// get current rect
			prctTemp = m_arrctCommandButtons[i];
			
			// validate, continue
			if(prctTemp != NULL)
			{
				// check point
				if(PtInRect(m_arrctCommandButtons[i], ptTest))
				{
					// set Ctrl ID
					lReturn = IDC_CMDF1 + i;

					// exit, we're done
					break;
				}
			}
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("An unexpected error occurred while attempting to determine which command button resides at the given point.");

		// set fail val
		lReturn = 0L;
	}

	// reset last error, if applicable
	if(lReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return lReturn;
}

///**
// * Highlights the active command button on the button bar. "Active" is
// * determined by whether or not the mouse is currently hovering over the
// * button.
// *
// * @param ptMouse
// *
// * @return TRUE if no errors occur, otherwise FALSE.
// */
//BOOL CMainWindow::highlightCommandButton(HDC hdcCommandButtons, POINT ptMouse)
//{
//	static int iLastIndex = -1;
//	BOOL bReturn = TRUE;
//
//	try
//	{
//		RECT *prctTemp = NULL;
//		int iIndex = -1;
//
//		// validate internal button RECT array
//		if(m_arrctCommandButtons == NULL)
//			return FALSE;
//
//		// validate DC
//		if(hdcCommandButtons == NULL)
//			return FALSE;
//
//		// locate command button...
//		for(int i = 0; i < LAYOUT_COUNT_COMMANDBUTTONS; i++)
//		{
//			// get current RECT
//			prctTemp = m_arrctCommandButtons[i];
//
//			// validate, continue
//			if(prctTemp != NULL)
//			{
//				// check
//				if(PtInRect(prctTemp, ptMouse))
//				{
//					// set button index
//					iIndex = i;
//
//					// exit loop
//					break;
//				}	
//			}
//		}
//
//		// make sure there is something to do
//		if(iLastIndex == iIndex)
//			return TRUE;
//
//		// Redraw prior command button in non-highlighted state
//		drawCommandButton(iLastIndex, hdcCommandButtons, 
//				CMainWindow::cbseNormal);
//		
//		if(iIndex > -1)
//		{
//			// draw button highlighted
//			drawCommandButton(iIndex, hdcCommandButtons, 
//				CMainWindow::cbseFocus);
//
//			//// set last index
//			//iLastIndex = iIndex;
//		}
//		else
//		{
//			// return fail val
//			bReturn = FALSE;
//		}
//
//		// set last index
//		iLastIndex = iIndex;
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		bReturn = FALSE;
//	}
//
//	// reset last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// return success / fail val
//	return bReturn;
//}

///**
// *
// *
// *
// *
// *
// *
// *
// *
// */
//BOOL CMainWindow::drawCommandButton(int iButtonIndex, 
//	HDC hdcCommandButtons, COMMANDBUTTONSTATEENUM cbseState)
//{
//	HPEN hpenButtonState = NULL,
//		 hpenPrevious = NULL;
//	HBRUSH hbrButtonState = NULL,
//		   hbrPrevious = NULL;
//	HDC hdcButtonImage = NULL;
//	HFONT hfontPrevious = NULL;
//	BOOL bReturn = TRUE;
//
//	try
//	{
//		RECT *prctButton = NULL;
//
//		// validate internal button RECT array
//		if(m_arrctCommandButtons == NULL)
//			return FALSE;
//
//		// validate DC
//		if(hdcCommandButtons == NULL)
//			return FALSE;
//
//		// validate index
//		if(iButtonIndex < 0 || iButtonIndex > ((int)LAYOUT_COUNT_COMMANDBUTTONS - 1))
//			return FALSE;
//	
//		// create compatible DC
//		hdcButtonImage = CreateCompatibleDC(hdcCommandButtons);
//
//		// Determine how to draw button
//		if(cbseState == CMainWindow::cbseFocus)
//		{
//		//	// create highlight pen
//		//	hpenButtonState = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
//		//	if(hpenButtonState)
//		//		hpenPrevious = (HPEN)SelectObject(hdcCommandButtons, 
//		//							hpenButtonState);
//
//		//	// create highlight brush
//		//	hbrButtonState = CreateSolidBrush(RGB(0, 0, 0));
//		//	if(hbrButtonState)
//		//		hbrPrevious = (HBRUSH)SelectObject(hdcCommandButtons, 
//		//							hbrButtonState);
//		//	
//		//	// Set text back and fore colors
//		//	SetTextColor(hdcCommandButtons, RGB(255, 255, 0));
//		//	SetBkColor(hdcCommandButtons, RGB(0, 0, 0));
//
//			SelectObject(hdcButtonImage, 
//				m_arbmpCommandButtons[(iButtonIndex * 2) + 1]);
//		}
//		else if(cbseState == CMainWindow::cbseNormal)
//		{
//		//	// create highlight pen
//		//	hpenButtonState = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
//		//	if(hpenButtonState)
//		//		hpenPrevious = (HPEN)SelectObject(hdcCommandButtons, 
//		//							hpenButtonState);
//
//		//	// create highlight brush
//		//	hbrButtonState = CreateSolidBrush(RGB(0, 0, 0));
//		//	if(hbrButtonState)
//		//		hbrPrevious = (HBRUSH)SelectObject(hdcCommandButtons, 
//		//							hbrButtonState);
//		//	
//		//	// Set text back and fore colors
//		//	SetTextColor(hdcCommandButtons, RGB(255, 255, 255));
//		//	SetBkColor(hdcCommandButtons, RGB(0, 0, 0));
//
//			SelectObject(hdcButtonImage, m_arbmpCommandButtons[iButtonIndex * 2]);
//		}
//
//		//// Set font
//		//hfontPrevious = (HFONT)SelectObject(hdcCommandButtons, 
//		//					m_hfontControls);
//
//		// Get rect for button
//		prctButton = m_arrctCommandButtons[iButtonIndex];
//
//		// validate, draw
//		if(prctButton != NULL && hdcButtonImage != NULL 
//			/*&& hpenButtonState != NULL 
//		   && hbrButtonState != NULL*/)
//		{
//			//TCHAR tstrCaption[10] = EMPTY_STRING;
//
//			//// create caption
//			//_stprintf(tstrCaption, _T("F%d"), iButtonIndex + 1);
//
//			//// Render "focus" (highlight) rect
//			//RoundRect(hdcCommandButtons, prctButton->left, prctButton->top,
//			//	prctButton->right, prctButton->bottom, 
//			//	LAYOUT_RADIUS_COMMANDBUTTONS, LAYOUT_RADIUS_COMMANDBUTTONS);
//
//			//// Draw caption
//			//DrawText(hdcCommandButtons, tstrCaption, lstrlen(tstrCaption),
//			//	prctButton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//
//			// Draw current button
//			StretchBlt(hdcCommandButtons, prctButton->left, prctButton->top,
//				prctButton->right - prctButton->left, 
//				prctButton->bottom - prctButton->top,
//				hdcButtonImage, 20, 5, 84, 20, SRCCOPY);
//		}
//		else
//			bReturn = FALSE;
//	}
//	catch(...)
//	{
//		// set last error
//		m_strLastError = _T("");
//
//		// set fail val
//		bReturn = FALSE;
//	}
//
//	// garbage collect, release GDI objects
//	if(hdcCommandButtons != NULL)
//	{
//		// restore original pen
//		if(hpenPrevious)
//			SelectObject(hdcCommandButtons, hpenPrevious);
//		
//		// destroy created pen
//		if(hpenButtonState)
//			DeleteObject(hpenButtonState);
//
//		// restore original brush
//		if(hbrPrevious)
//			SelectObject(hdcCommandButtons, hbrPrevious);
//
//		// destroy created brush
//		if(hbrButtonState)
//			DeleteObject(hbrButtonState);
//
//		// restore original font
//		if(hfontPrevious)
//			SelectObject(hdcCommandButtons, hfontPrevious);
//
//		// DO NOT destroy controls font object. It is owned by this object.
//	}
//
//	// Delete compatible DC
//	if(hdcButtonImage)
//	{
//		// delete DC
//		DeleteDC(hdcButtonImage);
//
//		// deref
//		hdcButtonImage = NULL;
//	}
//
//	// reset last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// return success / fail val
//	return bReturn;
//}

/**
 * Waits until all painting (more or less) has been completed before
 * returning.
 *
 * @return 
 */
BOOL CMainWindow::doEvents()
{
	BOOL bReturn = TRUE;

	try
	{
		MSG msg;

		// Hold for re-draw
		while(PeekMessage(&msg, m_hwndThis, WM_PAINT, WM_PAINT, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch(...)
	{
		// set fail return val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Copies the filenames (ONLY) from the active File Manager's file
 * list to the temporary "file manager" list for "lookup."
 */
BOOL CMainWindow::copyFileListToLookupList()
{
	BOOL bReturn = TRUE;

	try
	{
		FILE_INFORMATION *pfiTemp = NULL;
		HWND hwndTempList = NULL;

		// validate active File Manager's handle
		if(m_hwndActiveFileManager == NULL)
			return FALSE;

		// validate active File Manager's file list
		if(m_pllstActiveFileManager == NULL)
			return FALSE;

		// validate this dialog's handle
		if(m_hwndThis == NULL)
			return FALSE;

		// attempt to get temp list's handle...
		hwndTempList = GetDlgItem(m_hwndThis, IDC_LSTFILEMANAGERTEMP);

		// validate, continue
		if(hwndTempList)
		{
			tstring strFullpath = EMPTY_STRING;
			int iCtrlID;

			// clear list
			SendMessage(hwndTempList, LB_RESETCONTENT, (WPARAM)0, 0L);
			
			// Get active folder in case machine root is selected... If the
			//	 machine root is selected, we need to do something different.
			iCtrlID = GetDlgCtrlID(m_hwndActiveFileManager);
			if(iCtrlID == IDC_TVFILEMANAGER1)
				strFullpath = g_csetApplication.lastFolderFileManager1();
			else
				strFullpath = g_csetApplication.lastFolderFileManager2();

			// compare to machine root
			if(strFullpath != FOLDER_ROOT)
			{
				// Add static entries
				//	 Machine Root
				SendMessage(hwndTempList, LB_ADDSTRING, (WPARAM)0,
					(LPARAM)FOLDER_MACHINEROOT);
				//	 Parent Folder
				SendMessage(hwndTempList, LB_ADDSTRING, (WPARAM)0,
					(LPARAM)FOLDER_PARENT);

				// copy filenames to temp list
				for(long lcv = 0L; lcv < m_pllstActiveFileManager->getLength(); lcv++)
				{
					// get current item
					pfiTemp = m_pllstActiveFileManager->getEntry(lcv);
					
					// validate
					if(pfiTemp == NULL)
						continue;

					// add filename (or directory name) to list
					SendMessage(hwndTempList, LB_ADDSTRING, (WPARAM)0, 
						(LPARAM)pfiTemp->pwfdFileInfo->cFileName);
				}
			}
			else
			{
				TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;
				long lCount = 0L;

				// get count from File Manager
				lCount = SendMessage(m_hwndActiveFileManager, LB_GETCOUNT, 
							(WPARAM)0, 0L);

				// copy drive listing to temp list
				for(long lcv = 0L; lcv < lCount; lcv++)
				{
					// Get item from File Manager
					SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)lcv,
						(LPARAM)tstrBuffer);

					// Add to temp list
					SendMessage(hwndTempList, LB_ADDSTRING, (WPARAM)0,
						(LPARAM)tstrBuffer);
				}
			}
		}
	}
	catch(...)
	{
		// set fail return val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Extracts the CADImporter DLL from the resource file if it cannot be
 * found in the application folder.
 *
 * @note To avoid confusion, this method probably should be broken
 * into two separate processes: the 
 *
 * @return TRUE if the CADImporter library exists in the application 
 * folder OR it is extracted successfully, otherwise FALSE.
 */
BOOL CMainWindow::extractCADImporterLibrary()
{
  using std::ios;

	BOOL bReturn = TRUE;	// default to optimistic return val

    try
    {
		std::basic_ofstream<TCHAR> ofsLibrary;
		HRSRC hresLibrary = NULL;
		HGLOBAL hglbLibrary = NULL;
		HWND hwndTemp = NULL;
		tstring strLibraryFilename = EMPTY_STRING;
		BYTE *pbtCADImporterLibraryResource = NULL;
		DWORD dwLibrarySize = (DWORD)0,
			  dwExtracted = (DWORD)0,
			  dwChunkSize = (DWORD)131072,	// 128 Kb
			  dwWrite = (DWORD)0;

		// check this object's handle
		if(m_hwndThis == NULL)
			return FALSE;

		// validate application path
		if(lstrlen(g_csetApplication.applicationFolder()) == 0)
		{
			// set last error
			m_strLastError = _T("The application directory is invalid. The CAD Importer library CANNOT be located.");
			
			// return fail val
			return FALSE;
		}

		// create fullpath to library
		strLibraryFilename = g_csetApplication.applicationFolder();
		if(strLibraryFilename[strLibraryFilename.length() - 1] != _T('\\'))
			strLibraryFilename += _T("\\");
		strLibraryFilename += FILENAME_CADIMPORTERLIBRARY;

		// check and see if library exists...
		if(Exists((TCHAR *)strLibraryFilename.c_str()))
			return TRUE;

		// Library doesn't exist, being extraction attempt.

		// attempt to get handle to help text resource
		hresLibrary = FindResource(NULL, MAKEINTRESOURCE(IDR_CADIMPORTERLIBRARY), 
						RESOURCE_TYPE_FILES);
		if(hresLibrary == NULL)
		{
			// set last error
			m_strLastError = _T("Could not locate the CAD Importer resource and the file DOES NOT already exist in the application folder.");

			// return fail val
			return FALSE;
		}

		// Get resource size
		dwLibrarySize = SizeofResource(NULL, hresLibrary);
		
		// attempt to load help text resource
		hglbLibrary = LoadResource(NULL, hresLibrary);

		// validate, continue
		if(hglbLibrary == NULL)
		{
			// set last error
			m_strLastError = _T("Could not create a global handle for the CAD Importer library. The library CANNOT be extracted from the application's resources.");

			// return fail val
			return FALSE;
		}

		// lock resource and turn it into something "usable"
		pbtCADImporterLibraryResource = (BYTE *)LockResource(hglbLibrary);
		
		// validate, continue
		if(pbtCADImporterLibraryResource == NULL)
		{
			// set last error
			m_strLastError = _T("Could not create lock access to the CAD Importer library. The library CANNOT be extracted from the application's resources.");

			// return fail val
			return FALSE;
		}

		// attempt to open output file
		ofsLibrary.open((TCHAR *)strLibraryFilename.c_str(), ios::binary | ios::out);
		if(ofsLibrary)
		{
			// write date to file
			while(dwExtracted < dwLibrarySize)
			{
				// set write "buffer" size
				if(dwExtracted + dwChunkSize > dwLibrarySize)
					dwWrite = dwLibrarySize - dwExtracted;
				else
					dwWrite = dwChunkSize;

				// write chunk
				ofsLibrary.write((TCHAR *)&pbtCADImporterLibraryResource[dwExtracted], 
					dwWrite);

				// increment
				dwExtracted += dwWrite;

				// let things happen
				doEvents();
			}

			// close output file
			ofsLibrary.close();

			// double check output
			if(!Exists((TCHAR *)strLibraryFilename.c_str()))
			{
				// set last error
				m_strLastError = _T("Could not create output file for CAD Importer library.");

				// return fail val
				bReturn = FALSE;
			}
		}
		else
		{
			// set last error
			m_strLastError = _T("Could not create output file stream for extraction of CAD Importer library.");

			// return fail val
			bReturn = FALSE;
		}

		// All resource handles, etc. are automatically freed when this process
		//	 terminates.
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While extracting the CAD Importer library, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

/**
 * Creates an auto-wrapped messagebox. For machines with Vista or higher, the
 * internal wrapping mechanism is used. For machines with XP or lower, the
 * wrapping is synthesized.
 *
 * @param hWnd See Microsoft's help for MessageBox()
 * @param lpText See Microsoft's help for MessageBox()
 * @param lpCaption See Microsoft's help for MessageBox()
 * @param uType See Microsoft's help for MessageBox()
 *
 * @return the result from the call to the MessageBox() API function
 *
 */
/***********************************************************************************
	Function Name:	WrappedMessageBox
	In Parameters:	HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType
	Out Parameters: int
	Description:	To Display the message
	Date & Time:	12th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
int CMainWindow::WrappedMessageBox(LPCTSTR lpText, LPCTSTR lpCaption,
	UINT uType)
{
	tstring astrMsg;
  std::vector<TCHAR> astrLocal;
	TCHAR *pc = NULL;
	DWORD dwVersion, dwMajorVersion;

	// Get OS version info
	dwVersion = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

	if(dwMajorVersion > 5)
	{
		// replace double line feeds with special character...
    size_t len = _tcslen(lpText);
    astrLocal.resize(len+1);
    _tcscpy(&astrLocal[0], lpText);
		for(pc = _tcsstr(&astrLocal[0], _T("\n\n"));
			pc;
			pc = _tcsstr(&astrLocal[0], _T("\n\n")))
		{
			pc[1] = 1;
		}
	
		// remove existing formatting
		for(pc = &astrLocal[0]; pc && *pc; pc++)
		{
			if(*pc == 1)
				astrMsg += _T("\r\n\r\n");
			else if(*pc != _T('\r') && *pc != _T('\n'))
				astrMsg += *pc;
			else if(*pc == _T('\n'))
				astrMsg += _T(" ");
		}
	}
	else
		astrMsg = lpText;

	// call the messagebox function
	return MessageBox(m_hwndThis, astrMsg.c_str(), lpCaption, uType);
}

/***********************************************************************************
	Function Name:	InsertTabItem
	In Parameters:	HWND hTab, LPTSTR pszText, int iid
	Out Parameters: int
	Description:	To Remove item in the treeView
	Date & Time:	15th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
int CMainWindow::RemoveTabItem(HWND hTab, int iid)
{
	try
	{
		int iActualID = -1;
		int iCount  = TabCtrl_GetItemCount(hTab);

		char szActualText[50] = {0};
		switch(iid)
		{
		case 0:
			strcpy(szActualText, ZEROTHTAB);
			break;			
		case 1:
			strcpy(szActualText, FIRSTTAB);
			break;
		case 2:
			strcpy(szActualText, SECONDTAB);
			break;
		case 3:
			strcpy(szActualText, THIRDTAB);
			break;
		}

		//Get Actual Item ID
		for(int i = 0; i < iCount; i++)
		{
			char szItemText[50] = {0};
			TCITEM pitem;
			pitem.pszText = szItemText;
			pitem.cchTextMax = 50;
			pitem.mask = TCIF_TEXT;
			TabCtrl_GetItem(hTab, i, &pitem);

			if(strcmp(szItemText, szActualText) == 0)
			{
				iActualID = i;
				break;
			}
		}

		TCITEM ti = {0};
		return (int)SendMessage(hTab, TCM_DELETEITEM, iActualID, (LPARAM)&ti);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("Error occured in CMainWindow::InsertTabItem");
		return 0;
	}
}

/***********************************************************************************
	Function Name:	InsertTabItem
	In Parameters:	HWND hTab, LPTSTR pszText, int iid
	Out Parameters: int
	Description:	To Insert item in the treeView
	Date & Time:	15th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
int CMainWindow::InsertTabItem(HWND hTab, LPTSTR pszText, int iid)
{
	if(!m_objIntropCom.m_pIComIntrop)
	{
		HWND hwnd = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		SendMessage(hwnd, EM_REPLACESEL, (WPARAM)FALSE, 
						(LPARAM)(TCHAR *)m_objIntropCom.strLastError.data());
		return 0;
	}

	try
	{
		TCITEM ti = {0};
		ti.mask = TCIF_TEXT;
		ti.pszText = pszText;
		ti.cchTextMax = strlen(pszText);		
		SendMessage(hTab, TCM_INSERTITEM, iid, (LPARAM)&ti);

		ShowActivePage(iid);//show active page here & starts from 1

		TabCtrl_SetCurFocus(hTab, iid);

		return 1;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("Error occured in CMainWindow::InsertTabItem");
		return 0;
	}
}

/***********************************************************************************
	Function Name:	SetPercentageRange
	In Parameters:	int iRange
	Out Parameters: void
	Description:	To set the range before start process to calculate remaining percentage.
	Date & Time:	18th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
void CMainWindow::SetPercentageRange(int iRange)
{
	try
	{
		m_iRange = iRange;
		m_TempiRange = iRange;
		// set range
		SendMessage(progress_bar_handle(), PBM_SETRANGE32, (WPARAM)0, (LPARAM)100);

		//Set initial position
		SendMessage(progress_bar_handle(), PBM_SETSTEP, (WPARAM)1, 0L);

		// set initial value
		SendMessage(progress_bar_handle(), PBM_SETPOS, (WPARAM)0, 0L);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("Error occured in CMainWindow::SetPercentageRange");
		return;
	}
}

/***********************************************************************************
	Function Name:	SetPercentage
	In Parameters:	int iPercentage
	Out Parameters: void
	Description:	To set the percentage to progress control and percentage label.
	Date & Time:	18th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
void CMainWindow::SetPercentage(int iPercentage)
{
	try
	{

		if( iPercentage == 100 || iPercentage == 0)
		{
			// increment progressbar
			iPercentage == 0 ? SendMessage(progress_bar_handle(), PBM_SETPOS, (WPARAM)0, 0L):
				SendMessage(progress_bar_handle(), PBM_SETPOS, (WPARAM)100, 0L);
			char szBuffer[30] = {0};
			sprintf(szBuffer, "%d.00%%", iPercentage);
			OutputDebugString(szBuffer);
			SetDlgItemText(m_hwndThis, IDC_STATIC_PROGRESS_TEXT, szBuffer);
			return;
		}

		if(m_TempiRange == 0)
			return;

		float iProgress = (float)100/m_TempiRange;

		// increment progressbar
		SendMessage(progress_bar_handle(), PBM_SETPOS, (WPARAM)(int)iProgress, 0L); 		

		char szBuffer[30] = {0};
		sprintf(szBuffer, "%0.2f%%", iProgress);
		OutputDebugString(szBuffer);
		SetDlgItemText(m_hwndThis, IDC_STATIC_PROGRESS_TEXT, szBuffer);

		m_TempiRange--;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("Error occured in CMainWindow::SetPercentage");
		return;
	}
}

/***********************************************************************************
	Function Name:	GetSelectedItemParentPath
	In Parameters:	HWND hwndFileManager, HTREEITEM hTreeItem, TCHAR *strFullPath
	Out Parameters: bool
	Description:	To Get the parent Path of selected item.
	Date & Time:	19th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
bool CMainWindow::GetSelectedItemParentPath(HWND hwndFileManager, HTREEITEM hTreeItem, TCHAR *strFullPath)
{
	if (hTreeItem == NULL) // Nothing selected
		return false;
	try
	{
		// get parent text till root
		HTREEITEM hcurrItem = hTreeItem;
		std::vector<std::string> vPath;
		do
		{

			char Text1[255]="";
			TV_ITEM tv1; 
			tv1.mask=TVIF_TEXT;
			tv1.pszText=Text1;
			tv1.cchTextMax=256;

			hcurrItem = TreeView_GetParent(hwndFileManager, hcurrItem);  

			if(hcurrItem)
			{
				tv1.hItem=hcurrItem;
				if(TreeView_GetItem(hwndFileManager,&tv1))
				{
					// Parth Software Solution buf fixing
					std::string fullfilename = tv1.pszText;
					int found=fullfilename.find(SEARCHKEY_DIRECTORY);
					if (found!= tstring::npos)
					{											
						int found1 = fullfilename.find("|");
						tstring tmpfile = fullfilename.substr(found1 + 1);
						int found2 = tmpfile.find("|");
						tmpfile = fullfilename.substr(found1 + 1,found2-1);

						StringTrim(tmpfile);				

						TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
						_tcscpy(tv1.pszText,tmp1);

					}
					// it is file
					else
					{
						if (found!= tstring::npos)
						{
							int found1 = fullfilename.find("|");
							tstring tmpfile = fullfilename.substr(found1 + 1);
							int found2 = tmpfile.find("|");
							tmpfile = fullfilename.substr(found1 + 1,found2-1);

							StringTrim(tmpfile);				
						
							TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
							_tcscpy(tv1.pszText,tmp1);
							
						}
					}
					vPath.push_back(tv1.pszText);
				}
			}
		}while(hcurrItem != NULL);

		std::vector<std::string>::const_iterator itr; 
		_tcscpy(strFullPath , "");

		for(int i=vPath.size()-1;i >= 0;i--)
		{ 
			std::string str = vPath[i];
			if(strcmp(str.c_str(),TVFOLDER_ROOT) != 0)
			{
				//vPath.push_back(tv1.pszText);
				_tcscat(strFullPath , str.c_str());
				_tcscat(strFullPath , "\\");
			}
			_tcscat(strFullPath , str.c_str());
			_tcscat(strFullPath , "\\");
		} 
		if(_tcslen(strFullPath) > 0)
		{
			return true;
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("Error occured in CMainWindow::GetSelectedItemParentPath");
		return false;
	}
	return false;
}

/***********************************************************************************
	Function Name:	DeleteAllChildItems
	In Parameters:	HWND hWnd, HTREEITEM hSelectedTreeItem
	Out Parameters: bool
	Description:	To Delete the all child items from selected item.
	Date & Time:	20th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
bool CMainWindow::DeleteAllChildItems(HWND hWnd, HTREEITEM hSelectedTreeItem)
{
	try
	{
		// clear all child of selected tree view node
		// parse all children of selected item
		// delete it one by one
		HTREEITEM hChild1 = NULL;
		hChild1 = TreeView_GetChild(hWnd,hSelectedTreeItem);
		if(hChild1)
		{
			//get sibling of that child
			HTREEITEM hSibling = hChild1;
			BOOL retval = FALSE;
			TVITEM tvchild = {0};
			tvchild.mask = TVIF_TEXT;
			do
			{
				hSibling = TreeView_GetNextSibling(hWnd,hChild1);
				if(hSibling)
				{
					tvchild.hItem = hSibling;
					retval = TreeView_GetItem(hWnd,&tvchild);
					retval = TreeView_DeleteItem(hWnd,hSibling);
				}
			}while(hSibling != NULL);

			tvchild.hItem=hChild1;
			retval = TreeView_GetItem(hWnd,&hChild1);
			retval = TreeView_DeleteItem(hWnd,hChild1);
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("Error occured in CMainWindow::DeleteAllChildItems");
		return false;
	}
	return true;
}

/***********************************************************************************
	Function Name:	GetSelectedItemsCount
	In Parameters:	HWND hWnd
	Out Parameters: int 
	Description:	To Get the selected count
					Currently TreeView dont have multiple selection
					Delete previous line after implementation
	Date & Time:	22th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
int CMainWindow::GetSelectedItemsCount(HWND hWnd)
{
	return CWin32TreeView::GetSelectedCount(hWnd);
}

/***********************************************************************************
	Function Name:	CheckErrorGivePromptMessage
	In Parameters:	int iErrorId
	Out Parameters: tstring 
	Description:	To display the proper message regarding the Error ID 
					mostly Id returned by the SHFileOperation
	Date & Time:	22th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
tstring CMainWindow::CheckErrorGivePromptMessage(int iErrorId)
{
	tstring strError;
	switch(iErrorId)
	{
		case 0x71:
			strError = "The source and destination files are the same file. ";
			break;
		case 0x72:
			strError = "Multiple file paths were specified in the source buffer, but only one destination file path. ";
			break;
		case 0x73:
			strError = "Rename operation was specified but the destination path is a different directory. Use the move operation instead. ";
			break;
		case 0x74:
			strError = "The source is a root directory, which cannot be moved or renamed. ";
			break;
		case 0x75:
			strError = "The operation was cancelled by the user, or silently cancelled if the appropriate flags were supplied to SHFileOperation. ";
			break;
		case 0x76:
			strError = "The destination is a subtree of the source. ";
			break;
		case 0x78:
			strError = "Security settings denied access to the source. ";
			break;
		case 0x79:
			strError = "The source or destination path exceeded or would exceed MAX_PATH. ";
			break;
		case 0x7A:
			strError = "The operation involved multiple destination paths, which can fail in the case of a move operation. ";
			break;
		case 0x7C:
			strError = "The path in the source or destination or both was invalid. ";
			break;
		case 0x7D:
			strError = "The source and destination have the same parent folder. ";
			break;
		case 0x7E:
			strError = "The destination path is an existing file. ";
			break;
		case 0x80:
			strError = "The destination path is an existing folder. ";
			break;
		case 0x81:
			strError = "The name of the file exceeds MAX_PATH.";
			break;
		case 0x82:
			strError = "The destination is a read-only CD-ROM, possibly unformatted.";
			break;
		case 0x83:
			strError = "The destination is a read-only DVD, possibly unformatted.";
			break;
		case 0x84:
			strError = "The destination is a writable CD-ROM, possibly unformatted.";
			break;
		case 0x85:
			strError = "The file involved in the operation is too large for the destination media or file system.";
			break;
		case 0x86:
			strError = "The source is a read-only CD-ROM, possibly unformatted.";
			break;
		case 0x87:
			strError = "The source is a read-only DVD, possibly unformatted.";
			break;
		case 0x88:
			strError = "The source is a writable CD-ROM, possibly unformatted.";
			break;
		case 0xB7:
			strError = "MAX_PATH was exceeded during the operation.";
			break;
		case 0x402:
			strError = "An unknown error occurred. This is typically due to an invalid path in the source or destination. This error does not occur on Windows Vista and later";
			break;
		case 0x10000:
			strError = "An unspecified error occurred on the destination.";
			break;
		case 0x10074:
			strError = "Destination is a root directory and cannot be renamed.";
			break;
		default:
			strError = "";
			break;
	}
	return strError;
}

/***********************************************************************************
	Function Name:	SelectNextItem
	In Parameters:	HWND hWnd, HTREEITEM hSelectedTreeItem
	Out Parameters: HTREEITEM 
	Description:	To select the Next item in the tree control
	Date & Time:	27th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
HTREEITEM CMainWindow::SelectNextItem(HWND hWnd, HTREEITEM hSelectedTreeItem)
{
	try
	{
		HTREEITEM hSibling = TreeView_GetNextSibling(hWnd, hSelectedTreeItem);
		if(hSibling)
		{
			TreeView_SelectItem(hWnd, hSibling);			
			return hSibling;
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("Error occured in CMainWindow::DeleteAllChildItems");
		return NULL;
	}
	return NULL;
}

/***********************************************************************************
	Function Name:	SelectPreviousItem
	In Parameters:	HWND hWnd, HTREEITEM hSelectedTreeItem
	Out Parameters: HTREEITEM 
	Description:	To select the previous item in the tree control
	Date & Time:	27th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
HTREEITEM CMainWindow::SelectPreviousItem(HWND hWnd, HTREEITEM hSelectedTreeItem)
{
	try
	{
		HTREEITEM hSibling = TreeView_GetPrevSibling(hWnd, hSelectedTreeItem);
		if(hSibling)
		{
			TreeView_SelectItem(hWnd, hSibling);
			return hSibling;
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("Error occured in CMainWindow::DeleteAllChildItems");
		return NULL;
	}
	return NULL;
}

/***********************************************************************************
	Function Name:	GetSelectedItemText
	In Parameters:	HWND hWndControl , TCHAR *szText, int iSize, BOOL* IsFile
	Out Parameters: bool
	Description:	To get the selected item text.
	Date & Time:	19th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
bool CMainWindow::GetSelectedItemText( HWND hWndControl , TCHAR *szText, int iSize, BOOL* IsFile)
{
	// Get a handle to the selected item
	HTREEITEM hSelectedItem = TreeView_GetSelection(hWndControl);

	if (hSelectedItem == NULL) // Nothing selected
		return false;
	try
	{
		TVITEM item;
		item.hItem = hSelectedItem;
		item.mask = TVIF_TEXT;
		item.cchTextMax = iSize;
		item.pszText = szText;
		if (TreeView_GetItem(hWndControl, &item))
		{
			// Parth Software Solution- bug fixing
			//if(_tcsstr(item.pszText, SEARCHKEY_DIRECTORY))
			std::string fullfilename = item.pszText;
			int found=fullfilename.find(SEARCHKEY_DIRECTORY);
			if (found!= tstring::npos)
			{					
				int found1 = fullfilename.find("|");
				tstring tmpfile = fullfilename.substr(found1 + 1);
				int found2 = tmpfile.find("|");
				tmpfile = fullfilename.substr(found1 + 1,found2-1);
				
				StringTrim(tmpfile);
				int iLen = _tcslen(tmpfile.c_str());
				tmpfile.replace(1, 0, "" );

				TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
				_tcscpy(szText,tmp1);

			}
			// it is file
			else
			{				
				if (found!= tstring::npos)
				{
					int found1 = fullfilename.find("|");
					tstring tmpfile = fullfilename.substr(found1 + 1);
					int found2 = tmpfile.find("|");
					tmpfile = fullfilename.substr(found1 + 1,found2-1);
					
					StringTrim(tmpfile);

					TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
					_tcscpy(szText,tmp1);
					if(IsFile)
						*IsFile = TRUE;
				}
			}
			return true;
		}
	}
	catch(...)
	{
		// set last error
		OutputDebugString("Error occured in CMainWindow::GetSelectedItemText");
		return false;
	}
	return false;
}

/**
 * Displays the directory listing for the specified folder using the
 * specified control and "active" sort.
 *
 * @param hwndOutputControl
 *
 * @param pllstOutput
 *
 * @return
 */
BOOL CMainWindow::displayDirectoryListingForCheckedItem_TV(HWND hwndOutputControl, 
	LinkedListEx<FILE_INFORMATION> *pllstOutput)
{
	//static BOOL bDisplayingDirectoryListForCheckedItem = FALSE;
	BOOL bReturn = FALSE;	// default to failure val

	 try
	{
		WIN32_FIND_DATA *pwfdItem = NULL;
		ACERIGHTS *pacerightsItem = NULL;
		FILETIME ftLocal;
		SYSTEMTIME systLocal;
		NUMBERFMT numfmtItem;
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING,
			  tstrNumber[80] = EMPTY_STRING;
		long lLongestFileObjectName = 0L;
		tstring strAttributes = EMPTY_STRING,
					strPermissions = EMPTY_STRING;
		BOOL bSortSucceeded = TRUE;
		//Parth Software Solution
		int iDlgCntrlID = GetDlgCtrlID(hwndOutputControl);
		//Parth Software Solution

		// check "working" flag
		//if(bDisplayingDirectoryListForCheckedItem)
		//	return TRUE;
		//bDisplayingDirectoryListForCheckedItem = TRUE;

		// validate handle and list params
		if(pllstOutput == NULL)
			return bReturn;
		if(hwndOutputControl == NULL)
			return bReturn;

		// Clear contents
		//SendMessage(hwndOutputControl, LB_RESETCONTENT, (WPARAM)0,
		//	(LPARAM)0);
		//TreeView_DeleteAllItems(hwndOutputControl);

		// Add "Machine Root" entry
		//SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
		//	(LPARAM)FOLDER_MACHINEROOT);

		// Add "Up One Level" entry
		//SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
		//	(LPARAM)FOLDER_PARENT);
		TVINSERTSTRUCT tvinsert;//TV_INSERTSTRUCT
		//TVINSERTSTRUCT tvinsert2;
		BOOL retval = FALSE;
		
		HTREEITEM currRoot = NULL,
				  currparent = NULL;
		int iID1 = GetDlgCtrlID(hwndOutputControl);
		if(iID1 == IDC_TVFILEMANAGER1)
		{
			currRoot = Root;
		}
		
		if(iID1 == IDC_TVFILEMANAGER2)
		{
			currRoot = Root2;
		}
		if(!currRoot)
		{
			// creating image list and put it into the tree control
			//====================================================//
			
			hImageList=ImageList_Create(16,16,ILC_COLOR16,2,10);					  // Macro: 16x16:16bit with 2 pics [array]
			hBitMap=LoadBitmap(m_hinstApplication,MAKEINTRESOURCE(IDB_TREE));					  // load the picture from the resource
			ImageList_Add(hImageList,hBitMap,NULL);								      // Macro: Attach the image, to the image list
			DeleteObject(hBitMap);													  // no need it after loading the bitmap
		    //SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control
			SendDlgItemMessage(m_hwndThis,iID1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control
			
			tvinsert.hParent=NULL;			// top most level no need handle
			tvinsert.hInsertAfter=TVI_ROOT; // work as root level
            tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;//|TVI_SORT;
			
	        tvinsert.item.pszText= TVFOLDER_ROOT;//"My Computer";
			tvinsert.item.iImage=0;
			tvinsert.item.iSelectedImage=1;
			//Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
			currparent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iID1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
			currRoot=Parent;
			Before=Parent;                   // handle of the before root
			tvinsert.hParent=currparent;         // handle of the above data
			
			// code for Treeview2
			//tvinsert2.hParent=NULL;			// top most level no need handle
			//tvinsert2.hInsertAfter=TVI_ROOT; // work as root level
   //         tvinsert2.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;//|TVI_SORT;
			//
	  //      tvinsert2.item.pszText= TVFOLDER_ROOT;//"My Computer";
			//tvinsert2.item.iImage=0;
			//tvinsert2.item.iSelectedImage=1;
			//Parent2=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER2,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
			//Root2=Parent2;
			//Before2=Parent2;                   // handle of the before root
			//tvinsert2.hParent=Parent2;         // handle of the above data
			//
		}

		// create number format
		memset(&numfmtItem, 0, sizeof(numfmtItem));
		numfmtItem.lpThousandSep = THOUSANDS_SEPARATOR;
		numfmtItem.lpDecimalSep = DECIMAL_SEPARATOR;
		numfmtItem.Grouping = 3;



		// Get longest file object name
		for(long lcv = 0L; lcv < pllstOutput->getLength(); lcv++)
		{
			// Get current file object's information
			pwfdItem = pllstOutput->getEntry(lcv)->pwfdFileInfo;

			// check object's name
			if(lstrlen(pwfdItem->cFileName) > lLongestFileObjectName)
				lLongestFileObjectName = lstrlen(pwfdItem->cFileName);
		}


		if(m_pllstActiveFileManager)
		{
			pcmwndThis->SetPercentageRange(m_pllstActiveFileManager->getLength());
			pcmwndThis->SetPercentage(0);

			// Determine what sort to perform
			switch(m_aseActiveSort)
			{


			case aseNameDesc:
				bSortSucceeded = sortFileListByName(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseTypeAsc:
			case aseTypeDesc:
				bSortSucceeded = sortFileListByType(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseSizeAsc:
			case aseSizeDesc:
				bSortSucceeded = sortFileListBySize(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseDateAsc:
			case aseDateDesc:
				bSortSucceeded = sortFileListByDate(0, 
					m_pllstActiveFileManager->getLength());
				break;

			case aseExtensionAsc:
			case aseExtensionDesc:
				bSortSucceeded = sortFileListByExtension(0,
					m_pllstActiveFileManager->getLength());
				break;

				//Parth Software Solution bug fixing
			case aseDirAsc:
			case aseDirDesc:
				bSortSucceeded = sortFileListByDirOrFileFirst(0,
					m_pllstActiveFileManager->getLength());
				break;
				//Parth Software Solution

			default:
				break;
			}

			pcmwndThis->SetPercentage(100);
		}
		// check sort 
		if(!bSortSucceeded)
			WrappedMessageBox(
				_T("Could not sort the active file list."),
				MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

		// iterate through file objects
		int iID = GetDlgCtrlID(hwndOutputControl);
		if(iID <= -1L)
			return FALSE;

		Selected= CWin32TreeView::GetCurrentSelectedItem(m_hwndActiveFileManager);
		if(Selected == NULL)
		{
			return false;
		}

		if(hwndOutputControl)
			DeleteAllChildItems(hwndOutputControl, Selected);		

		for(long lcv = 0L; lcv < pllstOutput->getLength(); lcv++)
		{
			// Get current file object's information
			pwfdItem = pllstOutput->getEntry(lcv)->pwfdFileInfo;

			// Get current file object's rights
			pacerightsItem = pllstOutput->getEntry(lcv)->paceFileRights;

			// Make sure this isn't the parent / current directory
			if(pwfdItem != NULL)
			{
				//	 Convert date modified time
				//		For read-only file systems, the last accessed time
				//			will be zero (not present). Use the date created
				//			instead (this is what Windows(r) actually does).
				if(pwfdItem->ftLastAccessTime.dwLowDateTime &&
					pwfdItem->ftLastAccessTime.dwHighDateTime)
					FileTimeToLocalFileTime(&pwfdItem->ftLastAccessTime, &ftLocal);
				else
					FileTimeToLocalFileTime(&pwfdItem->ftCreationTime, &ftLocal);

				//	Convert to system time
				FileTimeToSystemTime(&ftLocal, &systLocal);

				// Create attributes string
				//	 Content Not Indexed
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
					strAttributes = _T("N");
				else
					strAttributes = _T("_");
				//	 Archive
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
					strAttributes += _T("A");
				else
					strAttributes += _T("_");
				//	 Read-only
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					strAttributes += _T("R");
				else
					strAttributes += _T("_");
				//	 Hidden
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
					strAttributes += _T("H");
				else
					strAttributes += _T("_");
				//	 System
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
					strAttributes += _T("S");
				else
					strAttributes += _T("_");
				//	 Compressed
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
					strAttributes += _T("C");
				else
					strAttributes += _T("_");
				//	 Encrypted
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
					strAttributes += _T("E");
				else
					strAttributes += _T("_");

				// Create permissions string
				strPermissions = pacerightsItem->toString();

				//	 Get format for type
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					_stprintf(tstrBuffer, FORMAT_DIRECTORY, lLongestFileObjectName,
						pwfdItem->cFileName,
						systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
						systLocal.wHour, systLocal.wMinute,
						strAttributes.c_str(),
						strPermissions.c_str());

					//	 create output string
					// Parth Software Solution bug fixing
					//_stprintf(tstrNumber, _T("%u"),pwfdItem->cFileName);
					//_stprintf(tstrBuffer, FORMAT_FILE1,pwfdItem->cFileName);
					//#define FORMAT_DIRECTORY			_T("<DIR>         %-*s %02d.%02d.%4d %02d:%02d %s %s")
					//#define FORMAT_DIRECTORY_TV			_T("%s  <DIR>  %02d.%02d.%4d %02d:%02d %s %s")
					//_stprintf(tstrBuffer, FORMAT_DIRECTORY_TV,
					//	pwfdItem->cFileName,
					//	systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
					//	//systLocal.wHour, systLocal.wMinute
					//	//strAttributes.c_str(),
					//	strPermissions.c_str());
					// Parth Software Solution
				}
				else
				{
					// Format file size
					_stprintf(tstrNumber, _T("%u"), pwfdItem->nFileSizeLow);
					GetNumberFormat(LOCALE_USER_DEFAULT, 0, tstrNumber,
						&numfmtItem, tstrBuffer, sizeof(tstrBuffer));
					lstrcpy(tstrNumber, tstrBuffer);

					// create output string
					if(lstrlen(pwfdItem->cFileName))
						_stprintf(tstrBuffer, FORMAT_FILE, tstrNumber, 
							lLongestFileObjectName, pwfdItem->cFileName,
							systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
							systLocal.wHour, systLocal.wMinute,
							strAttributes.c_str(),
							strPermissions.c_str());
				}
				
				//	 display string
				//SendMessage(hwndOutputControl, LB_ADDSTRING, (WPARAM)0, 
				//	(LPARAM)tstrBuffer);				
				
				//if(Selected != NULL)
				{
					tvinsert.hParent = Selected;
					tvinsert.hInsertAfter=TVI_LAST; 
					tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
					tvinsert.item.iImage=0;
					tvinsert.item.iSelectedImage=1;
					tvinsert.item.pszText= tstrBuffer;
					//Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
					Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iID,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
					
				}
			}
		}

		// If we made it here, return success
		bReturn = TRUE;
	
		// Compute gestimate for horizontal scrollbar width
		///HDC hdcTemp = GetDC(hwndOutputControl);
		//if(hdcTemp)
		//{
			//RECT rctTemp;
			//SIZE sszText;
			//TCHAR tstrBuffer[MAX_PATH * 2];
			//long lScrollRange = 0L;

			// get RECT for active File Manager
			//GetWindowRect(hwndOutputControl, &rctTemp);

			// Get text for first "normalized" item
			/*SendMessage(hwndOutputControl, LB_GETTEXT, 
				(WPARAM)INDEX_PARENTFOLDER + 1, (LPARAM)tstrBuffer);

			// get text width
			GetTextExtentPoint32(hdcTemp, tstrBuffer, lstrlen(tstrBuffer),
				&sszText);

			// Calc scroll range
			lScrollRange = sszText.cx + abs(sszText.cx - (rctTemp.right - rctTemp.left));

			// Set scrollbar's range to total required - control's width
			SendMessage(hwndOutputControl, LB_SETHORIZONTALEXTENT, (WPARAM)lScrollRange,*/ 
			//	0L);//ntf

			// Release DC
			///ReleaseDC(hwndOutputControl, hdcTemp);
		//}
	}
    catch(...)
    {
        // set last error
        m_strLastError = _T("While attempting to display the directory listing, an unexpected error occurred.");

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// clear "working" flag
	//bDisplayingDirectoryListForCheckedItem = FALSE;

    // return success / fail val
    return bReturn;
}

/**
 * Executes whatever the appropriate, double-click action is for the item 
 * in the active File Manager.
 *
 * @return
 */
BOOL CMainWindow::executeCheckedItem_TV(int iID)
{
    BOOL bReturn = FALSE;

    try
	{
		long lIndex = -1L;
		//int iID = -1;

		// validate active File Manager handle
		if(m_hwndActiveFileManager == NULL)
			return bReturn;
				
		//m_hwndActiveFileManager = GetDlgItem(m_hwndThis, IDC_TVFILEMANAGER1);	
		m_hwndActiveFileManager = GetDlgItem(m_hwndThis, iID);		
		HWND hTreectrl = m_hwndActiveFileManager;//GetDlgItem(m_hwndThis,IDC_TVFILEMANAGER1);
		// get currently selected item
		TV_ITEM tvi;
		char Text[255]="";
		memset(&tvi,0,sizeof(tvi));

		int iIndex = GetSelectedItemsCount(m_hwndActiveFileManager);
		if(iIndex == 0)
		{
			WrappedMessageBox("Please select atleast single item/s", 			MAINWINDOW_TITLE,
					MB_OK | MB_ICONINFORMATION);
			CWin32TreeView::TreeView_SelectAll(m_hwndActiveFileManager, false);
			return bReturn;
		}
		if(iIndex > 1)
		{
			WrappedMessageBox("Selection of folder should be only one", MAINWINDOW_TITLE,
				MB_OK | MB_ICONINFORMATION);
			CWin32TreeView::TreeView_SelectAll(m_hwndActiveFileManager, false);
			return bReturn;
		}

		Selected = CWin32TreeView::GetCurrentSelectedItem(m_hwndActiveFileManager);
		
		if(Selected==NULL)
		{
			MessageBox(m_hwndThis,"No Items in TreeView","Error",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}

		// check if user has selected root node
		HTREEITEM hRoot = TreeView_GetRoot(hTreectrl);
		if(hRoot == Selected)
			return FALSE;		
		
		TreeView_EnsureVisible(hTreectrl,Selected);
	    SendDlgItemMessage(m_hwndThis,iID,TVM_SELECTITEM,TVGN_CARET,(LPARAM)Selected);
		flagSelected=true;
		
        tvi.mask=TVIF_TEXT;
		tvi.pszText=Text;
		tvi.cchTextMax=256;
		tvi.hItem=Selected;
		
		// get parent text till root
		HTREEITEM hcurrItem = NULL;
		//BOOL IsRoot = FALSE;
		std::vector<std::string> vPath;
		hcurrItem = Selected;
		do
		{
			
			char Text1[255]="";
			TV_ITEM tv1; 
			tv1.mask=TVIF_TEXT;
			tv1.pszText=Text1;
			tv1.cchTextMax=256;
				
			//hcurrItem = (HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_GETITEM,TVGN_PARENT,(LPARAM)&tv1);
			hcurrItem = TreeView_GetParent(hTreectrl,hcurrItem);  

			if(hcurrItem)
			{
				tv1.hItem=hcurrItem;
				bReturn = TreeView_GetItem(hTreectrl,&tv1);
				if(strcmp(tv1.pszText,TVFOLDER_ROOT) != 0)
				{
					// Parth Software Solution buf fixing
					std::string fullfilename = tv1.pszText;
					int found=fullfilename.find(SEARCHKEY_DIRECTORY);
					if (found!= tstring::npos)
					{					
						int found1 = fullfilename.find("|");
						tstring tmpfile = fullfilename.substr(found1 + 1);
						int found2 = tmpfile.find("|");
						tmpfile = fullfilename.substr(found1 + 1,found2-1);

						StringTrim(tmpfile);
						TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
						_tcscpy(tv1.pszText,tmp1);

					}
					// it is file
					else
					{
						if (found!= tstring::npos)
						{
							int found1 = fullfilename.find("|");
							tstring tmpfile = fullfilename.substr(found1 + 1);
							int found2 = tmpfile.find("|");
							tmpfile = fullfilename.substr(found1 + 1,found2-1);
							
							StringTrim(tmpfile);
						
							TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
							_tcscpy(tv1.pszText,tmp1);
							
						}
					}
					//
					vPath.push_back(tv1.pszText);
				}
				//IsRoot = FALSE;
			}
		}while(hcurrItem != NULL);
		//lIndex = SendMessage(m_hwndActiveFileManager, LB_GETCURSEL, (WPARAM)0, 
		//			0L);

		// Get ID for current File Manager - i.e. determine whether this is
		//	 File Manager 1 or 2.
		//iID = GetDlgCtrlID(m_hwndActiveFileManager);

		// validate selected item, continue
		//if(lIndex > -1L)//ntf
		{
			TCHAR tstrBuffer[MAX_PATH];

			// get selected item
			/*if(SendMessage(m_hwndActiveFileManager, LB_GETTEXT, (WPARAM)lIndex,
			   (LPARAM)tstrBuffer) != LB_ERR)*/
			if(SendDlgItemMessage(m_hwndThis,iID,TVM_GETITEM,TVGN_CARET,(LPARAM)&tvi))
			{
				// Get base folder for selected item
				tstring strTemp = EMPTY_STRING;
				LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;

				pllstActive = m_pllstFileManager1;//shri
				strTemp = g_csetApplication.lastFolderFileManager1();				
				{
					//
					std::vector<std::string>::const_iterator itr; 
					_tcscpy(tstrBuffer , "");
					  //for (itr=vPath.end(); itr!=vPath.begin(); itr++) 
					  for(int i=vPath.size()-1;i >= 0;i--)
					  { 
						  std::string str = vPath[i];//(*itr);
						 _tcscat(tstrBuffer , str.c_str());
						_tcscat(tstrBuffer , "\\");
					  } 
					//
					//memcpy(tstrBuffer,"",260);
					//_tcscpy( tstrBuffer , strTemp.c_str());
					  //Parth Software Solution bug fixing
					  BOOL IsFile = FALSE;
					  if(_tcscmp(tstrBuffer,"") != 0)
					  {
						  GetSelectedItemText(m_hwndActiveFileManager, tvi.pszText, _countof(tstrBuffer),&IsFile);						
					  }
					  // Parth Software Solution
					_tcscat(tstrBuffer , tvi.pszText);
					if(IsFile)
						return TRUE;
					//_tcscat(tstrBuffer , "\\");
				}
				
				if(FILE_ATTRIBUTE_DIRECTORY != GetFileAttributes((LPCTSTR)tstrBuffer))
				{
					int lcv = _tcslen(tstrBuffer);
					if(tstrBuffer[lcv - 1] != _T(':'))
					{
						// store folder
						return FALSE;
					}
				}

				// Succeeded, store in settings
				if(iID == IDC_TVFILEMANAGER1)
					g_csetApplication.lastFolderFileManager1(tstrBuffer);
				else
					g_csetApplication.lastFolderFileManager2(tstrBuffer);					

				if(hTreectrl)
					DeleteAllChildItems(hTreectrl, Selected);

				// get directory listing
				bReturn = getDirectoryListing_TV(tstrBuffer, m_hwndActiveFileManager,
							pllstActive);

				// Copy to lookup list
				copyFileListToLookupList();

				// Select parent folder in list
				/*SendMessage(m_hwndActiveFileManager, LB_SETSEL, 
					(WPARAM)TRUE, (LPARAM)INDEX_PARENTFOLDER);*/

				//pllstActive = m_pllstFileManager1;
			}
		}

		//Parth Software Solution
		TreeView_SelectItem(m_hwndActiveFileManager,Selected);
		//Parth Software Solution
	}
    catch(...)
    {
		// set last error
		m_strLastError = _T("An error occurred while attempting to execute the selected item.");
        
        // set fail value
        bReturn = FALSE;
    }

	// reset last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

HTREEITEM CMainWindow::GetSelectedItem()
{
	int iID = GetDlgCtrlID(m_hwndActiveFileManager);

	Selected=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iID,TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)Selected);

	if(Selected==NULL)
	{
		MessageBox(m_hwndThis,"No Items in TreeView","Error",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return Selected;
}

/**
 * Executes whatever the appropriate, double-click action is for the item 
 * in the active File Manager.
 *
 * @return
 */
BOOL CMainWindow::SortExpandedItems_TV(int iID)
{
	BOOL bReturn = FALSE;
	HWND hTreectrl = GetDlgItem(m_hwndThis, iID);		

	std::vector<tstring> vExpandedItems;
	CWin32TreeView::TreeView_GetAllExpandedItems(m_hwndActiveFileManager, vExpandedItems);		

	//for loop for adding the virtual folders in the list
	std::vector<tstring>::const_iterator itVirtualFolders;
	for( itVirtualFolders = m_vVirtualFolders.begin(); itVirtualFolders !=  m_vVirtualFolders.end(); 
		itVirtualFolders++)
	{
		tstring tsVirtualFolders;
		tsVirtualFolders.assign((*itVirtualFolders).c_str());
		vExpandedItems.push_back( tsVirtualFolders);
	}

	SetPercentage(0);
	SetPercentageRange(vExpandedItems.size());

	std::vector<tstring>::const_iterator it;
	for( it = vExpandedItems.begin(); it !=  vExpandedItems.end(); it++)
	{
		TCHAR tstrBuffer[MAX_PATH] = {0};
		_tcscpy(tstrBuffer, (*it).c_str());
		LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;

		pllstActive = m_pllstFileManager1;//shri
		// get directory listing
		bReturn = getDirectoryListing_TV(tstrBuffer, hTreectrl,
			pllstActive, false, true, true);

		HTREEITEM hTreeItem = CWin32TreeView::GetTreeItemUsingFullPath(m_hwndActiveFileManager, tstrBuffer);

		TreeView_Expand(m_hwndActiveFileManager,hTreeItem, TVM_EXPAND);

		SetPercentage();
	}
	SetPercentage(100);
	return bReturn;
}

/***********************************************************************************
	Function Name:	displaySortedItems_TV
	In Parameters:	HWND hwndOutputControl, 
					LinkedListEx<FILE_INFORMATION> *pllstOutput, tstring strFullPath
	Out Parameters: BOOL
	Description:	Displays the directory listing for the multiple folders using the
					specified control and "active" 
	Date & Time:	5th Dec 2012
	Developer:		Parth Software Solution
***********************************************************************************/
BOOL CMainWindow::displaySortedItems_TV(HWND hwndOutputControl, 
	LinkedListEx<FILE_INFORMATION> *pllstOutput, tstring strFullPath)
{
	static BOOL bDisplayingDirectoryList = FALSE;
	BOOL bReturn = FALSE;	// default to failure val

	try
	{
		WIN32_FIND_DATA *pwfdItem = NULL;
		ACERIGHTS *pacerightsItem = NULL;
		FILETIME ftLocal;
		SYSTEMTIME systLocal;
		NUMBERFMT numfmtItem;
		TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING,
			tstrNumber[80] = EMPTY_STRING;
		long lLongestFileObjectName = 0L;
		tstring strAttributes = EMPTY_STRING,
			strPermissions = EMPTY_STRING;
		BOOL bSortSucceeded = TRUE;
		//Parth Software Solution
		int iDlgCntrlID = GetDlgCtrlID(hwndOutputControl);
		//Parth Software Solution

		// check "working" flag
		if(bDisplayingDirectoryList)
			return TRUE;
		bDisplayingDirectoryList = TRUE;

		// validate handle and list params
		if(pllstOutput == NULL)
			return bReturn;
		if(hwndOutputControl == NULL)
			return bReturn;

		TVINSERTSTRUCT tvinsert;//TV_INSERTSTRUCT
		BOOL retval = FALSE;

		HTREEITEM currRoot = NULL,
			currparent = NULL;
		int iID1 = GetDlgCtrlID(hwndOutputControl);
		if(iID1 == IDC_TVFILEMANAGER1)
		{
			currRoot = Root;
		}

		if(iID1 == IDC_TVFILEMANAGER2)
		{
			currRoot = Root2;
		}
		// creating image list and put it into the tree control
		//====================================================//

		//hImageList=ImageList_Create(16,16,ILC_COLOR16,2,10);					  // Macro: 16x16:16bit with 2 pics [array]
		//hBitMap=LoadBitmap(m_hinstApplication,MAKEINTRESOURCE(IDB_TREE));					  // load the picture from the resource
		//ImageList_Add(hImageList,hBitMap,NULL);								      // Macro: Attach the image, to the image list
		//DeleteObject(hBitMap);													  // no need it after loading the bitmap
		////SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control
		//SendDlgItemMessage(m_hwndThis,iID1,TVM_SETIMAGELIST,0,(LPARAM)hImageList); // put it onto the tree control

		//tvinsert.hParent=NULL;			// top most level no need handle
		//tvinsert.hInsertAfter=TVI_ROOT; // work as root level
		//tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;//|TVI_SORT;

		//tvinsert.item.pszText= TVFOLDER_ROOT;//"My Computer";
		//tvinsert.item.iImage=0;
		//tvinsert.item.iSelectedImage=1;
		////Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
		//currparent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iID1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
		//currRoot=Parent;
		//Before=Parent;                   // handle of the before root
		//tvinsert.hParent=currparent;         // handle of the above data

		// create number format
		memset(&numfmtItem, 0, sizeof(numfmtItem));
		numfmtItem.lpThousandSep = THOUSANDS_SEPARATOR;
		numfmtItem.lpDecimalSep = DECIMAL_SEPARATOR;
		numfmtItem.Grouping = 3;



		// Get longest file object name
		for(long lcv = 0L; lcv < pllstOutput->getLength(); lcv++)
		{
			// Get current file object's information
			pwfdItem = pllstOutput->getEntry(lcv)->pwfdFileInfo;

			// check object's name
			if(lstrlen(pwfdItem->cFileName) > lLongestFileObjectName)
				lLongestFileObjectName = lstrlen(pwfdItem->cFileName);
		}

		// Determine what sort to perform
		//switch(m_aseActiveSort)
		//{
		//case aseNone:
		//	bSortSucceeded = sortFileListByCustom(0, 
		//		m_pllstActiveFileManager->getLength());
		//	break;
		//case aseNameAsc:
		//case aseNameDesc:
		//	SortUsingCriteria(0,m_pllstActiveFileManager->getLength(),m_aseActiveSort);
		//	//bSortSucceeded = sortFileListByName(0, 
		//	//	m_pllstActiveFileManager->getLength());
		//	break;
		//case aseTypeAsc:
		//case aseTypeDesc:
		//	bSortSucceeded = sortFileListByType(0, 
		//		m_pllstActiveFileManager->getLength());
		//	break;

		//case aseSizeAsc:
		//case aseSizeDesc:
		//	bSortSucceeded = sortFileListBySize(0, 
		//		m_pllstActiveFileManager->getLength());
		//	break;

		//case aseDateAsc:
		//case aseDateDesc:
		//	bSortSucceeded = sortFileListByDate(0, 
		//		m_pllstActiveFileManager->getLength());
		//	break;

		//case aseExtensionAsc:
		//case aseExtensionDesc:
		//	bSortSucceeded = sortFileListByExtension(0,
		//		m_pllstActiveFileManager->getLength());
		//	break;

		//	//Parth Software Solution bug fixing
		//case aseDirAsc:
		//case aseDirDesc:
		//	bSortSucceeded = sortFileListByDirOrFileFirst(0,
		//		m_pllstActiveFileManager->getLength());
		//	break;
		//	//Parth Software Solution

		//default:
		//	break;
		//}

		// Determine what sort to perform
		bSortSucceeded = SortUsingCriteria(0,m_pllstActiveFileManager->getLength(),m_aseActiveSort);

		// check sort 
		if(!bSortSucceeded)
			WrappedMessageBox(
			_T("Could not sort the active file list."),
			MAINWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);

		// iterate through file objects
		int iID = GetDlgCtrlID(hwndOutputControl);
		if(iID <= -1L)
			return FALSE;


		HTREEITEM hItemToSort = CWin32TreeView::GetTreeItemUsingFullPath(hwndOutputControl,strFullPath); 
		if(hItemToSort == NULL)
		{
			return false;
		}

		if(hwndOutputControl)
			DeleteAllChildItems(hwndOutputControl, hItemToSort);		

		for(long lcv = 0L; lcv < pllstOutput->getLength(); lcv++)
		{
			// Get current file object's information
			pwfdItem = pllstOutput->getEntry(lcv)->pwfdFileInfo;

			// Get current file object's rights
			pacerightsItem = pllstOutput->getEntry(lcv)->paceFileRights;

			// Make sure this isn't the parent / current directory
			if(pwfdItem != NULL)
			{
				//	 Convert date modified time
				//		For read-only file systems, the last accessed time
				//			will be zero (not present). Use the date created
				//			instead (this is what Windows(r) actually does).
				if(pwfdItem->ftLastAccessTime.dwLowDateTime &&
					pwfdItem->ftLastAccessTime.dwHighDateTime)
					FileTimeToLocalFileTime(&pwfdItem->ftLastAccessTime, &ftLocal);
				else
					FileTimeToLocalFileTime(&pwfdItem->ftCreationTime, &ftLocal);

				//	Convert to system time
				FileTimeToSystemTime(&ftLocal, &systLocal);

				// Create attributes string
				//	 Content Not Indexed
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
					strAttributes = _T("N");
				else
					strAttributes = _T("_");
				//	 Archive
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
					strAttributes += _T("A");
				else
					strAttributes += _T("_");
				//	 Read-only
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					strAttributes += _T("R");
				else
					strAttributes += _T("_");
				//	 Hidden
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
					strAttributes += _T("H");
				else
					strAttributes += _T("_");
				//	 System
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
					strAttributes += _T("S");
				else
					strAttributes += _T("_");
				//	 Compressed
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
					strAttributes += _T("C");
				else
					strAttributes += _T("_");
				//	 Encrypted
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
					strAttributes += _T("E");
				else
					strAttributes += _T("_");

				// Create permissions string
				strPermissions = pacerightsItem->toString();

				//	 Get format for type
				if(pwfdItem->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					_stprintf(tstrBuffer, FORMAT_DIRECTORY, lLongestFileObjectName,
						pwfdItem->cFileName,
						systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
						systLocal.wHour, systLocal.wMinute,
						strAttributes.c_str(),
						strPermissions.c_str());
				}
				else
				{
					// Format file size
					_stprintf(tstrNumber, _T("%u"), pwfdItem->nFileSizeLow);
					GetNumberFormat(LOCALE_USER_DEFAULT, 0, tstrNumber,
						&numfmtItem, tstrBuffer, sizeof(tstrBuffer));
					lstrcpy(tstrNumber, tstrBuffer);

					// create output string
					if(lstrlen(pwfdItem->cFileName))
						_stprintf(tstrBuffer, FORMAT_FILE, tstrNumber, 
						lLongestFileObjectName, pwfdItem->cFileName,
						systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
						systLocal.wHour, systLocal.wMinute,
						strAttributes.c_str(),
						strPermissions.c_str());
				}

				tvinsert.hParent = hItemToSort;
				tvinsert.hInsertAfter=TVI_LAST; 
				tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				tvinsert.item.iImage=0;
				tvinsert.item.iSelectedImage=1;
				tvinsert.item.pszText= tstrBuffer;
				//Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,IDC_TVFILEMANAGER1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
				Parent=(HTREEITEM)SendDlgItemMessage(m_hwndThis,iID,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

			}
		}
		// If we made it here, return success
		bReturn = TRUE;

	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to display the directory listing, an unexpected error occurred.");

		// set fail value
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// clear "working" flag
	bDisplayingDirectoryList = FALSE;

	// return success / fail val
	return bReturn;
}

/***********************************************************************************
	Function Name:	getFileInformation
	In Parameters:	TCHAR *tstrFullpath, HWND hwndOutputControl,LinkedListEx<FILE_INFORMATION> *pllstOutput
	Out Parameters: BOOL
	Description:	Gets the directory listing for the specified folder and then displays it by
					calling displayDirectoryListing().
					returns TRUE if the directory listing is retrieved and the control populated
					successfully, otherwise FALSE
	Date & Time:	19th Oct 2012
	Developer:		Parth Software Solution
***********************************************************************************/
BOOL CMainWindow::getFileInformation(const TCHAR *tstrFullpath, HWND hwndOutputControl,
	LinkedListEx<FILE_INFORMATION> *pllstOutput)
{
	BOOL bReturn = FALSE;	// default to failure val
	CACLInfo *pcaclItem = NULL;
	tstring strBasePath = EMPTY_STRING,
			strFullpath = EMPTY_STRING;
	HANDLE hFolderListing = NULL;
	try
	{
		WIN32_FIND_DATA wfdItem;
		ACERIGHTS aceItem;
		FILE_INFORMATION *pfinfTemp = NULL;

		// validate fullpath and handle params
		if(lstrlen(tstrFullpath) == 0)
			return bReturn;
		if(hwndOutputControl == NULL)
			return bReturn;

		// validate file list param
		if(pllstOutput == NULL)
			return bReturn;

		// Clear File List's contents
		pllstOutput->clear();

		// attempt to get first file/folder
		hFolderListing = FindFirstFile(tstrFullpath, &wfdItem);
		if(hFolderListing == INVALID_HANDLE_VALUE)
		{	
			// reset last error
			m_strLastError = EMPTY_STRING;

			// return success
			return TRUE;
		}

		// attempt to create ACL object
		pcaclItem = new CACLInfo(EMPTY_STRING);

		// Examine returned file system object
		if(wfdItem.cFileName[0] != _T('.'))
		{
			// Get ACL for file/directory
			pcaclItem->setPath((TCHAR *)tstrFullpath);
			pcaclItem->Output(aceItem);

			// Create and add to active File List
			pfinfTemp = new FILE_INFORMATION(wfdItem, aceItem);
			if(pfinfTemp)
			{
				// add to list
				pllstOutput->add(pfinfTemp);

				// de-ref
				pfinfTemp = NULL;
			}
		}
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While attempting to get the directory listing, an unexpected error occurred.");

		// set fail value
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// perform final garbage collection
	if(pcaclItem)
	{
		delete pcaclItem;
		pcaclItem = NULL;
	}

	// return success / fail val
	return bReturn;
}

/***********************************************************************************
	Function Name:	CreateTabPageDialogs
	In Parameters:	void
	Out Parameters: void
	Description:	Create all tab pages dialog at single time
					when program starts.
	Date & Time:	5th Jan 2013
	Developer:		Parth Software Solution
***********************************************************************************/
void CMainWindow::CreateTabPageDialogs()
{
	HWND hWndTabcntrl = GetDlgItem(m_hwndThis, IDC_TAB_CONTROL);

	m_objIntropCom.CreateComcomponant();
	if(!m_objIntropCom.m_pIComIntrop)
	{
		HWND hwnd = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		SendMessage(hwnd, EM_REPLACESEL, (WPARAM)FALSE, 
						(LPARAM)(TCHAR *)m_objIntropCom.strLastError.data());
		return;
	}
	m_objIntropCom.m_pIComIntrop->CreateTabPages((long)hWndTabcntrl);

	//m_pFirstTabDlg = new CFirstTabDialog(hWndTabcntrl);
	//if(m_pFirstTabDlg)
	//{
	//	m_pFirstTabDlg->CreateDialogWindow();
	//}

	//m_pSecondTabDlg = new CSecondTabDialog(hWndTabcntrl);
	//if(m_pSecondTabDlg)
	//{
	//	m_pSecondTabDlg->CreateDialogWindow();
	//	m_pSecondTabDlg->HideDialog();//only 1st dialog at Start
	//}
}

/***********************************************************************************
	Function Name:	ShowActivePage
	In Parameters:	int iPageIndex
	Out Parameters: void
	Description:	To show only active page
	Date & Time:	5th Jan 2013
	Developer:		Parth Software Solution
***********************************************************************************/
void CMainWindow::ShowActivePage(int iPageIndex)
{
	if(!m_objIntropCom.m_pIComIntrop)
	{
		HWND hwnd = GetDlgItem(m_hwndThis, IDC_TXTCMDPROMPTCONSOLE);
		SendMessage(hwnd, EM_REPLACESEL, (WPARAM)FALSE, 
						(LPARAM)(TCHAR *)m_objIntropCom.strLastError.data());
		return;
	}
	m_objIntropCom.m_pIComIntrop->ShowTab(iPageIndex);

	//CoInitialize(NULL);
	//XLanceDLL::IMyClass1Ptr pIMyClass1;
	//HRESULT hRes = pIMyClass1.CreateInstance(__uuidof(XLanceDLL::Class1));
	//if (hRes == S_OK)
	//{
	//	pIMyClass1->ShowTab(iPageIndex);
	//}
	//CoUninitialize();
}

HWND CMainWindow::GetMainWindowHandle()
{
	return m_hwndThis;
}

bool HandleCapturedCommandPromptMessages(bool bIsLoad, char *szData)
{

	HWND hWndTabcntrl = GetDlgItem(pcmwndThis->GetMainWindowHandle(), IDC_TAB_CONTROL);

	bool isMatchFound = true;

	bool bIsAll = true;
	for(unsigned int i = 0 ; i < strlen(szData); i++)
	{
		if(szData[i] == '1')
		{
			bIsAll = false;
		}
	}
    
	isMatchFound = true;		
	if(bIsLoad)
	{
		TabCtrl_DeleteAllItems(hWndTabcntrl);
		if(bIsAll)
		{
			pcmwndThis->InsertTabItem(hWndTabcntrl, ZEROTHTAB, 0);
			pcmwndThis->InsertTabItem(hWndTabcntrl, FIRSTTAB, 1);
			pcmwndThis->InsertTabItem(hWndTabcntrl, SECONDTAB, 2);
			pcmwndThis->InsertTabItem(hWndTabcntrl, THIRDTAB, 3);
		}
		if(szData[0] == '1')
		{
			pcmwndThis->InsertTabItem(hWndTabcntrl, ZEROTHTAB, 0);
		}
		if(szData[1] == '1')
		{
			pcmwndThis->InsertTabItem(hWndTabcntrl, FIRSTTAB, 1);
		}
		if(szData[2] == '1')
		{
			pcmwndThis->InsertTabItem(hWndTabcntrl, SECONDTAB, 2);
		}
		if(szData[3] == '1')
		{
			pcmwndThis->InsertTabItem(hWndTabcntrl, THIRDTAB, 3);
		}
	}
	else
	{
		if(bIsAll)
		{
			TabCtrl_DeleteAllItems(hWndTabcntrl);
		}
		if(szData[0] == '1')
		{
			pcmwndThis->RemoveTabItem(hWndTabcntrl, 0);
		}
		if(szData[1] == '1')
		{
			pcmwndThis->RemoveTabItem(hWndTabcntrl, 1);
		}
		if(szData[2] == '1')
		{
			pcmwndThis->RemoveTabItem(hWndTabcntrl, 2);
		}
		if(szData[3] == '1')
		{
			pcmwndThis->RemoveTabItem(hWndTabcntrl, 3);
		}
	}
 
	//switch(iCmdID)
	//{
	//case 0:
	//	isMatchFound = false;
	//case XLV + LOAD + ALL +  W:
	//case XLV + LOAD +  ALL + F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, ZEROTHTAB, 0);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, FIRSTTAB, 1);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, SECONDTAB, 2);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, THIRDTAB, 3);
	//	break;
	//case XLV + UNLOAD + ALL + W:
	//case XLV + UNLOAD + ALL + F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	break;
	//case XLV + LOAD + TABZERO + W:
	//case XLV + LOAD + TABZERO + F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, ZEROTHTAB, 0);
	//	break;
	//case XLV + LOAD + TABONE+ W:
	//case XLV + LOAD + TABONE+ F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, FIRSTTAB, 0);
	//	break;
	//case XLV + LOAD + TABTWO+ W:
	//case XLV + LOAD + TABTWO+ F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, SECONDTAB, 1);
	//	break;
	//case XLV + LOAD + TABTHREE+ W:
	//case XLV + LOAD + TABTHREE+ F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, THIRDTAB, 2);
	//	break;
	//case XLV + LOAD + TABONE+  TABTWO +  W:
	//case XLV + LOAD + TABONE+  TABTWO +  F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, FIRSTTAB, 0);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, SECONDTAB, 1);
	//	break;
	//case XLV + LOAD + TABONE+  TABTHREE +  W:
	//case XLV + LOAD + TABONE+  TABTHREE +  F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, FIRSTTAB, 0);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, THIRDTAB, 2);
	//	break;
	//case XLV + LOAD + TABTWO +  TABTHREE +  W:
	//case XLV + LOAD + TABTWO+  TABTHREE +  F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, SECONDTAB, 1);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, THIRDTAB, 2);
	//	break;
	//case XLV + LOAD +  TABONE + TABTWO + TABTHREE + W:
	//case XLV + LOAD +  TABONE + TABTWO + TABTHREE + F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, FIRSTTAB, 0);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, SECONDTAB, 1);
	//	pcmwndThis->InsertTabItem(hWndTabcntrl, THIRDTAB, 2);
	//	break;
	//case XLV + UNLOAD + TABONE+ W:
	//case XLV + UNLOAD + TABONE+ F:
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 0);
	//	break;
	//case XLV + UNLOAD + TABTWO+ W:
	//case XLV + UNLOAD + TABTWO+ F:
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 1);
	//	break;
	//case XLV + UNLOAD + TABTHREE+ W:
	//case XLV + UNLOAD + TABTHREE+ F:
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 2);
	//	break;
	//case XLV + UNLOAD + TABONE+  TABTWO +  W:
	//case XLV + UNLOAD + TABONE+  TABTWO +  F:
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 0);
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 1);
	//	break;
	//case XLV + UNLOAD + TABONE+  TABTHREE +  W:
	//case XLV + UNLOAD + TABONE+  TABTHREE +  F:
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 0);
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 3);
	//	break;
	//case XLV + UNLOAD + TABTWO +  TABTHREE +  W:
	//case XLV + UNLOAD + TABTWO +  TABTHREE +  F:
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 1);
	//	pcmwndThis->RemoveTabItem(hWndTabcntrl, 2);
	//	break;
	//case XLV + UNLOAD + TABONE+ TABTWO + TABTHREE + W:
	//case XLV + UNLOAD + TABONE+ TABTWO + TABTHREE + F:
	//	TabCtrl_DeleteAllItems(hWndTabcntrl);
	//	break;
	//default:
	//	isMatchFound = false;
	//}

	//HWND hWnd = GetDlgItem(pcmwndThis->GetMainWindowHandle(), IDC_TXTCMDPROMPT);
	//if(hWnd)
	//	SetFocus(hWnd);

	SendMessage(pcmwndThis->GetMainWindowHandle(), WM_SIZE, 0, 0);
	return isMatchFound;
}

int GetMainWindowMode()
{
	if(IsMaximized(pcmwndThis->GetMainWindowHandle()))
		return SW_MAXIMIZE;
	else
		return SW_NORMAL;
}

bool CMainWindow::StartNamedPipeThread()
{
	m_pObjXlvCommunicatorServer = new CXlvCommunicatorServer(_NAMED_PIPE_CLIENT, CMainWindow::OnScanDataReceivedCallBack, sizeof(XLV_PIPE_DATA_REG));
	m_pObjXlvCommunicatorServer->Run();
	return true;
}

/*--------------------------------------------------------------------------------------
Function       : OnScanDataReceivedCallBack
In Parameters  : LPVOID lpParam, 
Out Parameters : void 
Description    : Call back for data recive from backend communicator.
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CMainWindow::OnScanDataReceivedCallBack(LPVOID lpParam)
{
	OutputDebugString(">>> CMainWindow::OnScanDataReceivedCallBack");
	__try
	{
		LPXLV_PIPE_DATA_REG sXlvPipeData = (XLV_PIPE_DATA_REG*)lpParam;
		if(sXlvPipeData)
		{
			if(sXlvPipeData->eMessageInfo == 100)
			{
			 	pcmwndThis->SendBackDatatoServer();
			}
			else if(sXlvPipeData->eMessageInfo == 102)
			{
				pcmwndThis->AddVirtualFolder(sXlvPipeData->bData);
			}
			else if(sXlvPipeData->eMessageInfo == 103)
			{
				pcmwndThis->RemoveVirtualFolder(sXlvPipeData->bData);
			}
			else if(sXlvPipeData->eMessageInfo == 104)
			{
				pcmwndThis->ShowProgressDetails(sXlvPipeData->bData);
			}
		}
	}
__except(0)
	{
	}
}

void CMainWindow::SendBackDatatoServer()
{
	XLV_PIPE_DATA_REG sXlvPipeData = {0};
	sXlvPipeData.eMessageInfo = 100;

	char *szData = GetSlectedItemsPath();
	memcpy(&sXlvPipeData.bData, szData, strlen(szData) );
	CXlvCommunicator objXlvCommunicator(_NAMED_PIPE_SERVER, true);
	objXlvCommunicator.SendData(&sXlvPipeData, sizeof(XLV_PIPE_DATA_REG));

	//new list is loaded need to clear virtual folder vector
	ClearVirtualFolderList();
}

char * CMainWindow::GetSlectedItemsPath()
{
	char * szTokenizedString = new char[50000];
	strcpy(szTokenizedString, "");

	std::vector<tstring> vSelectedPaths;
	CWin32TreeView::TreeView_GetAllSelectedItemsPath(m_hwndActiveFileManager, vSelectedPaths);
	for(unsigned int iIndex = 0; iIndex < vSelectedPaths.size(); iIndex++)
	{
		tstring str = vSelectedPaths.at(iIndex).c_str();
		StringTrim(str);
		strcat(szTokenizedString, str.c_str());
		strcat(szTokenizedString, ";");
	}	
	return szTokenizedString;
}

BOOL SetWindowPosition (HWND hwndChild, RECT *lpRect)
{
    //RECT    rChild;
    //int     wChild, hChild;
    //int     xNew, yNew;
 
    //// Get the Height and Width of the child window
    //GetWindowRect (hwndChild, &rChild);
    //wChild = rChild.right - rChild.left;
    //hChild = rChild.bottom - rChild.top;
 
    //// Calculate new X position, then adjust for workarea
    //xNew = wChild + 10;
 
    //// Calculate new Y position, then adjust for workarea
    //yNew = hChild + 10;
 
    // Set it, and return
 //   SetWindowPos (hwndChild, NULL, 40, 20, 10, 10, SWP_NOSIZE | SWP_NOZORDER);

	MoveWindow(hwndChild, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, TRUE);
	return 0;
}

BOOL CALLBACK EnumChildProcedure(HWND hWnd,LPARAM lParam)
{
	char name[256] = {0}; 
	GetWindowText(hWnd,name,256);

	char ClassName[256] = {0};
	GetClassName(hWnd,ClassName,256);

	if((strcmp(ClassName,"WindowsForms10.Window.8.app.0.378734a")==0) && 
		(	(strcmp(name,"ZeroPageForm")==0) ||
			(strcmp(name,"FirstPageForm")==0) ||
			(strcmp(name,"SecondPageForm")==0)||
			(strcmp(name,"ThirdPageForm")==0)
		))
	{
		RECT *lpRect = (RECT*)lParam;
		SetWindowPosition(hWnd, lpRect);
	}
	if(name==NULL)
		return FALSE;
	return TRUE;
}

bool CMainWindow::SetTabControlwindowPostion(int iLeft, int iTop, int iRight, int iBottom)
{
	RECT lpRect;
	lpRect.left = iLeft;
	lpRect.top = iTop;
	lpRect.right = iRight;
	lpRect.bottom = iBottom;

	HWND hWnd = NULL;
	hWnd = ::FindWindow(NULL,"XLance View");
	if(!hWnd)
	{
		//"Status : XLance Window not found";
		return false;
	}
    EnumChildWindows(hWnd,EnumChildProcedure,(LPARAM)&lpRect);
			
	return true;
}

void CMainWindow::AddVirtualFolder(BYTE *bytes)
{
	if(!bytes)
		return;

	char szBufferString[200] = {0};
	char szBasePath[200] = {0};
	char szItemPath[200] = {0};
	tstring strBasePath = EMPTY_STRING;
	strcpy(szBufferString, (char*)bytes);

	//char *lpItempath = strrchr(szBufferString, '\\') + 1;

	//if(!lpItempath)
	//	return;

	strcpy(szItemPath, szBufferString);
	//lpItempath--;
	//lpItempath[0] = '\0';

	//strcpy(szBasePath, szBufferString);	

	m_vVirtualFolders.push_back(szItemPath);

	ExpandAllVirtualFolders();

	//InsertItemInTreeControl(szBasePath, szItemPath);
}

void CMainWindow::ShowProgressDetails(BYTE *bytes)
{
	//if(!bytes)
	//	return;
	HWND hwndTemp = NULL;
	tstring strTemp = EMPTY_STRING;
	char szBufferString[200] = {0};
	char szBasePath[200] = {0};
	strcpy(szBufferString, (char*)bytes);
	strcpy(szBasePath, szBufferString);
	hwndTemp = GetDlgItem(pcmwndThis->m_hwndThis, 
					IDC_TXTCMDPROMPTCONSOLE);
	//strTemp = "abc\r\n";

	// Move to end of current text
	long lReturn = 0L;
	lReturn = SendMessage(hwndTemp, WM_GETTEXTLENGTH, (WPARAM)0, 
				(LPARAM)0);
	SendMessage(hwndTemp, EM_SETSEL, (WPARAM)lReturn, 
		(LPARAM)lReturn);
	SendMessage(hwndTemp, EM_REPLACESEL, (WPARAM)strTemp.length(), 
			(LPARAM)(TCHAR *)bytes);

}

void CMainWindow::RemoveVirtualFolder(BYTE *bytes)
{
	if(!bytes)
		return;

	char szBufferString[200] = {0};
	char szBasePath[200] = {0};
	char szItemPath[200] = {0};
	tstring strBasePath = EMPTY_STRING;
	strcpy(szBufferString, (char*)bytes);

	//char *lpItempath = strrchr(szBufferString, '\\') + 1;

	//if(!lpItempath)
	//	return;

	strcpy(szItemPath, szBufferString);
	//lpItempath--;
	//lpItempath[0] = '\0';

	//strcpy(szBasePath, szBufferString);	
	//for loop for adding the virtual folders in the list
	bool bFound = false;
	std::vector<tstring>::const_iterator itVirtualFolders;
	for( itVirtualFolders = m_vVirtualFolders.begin(); itVirtualFolders !=  m_vVirtualFolders.end(); 
		itVirtualFolders++)
	{
		tstring tsVirtualFolders;
		tsVirtualFolders.assign((*itVirtualFolders).c_str());
		if(tsVirtualFolders.compare(szItemPath) == 0)
		{
			bFound = true;
			break;
		}
	}
	if(bFound)
		m_vVirtualFolders.erase(itVirtualFolders);

	ExpandAllVirtualFolders();
	//InsertItemInTreeControl(szBasePath, szItemPath);
}

bool CMainWindow::InsertItemInTreeControl(LPSTR lpParentPath, LPSTR lpNewItemName)
{
	LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;
	WIN32_FIND_DATA wfdItem = {0};
	ACERIGHTS aceItem;
	FILE_INFORMATION *pfinfTemp = NULL;
	bool bReturn = false;

	// get active File Manager's ID
	int iCtrlID = GetDlgCtrlID(pcmwndThis->m_hwndActiveFileManager);

	// Determine which file list should be used
	if(iCtrlID == IDC_LSTFILEMANAGER1)
		pllstActive = m_pllstFileManager1;
	else
		pllstActive = m_pllstFileManager2;

	//// get directory listing
	bReturn = getDirectoryListing_TV(lpParentPath, pcmwndThis->m_hwndActiveFileManager,
		pllstActive, false, true, true);

	// validate file list param
	if(pllstActive == NULL)
		return bReturn;

	wfdItem.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
	strcpy(wfdItem.cFileName, lpNewItemName);

	// Examine returned file system object
	if(wfdItem.cFileName[0] != _T('.'))
	{
		// Create and add to active File List
		pfinfTemp = new FILE_INFORMATION(wfdItem, aceItem);
		if(pfinfTemp)
		{
			// add to list
			pllstActive->add(pfinfTemp);

			// de-ref
			pfinfTemp = NULL;
		}
	}
	pcmwndThis->m_pllstActiveFileManager = pllstActive;

	// Determine which file list should be used
	if(iCtrlID == IDC_LSTFILEMANAGER1)
		m_pllstFileManager1 = pllstActive ;
	else
		m_pllstFileManager2 = pllstActive;

	m_aseActiveSort = aseCustom;

	// If we made it here, call display and return it's return value
	bReturn = displayDirectoryListing_TV(pcmwndThis->m_hwndActiveFileManager, pllstActive);

	//WIN32_FIND_DATA *pwfdItem = NULL;

	//TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;
	//tstring strAttributes = "_______",
	//	strPermissions = "   -   -   -   ";
	//long lLongestFileObjectName = 0L;
	//SYSTEMTIME systLocal;
	//FILETIME ftLocal;

	////if(!getDirectoryListing_TV(lpParentPath,
	////	m_hwndActiveFileManager, m_pllstFileManager1))
	////	displayDirectoryListing_TV( m_hwndActiveFileManager, m_pllstFileManager1 ); 


	//HTREEITEM hItemParent = CWin32TreeView::GetTreeItemUsingFullPath(pcmwndThis->m_hwndActiveFileManager, lpParentPath);

	//if(!hItemParent)
	//	return false;

	//// Get longest file object name
	//for(long lcv = 0L; lcv < m_pllstFileManager1 ->getLength(); lcv++)
	//{
	//	// Get current file object's information
	//	pwfdItem = m_pllstFileManager1 ->getEntry(lcv)->pwfdFileInfo;

	//	// check object's name
	//	if(lstrlen(pwfdItem->cFileName) > lLongestFileObjectName)
	//		lLongestFileObjectName = lstrlen(pwfdItem->cFileName);
	//}

	//GetSystemTime(&systLocal);              // gets current time

	////	Convert to system time
	//SystemTimeToFileTime(&systLocal, &ftLocal);

	//_stprintf(tstrBuffer, FORMAT_DIRECTORY, lLongestFileObjectName,
	//	lpNewItemName,
	//	systLocal.wDay, systLocal.wMonth, systLocal.wYear, 
	//	systLocal.wHour, systLocal.wMinute,
	//	strAttributes.c_str(),
	//	strPermissions.c_str());

	//TVINSERTSTRUCT tvinsert;//TV_INSERTSTRUCT
	//tvinsert.hParent = hItemParent;
	//tvinsert.hInsertAfter=TVI_LAST; 
	//tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	//tvinsert.item.iImage=0;
	//tvinsert.item.iSelectedImage=1;
	//tvinsert.item.pszText= tstrBuffer;
	//int iDlgCntrlID = GetDlgCtrlID(pcmwndThis->m_hwndActiveFileManager);
	//SendDlgItemMessage(m_hwndThis, iDlgCntrlID, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
	return bReturn;
}

void CMainWindow::ExpandAllVirtualFolders()
{
	//for loop for adding the virtual folders in the list
	std::vector<tstring>::const_iterator itVirtualFolders;
	for( itVirtualFolders = m_vVirtualFolders.begin(); itVirtualFolders !=  m_vVirtualFolders.end(); 
		itVirtualFolders++)
	{
		tstring tsVirtualFolders;
		tsVirtualFolders.assign((*itVirtualFolders).c_str());
		ParseVirtualString(tsVirtualFolders);
	}	
}

bool CMainWindow::ParseVirtualString(tstring tsVirtualFolders)
{

	if(tsVirtualFolders.length() <= 0)
		return false;

	bool bReturn = false;
	char sztempStr[500] = {0};
	strcpy(sztempStr, tsVirtualFolders.c_str());
	char szFullPath[100] = {0};
	LinkedListEx<FILE_INFORMATION> *pllstActive = NULL;

	m_aseActiveSort = aseAtrrAsc;

	char * pch;
	pch = strtok (sztempStr,"\\");
	while (pch != NULL)
	{
		strcat(szFullPath, pch);

		TCHAR tcFullPath[100] = {0};
		//AToWChar(szFullPath, tcFullPath);
		pllstActive = m_pllstFileManager1;//shri

		if(!CWin32TreeView::TreeView_ISExpandedItem(m_hwndActiveFileManager, szFullPath))
		{
			// get directory listing
			if(getDirectoryListing_TV(szFullPath, m_hwndActiveFileManager,
				pllstActive, false, true, true))
			{
				HTREEITEM hTreeItem = CWin32TreeView::GetTreeItemUsingFullPath(m_hwndActiveFileManager, szFullPath);
				TreeView_Expand(m_hwndActiveFileManager,hTreeItem, TVM_EXPAND);
			}
		}
		strcat(szFullPath, "\\");
		pch = strtok (NULL, "\\");
	}
	return bReturn;
}

void CMainWindow::ClearVirtualFolderList()
{
	m_vVirtualFolders.clear();
}