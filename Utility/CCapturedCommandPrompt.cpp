///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor       
// Purpose:   CCapturedCommandPrompt object implementation
//
//
//
// Date:      
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include <windows.h>
#include <TChar.h>
#include <stdio.h>
#include <strstream>
#include <fstream>
#include "..\XLanceView.h"
#include "..\Resource\resource.h"
#include "CCapturedCommandPrompt.h"
#include "..\Dialogs\CMainWindow.h"

static CCapturedCommandPrompt *pcapcmdThis = NULL;

///////////////////////////////////////////////////////////////////////////////
// Object constants
///////////////////////////////////////////////////////////////////////////////

//	NO-Op Commands
#define COMMAND_CLOSE					_T("exit")

//	Standard Commands
#define FILENAME_COMMANDPROMPT			_T("cmd.exe")
#define COMMAND_CHANGEDIRECTORY			_T("cd")
#define COMMAND_CHANGEDIRECTORY2		_T("chdir")
#define COMMAND_DELETE					_T("del")
#define COMMAND_DELETE2 				_T("erase")
#define COMMAND_COPY					_T("copy")
#define COMMAND_MOVE					_T("move")
#define COMMAND_XCOPY					_T("xcopy")
#define COMMAND_REMOVEDIRECTORY			_T("rmdir")
#define COMMAND_REMOVEDIRECTORY2		_T("rd")
#define COMMAND_CLEARSCREEN				_T("cls")
#define COMMAND_SHOWUNSUPPORTEDCOMMANDS _T("unsupported")

//	 M(k)(dir) DOES NOT allow overwriting of directories and DOES NOT prompt for
//	  any confirmation, so it has been removed (i.e. no special processing is
//	  required).
//#define COMMAND_MAKEDIRECTORY			_T("mkdir ")
//#define COMMAND_MAKEDIRECTORY2			_T("md ")

//	 Ren(ame) DOES NOT allow overwriting of directories and DOES NOT prompt for
//	  any confirmation, so it has been removed (i.e. no special processing is
//	  required).
//#define COMMAND_RENAME					_T("ren ")
//#define COMMAND_RENAME2					_T("rename ")

//	 Disallowed Commands
#define COMMAND_DISALLOWEDCOMMANDS		_T("")//_T(",bcdedit,cacls,chkdsk,cmd,color,comp,compact,convert,date,diskpart,fc,find,findstr,format,graftabl,gpresult,help,icacls,label,mode,more,openfiles,popd,print,prompt,pushd,recover,replace,robocopy,sc,schtasks,shutdown,sort,taskkill,time,wmic,")
#define COMMAND_REFRESHCOMMANDS			_T(",attrib,copy,del,diskcopy,erase,md,mkdir,move,rd,ren,rename,rmdir,xcopy,")

#define FOLDER_ROOT						_T("\\\\")

#define MAX_SHORT						16535

//  Command Prompt Instance
#define COMMANDPROMPT_CLASSNAME			_T("ConsoleWindowClass")
#define COMMANDPROMPT_TITLE_PREFIX		_T("Administrator: ")

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor, initializes all fields to their defaults.
 */
CCapturedCommandPrompt::CCapturedCommandPrompt()
{
	// initialize fields to their defaults
	memset(&m_procinfoThis, 0, sizeof(m_procinfoThis));
	m_hwndCommandPrompt = NULL;
	m_hExecuteWritePipe = NULL;
	m_hExecuteReadPipe = NULL;
	m_hInputReadPipe = NULL;
	m_hInputWritePipe = NULL;

	m_strLastError = EMPTY_STRING;
	m_strCommand = EMPTY_STRING;
	m_strCurrentFolder = EMPTY_STRING;
	m_strCommandPromptTitle = EMPTY_STRING;

	m_dwCommandStart = (DWORD)0;
	m_bShouldRefresh = FALSE;
	m_bShouldPromptUser = FALSE;

	// set static object var
	pcapcmdThis = this;
}
                                 
/**
 * Destructor, performs clean-up on fields.
 */
CCapturedCommandPrompt::~CCapturedCommandPrompt()
{
	// perform final garbage collection
	cleanup();
}

///////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Dispatches the message and its params to the command prompt instance's
 * window.
 *
 * @param hwndCommandControl handle to the window that is "issuing" this
 * input.
 *
 * @param hwndOutputControl handle to a window that will display any
 * resultant output.
 *
 * @param uMsg the message to be dispatched
 *
 * @param wParam the WPARAM of the message being dispatched
 *
 * @param lParam the LPARAM of the message being dispatched
 *
 * @return TRUE if no errors occur and the message is dispatched, otherwise
 * FALSE.
 */
BOOL CCapturedCommandPrompt::pipeInput(HWND hwndMainWnd, HWND hwndCommandControl, 
	HWND hwndOutputControl, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bReturn = FALSE;

	try
	{
		// validate window handle, attempt to retrieve if necessary
		if(m_hwndCommandPrompt == NULL)
		{
			// attempt to retrieve
			getCommandPromptHandle();
		}

		// validate command and output controls
		if(hwndCommandControl == NULL)
		{
			// set last error
			m_strLastError = _T("The command control handle is invalid.");

			// return fail val
			return FALSE;
		}
		if(hwndOutputControl == NULL)
		{
			// set last error
			m_strLastError = _T("The output control handle is invalid.");

			// return fail val
			return FALSE;
		}

		// Reset "should refresh" flag
		//m_bShouldRefresh = FALSE;

		// Pre-process command to prevent user from being able to perform
		//	 certain navigation functions that could navigate away from
		//	 the directory currently selected in the File Manager on 
		//	 CMainWindow.
		//
		// NOTE: if the m_hwndCommandPrompt is good, then input should be
		// sent to it regardless of whether the other conditions might be
		// met.
		//
		if((Msg == WM_CHAR && wParam == VK_RETURN) && (m_hwndCommandPrompt == NULL))
		{
			TCHAR tstrBuffer[MAX_PATH] = EMPTY_STRING;
			BOOL bPromptUser = FALSE;

			// get command
			SendMessage(hwndCommandControl, WM_GETTEXT,
				(WPARAM)sizeof(tstrBuffer), (LPARAM)tstrBuffer);
			
			// go ahead and clear output control, indicating that command has
			//	 been "committed."
			SendMessage(hwndCommandControl, WM_SETTEXT, (WPARAM)0, 
				(LPARAM)NULL);

			// copy to member field
			m_strCommand = tstrBuffer;

			// run preparse, but first... get command ONLY
			for(int i = 0; i < (int)m_strCommand.length(); i++)
			{
				if(tstrBuffer[i] == _T(' '))
				{
					// terminate string at command's end
					tstrBuffer[i] = 0;

					// we're done, exit
					break;
				}
			}
			
			// Run preparse....
			if(!preparseCommand(tstrBuffer, hwndOutputControl, bPromptUser, 
				 m_bShouldRefresh))
			{
				// clear anything in the command prompt instance... this is
				//	 the non-technical way to do it.
				for(int i = 0; i < (int)m_strCommand.length(); i++)
				{
					// back it up...
					SendMessage(m_hwndCommandPrompt, WM_CHAR,
						(WPARAM)VK_BACK, 0L);
				}

				// exit, returning success
				return TRUE;
			}

			//bReturn = executeCommand(hwndCommandControl, hwndOutputControl, 
			//			m_bShouldRefresh);

			bReturn = launchExecuteThread(hwndMainWnd, hwndCommandControl, hwndOutputControl);
		}
		else
		{
			// Send to command prompt if there is an active instance
			if(m_hwndCommandPrompt != NULL)
			{
				TCHAR tchCurrent = 0;
				DWORD dwWrite = (DWORD)sizeof(TCHAR);

				// Send message...

				// If wParam equals VK_RETURN, convert to line feed (0x0A).
				if(wParam != VK_RETURN)
					tchCurrent = wParam;
				else
					tchCurrent = _T('\n');
				
				// write input to stdin of command process
				if(m_hInputWritePipe)
					WriteFile(m_hInputWritePipe, &tchCurrent, dwWrite, &dwWrite, NULL);

				// Because we can't tell for certain if the application on the
				//	 other end expects the ENTER key to be pressed, clear input
				//	 from the command control.
				if(wParam == VK_RETURN)
					SendMessage(hwndCommandControl, WM_SETTEXT, (WPARAM)0, (LPARAM)NULL);
			}
		}

		// if we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While dispatching the message to the command prompt, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;	
}

///////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////

/**
 * Closes all open handles and destroys the command prompt process created
 * by initializeCommandPrompt().
 */
BOOL CCapturedCommandPrompt::cleanup()
{
	BOOL bReturn = FALSE;

	try
	{
		// Close thread and process handles
		if(m_procinfoThis.dwProcessId != (DWORD)0 
		    && m_procinfoThis.hProcess != NULL)
		{
			// attempt to get the command prompt instance's handle if it hereto
			//	 hasn't been retrieved
			if(m_hwndCommandPrompt == NULL)
				getCommandPromptHandle();

			// close command prompt
			if(m_hwndCommandPrompt != NULL)
			{
				// send quit message to command prompt
				SendMessage(m_hwndCommandPrompt, WM_CLOSE, (WPARAM)0, 0L);

				// reset command prompt handle
				m_hwndCommandPrompt = NULL;
			}
			else
			{
				// attempt to destroy process forcefully
				TerminateProcess(m_procinfoThis.hProcess, (UINT)0);
			}

			// close handles
			CloseHandle(m_procinfoThis.hProcess);
			CloseHandle(m_procinfoThis.hThread);

			// reset structure
			memset(&m_procinfoThis, 0, sizeof(m_procinfoThis));
		}

		// Reset flags
		m_bShouldRefresh = FALSE;
		m_bShouldPromptUser = FALSE;

		// Clear thread parameter structure
		memset(&m_ecpProc, 0, sizeof(m_ecpProc));

		// Close pipes
		if(m_hExecuteReadPipe != NULL)
		{
			CloseHandle(m_hExecuteReadPipe);
			m_hExecuteReadPipe = NULL;
		}
		if(m_hExecuteWritePipe != NULL)
		{
			CloseHandle(m_hExecuteWritePipe);
			m_hExecuteWritePipe = NULL;
		}
		if(m_hInputReadPipe != NULL)
		{
			CloseHandle(m_hInputReadPipe);
			m_hInputReadPipe = NULL;
		}
		if(m_hInputWritePipe != NULL)
		{
			CloseHandle(m_hInputWritePipe);
			m_hInputWritePipe = NULL;
		}

		// If we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While performing cleanup of the command prompt, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Heads off certain commands which are not "displayable," commands which need
 * to have certain flags set, or commands which cannot be executed from within
 * the command prompt.
 *
 * @param tstrCommandBuffer
 *
 * @param hwndOutputControl
 *
 * @return FALSE if for any reason further processing of the command should NOT
 * proceed (handled, invalid, error, etc.), otherwise TRUE.
 */
BOOL CCapturedCommandPrompt::preparseCommand(TCHAR *tstrCommandBuffer,
	HWND hwndOutputControl, BOOL &bPromptUser, BOOL &bShouldRefresh)
{
	BOOL bReturn = TRUE;

	try
	{
		DWORD dwBufferSize = (DWORD)0;

		// validate command
		if(lstrlen(tstrCommandBuffer) == 0)
			return FALSE;

		// validate output control handle
		if(hwndOutputControl == NULL)
			return FALSE;

		// Initialize "out" params
		bPromptUser = FALSE;
		bShouldRefresh = FALSE;

		// make command lowercase for all comparisons
		dwBufferSize = lstrlen(tstrCommandBuffer) + 1;		
		if(_tcslwr_s(tstrCommandBuffer, dwBufferSize) == 0)
		{
			tstring strTemp = EMPTY_STRING;

			// check disallowed commands first
			strTemp = _T(',');
			strTemp += tstrCommandBuffer;
			strTemp += _T(',');
			if(_tcsstr(COMMAND_DISALLOWEDCOMMANDS, (TCHAR *)strTemp.data()) 
			    != NULL)
			{
				// create no-no message
				strTemp = _T("\r\n\r\n");
				strTemp += _T("The current command is unsupported because it either requires \"normal\" command prompt funtionality or can require additional input.\r\n\r\nFor a list of unsupported commands, type UNSUPPORTED.\r\n\r\n");
				strTemp += m_strCurrentFolder;
				if(m_strCurrentFolder.length() > 3)
				{
					if(m_strCurrentFolder[m_strCurrentFolder.length() - 1] == _T('\\'))
						strTemp = strTemp.substr(0, strTemp.length() - 1);
				}
				strTemp += _T(">");

				// display no-no message
				SendMessage(hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE, 
					(LPARAM)(TCHAR *)strTemp.data());

				// return fail val
				bReturn = FALSE;
			}

			// Next, check for commands which should result in the current file 
			//	 manager being refreshed.
			if(_tcsstr(COMMAND_REFRESHCOMMANDS, (TCHAR *)strTemp.data()) != NULL)
				bShouldRefresh = TRUE;			

			// Reset temp string
			strTemp = EMPTY_STRING;

			// Check and see if the user is trying to change directories
      if(!_tcscmp(tstrCommandBuffer, COMMAND_CHANGEDIRECTORY)
			   ||
			   !_tcscmp(tstrCommandBuffer, COMMAND_CHANGEDIRECTORY2))
			{
				// create no-no message
				strTemp = _T("\r\n\r\n");
				strTemp += _T("You cannot change directory from the command line. Please use a File Manager.\r\n\r\n");
				strTemp += m_strCurrentFolder;
				if(m_strCurrentFolder.length() > 3)
				{
					if(m_strCurrentFolder[m_strCurrentFolder.length() - 1] == _T('\\'))
						strTemp = strTemp.substr(0, strTemp.length() - 1);
				}
				strTemp += _T(">");

				// display no-no message
				SendMessage(hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE, 
					(LPARAM)(TCHAR *)strTemp.data());

				// return fail val
				bReturn = FALSE;
			}
			else if(lstrcmp(tstrCommandBuffer, COMMAND_CLEARSCREEN) == 0)
			{
				// Clear all text
				SendMessage(hwndOutputControl, WM_SETTEXT,
					(WPARAM)0, (LPARAM)0);

				// Output current directory

				//	 Create
				strTemp += m_strCurrentFolder;
				if(m_strCurrentFolder.length() > 3)
				{
					if(strTemp[strTemp.length() - 1] == _T('\\'))
						strTemp = strTemp.substr(0, strTemp.length() - 1);
				}
				strTemp += _T(">");

				//	 Display
				SendMessage(hwndOutputControl, EM_REPLACESEL,
					(WPARAM)FALSE, 
					(LPARAM)(TCHAR *)strTemp.data());

				// Set do not process further flag
				bReturn = FALSE;
			}
			else if(tstrCommandBuffer[1] == _T(':') 
					&& lstrlen(tstrCommandBuffer) == 2)
			{
				// User is trying to change drives

				// create no-no message
				strTemp = _T("\r\n\r\n");
				strTemp += _T("You cannot change drives from the command line. Please use a File Manager.\r\n\r\n");
				strTemp += m_strCurrentFolder;
				if(m_strCurrentFolder.length() > 3)
				{
					if(m_strCurrentFolder[m_strCurrentFolder.length() - 1] == _T('\\'))
						strTemp = strTemp.substr(0, strTemp.length() - 1);
				}
				strTemp += _T(">");

				// display no-no message
				SendMessage(hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE, 
					(LPARAM)(TCHAR *)strTemp.data());

				// return fail val
				bReturn = FALSE;				
			}
      else if(!_tcscmp(tstrCommandBuffer, 
						   COMMAND_DELETE)
				    ||
					!_tcscmp(tstrCommandBuffer,
						   COMMAND_DELETE2))
			{
				// add auto-confirmation flags
				//m_strCommand += _T(" /Q /F");

				// set confirm flag
				bPromptUser = TRUE;
			}
			else if(!_tcscmp(tstrCommandBuffer, 
						   COMMAND_COPY))
			{
				// add auto-confirmation flags
				//m_strCommand += _T(" /Y");

				// set confirm flag
				bPromptUser = TRUE;
			}
			else if(!_tcscmp(tstrCommandBuffer, 
						   COMMAND_MOVE))
			{
				// add auto-confirmation flags
				//m_strCommand += _T(" /Y");

				// set confirm flag
				bPromptUser = TRUE;
			}
			else if(!_tcscmp(tstrCommandBuffer, 
						   COMMAND_REMOVEDIRECTORY)
				    ||
					!_tcscmp(tstrCommandBuffer, COMMAND_REMOVEDIRECTORY2))
			{
				// add auto-confirmation flags
				//m_strCommand += _T(" /S /Q");

				// set confirm flag
				bPromptUser = TRUE;
			}
			else if(!_tcscmp(tstrCommandBuffer,
						   COMMAND_XCOPY))
			{
				// add auto-confirmation flags
				//m_strCommand += _T(" /R /Y");

				// set confirm flag
				bPromptUser = TRUE;
			}
			else if(lstrcmp(tstrCommandBuffer, COMMAND_SHOWUNSUPPORTEDCOMMANDS) == 0)
			{
				tstring strCommands = EMPTY_STRING;
				TCHAR *ptc = NULL;

				// copy to local var
				strCommands = COMMAND_DISALLOWEDCOMMANDS;

				// loop through and get commands
				for(strTemp = EMPTY_STRING, 
						ptc = _tcstok((TCHAR *)strCommands.data(), _T(","));
					ptc;
					ptc = _tcstok(NULL, _T(",")))
				{
					if(lstrlen(ptc))
					{
						// append CrLf
						if(strTemp.length())
							strTemp += _T("\r\n");

						// add commands to buffer
						strTemp += ptc;
					}
				}	

				// Display list
				strTemp += _T("\r\n\r\n");
				strTemp += m_strCurrentFolder;
				if(m_strCurrentFolder.length() > 3)
				{
					if(m_strCurrentFolder[m_strCurrentFolder.length() - 1] == _T('\\'))
						strTemp = strTemp.substr(0, strTemp.length() - 1);
				}
				strTemp += _T(">");
				
				// display message/list
				SendMessage(hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE, 
					(LPARAM)(TCHAR *)strTemp.data());

				// set no process return val
				bReturn = FALSE;
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

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Sets the current working directory
 */
BOOL CCapturedCommandPrompt::setCurrentDirectory(TCHAR *tstrNewDirectory, 
	HWND hwndOutputControl)
{
	BOOL bReturn = FALSE;

	try
	{
		tstring strTemp = EMPTY_STRING;

		// validate control handle
		if(hwndOutputControl == NULL)
			return bReturn;
		
		// validate directory
		if(lstrlen(tstrNewDirectory) == 0)
			return bReturn;

		// copy output directory
		strTemp = tstrNewDirectory;
		
		// remove trailing backslash, if any
		if(lstrlen(tstrNewDirectory) > 3)
		{
			if(strTemp[strTemp.length() - 1] == _T('\\'))
				strTemp = strTemp.substr(0, strTemp.length() - 1);
		}

		// append > to "prompt"
		strTemp += _T('>');
		
		// display
		SendMessage(hwndOutputControl, WM_SETTEXT, (WPARAM)strTemp.length(), 
			(LPARAM)(TCHAR *)strTemp.data());

		// store
		m_strCurrentFolder = tstrNewDirectory;

		// get command start
		//m_dwCommandStart = SendMessage(hwndOutputControl, WM_GETTEXTLENGTH, (WPARAM)0,
		//					(LPARAM)0L);

		// if we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;	
}

/**
 * Retrieves the window handle for the command prompt instance. This method
 * should be called anytime the command prompt's handle is needed but
 * checked and found to be NULL.
 *
 * @return TRUE if the command prompt's window handle is retrieved and no
 * errors occur, otherwise FALSE.
 */
BOOL CCapturedCommandPrompt::getCommandPromptHandle()
{
	BOOL bReturn = FALSE;

	try
	{
        DWORD dwMajorVersion, dwVersion;
		tstring strTitle = EMPTY_STRING;

		// validate process
		if(m_procinfoThis.hProcess == NULL)
			return FALSE;

		// check title
		if(m_strCommandPromptTitle.length() == 0)
			return FALSE;

		// Depending on the OS version, the command prompt is titled
		//	 differently. For Vista or newer, the command prompt's
		//	 title has "Administrator: " in front of the title. For
		//	 XP or older, ONLY the title appears.
		
		// Get OS version
        dwVersion = GetVersion();

		// Get Major
        dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

		// check
		//if(dwMajorVersion > 5)
		//{
		//	// Vista or newere
		//	// construct title as it will appear
		//	strTitle = COMMANDPROMPT_TITLE_PREFIX;
		//	strTitle += m_strCommandPromptTitle;
		//}
		//else
		{
			// XP or older
			strTitle = m_strCommandPromptTitle;
		}

		// Get command prompt instance's window handle... this is more
		//	 difficult than it appears.
		m_hwndCommandPrompt = FindWindow(COMMANDPROMPT_CLASSNAME, 
								(TCHAR *)strTitle.data());

		// check and see if handle was retrieved successfully
		bReturn = (m_hwndCommandPrompt != NULL);
	}
	catch(...)
	{
		// set last error
		m_strLastError = _T("While locating the handle of the command prompt, an unexpected error occurred.");

		// set fail val
		bReturn = FALSE;
	}

	// clear last error, if applicable
	if(bReturn)
		m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

/**
 * Launches a separate thread to perform the command execution process.
 */
BOOL CCapturedCommandPrompt::launchExecuteThread( HWND hwndMainWnd, HWND hwndEditorControl, 
	HWND hwndOutputControl)
{
	BOOL bReturn = FALSE;

	try
	{
		SECURITY_ATTRIBUTES secattrThread;
		DWORD dwThreadID;

		// prepare thread security
		secattrThread.nLength = sizeof(secattrThread);
		secattrThread.bInheritHandle = TRUE;
		secattrThread.lpSecurityDescriptor = NULL;

		// prepare thread parameter
		m_ecpProc.pbShouldRefresh = &m_bShouldRefresh;
		m_ecpProc.hwndCommandControl = hwndEditorControl;
		m_ecpProc.hwndOutputControl = hwndOutputControl;
		m_ecpProc.hwndMainWnd = hwndMainWnd;

		// attempt to create thread
		m_hExecuteThread = CreateThread(&secattrThread, 0, 
								executeCommand, &m_ecpProc, 
								CREATE_SUSPENDED, &dwThreadID);
		if(m_hExecuteThread != NULL)
		{
			SetThreadPriority(m_hExecuteThread, THREAD_MODE_BACKGROUND_BEGIN);
			ResumeThread(m_hExecuteThread);
		}
	}
	catch(...)
	{
		// reset fail val
		bReturn = FALSE;
	}

	// return success/fail val
	return bReturn;
}

/**
 * Loads cmd.exe, creates read and write pipes, and prepares object for
 * use.
 */
DWORD WINAPI CCapturedCommandPrompt::executeCommand(LPVOID lpParameter)
{
	TCHAR *ptcCommandBuffer = NULL;
	BOOL bReturn = FALSE;

	try
	{
		EXECUTECOMMANDPARAMETER *pecpParam = NULL;
		STARTUPINFO stinfPrompt;
		SECURITY_ATTRIBUTES secattrPrompt;
		tstring strTemp = EMPTY_STRING;
		TCHAR tstrBuffer[MAX_PATH + 1] = EMPTY_STRING,
			  *ptc = NULL;
		DWORD dwRead = (DWORD)0,
			  dwLength = (DWORD)0;
		long lReturn = 0L;
		BOOL bPromptUser = FALSE;

		// cast incoming parameter
		pecpParam = (EXECUTECOMMANDPARAMETER *)lpParameter;

		// check parameter
		if(pecpParam == NULL)
			return FALSE;

		// check controls
		if(pecpParam->hwndMainWnd == NULL)
			return FALSE;
		if(pecpParam->hwndOutputControl == NULL)
			return FALSE;
		if(pecpParam->hwndCommandControl == NULL)
			return FALSE;


		// If the current directory is the machine root, then DO NOT allow any commands
		if(pcapcmdThis->m_strCurrentFolder == FOLDER_ROOT)
		{
			// Clear current command
			SendMessage(pecpParam->hwndCommandControl, WM_SETTEXT, (WPARAM)0, (LPARAM)0);			

			// Move to end of current text
			lReturn = SendMessage(pecpParam->hwndOutputControl, WM_GETTEXTLENGTH, (WPARAM)0, 
						(LPARAM)0);
			SendMessage(pecpParam->hwndOutputControl, EM_SETSEL, (WPARAM)lReturn, 
				(LPARAM)lReturn);

			// Create no-no message
			strTemp = _T("\r\n\r\n");
			strTemp += _T("While the machine root is selected, you cannot execute any commands. Please choose a drive or directory from the File Managers.\r\n\r\n");
			strTemp += pcapcmdThis->m_strCurrentFolder;
			strTemp += _T(">");
			
			// Display no-no message
			SendMessage(pecpParam->hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE,
				(LPARAM)(TCHAR *)strTemp.data());

			// exit
			return FALSE;
		}

		// Move to end of current text
		lReturn = SendMessage(pecpParam->hwndOutputControl, WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);
		SendMessage(pecpParam->hwndOutputControl, EM_SETSEL, (WPARAM)lReturn, (LPARAM)lReturn);

		// Append current command
		char szCommand[50] = {0};
		strcpy(szCommand, pcapcmdThis->m_strCommand.c_str());
		_strlwr(szCommand);

		strTemp = szCommand;

		//here we need to check for program commands commands
		if(pcapcmdThis->CheckIfNonCmdCommand(strTemp))
		{
			tstring strSyntaxError;
			strSyntaxError = strTemp + "\nThe syntax of command is incorrect\n" +
			"e.g.\t1) xlv load all w\n" + 
				"    \t2) xlv unload t1 t2 t3 f\n";
			if(!pcapcmdThis->ISSyntaxError(strTemp))
			{	
				bool bIsLoad = true;
				char szData[6] = {0};

				strcpy(szData, "0000");
				szData[5] = '\0';

				pcapcmdThis->GetCommandId(strTemp, bIsLoad, szData);
				if(!HandleCapturedCommandPromptMessages(bIsLoad, szData))
				{
					SendMessage(pecpParam->hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE, 
						(LPARAM)(TCHAR *)strSyntaxError.data());
					pcapcmdThis->RefreshTrailingPrompt(lpParameter);
					return FALSE;
				}
			}
			else
			{
				tstring sztemp;
				char szStr[50] = {0};
				strcpy(szStr, strTemp.c_str());

				sztemp += szCommand;
				sztemp += "\n";
				if(	szStr[strlen(szStr) - 1] == 'w'  && GetMainWindowMode() == SW_MAXIMIZE)
					sztemp += "Please use full mode window commands";
				if(	szStr[strlen(szStr) - 1] == 'f' && GetMainWindowMode() == SW_NORMAL)
					sztemp += "Please use normal mode window commands";
				else
					sztemp  = strSyntaxError;

				SendMessage(pecpParam->hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE, 
				(LPARAM)(TCHAR *)sztemp.data());
				pcapcmdThis->RefreshTrailingPrompt(lpParameter);
				return FALSE;
			}			
			return FALSE;
		}

		strTemp += _T("\r\n");
		SendMessage(pecpParam->hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE, 
			(LPARAM)(TCHAR *)strTemp.data());

		// initialize security attributes structure
		secattrPrompt.nLength = sizeof(secattrPrompt);
		secattrPrompt.bInheritHandle = TRUE;
		secattrPrompt.lpSecurityDescriptor = NULL;

		// attempt to create pipes
		lReturn = CreatePipe(&pcapcmdThis->m_hExecuteReadPipe, &pcapcmdThis->m_hExecuteWritePipe, 
					&secattrPrompt, (DWORD)MAX_PATH);
		lReturn += CreatePipe(&pcapcmdThis->m_hInputReadPipe, &pcapcmdThis->m_hInputWritePipe,
					&secattrPrompt, (DWORD)MAX_PATH);
		if(lReturn == 0L)
		{
			// set last error
			pcapcmdThis->m_strLastError = _T("Could not create read and write mechanism for captured command prompt.");

			// return fail val
			return FALSE;
		}

		// Create unique command prompt title
		_stprintf(tstrBuffer, _T("%s - %u"), MAINWINDOW_TITLE, GetTickCount());
		pcapcmdThis->m_strCommandPromptTitle = tstrBuffer;

		// Initialize startup info struct
		memset(&stinfPrompt, 0, sizeof(stinfPrompt));
		stinfPrompt.cb = sizeof(stinfPrompt);
		stinfPrompt.hStdInput = pcapcmdThis->m_hInputReadPipe;
		stinfPrompt.hStdError = pcapcmdThis->m_hExecuteWritePipe;
		stinfPrompt.hStdOutput = pcapcmdThis->m_hExecuteWritePipe;
		stinfPrompt.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		stinfPrompt.lpTitle = (TCHAR *)pcapcmdThis->m_strCommandPromptTitle.data();
//#ifdef _DEBUG
//		stinfPrompt.wShowWindow = SW_NORMAL;
//#else
		stinfPrompt.wShowWindow = SW_HIDE;
//#endif
		
		// set executable path and arguments
		strTemp = FILENAME_COMMANDPROMPT;
		strTemp += _T(" /C ");
		strTemp += (TCHAR *)pcapcmdThis->m_strCommand.data();

		// Attempt to create process
		lReturn = CreateProcess(NULL, (TCHAR *)strTemp.data(), &secattrPrompt, 
					&secattrPrompt, TRUE, 
					NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, NULL, 
					(TCHAR *)pcapcmdThis->m_strCurrentFolder.data(),
					&stinfPrompt, &pcapcmdThis->m_procinfoThis);

		if(lReturn == 0L)
		{
			// set last error
			pcapcmdThis->m_strLastError = _T("Could not create an instance of the command prompt.");

			// return fail val
			return FALSE;
		}

		// Close write pipe
		if(pcapcmdThis->m_procinfoThis.dwProcessId)
			CloseHandle(pcapcmdThis->m_hExecuteWritePipe);
		pcapcmdThis->m_hExecuteWritePipe = NULL;

		// Read returned data
    char strBuffer[MAX_PATH+1];
		while(ReadFile(pcapcmdThis->m_hExecuteReadPipe, strBuffer, MAX_PATH, &dwRead, NULL) && dwRead != 0)
		{
			// terminate string
      strBuffer[dwRead] = 0;

      OemToAnsi(strBuffer, strBuffer);

      tstring transl_buf;
      AToTChar(strBuffer, transl_buf);

			// check string for "key" characters and convert
			if(dwRead == (DWORD)1)
			{
				if(transl_buf[0] == _T('\n'))
				{	
					// convert to full CrLf
          transl_buf = _T("\r\n");
					
					// increase "read" count
          dwRead = transl_buf.length();
				}
			}
			
			// get length of current text
			lReturn = SendMessage(pecpParam->hwndOutputControl, WM_GETTEXTLENGTH, (WPARAM)0,
						0L);

			// if length exceeds allowable (addressable) limit, clear
      if((lReturn + transl_buf.length()) > MAX_SHORT)
				SendMessage(pecpParam->hwndOutputControl, WM_SETTEXT, (WPARAM)0, (LPARAM)NULL);

			// append string to "log"
			lReturn = SendMessage(pecpParam->hwndOutputControl, EM_REPLACESEL, 
						(WPARAM)FALSE,  (LPARAM)transl_buf.c_str());
		}

		// Add trailing "prompt"
		//	 Create prompt
		
		pcapcmdThis->RefreshTrailingPrompt(lpParameter);

		// If we made it here, set success val
		bReturn = TRUE;
	}
	catch(...)
	{
		// set last error
		pcapcmdThis->m_strLastError = _T("");

		// set fail val
		bReturn = FALSE;
	}
	
	// garbage collect
	if(ptcCommandBuffer)
		delete[] ptcCommandBuffer;

	// clear last error, if applicable
	if(bReturn)
		pcapcmdThis->m_strLastError = EMPTY_STRING;

	// return success / fail val
	return bReturn;
}

//BOOL CCapturedCommandPrompt::()
//{
//	BOOL bReturn = FALSE;
//
//	try
//	{
//
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
//	// clear last error, if applicable
//	if(bReturn)
//		m_strLastError = EMPTY_STRING;
//
//	// return success / fail val
//	return bReturn;
//}

bool CCapturedCommandPrompt::CheckIfNonCmdCommand(tstring strTemp)
{
	StringTrim(strTemp);
	if(strTemp.find("xlv") != -1)
	{
		return true;
	}
	return false;
}

void CCapturedCommandPrompt::GetCommandId(tstring strTemp, bool &bIsLoad, char *bTabArray)
{	
	if(strTemp.find("unload") != -1 )
	{
		bIsLoad = false;
	}
	else if(strTemp.find("load") != -1 )
	{
		bIsLoad = true;
	}

	if(strTemp.find("t0") != -1 )
	{
		bTabArray[0] = '1';
	}
	if(strTemp.find("t1") != -1 )
	{
		bTabArray[1] = '1';
	}
	if(strTemp.find("t2") != -1 )
	{
		bTabArray[2] = '1';
	}
	if(strTemp.find("t3") != -1 )
	{
		bTabArray[3] = '1';
	}
}
  
bool CCapturedCommandPrompt::ISSyntaxError(tstring strTemp)
{
	bool bReturn, bFirst, bSecond, bThird, bForth;
	int iCount = 0;
	char sztempStr[50] = {0},
		 szStr[50] = {0};
	strcpy(sztempStr, strTemp.c_str());
	strcpy(szStr, strTemp.c_str());

	bReturn = true;		
	bFirst = bSecond = bThird = bForth = false;
	char * pch;
	pch = strtok (sztempStr," ");
	while (pch != NULL)
	{

		if(iCount == 0 && strcmp(pch,"xlv") == 0)
			bFirst  = true;
		
		if( (iCount == 1 && strcmp(pch,"load") == 0) ||
			(iCount == 1 && strcmp(pch,"unload") == 0 ))
			bSecond = true;

		if( (iCount == 2 && strcmp(pch,"all") == 0) || 
			(iCount == 2 && strcmp(pch,"t0") == 0) ||
			(iCount == 2 && strcmp(pch,"t1") == 0) ||
			(iCount == 2 && strcmp(pch,"t2") == 0) ||
			(iCount == 2 && strcmp(pch,"t3") == 0))
			bThird  = true;

		iCount++;
		pch = strtok (NULL, " ");
	}
	
	if(	szStr[strlen(szStr) - 1] == 'w'  && GetMainWindowMode() == SW_NORMAL ||
		szStr[strlen(szStr) - 1] == 'f' && GetMainWindowMode() == SW_MAXIMIZE )
		bForth = true;

	if(bFirst && bSecond && bThird && bForth)
		bReturn = false;

	return bReturn;
}

void CCapturedCommandPrompt::RefreshTrailingPrompt(LPVOID lpParameter)
{
	tstring strTemp = EMPTY_STRING;

	EXECUTECOMMANDPARAMETER *pecpParam = NULL;

	// cast incoming parameter
	pecpParam = (EXECUTECOMMANDPARAMETER *)lpParameter;

	// check parameter
	if(pecpParam == NULL)
		return;

	// check controls
	if(pecpParam->hwndOutputControl == NULL)
		return;
	if(pecpParam->hwndCommandControl == NULL)
		return;

	strTemp = _T("\r\n");
	if(pcapcmdThis->m_strCurrentFolder.length() == 3)
	{
		strTemp += pcapcmdThis->m_strCurrentFolder;
		strTemp += _T('>');
	}
	else
	{
		strTemp += pcapcmdThis->m_strCurrentFolder;
		if(strTemp[strTemp.length() - 1] == _T('\\'))
			strTemp = strTemp.substr(0, strTemp.length() - 1);
		strTemp += _T(">");
	}

	//	 Display prompt
	SendMessage(pecpParam->hwndOutputControl, EM_REPLACESEL, (WPARAM)FALSE, 
		(LPARAM)(TCHAR *)strTemp.data());

	// Order refresh, if applicable
	if(*pecpParam->pbShouldRefresh)
		SendMessage(pecpParam->hwndCommandControl, WM_APP, 
		(WPARAM)AM_REFRESHFILEMANAGERS, 0L);

	// cleanup process
	pcapcmdThis->cleanup();
}