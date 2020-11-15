#include "Win32TreeView.h"

int CWin32TreeView::m_iWin32NodeCount = 0;
int CWin32TreeView::m_iWin32CheckedItems = 0;
HTREEITEM CWin32TreeView::m_hSlectedTreeItem = NULL;
bool CWin32TreeView::m_bSetSelectedPaths = false;
bool CWin32TreeView::m_bCheckUncheckAll = false;
bool CWin32TreeView::m_bFindItemUsingFullPath = false;
bool CWin32TreeView::m_bCheck = false;
std::vector<HTREEITEM> CWin32TreeView::m_vSelectedPaths;
std::vector<tstring> CWin32TreeView::m_vSelectedItems;
std::vector<tstring> CWin32TreeView::m_vExpandedItems;
HTREEITEM CWin32TreeView::m_hItemUsingFullPath = NULL;
tstring g_stringTempItem;

CWin32TreeView::CWin32TreeView()
{
	m_bSetSelectedPaths = false;
	m_iWin32NodeCount = 0;
	m_iWin32CheckedItems = 0;
	m_bCheckUncheckAll = false;
	m_bFindItemUsingFullPath = false;	
	m_bCheck = false;
	m_hSlectedTreeItem = NULL;
}

CWin32TreeView::~CWin32TreeView()
{
}

void CWin32TreeView::Iterate(HWND hTreeWnd, HTREEITEM hItem, bool retriveCurrentItem)
{
	if (hItem)
	{
		// Use the tree node corresponding to hItem
		// .....
		// End of using hItem
		hItem = TreeView_GetNextItem(hTreeWnd, hItem, TVGN_CHILD);
		while (hItem)
		{
			m_iWin32NodeCount++;
			if(TreeView_GetCheckState(hTreeWnd, hItem))
			{
				TCHAR szTempString[MAX_PATH] = {0};
				GetItemText(hTreeWnd, hItem, szTempString, MAX_PATH);
				OutputDebugString(szTempString);
				if(m_bSetSelectedPaths)
				{
					m_vSelectedPaths.push_back(hItem);

					tstring strfullpath = "";
					GetItemFullPath(hTreeWnd, hItem, strfullpath);
					m_vSelectedItems.push_back(strfullpath);
				}
				else if(retriveCurrentItem)
				{
					m_hSlectedTreeItem = hItem;
					break;
				}
				m_iWin32CheckedItems++;
			}
			//Check for child item if present, ready to sort
			HTREEITEM hCheckChild = TreeView_GetChild(hTreeWnd, hItem);
			if(hCheckChild)
			{
				tstring szFullItemPath;
				GetItemFullPath(hTreeWnd, hItem, szFullItemPath);
				if(szFullItemPath.length() > 0)
					m_vExpandedItems.push_back(szFullItemPath);
			}
			if(m_bCheckUncheckAll)
			{
				TreeView_SetCheckState(hTreeWnd, hItem, m_bCheck)
			}
			if(m_bFindItemUsingFullPath)
			{
				tstring szFullItemPath;
				GetItemFullPath(hTreeWnd, hItem, szFullItemPath);
				if(szFullItemPath.length() > 0)
				{
					if(g_stringTempItem.length() > 0)
					{
						if(g_stringTempItem == szFullItemPath)
						{
							m_hItemUsingFullPath = hItem;
						}
					}
				}
			}
			CWin32TreeView::Iterate(hTreeWnd, hItem, retriveCurrentItem);
			hItem = TreeView_GetNextItem(hTreeWnd, hItem, TVGN_NEXT);
		}
	}
	else
	{
		m_hSlectedTreeItem = NULL;
		m_vSelectedPaths.clear();
		m_vExpandedItems.clear();
		m_vSelectedItems.clear();
		HTREEITEM hItem = TreeView_GetNextItem(hTreeWnd, NULL, TVGN_ROOT);
		while (hItem)
		{
			m_iWin32NodeCount++;
			if(TreeView_GetCheckState(hTreeWnd, hItem))
			{
				TCHAR szTempString[MAX_PATH] = {0};
				GetItemText(hTreeWnd, hItem, szTempString, MAX_PATH);
				OutputDebugString(szTempString);
				if(m_bSetSelectedPaths)
				{
					m_vSelectedPaths.push_back(hItem);
					tstring strfullpath = "";
					GetItemFullPath(hTreeWnd, hItem, strfullpath);
					m_vSelectedItems.push_back(strfullpath);
				}
				else if(retriveCurrentItem)
				{
					m_hSlectedTreeItem = hItem;
					break;
				}
				m_iWin32CheckedItems++;
			}
			//Check for child item if present, ready to sort
			HTREEITEM hCheckChild = TreeView_GetChild(hTreeWnd, hItem);
			if(hCheckChild)
			{
				tstring szFullItemPath;
				GetItemFullPath(hTreeWnd, hItem, szFullItemPath);
				if(szFullItemPath.length() > 0)
					m_vExpandedItems.push_back(szFullItemPath);
			}

			//if(m_bCheckUncheckAll)
			//{
			//	TreeView_SetCheckState(hTreeWnd, hItem, m_bCheck)
			//}
			if(m_bFindItemUsingFullPath)
			{
				tstring szFullItemPath;
				GetItemFullPath(hTreeWnd, hItem, szFullItemPath);
				if(szFullItemPath.length() > 0)
				{
					if(g_stringTempItem.length() > 0)
					{
						if(g_stringTempItem == szFullItemPath)
						{
							m_hItemUsingFullPath = hItem;
						}
					}
				}
			}
			CWin32TreeView::Iterate(hTreeWnd, hItem, retriveCurrentItem);
			hItem = TreeView_GetNextItem(hTreeWnd, hItem, TVGN_NEXT);
		}
	}
}


int CWin32TreeView::GetSelectedCount(HWND hTreeWnd)
{
	CWin32TreeView::m_iWin32NodeCount = 0;
	CWin32TreeView::m_iWin32CheckedItems = 0;
	Iterate(hTreeWnd, NULL);
	return CWin32TreeView::m_iWin32CheckedItems;
}

HTREEITEM CWin32TreeView::GetCurrentSelectedItem(HWND hTreeWnd)
{
	Iterate(hTreeWnd, NULL, true);
	return m_hSlectedTreeItem;
}

/***********************************************************************************
	Function Name:	GetSelectedItemText
	In Parameters:	HWND hWndControl , TCHAR *szText, int iSize, BOOL* IsFile
	Out Parameters: bool
	Description:	To get the selected item text.
	Date & Time:	19th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
bool CWin32TreeView::GetWin32SelectedItemText( HWND hWndControl , TCHAR *szText, int iSize, BOOL* IsFile)
{
	// Get a handle to the selected item
	HTREEITEM hSelectedItem = GetCurrentSelectedItem(hWndControl);

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

/***********************************************************************************
	Function Name:	GetItemText
	In Parameters:	HWND hWndControl, HTREEITEM hSelectedItem, TCHAR *szText, int iSize
	Out Parameters: bool
	Description:	To get the selected item text.
	Date & Time:	19th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
bool CWin32TreeView::GetItemText( HWND hWndControl, HTREEITEM hSelectedItem, TCHAR *szText, int iSize)
{
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
			// Parth Software Solution //bug fixing
			std::string fullfilename = item.pszText;
			int found=fullfilename.find(SEARCHKEY_DIRECTORY);
			if (found!= tstring::npos)
			{					
				int found1 = fullfilename.find("|");
				tstring tmpfile = fullfilename.substr(found1 + 1);
				int found2 = tmpfile.find("|");
				tmpfile = fullfilename.substr(found1 + 1,found2-1);

				StringTrim(tmpfile);				
				TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
				_tcscpy(szText,tmp1);

			}
			// it is file
			else
			{
				int found1 = fullfilename.find("|");
				tstring tmpfile = fullfilename.substr(found1 + 1);
				int found2 = tmpfile.find("|");
				tmpfile = fullfilename.substr(found1 + 1,found2-1);

				StringTrim(tmpfile);				

				TCHAR* tmp1 =  (TCHAR*)tmpfile.c_str();
				_tcscpy(szText,tmp1);
				//*IsFile = TRUE;
			}
			// Parth Software Solution
			return true;
		}
	}
	catch(...)
	{
		// set last error
		OutputDebugString(_T("Error occured in CMainWindow::GetItemText"));
		return false;
	}
	return false;
}

bool CWin32TreeView::TreeView_GetAllSelectedItems(HWND hwnd, std::vector<HTREEITEM> &vPaths)
{
	m_bSetSelectedPaths = true;
	Iterate(hwnd, NULL);
	vPaths = m_vSelectedPaths;
	m_bSetSelectedPaths = false;
	return true;
}

bool CWin32TreeView::TreeView_GetAllSelectedItemsPath(HWND hwnd, std::vector<tstring> &vPaths)
{
	m_bSetSelectedPaths = true;
	Iterate(hwnd, NULL);
	vPaths = m_vSelectedItems;
	m_bSetSelectedPaths = false;
	return true;
}

bool CWin32TreeView::TreeView_ISExpandedItem(HWND hwnd, tstring strExpItem)
{
	Iterate(hwnd, NULL);
	bool bFound = false;
	std::vector<tstring>::const_iterator itExpItems;
	for( itExpItems = m_vExpandedItems.begin(); itExpItems !=  m_vExpandedItems.end(); 
		itExpItems++)
	{
		tstring tsExpItems;
		tsExpItems.assign((*itExpItems).c_str());
		if(tsExpItems.c_str() == strExpItem)
		{
			bFound = true;
			break;		
		}
	}	
	return bFound;
}

bool CWin32TreeView::TreeView_GetAllExpandedItems(HWND hwnd, std::vector<tstring> &vExItems)
{
	Iterate(hwnd, NULL);
	vExItems = m_vExpandedItems;
	return true;
}

bool CWin32TreeView::TreeView_SelectAll(HWND hwnd, bool bSelect, HTREEITEM hTreeItem)
{
	m_bCheckUncheckAll = true;
	m_bCheck = bSelect;
	Iterate(hwnd, hTreeItem);
	m_bCheckUncheckAll = false;
	return true;
}

void CWin32TreeView::SetItemBold(HWND hTreewnd, HTREEITEM hItem, BOOL bBold)
{
	TreeView_SetItemState( hTreewnd, hItem, bBold ? TVIS_BOLD: 0, TVIS_BOLD );
}

/***********************************************************************************
	Function Name:	GetSelectedItemParentPath
	In Parameters:	HWND hwndFileManager, HTREEITEM hTreeItem, TCHAR *strFullPath
	Out Parameters: bool
	Description:	To Get the parent Path of selected item.
	Date & Time:	19th Sep 2012
	Developer:		Parth Software Solution
***********************************************************************************/
bool CWin32TreeView::GetSelectedItemParentPath(HWND hwndFileManager, HTREEITEM hTreeItem, TCHAR *strFullPath)
{
	if (hwndFileManager == NULL && hTreeItem == NULL) // Nothing selected
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
			// Parth Software Solution
			if(strcmp(str.c_str(),TVFOLDER_ROOT) != 0)
			{				
				_tcscat(strFullPath , str.c_str());
				_tcscat(strFullPath , "\\");
			}
		} 
	}
	catch(...)
	{
		return false;
	}
	return true;
}

bool CWin32TreeView::GetItemFullPath(HWND hwndFileManager, HTREEITEM hTreeItem, tstring &strFullPath)
{

	TCHAR szFullPath[MAX_PATH] = {0}, szTempString[MAX_PATH] = {0};
	if(GetSelectedItemParentPath(hwndFileManager, hTreeItem, szFullPath))
	{
		GetItemText(hwndFileManager, hTreeItem, szTempString, MAX_PATH);
		strFullPath.assign(szFullPath);
		strFullPath.append(szTempString);
	}
	return true;
}

HTREEITEM CWin32TreeView::GetTreeItemUsingFullPath(HWND hwnd, tstring strFullPath)
{
	m_hItemUsingFullPath = NULL;
	g_stringTempItem = strFullPath;
	m_bFindItemUsingFullPath = true;
	Iterate(hwnd, NULL);
	m_bFindItemUsingFullPath = false;
	g_stringTempItem = "";
	return m_hItemUsingFullPath;
}