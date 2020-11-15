#include "StdAfx.h"
#include <commctrl.h>

class CWin32TreeView
{
public:
	CWin32TreeView();
	~CWin32TreeView();
	static void			Iterate(HWND hTreeWnd, HTREEITEM hItem = NULL, bool retriveCurrentItem = false);
	static int			GetSelectedCount(HWND hTreeWnd);
	static HTREEITEM	GetCurrentSelectedItem(HWND hTreeWnd);
	static bool			GetWin32SelectedItemText( HWND hWndControl , TCHAR* szText, int iSize, BOOL* IsFile=FALSE);
	 /* Get Text From selected Item	 */
	static bool			GetItemText( HWND hWndControl, HTREEITEM hSelectedItem, TCHAR *szText, int iSize);
	 /* Get All selected items */
	static bool			TreeView_GetAllSelectedItems(HWND hwnd, std::vector<HTREEITEM> &vPaths);
	static bool			TreeView_GetAllSelectedItemsPath(HWND hwnd, std::vector<tstring> &vPaths);
	static bool			TreeView_SelectAll(HWND hwnd, bool bSelect = false, HTREEITEM hTreeItem = NULL);
	static void			SetItemBold(HWND hTreewnd, HTREEITEM hItem, BOOL bBold);
	static bool			TreeView_GetAllExpandedItems(HWND hwnd, std::vector<tstring> &vExItems);
	static bool			TreeView_ISExpandedItem(HWND hwnd, tstring strExpItem);

	static bool			GetSelectedItemParentPath(HWND hwndFileManager, HTREEITEM hTreeItem, TCHAR *strFullPath);
	static bool			GetItemFullPath(HWND hwndFileManager, HTREEITEM hTreeItem, tstring &strFullPath);
	static HTREEITEM	GetTreeItemUsingFullPath(HWND hwnd, tstring strFullPath);
public:
	static int			m_iWin32NodeCount;
	static int			m_iWin32CheckedItems;
	static HTREEITEM	m_hSlectedTreeItem;
	static HTREEITEM	m_hItemUsingFullPath;	
	static std::vector<HTREEITEM> m_vSelectedPaths;
	static std::vector<tstring> m_vExpandedItems;
	static std::vector<tstring> m_vSelectedItems;
	static bool			m_bSetSelectedPaths;
	static bool			m_bCheckUncheckAll;
	static bool			m_bFindItemUsingFullPath;	
	static bool			m_bCheck;
};
