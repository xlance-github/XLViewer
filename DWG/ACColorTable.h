#ifndef _ACCOLORTABLE_
#define _ACCOLORTABLE_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor    
// Purpose:   AutoCad(r) color table definition and structures 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "..\XLanceView.h"

// There are basically two types of colors defined for AutoCad drawings:
//	 "Indexed" and "True" color. The latter results in RGB values being
//	 stored in the drawing whereas the former has an index and is a 
//	 direct reference to a built-in immutable color table.

#define MAXLENGTH_COLORNAME			30

/**
 * AC Color Table Entry structure.
 */
typedef struct _ACCOLOR_ENTRY
{
	COLORREF clrColor;
	TCHAR tstrName[MAXLENGTH_COLORNAME];
	int iIndex;

	/**
	 * Default constructor
	 */
	_ACCOLOR_ENTRY()
	{
		// initialize members to their defaults
		_tcscpy(tstrName, _T("White"));
		clrColor = RGB(255, 255, 255);
		iIndex = 0;
	}

	/**
	 * Constructor which accepts and index and COLORREF.
	 */
	_ACCOLOR_ENTRY(int iTableIndex, COLORREF clrTableColor,
		TCHAR *tstrTableName)
	{
		// initialize fields with in params
		_tcscpy(tstrName, tstrTableName);
		clrColor = clrTableColor;
		iIndex = iTableIndex;
	}
}ACCOLOR_ENTRY, *PACCOLOR_ENTRY;

#endif