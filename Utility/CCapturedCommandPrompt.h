#ifndef _CCAPTUREDCOMMANDPROMPT_
#define _CCAPTUREDCOMMANDPROMPT_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor       
// Purpose: CCapturedCommandPrompt object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Structures
///////////////////////////////////////////////////////////////////////////////

struct EXECUTECOMMANDPARAMETER
{
	HWND hwndCommandControl,
		 hwndOutputControl,
		 hwndMainWnd;
	BOOL *pbShouldRefresh;
	
	/**
	 * Default "constructor"
	 */
	EXECUTECOMMANDPARAMETER()
	{
		// initialize members to defaults
		hwndCommandControl = NULL;
		hwndOutputControl = NULL;
		hwndMainWnd = NULL;		
		pbShouldRefresh = NULL;
	}
};

///////////////////////////////////////////////////////////////////////////////
// CCapturedCommandPrompt object definition
///////////////////////////////////////////////////////////////////////////////

class CCapturedCommandPrompt
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////
	
	PROCESS_INFORMATION m_procinfoThis;

	HANDLE m_hExecuteReadPipe,
		   m_hExecuteWritePipe,
		   m_hInputReadPipe,
		   m_hInputWritePipe,
		   m_hExecuteThread;

	HWND m_hwndCommandPrompt;

	EXECUTECOMMANDPARAMETER m_ecpProc;

	tstring m_strCommand,
				m_strCurrentFolder,
				m_strCommandPromptTitle,
				m_strLastError;
	
	BOOL m_bShouldRefresh,
		 m_bShouldPromptUser;

	DWORD m_dwCommandStart;
	
	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Closes the command prompt instance and frees associated thread and
	 * process handles.
	 */
	BOOL cleanup();

	/**
	 * Retrieves the window handle for the command prompt instance.
	 */
	BOOL getCommandPromptHandle();

	/**
	 * Heads off certain commands which are not "displayable" or need to be
	 * handled in a special fashion.
	 */
	BOOL preparseCommand(TCHAR *tstrCommandBuffer, HWND hwndOutputControl, 
		BOOL &bPromptUser, BOOL &bShouldRefresh);

	/**
	 * Loads cmd.exe, creates read and write pipes, and prepares object for
	 * use.
	 */
	static DWORD WINAPI executeCommand(LPVOID lpParameter);

	/**
	 * Launches a separate thread to perform the command execution process.
	 */
	BOOL launchExecuteThread(HWND hwndMainWnd, HWND hwndEditorControl, HWND hwndOutputControl);

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CCapturedCommandPrompt();

	/**
	 * Destructor, performs clean-up.
	 */
	~CCapturedCommandPrompt();

	///////////////////////////////////////////////////////////////////////////
	// Commo Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Dispatches the message and its params to the command prompt instance's
	 * window.
	 */
	BOOL pipeInput(HWND hwndMainWnd, HWND hwndCommandControl, HWND hwndOutputControl, UINT Msg, 
		WPARAM wParam, LPARAM lParam);

	///////////////////////////////////////////////////////////////////////////
	// Getter Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Returns the last error encountered, if any.
	 */
	TCHAR *getLastError() {return (TCHAR *)m_strLastError.data();}

	/**
	 * Returns the current working directory.
	 */
	TCHAR *getCurrentDirectory() {return (TCHAR *)m_strCurrentFolder.data();}

	/**
	 * Gets whether or not the last operation could necessitate a directory
	 * refresh.
	 */
	BOOL getShouldRefresh() {return m_bShouldRefresh;}

	///////////////////////////////////////////////////////////////////////////
	// Setter Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Sets the current working directory
	 */
	BOOL setCurrentDirectory(TCHAR *tstrNewDirectory, HWND hwndOutputControl); 

	/**
	 * to check for non Command prompt commands
	 */
	bool CheckIfNonCmdCommand(tstring strTemp);

	/**
	 * to Get the command ID
	 */
	//int GetCommandId(tstring strTemp);
	void GetCommandId(tstring strTemp, bool &bIsLoad, char *bTabArray);

	/**
	 * Check for syntax Error in Command
	 */
	bool ISSyntaxError(tstring strTemp);

	/**
	 * add the Trailing prompt and refresh
	 */
	void RefreshTrailingPrompt(LPVOID lpParameter);

};

#endif // End _CCAPTUREDCOMMANDPROMPT_
