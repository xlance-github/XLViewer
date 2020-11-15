#ifndef	__CCSPLITTER_H__
#define	__CCSPLITTER_H__

/********************************************************************************/
/* Include files								*/
/********************************************************************************/

#include <windows.h>
#include <windowsx.h>

/********************************************************************************/
/* Constant definitions								*/
/********************************************************************************/

#define	CCSP_DEFWIDTH		3	// Default split bar width in pixels

#define	CCSPN_FIRST		(0U-2000U)
#define	CCSPN_LAST		(0U-2019U)

// Splitter flags
#define	CCSP_POS		0x00000001
#define	CCSP_FPOS		0x00000002
#define	CCSP_LOWER		0x00000004
#define	CCSP_FLOWER		0x00000008
#define	CCSP_UPPER		0x00000010
#define	CCSP_FUPPER		0x00000020
#define	CCSP_WIDTH		0x00000040

// Splitter messages
#define	CCSPM_SETSPLIT		(WM_USER+100)
#define	CCSPM_SETHANDLE		(WM_USER+101)

#define	CCSPM_GETSPLIT		(WM_USER+200)
#define	CCSPM_GETHANDLE		(WM_USER+201)
#define	CCSPM_GETRECT		(WM_USER+202)

// Splitter notifications
#define	CCSPN_POSCHANGED	(CCSPN_FIRST-1)
#define	CCSPN_SPLITCHANGED	(CCSPN_FIRST-2)

// Splitter view IDs
#define	CCSPV_TOPLEFT		0
#define	CCSPV_TOPRIGHT		1
#define	CCSPV_BOTTOMLEFT	2
#define	CCSPV_BOTTOMRIGHT	3

#define	CCSPV_TOP		CCSPV_TOPLEFT
#define	CCSPV_BOTTOM		CCSPV_BOTTOMLEFT
#define	CCSPV_LEFT		CCSPV_TOPLEFT
#define	CCSPV_RIGHT		CCSPV_TOPRIGHT

// Splitter window class
#define	WC_CCSPLITTER		"CCSPLITTER"

/********************************************************************************/
/* Macro definitions								*/
/********************************************************************************/

#define	CCsplitter_GetHandle(hWnd,idView) \
	    (HWND)SendMessage ((hWnd), CCSPM_GETHANDLE, (idView), 0)

#define	CCsplitter_GetRect(hWnd,idView,lpRect) \
	    SendMessage ((hWnd), CCSPM_GETRECT, (idView), (LPARAM)(lpRect))

#define	CCsplitter_GetSplit(hWnd,lpSplitX,lpSplitY) \
	    SendMessage ((hWnd), CCSPM_GETSPLIT, (WPARAM)(lpSplitX), (LPARAM)(lpSplitY))


#define	CCsplitter_SetHandle(hWnd,idView,hView) \
	    SendMessage ((hWnd), CCSPM_SETHANDLE, (idView), (LPARAM)(hView))

#define	CCsplitter_SetSplit(hWnd,lpSplitX,lpSplitY) \
	    SendMessage ((hWnd), CCSPM_SETSPLIT, (WPARAM)(lpSplitX), (LPARAM)(lpSplitY))

/********************************************************************************/
/* Type definitions								*/
/********************************************************************************/

typedef	struct {
    UINT    cbSize;			// Size of CCSPLIT structure
    DWORD   flags;			// Set/get flags
    int	    pos;			// Split position
    int	    width;			// Split width
    int	    lower;			// Split lower (left/top) drag margin
    int	    upper;			// Split upper (right/bottom) drag margin
} CCSPLIT;

typedef	struct {
    NMHDR   hdr;			// WM_NOTIFY message header
    int	    x;				// X-parameter of notification
    int	    y;				// Y-parameter of notification
} NMCCSPLIT;

/********************************************************************************/
/* Function prototypes								*/
/********************************************************************************/

BOOL	InitCCsplitter (
    void
);

/********************************************************************************/
/* End of conditional compilation						*/
/********************************************************************************/

#endif	__CCSPLITTER_H__

/********************************************************************************/
/* End of file									*/
/********************************************************************************/
