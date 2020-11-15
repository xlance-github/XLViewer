///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor      
// Purpose:   CFirstTabDialog object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "FirstTabDialog.h"
#include "..\Resource\Resource.h"
#include "..\XLanceView.h"

//#ifdef _DEBUG
//#import "..\\Debug\\XLanceDLL.tlb" 
//#else
//#import "..\\Release\\XLanceDLL.tlb" 
//#endif

// Module Level Vars
static CFirstTabDialog *pcabtdlgThis = NULL;

void LoadHelloWorldClass(char *szString) 
{
	if(!szString)
		return; 
	//Create object of C# assembly here..
	//CoInitialize(NULL);
	//XLanceDLL::IMyClass1Ptr pIMyClass1;
	//HRESULT hRes = pIMyClass1.CreateInstance(__uuidof(XLanceDLL::IMyClass1Ptr));
	//if (hRes == S_OK)
	//{
	//	pIMyClass1->helloWorld();
	//}
	//CoUninitialize();
}


/**
 * Default constructor, initializes all fields to their defaults.
 */
CFirstTabDialog::CFirstTabDialog()
{
	// initialize fields to their defaults
	m_hParent = NULL;
	m_hwndThis = NULL;
	//m_hbrBackground = NULL;
	//m_hfontControls = NULL;
	m_strLastError = EMPTY_STRING;
	// set module static so message loop can have access to *this*
	pcabtdlgThis = this;
}

/**
 * Constructor which accepts the application HINSTANCE as an argument.
 *
 * @param hInstance
 *
 */
CFirstTabDialog::CFirstTabDialog(HWND hParent)
{
	try
	{
		// initialize fields 
		m_strLastError = EMPTY_STRING;
		//m_hbrBackground = NULL;
		//m_hfontControls = NULL;
		m_hParent = hParent;

		// set module static so message loop can have access to *this*
		pcabtdlgThis = this;
	}
	catch(...)
	{
		// garbage collect, clear references
		m_hParent = NULL;
		if(m_hwndThis)
		{
			DestroyWindow(m_hwndThis);
			m_hwndThis = NULL;
		}

		// set last error
		m_strLastError = _T("While intializing the startup dialog object, an unspecified exception occurred.");
	}
}
                                 
/**
 * Destructor, performs clean-up on fields.
 */
CFirstTabDialog::~CFirstTabDialog()
{
	// destroy created GDI objects
	//if(m_hfontControls)
		//DeleteObject(m_hfontControls);
	//if(m_hbrBackground)
		//DeleteObject(m_hbrBackground);

	// destroy window object
	if(m_hwndThis)
		DestroyWindow(m_hwndThis);
}

/**
 * Handles the processing of all messages for *this* object's GUI
 * (dialog).
 *
 * @param hwnd
 *
 * @param uMsg
 *
 * @param wParam
 *
 * @param lParam
 *
 * @return 0 if the message is processed by this message pump, 
 * otherwise DefWindowProc() is called and its return value used.
 */
LRESULT CALLBACK CFirstTabDialog::WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	HWND hwndTemp = NULL;
	HDC hdcTemp = NULL;
	BOOL bReturn = FALSE;
	static BOOL bStartupComplete = FALSE;
	int iReturn = 0,
		iCtrlID = 0;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			// set handle
			pcabtdlgThis->m_hwndThis = hwnd;

			// load this dialog's preferences
			//pcabtdlgThis->loadPreferences();

			// set icon
			pcabtdlgThis->loadIcon();


			pcabtdlgThis->InitializeDialog();
			// set font for controls
			//pcabtdlgThis->setControlFonts();

			// load help text
			//pcabtdlgThis->loadAboutText();

			pcabtdlgThis->refreshLayout(TRUE);

			// Register escape key
			RegisterHotKey(hwnd, HOTKEY_ESCAPE, (UINT)0, VK_ESCAPE);
			break;			

		//case WM_CTLCOLORLISTBOX:
		//case WM_CTLCOLORSTATIC:
		//case WM_CTLCOLORDLG:
		//	// get DC
		//	hdcTemp = (HDC)wParam;

		//	// validate DC, continue
		//	if(hdcTemp)
		//	{
		//		// create black brush
		//		if(pcabtdlgThis->m_hbrBackground == NULL)
		//			pcabtdlgThis->m_hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

		//		if(wParam == WM_CTLCOLORSTATIC)
		//			SetBkMode(hdcTemp, TRANSPARENT);
		//		
		//		// Set background color for text
		//		SetBkColor(hdcTemp, RGB(0, 0, 0));
		//		
		//		// set text color
		//		SetTextColor(hdcTemp, RGB(255, 255, 255));
		//		
		//		// return brush
		//		return (LRESULT)pcabtdlgThis->m_hbrBackground;
		//	}
		//	break;

		case WM_DESTROY:
			// save preferences for this dialog ALWAYS
			//pcabtdlgThis->savePreferences();

			// clear handle because window is being closed manually.
			pcabtdlgThis->m_hwndThis = NULL;

			// Unregister Escape key
			UnregisterHotKey(hwnd, HOTKEY_ESCAPE);
			break;

		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
			break;
	
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_CMDABOUT:
					break;
				case IDC_BUTTON_ADD:
					pcabtdlgThis->OnBnClickedAdd();
					break;
				default:	// do nothing
					break;
			}
			break;

		case WM_HOTKEY:		
			EndDialog(hwnd, IDCANCEL);
			break;

		case WM_SHOWWINDOW:
			// remove "auto" selection
			//hwndTemp = GetDlgItem(hwnd, IDC_TXTABOUT);

			// validate, continue
			//if(hwndTemp != NULL)
				// clear selection and move caret to start of text
			//	SendMessage(hwndTemp, EM_SETSEL, (WPARAM)0, 0L);
			break;

		default:
			return 0;//DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	// return "message processed"
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Creates and displays this object's dialog. This object represents a non-
 * modal dialog which contains the settings for the application.
 *
 * @return TRUE if no errors occur, otherwise FALSE
 */
int CFirstTabDialog::CreateDialogWindow()
{
    int iReturn = 0;	// default to pesimistic return val

    try
    {
	   m_hwndThis = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FIRST_TABDIALOG), 
	   /*handle of the tab control*/ m_hParent, (DLGPROC)WindowProc);
	   ShowWindow(m_hwndThis, SW_SHOW);
    }
    catch(...)
    {
        // set fail value
        iReturn = 0;
    }

    // return success / fail val
    return iReturn;
}
/**
 * Loads the icon for this window at startup.
 *
 * @param hwndThis HWND of this object's GUI (window)
 *
 * @return TRUE if no errors occur, otherwise FALSE
 *
 */
BOOL CFirstTabDialog::loadIcon()
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
		if(m_hParent == NULL)
			return FALSE;

		// attempt to load icon
		hiconThis = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON48X48));
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
        m_strLastError = EMPTY_STRING;

        // set fail value
        bReturn = FALSE;
    }

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

    // return success / fail val
    return bReturn;
}

HWND CFirstTabDialog::GetWindowHandle()
{
	return m_hwndThis;
}

/**
 * Calculates control sizes and sets the graphics device mode (if applicable)
 * selected by the user.
 *
 * @return TRUE if the layout is intialized and no errors occur, otherwise
 * FALSE.
 */ 
BOOL CFirstTabDialog::refreshLayout(BOOL bAsOnStart)
{
	RECT rectThis;
	GetClientRect(m_hwndThis, &rectThis);

	if(bAsOnStart)
	{
		//move the window slight down its manaul adjustment
		MoveWindow(m_hwndThis, 1, 22, rectThis.right - rectThis.left, rectThis.bottom - rectThis.top, TRUE);
	}

	return FALSE;
}

void CFirstTabDialog::ShowDialog()
{
	ShowWindow(m_hwndThis, SW_SHOW);
}

void CFirstTabDialog::HideDialog()
{
	ShowWindow(m_hwndThis, SW_HIDE);
}

void CFirstTabDialog::InitializeDialog()
{
	char szString[MAX_PATH] = {0};
	try
	{
		LoadHelloWorldClass(szString);
	}
	catch(...)
	{
		m_strLastError = _T("Error occured accesing LoadHelloWorldClass");
	}
	if(strlen(szString) > 0)
	{
		SetWindowText(GetDlgItem(m_hwndThis, IDC_STATIC_EXAMPLE), szString);
		ShowWindow(GetDlgItem(m_hwndThis, IDC_STATIC_EXAMPLE), SW_SHOW);
	}
	else
	{
		ShowWindow(GetDlgItem(m_hwndThis, IDC_STATIC_EXAMPLE), SW_HIDE);
	}
}

void CFirstTabDialog::OnBnClickedAdd()
{
	//int  iInputOne = 0, iInputTwo = 0, iResult = 0;
	//char szInputOne[10] = {0}, szInputTwo[10] = {0},
	//	 szResult[10] = {0};

	//GetWindowText(GetDlgItem(m_hwndThis, IDC_EDIT_INPUT_1), szInputOne, 10);
	//GetWindowText(GetDlgItem(m_hwndThis, IDC_EDIT_INPUT_2), szInputTwo, 10);
	//if(strlen(szInputOne) > 0 && strlen(szInputTwo) > 0 )
	//{
	//	try
	//	{
	//		iInputOne = atoi(szInputOne);
	//		iInputTwo = atoi(szInputTwo);
	//		CoInitialize(NULL);
	//		XLanceDLL::IMyClass1Ptr pIMyClass1;
	//		HRESULT hRes = pIMyClass1.CreateInstance(__uuidof(XLanceDLL::Class1));
	//		if (hRes == S_OK)
	//		{
	//			iResult = pIMyClass1->AddIntegers(iInputOne, iInputTwo);
	//			sprintf(szResult, "%d", iResult);
	//			SetWindowText(GetDlgItem(m_hwndThis, IDC_EDIT_OUTPUT_1), szResult);
	//		}
	//		else
	//		{
	//			char szString[50] = {0};
	//			ErrorDescription(hRes, szString);
	//			SetWindowText(GetDlgItem(m_hwndThis, IDC_EDIT_OUTPUT_1), szString);
	//		}
	//		CoUninitialize();
	//	}
	//	catch(...)
	//	{
	//		MessageBox(NULL, "Exception Occuered CFirstTabDialog::OnBnClickedAdd", "Error", MB_ICONERROR);
	//	}

	//}
	//else
	//{
	//	MessageBox(NULL, "Please Enter Input", "Error", MB_ICONERROR);
	//}
}