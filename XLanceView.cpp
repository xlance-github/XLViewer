/******************************************************************************/
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   
//
//
//
// Date:      
//            
// NOTES:     
/******************************************************************************/
#include <stdafx.h>
#include "XLanceView.h"
#include "Dialogs\CStartupDialog.h"
#include "Dialogs\CMainWindow.h"
#include "Resource\Resource.h"
#include "Settings\CSettings.h"
#include "Utility\CGraphicsDeviceInformation.h"
#include "Splitter\easysplit.h"

// Leave out for now... this should enable theme support.
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' \
version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

////////////////////////////////////////////////////////////////////////////////
// Module constants
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////
HWND	g_hwndApplication = NULL;
HACCEL	g_hacclApplication = NULL;
CSettings g_csetApplication;
CGraphicsDeviceInformation g_cginfPrimaryDevice;
HINSTANCE hAppInstance;
FILE* log_file;

////////////////////////////////////////////////////////////////////////////////
// Function forward declares
////////////////////////////////////////////////////////////////////////////////
//LRESULT CALLBACK appWndProc				(HWND, UINT, WPARAM, LPARAM);
//DWORD			 GetScreenDimensions	(VOID);
//BOOL             InitApp				(HINSTANCE, HINSTANCE, LPSTR, int);
//VOID			 CleanupAndPostQuit		(VOID);


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/**
 *
 *
 *
 *
 *
 */
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpszCmdLine, int intCmdShow)
{
	CStartupDialog *dlgStartup = NULL;	
	CMainWindow *dlgMainWindow = NULL;
	MSG msg;	
	int iReturn = IDCANCEL;

  hAppInstance = hInstance;
  RegisterEasySplit(hAppInstance);

  LoadLibrary(_T("riched20.dll"));  //Manually?

#ifdef _DEBUG
  log_file = fopen("log.txt", "wt");
  fprintf(log_file, "Vectical splitter style in hex: %x\n", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_VERT | ES_LINE | ES_TRACK | ES_DOCK);
  fprintf(log_file, "Horizontal splitter style in hex: %x\n", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_HORZ | ES_LINE | ES_TRACK | ES_DOCK);
#endif

	try
	{
		// Initialize message to default
		msg.wParam = 1;
		
		// create startup dialog
		dlgStartup = new CStartupDialog(hInstance);

		// display dialog
		if(dlgStartup)
		{
			// display startup dialog modally
			iReturn = dlgStartup->show();

			// destroy startup dialog
			delete dlgStartup;
			dlgStartup = NULL;
		}

		if(iReturn == IDOK)
		{
			// Create main window
			dlgMainWindow = new CMainWindow(hInstance);

			// validate, display... and let the fun begin
			if(dlgMainWindow != NULL)
			{
				g_hacclApplication = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_MAIN));
				dlgMainWindow->show(g_csetApplication.alwaysLaunchFullScreen());
			}

			// Application loop. Continues until it receives the WM_QUIT message
			for(;;)
			{
				if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // check message queue for WM_QUIT
				{
					if(msg.message == WM_QUIT)				 // received, so exit
						break;

					if(!TranslateAccelerator(g_hwndApplication, g_hacclApplication, &msg))
					{
						TranslateMessage(&msg);				 // translate accelerator keys
						DispatchMessage(&msg);				 // process accelerator keys
					}
				}
				else
					WaitMessage();							 // no messages being received, wait
			}
		}
	}
	catch(...)
	{

	}

	// perform final garbage collection
	if(dlgStartup)
		delete dlgStartup;
	if(dlgMainWindow)
		delete dlgMainWindow;
	if(g_hacclApplication)
		DestroyAcceleratorTable(g_hacclApplication);

#ifdef _DEBUG
  fclose(log_file);
#endif

	// return exit code
	return msg.wParam;			// should be the value of the WM_QUIT message's wParam
}

///******************************************************************************/
//// Author(s): Chad R. Hearn
//// Legal:     ©2001 Chad R. Hearn (BanderDach)
//// Purpose:   Application Window's message handler. All messages received by the
////			  window are routed through here and then either processed locally, 
////			  or sent to the default Windows message handler (DefWindowProc). A
////			  message handler should return zero when it handles a message.
////            
//// Date:      03/27/2001
////            
//// NOTES:     
///******************************************************************************/
//LRESULT CALLBACK appWndProc (HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
//{
//	PAINTSTRUCT ps;
//
//	switch(uMsg)
//	{
//		case WM_DESTROY:
//			CleanupAndPostQuit();
//			break;
//		
//		// other messages go here	
//
//		case WM_PAINT:
//			if(!IsIconic(m_hwndApplication))
//			{
//				BeginPaint(m_hwndApplication, &ps);
//				// 
//				EndPaint(m_hwndApplication, &ps);
//			}
//			break;
//
//		default:
//			return DefWindowProc(hwnd, uMsg, wparam, lparam);
//	}
//	
//	return 0L;
//}
//
///**
// * Creates and register the application class and creates and shows
// * the default window.
// *
// *
// *
// *
// *
// */
//BOOL InitApp (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpszCmdLine, int intCmdShow)
//{
//	WNDCLASS me;
//	DWORD dwScreenDim;
//
//	me.hInstance		= hInst;
//	me.lpszClassName	= (LPSTR)APPCLASS;
//	me.lpszMenuName		= NULL;		// MAKEINTRESOURCE(YOUR_MENU_RESOURCE_NAME);
//	me.style			= CS_VREDRAW | CS_HREDRAW;
//	// Loads the default Windows application icon
//	me.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
//	// me.hIcon			= LoadIcon(hInst, MAKEINTRESOURCE(YOUR_ICON_RESOURCE_NAME));
//	me.hCursor			= LoadCursor(NULL, IDC_ARROW);
//	me.hbrBackground	= (HBRUSH)GetStockObject(LTGRAY_BRUSH);	// Battleship gray form...
//	me.lpfnWndProc		= (WNDPROC)appWndProc;
//	me.cbClsExtra		= 0;
//	me.cbWndExtra		= sizeof(DWORD);
//	
//	if(!RegisterClass(&me))
//		return FALSE;
//
//	g_hacclApplication = LoadAccelerators(hInst, MAKEINTRESOURCE(IDA_MAIN));
//	
//	// validate
//	if(g_hacclApplication == NULL)
//	{
//		UnregisterClass(APPCLASS, hInst);		// unregister class before exiting...
//		return FALSE;
//	}
//	
//	// If you want to center the window on the screen, unremark the following lines of code
//	dwScreenDim = GetScreenDimensions();
//
//	m_hwndApplication = CreateWindow((LPCTSTR)APPCLASS, (LPCTSTR)MAINWINDOW_TITLE, 
//							WS_OVERLAPPEDWINDOW, 
//	      					(HIWORD(dwScreenDim) - 1024) / 2,	// set left
//			    			(LOWORD(dwScreenDim) - 768) / 2,	// set top
//				    		1024, 768, NULL, NULL, hInst, NULL);
//
//	if(!m_hwndApplication)
//	{
//		DestroyAcceleratorTable(g_hacclApplication);	// destroy accelerator table
//		UnregisterClass(APPCLASS, hInst);				// unregister class before exiting...
//		return FALSE;
//	}
//
//	//ShowWindow(m_hwndApplication, intCmdShow);
//
//	//if(!UpdateWindow(m_hwndApplication))				// could not update window, so
//	//{
//	//	DestroyAcceleratorTable(g_hacclApplication);	// destroy accelerator table
//	//	UnregisterClass(APPCLASS, hInst);				// unregister class before exiting
//	//	m_hwndApplication = NULL;
//	//	g_hacclApplication = NULL;
//	//	return FALSE;
//	//}
//	
//	return  TRUE;
//}
//
///******************************************************************************/
//// Author(s): Chad R. Hearn
//// Legal:     ©2001 Chad R. Hearn (BanderDach)
//// Purpose:   Gets the screen's dimensions and returns them as a DWORD. Width
////		      is returned as the HIWORD and height is returned as the LOWORD.	
////            
//// Date:      03/27/2001
////            
//// NOTES:     
///******************************************************************************/
//DWORD GetScreenDimensions (VOID)
//{
//	WORD wWidth, wHeight;
//
//	wWidth = (WORD)GetSystemMetrics(SM_CXSCREEN);	// get screen width
//	wHeight = (WORD)GetSystemMetrics(SM_CYSCREEN);	// get screen height
//
//	return ((DWORD)wWidth)<<16 | wHeight;			// LOWORD = height, HIWORD = width
//}
//
///******************************************************************************/
//// Author(s): Chad R. Hearn
//// Legal:     ©2001 Chad R. Hearn (BanderDach)
//// Purpose:   Garbage collection is handled here. Make certain that you destroy
////			  all objects you may have created, DC's, handle associations, and
////			  acclerator tables; unregister classes and post the quit message.
////            
//// Date:      03/27/2001
////            
//// NOTES:     
///******************************************************************************/
//VOID CleanupAndPostQuit (VOID)
//{
//	
//	DestroyAcceleratorTable(g_hacclApplication);	// destroy accelerator table
//	g_hacclApplication = NULL;				// accelerator handle to NULL, jic
//	
//	// Unregister the Application class
//	UnregisterClass(APPCLASS, (HINSTANCE)GetWindowLong(m_hwndApplication, GWL_HINSTANCE));
//
//	m_hwndApplication = NULL;	// set app's handle to null	
//	PostQuitMessage(0);			// post the quit message to the thread
//	return;
//}