#ifndef _CMAINWINDOW_
#define _CMAINWINDOW_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor  
// Purpose:   CMainWindow object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include "..\StdAfx.h"
#include <commctrl.h>
#include <map>
#include "..\LinkedList.h"
#include "..\FileInformation.h"
#include "..\Communication\XlvCommunicatorServer.h"
#include "FirstTabDialog.h"
#include "SecondTabDialog.h"
#include "IntropCom.h"

// Forward declares
class CCapturedCommandPrompt;
class CDWGRenderEngine;

//global function to handle the messages
bool HandleCapturedCommandPromptMessages(bool bIsLoad, char *szData);
//Get main window handle here
int GetMainWindowMode();

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////

#define LAYOUT_COUNT_BUTTONSALLSTATES			24		// 12 buttons, 2 states

// Package file object definition
class CMainWindow
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Enums
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Enum for sort "types."
	 */
	enum ACTIVESORTENUM
	{
		aseNone,			// No sort has been selected
		aseNameAsc,			// Sort by name, ascending
		aseNameDesc,		// Sort by name, descending
		aseTypeAsc,			// Sort by type, ascending
		aseTypeDesc,		// Sort by type, descending
		aseSizeAsc,			// Sort by size, ascending
		aseSizeDesc,		// Sort by size, descending
		aseDateAsc,			// Sort by date, ascending
		aseDateDesc,		// Sort by date, descending
		aseExtensionAsc,	// Sort by extension, ascending
		aseExtensionDesc,	// Sort by extension, descending
		aseAtrrAsc,			// Sort by extension, ascending //Parth Software Solution
		aseAttrDesc,			// Sort by extension, descending //Parth Software Solution
		aseDirAsc,			// sort as per Dir first
		aseDirDesc,			// sort as per File first
		aseCustom			// sort as Cutom using keyboard for one item
	};

	/**
	 * Enum for command button states.
	 */
	enum COMMANDBUTTONSTATEENUM
	{
		cbseNormal,
		cbseFocus,
		cbseDown
	};

	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////
	
	CCapturedCommandPrompt *m_ccapcmdThis;

	CDWGRenderEngine *m_cdwgengThis;

	LinkedListEx<FILE_INFORMATION> *m_pllstFileManager1,
								   *m_pllstFileManager2,
								   *m_pllstActiveFileManager;

	//Parth Software Solution
	LinkedListEx<FILE_INFORMATION> *m_pTvFileManager1;
	LinkedListEx<FILE_INFORMATION> *m_pTvFileManager2;
	//Parth Software Solution
	
	RECT **m_arrctCommandButtons;
	HBITMAP m_arbmpCommandButtons[LAYOUT_COUNT_BUTTONSALLSTATES];

	HINSTANCE m_hinstApplication;

	WNDPROC m_wndprocPreviousFileManager1,
			m_wndprocPreviousFileManager2,
			m_wndprocPreviousCommandConsolePrompt,
			m_wndprocPreviousCommandPrompt,
			m_wndprocPreviousCommandButtons,
			m_wndprocPreviousProductName,
			m_wndprocPreviousTrialVersion,
			m_wndprocPreviousFileManager3,//Parth Software Solution
			m_wndprocPreviousFileManager4;//Parth Software Solution

	HWND m_hwndThis,
		 m_hwndActiveFileManager,
		 m_hwndProgressBar,
		 m_hwndPercentageText;

	HFONT m_hfontControls;

	HBRUSH m_hbrControlBackgrounds,
		   m_hbrCommandPromptBackground,
		   m_hbrTitlebarBackground;

	tstring m_strLastError;

	int m_iWindowState,
		m_iControlFontHeight,
		*m_ariFileManager1Selection,
		*m_ariFileManager2Selection,
		 m_iCurrentSelectionCountFileManager1,
		 m_iCurrentSelectionCountFileManager2,
		 m_iLastButtonIndex,
		 m_iLastButtonState;

	ACTIVESORTENUM m_aseActiveSort;

	//Parth Software Solution	
	ACTIVESORTENUM m_ActNameSort;
	ACTIVESORTENUM m_ActExtSort;
	ACTIVESORTENUM m_ActSizeSort;
	ACTIVESORTENUM m_ActDateSort;
	ACTIVESORTENUM m_ActDirSort;
	//Parth Software Solution

  typedef std::map<UINT, HBRUSH> CtrlID2Brush;
  CtrlID2Brush ctrl2br;

	BOOL m_bGraphicsDeviceModeChanged,
		 m_bIsChangingWindowMode,
		 m_bInFullScreenMode,
		 m_bMouseLeftCommandButtons;

	static int m_iRange;
	static int m_TempiRange;
				
	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Loads, from the registry, the user's preferences for this dialog.
	 */
	BOOL loadPreferences();

	/**
	 * Saves, to the registry, the user's preferences for this dialog.
	 */
	BOOL savePreferences();

	/**
	 * Loads the application icon and assigns it to this object's dialog.
	 */
	BOOL loadIcon();

	/**
	 * Sets the font for all controls to a DOS-like Terminal font.
	 */
	BOOL setFonts();

	/**
	 * Changes the current graphics device mode to the one selected by the
	 * user.
	 */
	BOOL changeGraphicsDeviceMode();

	/**
	 * Restores the original graphics device mode.
	 */
	BOOL restoreGraphicsDeviceMode();

	/**
	 * Determines whether the current graphics mode should be changed and 
	 * maximizes this window accordingly.
	 */
	BOOL initializeWindow();

	/**
	 * Initializes the captured command prompt
	 */
	BOOL initializeCommandPrompt();

	/**
	 * Initializes the DWG rendering engine.
	 */
	BOOL initializeDWGRenderEngine();

	/**
	 * Initializes File Manager 1 & 2 by loading the directing listing for their
	 * last browse folder.
	 */
	BOOL initializeFileManagers();

	/**
	 * Initializes Tab control adding the Pages in that
	 */
	BOOL initializeTabControl();

	/**
	 * Gets the directory listing for the specified folder and then displays it
	 * by calling displayDirectoryListing().
	 */
	BOOL getDirectoryListing(const TCHAR *tstrFullpath, HWND hwndOutputControl,
		LinkedListEx<FILE_INFORMATION> *pllstOutput);

	//Parth Software Solution
	/**
	 * Gets the directory listing for the specified folder and then displays it
	 * by calling displayDirectoryListing() in Tree view explorer.
	 */
	BOOL getDirectoryListing_TV(const TCHAR *tstrFullpath, HWND hwndOutputControl,
	LinkedListEx<FILE_INFORMATION> *pllstOutput, bool bUsingCheckBox = false, bool bDisplyResult = true,
	bool bDiplayListUsingOnlyPath = false);

	/**
	 * Displays the directory listing for the specified folder using the
	 * specified control and "active" sort.
	 */
	BOOL displayDirectoryListing_TV(HWND hwndOutputControl, 
	LinkedListEx<FILE_INFORMATION> *pllstOutput);
	//Parth Software Solution

	/**
	 * Displays the directory listing for the specified folder using the
	 * specified control and "active" sort.
	 */
	BOOL displayDirectoryListing(HWND hwndOutputControl, 
		LinkedListEx<FILE_INFORMATION> *pllstOutput);

	/**
	 * Sorts the directory listing by whatever the active sort is and re-displays
	 * the results using displayDirectoryListing().
	 */
	BOOL sortDirectoryListing();

	/**
	 * Subclasses all appropriate controls.
	 */
	BOOL subclassControls();

	/**
	 * Changes from full screen to windowed or windowed to full screen "window
	 * modes."
	 */
	BOOL changeWindowMode(BOOL bFullScreen = FALSE);

	/**
	 * Opens the currently selected DWG in AutoCad
	 */
	BOOL openInAutoCad();

	///**
	// * Checks to see if the specified filename has a valid AutoCad drawing
	// * file extension.
	// */
	//BOOL hasAutoCadExtension(TCHAR *tstrFilename);

	/**
	 * Displays information about the active DWG. The "active DWG" is the one 
	 * being viewed.
	 */
	BOOL getDWGInformation();

	/**
	 * Selects all files in the currently active File Manager. Returns the 
	 * number of files selected.
	 */
	long selectAllFileObjects();

	/**
	 * Executes whatever the appropriate, double-click action is for the item 
	 * in the active File Manager.
	 */
	BOOL executeSelectedItem();

	// Parth Software Solution
	/**
	 * Executes whatever the appropriate, double-click action is for the item 
	 * in the active File Manager.
	 */
	BOOL executeSelectedItem_TV(int iId);
	// Parth Software Solution
	/**
	 * Given a formatted "File Manager" string, returns the name of the 
	 * directory contained in it, if any.
	 */
	BOOL getDirectoryNameFromItem(TCHAR *tstrItem, TCHAR *tstrOutDirectoryName);
	
	/**
	 * Given a formatted "File Manager" string, returns the name of the 
	 * file contained in it, if any.
	 */
	BOOL getFilenameFromItem(TCHAR *tstrItem, TCHAR *tstrOutFilename);
	
	/**
	 * Given a formatted "File Manager" string, returns the name of the 
	 * drive contained in it, if any.
	 */
	BOOL getDriveNameFromItem(TCHAR *tstrItem, TCHAR *tstrOutDriveName);

	/**
	 * Populates the active File Manager with the "Machine Root" (or drive 
	 * listing).
	 */
	BOOL getDriveListing(HWND hwndOutputControl);

	//Parth Software Solution
	/**
	 * Populates the active Tree view File Manager with the "Machine Root" (or drive 
	 * listing).
	 */
	BOOL getDriveListing_TV(HWND hwndOutputControl);
	//Parth Software Solution
	/**
	 * Using the active File Manager, creates a new directory within the current
	 * folder.
	 */
	BOOL createNewDirectory();

	/**
	 * Selects, depending on the is directory flag, a file or folder in the 
	 * currently active File Manager whose name matches the on specified.
	 */
	int selectFileOrDirectory(TCHAR *tstrFileSystemObjectName, 
		BOOL bIsDirectory = FALSE);

  void toggleCurrentFMSelection(HWND hwnd);

	/**
	 * Displays the settings dialog so the user can choose a new resolution / 
	 * display settings.
	 */
	BOOL displaySettings();

	/**
	 * Displays the options dialog. NOTE: The options dialog currently contains
	 * no options and is reserved for future use.
	 */
	BOOL displayOptions();

	/**
	 * Displays the help dialog.
	 */
	BOOL displayHelp();

	// Parth Software Solution
	// functions for sorted buttons
	BOOL SortTV1AsPerExt1(int iID);
	BOOL SortTV1AsPerName1(int iID);
	BOOL SortTV1AsPerSize1(int iID);
	BOOL SortTV1AsPerDate1(int iID);
	BOOL SortTV1AsPerPermission1(int iID);
	BOOL SortTV1AsPerAlt1(int iID);
	BOOL SortTVAsPerDir(int iID);
	// Parth Software Solution
	/**
	 * Displays the layer control dialog.
	 */
	BOOL displayLayerControl();

	/**
	 * Displays the file attributes and permissions for the currently selected
	 * file in the active File Manager.
	 */
	BOOL displayFileAttributesAndPermissions();

	/**
	 * Prompts the user for a file mask, then applies the mask selecting all
	 * files which meet the criteria.
	 */
	long selectFilesByMask();

	/**
	 * Copies files and directories between File Managers.
	 */
	BOOL copyFilesAndDirectories();

  /**
   * Performing operation on file system objects from between File Managers
   */
  BOOL operateFSOFromTo(UINT iOperation);

	/**
	 * Deletes files and directories in the active File Manager.
	 */
	BOOL deleteFilesAndDirectories();

  /**
	 * Renames files and directories in the active File Manager.
	 */
	BOOL switchFileManagers();

	/**
	 * Renames files and directories in the active File Manager.
	 */
	BOOL renameFilesAndDirectories();

	/**
	 * Toggles the selected state of the currently selected file object in the
	 * active File Manager.
	 */
	BOOL toggleSelectedFileObject();

	/**
	 * Draws the command buttons within the "command button bar."
	 */
	BOOL drawCommandButtons(HWND hwndBar, HDC hdcBar, RECT rctBar);

	/**
	 * Creates an auto-wrapped messagebox. For machines with Vista or higher, 
	 * the internal wrapping mechanism is used. For machines with XP or lower, 
	 * the wrapping is synthesized.
	 */
	int WrappedMessageBox(LPCTSTR lpText, LPCTSTR lpCaption,
		UINT uType);

	/**
	 * Sorts the active file list by name either in ascending or descending
	 * order depending on what the active sort is.
	 */
	BOOL sortFileListByName(long lStart, long lLength);

	/**
	 * Sorts the active file list by name either in ascending or descending
	 * order depending on what the user want to move up and down in the tree control.
	 */
	BOOL sortFileListByCustom(long lStart, long lLength);

	/**
	 * Sorts the active file list by type either in ascending or descending
	 * order depending on what the active sort is.
	 */
	BOOL sortFileListByType(long lStart, long lLength);

	/**
	 * Sorts the active file list by size either in ascending or descending
	 * order depending on what the active sort is.
	 */
	BOOL sortFileListBySize(long lStart, long lLength);

	/**
	 * Sorts the active file list by date either in ascending or descending
	 * order depending on what the active sort is.
	 */
	BOOL sortFileListByDate(long lStart, long lLength);

	/**
	 * Sorts the active file list by extension either in ascending or 
	 * descending order depending on what the active sort is.
	 */
	BOOL sortFileListByExtension(long lStart, long lLength);

	// Parth Software Solution
	// sort func for dir
	BOOL SortDirsinDirs(int lStart,int Len);
	BOOL SortFilesInDirs(int lStart,int Len);
	BOOL sortFileListByDirOrFileFirst(long lStart, long lLength);
	BOOL SortUsingCriteria(long lStart, long lLength,ACTIVESORTENUM ActiveSortType);
	BOOL SortUsingCriteria(ACTIVESORTENUM ActiveSortType,WIN32_FIND_DATA *pwfdItemOne ,WIN32_FIND_DATA *pwfdItemTwo,long lcv);
	void ArrangeFoldersAndFiles();
	TCHAR * GetExtension(WIN32_FIND_DATA *pwfdItemOne);
		// Parth Software Solution
	/**
	 * Gets the currently selected items for the active File Manager and 
	 * stores it in the appropriate selection array (member field).
	 */
	BOOL getCurrentSelection(HWND hwndLastActive);

	/**
	 * Sets the currently selected items for the active File Manager.
	 */
	BOOL setCurrentSelection();

	/**
	 * Determines if the specified point is "inside" a command button in the
	 * "button bar." This method returns the Ctrl ID of the command button.
	 */
	long getCommandButtonAtPoint(POINT ptTest);

	/**
	 * Highlights the active command button on the button bar. "Active" is
	 * determined by whether or not the mouse is currently hovering over the
	 * button.
	 */
	BOOL highlightCommandButton(HDC hdcCommandButtons, POINT ptMouse);

	/**
	 * Draws the command button using whatever "state" is specified.
	 */
	BOOL drawCommandButton(int iButtonIndex, HDC hdcCommandButtons,
		COMMANDBUTTONSTATEENUM cbseState);

	/**
	 * Copies the filenames (ONLY) from the active File Manager's file
	 * list to the temporary "file manager" list for "lookup."
	 */
	BOOL copyFileListToLookupList();

	/**
	 * Waits until all painting (more or less) has been completed before
	 * returning.
	 */
	BOOL doEvents();

	/**
	 * Extracts the CADImporter DLL from the resource file if it cannot be
	 * found in the application folder.
	 */
	BOOL extractCADImporterLibrary();

public:

	///////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CMainWindow();

	/**
	 * Constructor which accepts the application instance.
	 */
	CMainWindow(HINSTANCE hInstance);
	
	/**
	 * Destructor, performs clean-up.
	 */
	~CMainWindow();
	
	///////////////////////////////////////////////////////////////////////////
	// Message Loop
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Handles processing of all messages sent to *this* window.
	 */
	static LRESULT CALLBACK WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, 
		LPARAM lParam);

  bool SetColorsForDC(HDC hdc, UINT ctrl_id, bool selected, COLORREF* bg=NULL, COLORREF* fg=NULL);

	/**
	 * Handles processing of all messages sent to File Manager 1.
	 */
	static LRESULT CALLBACK FileManager1WindowProc (HWND hwnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);

	//Parth Software Solution
	/**
	 * Handles processing of all messages sent to Tree view File Manager 1.
	 */
	static LRESULT CALLBACK FileManager3WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK FileManager4WindowProc (HWND hwnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam);
	//Parth Software Solution
	/**
	 * Handles processing of all messages sent to File Manager 2.
	 */
	static LRESULT CALLBACK FileManager2WindowProc (HWND hwnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);

	/**
	 * Handles processing of all messages sent to the command prompt.
	 */
	static LRESULT CALLBACK CommandPromptConsoleWindowProc (HWND hwnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);

  /**
	 * Handles processing of all messages sent to the command prompt.
	 */
	static LRESULT CALLBACK CommandPromptWindowProc (HWND hwnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);

	/**
	 * Handles processing of all messages sent to the function buttons.
	 */
	static LRESULT CALLBACK CommandButtonsWindowProc (HWND hwnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);

	/**
	 * Handles processing of all messages sent to the product name label.
	 */
	static LRESULT CALLBACK ProductNameWindowProc (HWND hwnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);

	/**
	 * Handles processing of all messages sent to the trial version label.
	 */
	static LRESULT CALLBACK TrialVersionWindowProc (HWND hwnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);

	///////////////////////////////////////////////////////////////////////////
	// Getter Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Returns the last error encountered, if any.
	 */
	TCHAR *getLastError() {return (TCHAR *)m_strLastError.c_str();}

	/**
	 * Returns whether or not *this* object is currently changing graphics
	 * modes (full screen to windowed or windowed to full screen).
	 */
	BOOL isChangingGraphicModes() {return m_bIsChangingWindowMode;}

	///////////////////////////////////////////////////////////////////////////
	// Setter Methods
	///////////////////////////////////////////////////////////////////////////
	

	///////////////////////////////////////////////////////////////////////////
	// UI Methods
	///////////////////////////////////////////////////////////////////////////

  HWND progress_bar_handle() const
  {
    return m_hwndProgressBar;
  }
  void progress_bar_handle(HWND hValue)
  {
    m_hwndProgressBar = hValue;
  }

  void EditsBkColor();
	
	/**
	 * Creates and displays this object's dialog.
	 */
	BOOL show(BOOL bFullScreen = FALSE);	

	//Parth Software Solution
	public:
		//TV_ITEM tvi;
		HTREEITEM Selected;
		//TV_INSERTSTRUCT tvinsert;   // struct to config out tree control
		HTREEITEM Parent;           // Tree item handle
		HTREEITEM Before;           // .......
		HTREEITEM Root;             // .......
		HIMAGELIST hImageList;      // Image list array hadle
		HBITMAP hBitMap;            // bitmap handler
		bool flagSelected;//=false;
	//Parth Software Solution

	/*
	* to Insert item
	*/
	int InsertTabItem(HWND hTab, LPTSTR pszText, int iid);

	/*
	* to Remove item
	*/	int RemoveTabItem(HWND hTab, int iid);

	/**
	* Parth Software Solution
	 * Set percentage as per curser cel of progress Bar
	 */
	void CMainWindow::SetPercentage(int iPercentage = 1);

	/**
	* Parth Software Solution
	 * Set percentage Range to calculate 100% as a result
	 */
	void SetPercentageRange(int iRange);

	/**
	* Parth Software Solution
	 * Get Selected Text from tree control
	 */
	bool GetSelectedItemText( HWND hWndControl , TCHAR* szText, int iSize, BOOL* IsFile=FALSE);

	/**
	* Parth Software Solution
	 * Get Full path of selected item
	 */
	bool GetSelectedItemParentPath(HWND hwndFileManager, HTREEITEM hTreeItem, TCHAR *strFullPath);

	/**
	* Parth Software Solution
	 * Get Full path of selected item
	 */
	bool DeleteAllChildItems(HWND hWnd, HTREEITEM hTreeItem);

	/**
	 * Parth Software Solution
	 * Get Full path of selected item
	 */	int GetSelectedItemsCount(HWND hWnd);
	
	/**
	 * Parth Software Solution
	 * Display Proper Error Message 
	 */
	tstring CheckErrorGivePromptMessage(int iErrorId);

	/**
	 * Parth Software Solution
	 * Select Next Item
	 */
	HTREEITEM SelectNextItem(HWND hWnd, HTREEITEM hSelectedTreeItem);

	/**
	//Parth Software Solution
	 * Select previous Item
	 */
	HTREEITEM SelectPreviousItem(HWND hWnd, HTREEITEM hSelectedTreeItem);

	/**
	 * Displays the directory listing for the specified folder using the
	 * specified control and "active" sort.
	 */
	BOOL displayDirectoryListingForCheckedItem_TV(HWND hwndOutputControl, 
	LinkedListEx<FILE_INFORMATION> *pllstOutput);

	/*execute the checked item */
	BOOL executeCheckedItem_TV(int iID);

	/*to set selected text band and text*/
	void SetSelectedTextAndBand();

	/*to get selected item in treeview not using checkBox*/
	HTREEITEM GetSelectedItem();

	BOOL SortExpandedItems_TV(int iID);

	BOOL displaySortedItems_TV(HWND hwndOutputControl, 
	LinkedListEx<FILE_INFORMATION> *pllstOutput, tstring strFullPath);

	/**
	 * Calculates control sizes and resizes them according to the current window
	 * size.
	 */ 
	BOOL refreshLayout(BOOL bAsOnStart = FALSE);

	//Parth Software Solution
	/**
	 * Gets the directory listing for the specified folder and then displays it
	 * by calling displayDirectoryListing() in Tree view explorer.
	 */
	BOOL getFileInformation(const TCHAR *tstrFullpath, HWND hwndOutputControl,
	LinkedListEx<FILE_INFORMATION> *pllstOutput);

	/* Create all the Tab pages dialogs in one page*/
	void CreateTabPageDialogs();

	//show only active page
	void ShowActivePage(int iPageIndex);

	//Get main window handle here
	HWND GetMainWindowHandle();

	//Start NamedPipe Thread here
	bool StartNamedPipeThread();

	static void OnScanDataReceivedCallBack(LPVOID lpParam);

	//Get selected items path
	char * GetSlectedItemsPath();

	void SendBackDatatoServer();

	//Set the tab control window position
	bool SetTabControlwindowPostion(int iLeft, int iTop, int iRight, int iBottom);

	//add virtual folder
	void AddVirtualFolder(BYTE *bytes);

	//Remove Virtual folder
	void RemoveVirtualFolder(BYTE *bytes);

	void ShowProgressDetails(BYTE *bytes);

	//Insert the virtual item in the tree control
	bool InsertItemInTreeControl(LPSTR lpParentPath, LPSTR lpNewItemName);

	//Expand all virtual folders
	void ExpandAllVirtualFolders();

	//parse the virtual folder string using tokenize
	bool ParseVirtualString(tstring tsVirtualFolders);

	//clear all virtual folder list
	void ClearVirtualFolderList();


	//Members for Treeview 2
		HTREEITEM Selected2;
		//TV_INSERTSTRUCT tvinsert;   // struct to config out tree control
		HTREEITEM Parent2;           // Tree item handle
		HTREEITEM Before2;           // .......
		HTREEITEM Root2;             // .......
		HIMAGELIST hImageList2;      // Image list array hadle
		HBITMAP hBitMap2;            // bitmap handler
		bool flagSelected2;//=false;
	//Parth Software Solution

		tstring m_strFullFilepath;
		CFirstTabDialog *m_pFirstTabDlg;
		CSecondTabDialog *m_pSecondTabDlg;
	//Create COM object here
		CIntropCom m_objIntropCom;	
	//Pipe Communication pointer object

	//Pipe communicator object pointer
	CXlvCommunicatorServer* m_pObjXlvCommunicatorServer;

	//Vector to store virtual folder path
	std::vector<tstring> m_vVirtualFolders;
};

#endif // End _CMAINWINDOW_
