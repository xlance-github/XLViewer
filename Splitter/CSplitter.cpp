/********************************************************************************/
/* Include files								*/
/********************************************************************************/

#include "CSplitter.h"

/********************************************************************************/
/* Constant definitions								*/
/********************************************************************************/

#define	GWL_SPINFO  0			// GetWindowLong offset of SPINFO ptr

#define	NVIEWS	    4			// Number of View windows

/********************************************************************************/
/* Macro definitions								*/
/********************************************************************************/

#define	SPLT(pos,w)			((pos) - (w)/2 + 1)
#define	SPRB(pos,w)			((pos) - (w)/2 + 1 + (w))

#define	SPLEFT				SPLT
#define	SPTOP				SPLT
#define	SPRIGHT				SPRB
#define	SPBOTTOM			SPRB

#define	CURSORPOS_OK(cur,spl,w)		((spl)>=0 && (cur)>=SPLT(spl,w) && (cur)<=SPRB(spl,w))

/********************************************************************************/
/* Type definitions								*/
/********************************************************************************/

typedef	struct	spinfo	SPINFO;		// Splitter info

struct	spinfo {
    HWND    hView[NVIEWS];		// View window handles
    int	    xpos;			// X-position of split (-1 = no split)
    int	    ypos;			// Y-position of split (-1 = no split)
    int	    xwidth;			// X-width of split bar
    int	    ywidth;			// Y-width of split bar
    int	    xlower;			// X-lower drag margin
    int	    xupper;			// X-upper drag margin
    int	    ylower;			// Y-lower drag margin
    int	    yupper;			// Y-upper drag margin high
    int	    xdrag;			// X-position during drag (-1 = no drag)
    int	    ydrag;			// Y-position during drag (-1 = no drag)
    int	    dragDone;			// Drag operation completed flag
};

/********************************************************************************/
/* Function prototypes								*/
/********************************************************************************/

static	LRESULT CALLBACK    SpWindowProc (
    HWND	hSplitter,			// Splitter handle
    UINT	uMsg,			// Message code
    WPARAM	wParam,			// Message parameter
    LPARAM	lParam			// Message parameter
);

static	int	SpGetRect (
    HWND	hSplitter,		// Splitter window handle
    int		id,			// Rectangle ID
    RECT       *prc			// Rectangle data ptr
);

static	void	SpNotifyParent (
    HWND	hSplitter,		// Splitter window handle
    int		code,			// Notification code
    int		xParam,			// X-parameter to notification
    int		yParam			// Y-parameter to notification
);

static	void	SpPositionViewWindows (
    HWND	hSplitter		// Splitter window handle
);

/********************************************************************************/
/* Private date definitions							*/
/********************************************************************************/

static	HCURSOR	hcurVert;		// Cursor for North/South split
static	HCURSOR	hcurHorz;		// Cursor for East/West split
static	HCURSOR	hcurBoth;		// Cursor for double split
static	HBRUSH	hbrDotty;		// Dotty brush handle

static	WORD	dottyData[] = {
    0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA
};

/********************************************************************************/
/* Public procedure definitions							*/
/********************************************************************************/

BOOL	InitCCsplitter ()
{
    WNDCLASSEX	wc;			// Window class data
    HBITMAP	hbm;			// Bitmap handle

    if (GetClassInfoEx (GetModuleHandle (0), WC_CCSPLITTER, &wc))
	return TRUE;

    hcurVert = LoadCursor (NULL, IDC_SIZENS);
    hcurHorz = LoadCursor (NULL, IDC_SIZEWE);
    hcurBoth = LoadCursor (NULL, IDC_SIZEALL);

    hbm      = CreateBitmap (8, 8, 1, 1, dottyData);
    hbrDotty = CreatePatternBrush (hbm);
    DeleteObject (hbm);

    ZeroMemory (&wc, sizeof(wc));
    wc.cbSize	     = sizeof (wc);
    wc.cbWndExtra    = sizeof (SPINFO *);
    wc.hInstance     = GetModuleHandle (0);
    wc.hCursor	     = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush (COLOR_ACTIVEBORDER);
    wc.lpfnWndProc   = SpWindowProc;
    wc.lpszClassName = WC_CCSPLITTER;

    return RegisterClassEx (&wc) ? TRUE : FALSE;
}

/********************************************************************************/
/* Private procedure definitions - window procedure				*/
/********************************************************************************/

static	LRESULT CALLBACK    SpWindowProc (
	HWND	hSplitter,		// Splitter window handle
	UINT	uMsg,			// Message code
	WPARAM	wParam,			// Message parameter
	LPARAM	lParam)			// Message parameter
{
    CCSPLIT    *spx;			// X-split data ptr
    CCSPLIT    *spy;			// Y-split data ptr
    SPINFO     *sp;			// Splitter info
    RECT	rc;			// Window rectangle
    HDC		hdc;			// Device Context handle
    HBRUSH	hbr;			// Brush handle
    POINT	pt;			// Coordinate of point
    DWORD	dw;			// Double word temp

    sp = (SPINFO *)GetWindowLong (hSplitter, GWL_SPINFO);

    switch (uMsg) {
	case WM_CAPTURECHANGED:
		if (sp->xdrag >= 0 || sp->ydrag >= 0) {
		    GetClientRect (hSplitter, &rc);

		    hdc = GetDCEx (hSplitter, NULL, DCX_PARENTCLIP);
		    hbr = static_cast<HBRUSH>(SelectObject (hdc, hbrDotty));

		    if (sp->ydrag >= 0) {
			PatBlt (hdc, 0, SPTOP (sp->ydrag, sp->ywidth), rc.right, sp->ywidth, PATINVERT);
			if (sp->dragDone)
			    sp->ypos = sp->ydrag;
			sp->ydrag = -1;
		    }
		    if (sp->xdrag >= 0) {
			PatBlt (hdc, SPLEFT (sp->xdrag, sp->xwidth), 0, sp->xwidth, rc.bottom, PATINVERT);
			if (sp->dragDone)
			    sp->xpos = sp->xdrag;
			sp->xdrag = -1;
		    }

		    SelectObject (hdc, hbr);
		    ReleaseDC (hSplitter, hdc);

		    if (sp->dragDone) {
			int	xold = sp->xpos;    // Old X-position
			int	yold = sp->ypos;    // Old Y-position

			if (sp->xpos <= 0 || sp->xpos >= rc.right)
			    sp->xpos = -1;
			if (sp->ypos <= 0 || sp->ypos >= rc.bottom)
			    sp->ypos = -1;
			sp->dragDone = FALSE;

			SpPositionViewWindows (hSplitter);

			if ((xold>=0 && sp->xpos<0) || (yold>=0 && sp->ypos<0))
			    SpNotifyParent (hSplitter, CCSPN_SPLITCHANGED, sp->xpos>=0, sp->ypos>=0);
		    }
		}
		return TRUE;

	case WM_CREATE:
    sp = static_cast<SPINFO*>( GlobalAlloc (GPTR, sizeof (*sp)) );

		sp->xpos   = sp->xdrag  = sp->ypos   = sp->ydrag  = -1;
		sp->xlower = sp->xupper = sp->ylower = sp->yupper = 0;
		sp->xwidth =              sp->ywidth              = CCSP_DEFWIDTH;

		SetWindowLong (hSplitter, GWL_SPINFO, (long)sp);
		SetWindowLong (hSplitter, GWL_STYLE, GetWindowLong (hSplitter, GWL_STYLE) | WS_CLIPSIBLINGS);
		break;

	case WM_DESTROY:
		GlobalFree (sp);
		break;

	case WM_LBUTTONDOWN:
		dw = GetMessagePos ();
		pt.x = GET_X_LPARAM (dw);
		pt.y = GET_Y_LPARAM (dw);
		ScreenToClient (hSplitter, &pt);

		sp->xdrag = CURSORPOS_OK (pt.x, sp->xpos, sp->xwidth) ? pt.x : -1;
		sp->ydrag = CURSORPOS_OK (pt.y, sp->ypos, sp->ywidth) ? pt.y : -1;

		if (sp->xdrag >=0 || sp->ydrag >= 0) {
		    GetClientRect (hSplitter, &rc);

		    hdc = GetDCEx (hSplitter, NULL, DCX_PARENTCLIP);
		    hbr = static_cast<HBRUSH>(SelectObject (hdc, hbrDotty));

		    if (sp->xdrag >= 0)
			PatBlt (hdc, SPLEFT (sp->xdrag, sp->xwidth), 0, sp->xwidth, rc.bottom, PATINVERT);
		    if (sp->ydrag >= 0)
			PatBlt (hdc, 0, SPTOP (sp->ydrag, sp->ywidth), rc.right, sp->ywidth, PATINVERT);

		    SelectObject (hdc, hbr);
		    ReleaseDC (hSplitter, hdc);

		    SetCapture (hSplitter);
		}
		return TRUE;

	case WM_LBUTTONUP:
		if (sp->xdrag >= 0 || sp->ydrag >= 0) {
		    sp->dragDone = TRUE;
		    ReleaseCapture ();
		}
		return TRUE;

	case WM_MOUSEMOVE:
		if (sp->xdrag >= 0 || sp->ydrag >= 0) {
		    GetClientRect (hSplitter, &rc);

		    dw = GetMessagePos ();
		    pt.x = GET_X_LPARAM (dw);
		    pt.y = GET_Y_LPARAM (dw);
		    ScreenToClient (hSplitter, &pt);

		    if (pt.x < sp->xlower)
			pt.x = sp->xlower;
		    if (pt.y < sp->ylower)
			pt.y = sp->ylower;
		    if (pt.x > rc.right  - sp->xupper)
			pt.x = rc.right  - sp->xupper;
		    if (pt.y > rc.bottom - sp->yupper)
			pt.y = rc.bottom - sp->yupper;

		    hdc = GetDCEx (hSplitter, NULL, DCX_PARENTCLIP);
		    hbr = static_cast<HBRUSH>(SelectObject (hdc, hbrDotty) );

		    if (sp->ydrag >= 0) {
			PatBlt (hdc, 0, SPTOP (sp->ydrag, sp->ywidth), rc.right, sp->ywidth, PATINVERT);
			sp->ydrag = pt.y;
		    }
		    if (sp->xdrag >= 0) {
			PatBlt (hdc, SPLEFT (sp->xdrag, sp->xwidth), 0, sp->xwidth, rc.bottom, PATINVERT);
			sp->xdrag = pt.x;
			PatBlt (hdc, SPLEFT (sp->xdrag, sp->xwidth), 0, sp->xwidth, rc.bottom, PATINVERT);
		    }
		    if (sp->ydrag >= 0) {
			PatBlt (hdc, 0, SPTOP (sp->ydrag, sp->ywidth), rc.right, sp->ywidth, PATINVERT);
		    }

		    SelectObject (hdc, hbr);
		    ReleaseDC (hSplitter, hdc);
		}
		return TRUE;

	case WM_SETCURSOR:
		dw = GetMessagePos ();
		pt.x = GET_X_LPARAM (dw);
		pt.y = GET_Y_LPARAM (dw);
		ScreenToClient (hSplitter, &pt);

		if (CURSORPOS_OK (pt.x, sp->xpos, sp->xwidth)) {
		    if (CURSORPOS_OK (pt.y, sp->ypos, sp->ywidth))
			SetCursor (hcurBoth);
		    else
			SetCursor (hcurHorz);
		    return TRUE;
		}
		else
		if (CURSORPOS_OK (pt.y, sp->ypos, sp->ywidth)) {
		    SetCursor (hcurVert);
		    return TRUE;
		}
		break;

	case WM_SIZE:
		SpPositionViewWindows (hSplitter);
		break;

	case CCSPM_GETHANDLE:
		switch (wParam) {
		    case CCSPV_TOPLEFT:
		    case CCSPV_TOPRIGHT:
		    case CCSPV_BOTTOMLEFT:
		    case CCSPV_BOTTOMRIGHT:
			    return (LRESULT)sp->hView[wParam];
		}
		return (LRESULT)NULL;

	case CCSPM_GETRECT:
		return SpGetRect (hSplitter, wParam, (RECT *)lParam);

	case CCSPM_GETSPLIT:
		if ((spx = (CCSPLIT *)wParam) != NULL)
		    if (spx->cbSize != sizeof (*spx))
			return FALSE;

		if ((spy = (CCSPLIT *)lParam) != NULL)
		    if (spy->cbSize != sizeof (*spy))
			return FALSE;
		
		if (spx) {
		    spx->flags = CCSP_POS|CCSP_LOWER|CCSP_UPPER|CCSP_WIDTH;
		    spx->pos   = sp->xpos;
		    spx->lower = sp->xlower;
		    spx->upper = sp->xupper;
		    spx->width = sp->xwidth;
		}

		if (spy) {
		    spy->flags = CCSP_POS|CCSP_LOWER|CCSP_UPPER|CCSP_WIDTH;
		    spy->pos   = sp->ypos;
		    spy->lower = sp->ylower;
		    spy->upper = sp->yupper;
		    spy->width = sp->ywidth;
		}

		return TRUE;

	case CCSPM_SETHANDLE:
		switch (wParam) {
		    case CCSPV_TOPLEFT:
		    case CCSPV_TOPRIGHT:
		    case CCSPV_BOTTOMLEFT:
		    case CCSPV_BOTTOMRIGHT:
			    if ((HWND)lParam == NULL || IsWindow ((HWND)lParam)) {
				if (sp->hView[wParam] != NULL && sp->hView[wParam] != (HWND)lParam)
				    ShowWindow (sp->hView[wParam], SW_HIDE);
				sp->hView[wParam] = (HWND)lParam;
				SetWindowPos (hSplitter, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
				SpPositionViewWindows (hSplitter);
				return TRUE;
			    }
			    break;
		}
		return FALSE;

	case CCSPM_SETSPLIT:
	        GetClientRect (hSplitter, &rc);

		if ((spx = (CCSPLIT *)wParam) != NULL)
		    if (spx->cbSize != sizeof (*spx))
			return FALSE;

		if ((spy = (CCSPLIT *)lParam) != NULL)
		    if (spy->cbSize != sizeof (*spy))
			return FALSE;

		if (spx) {
		    if (spx->flags & CCSP_FPOS)	    sp->xpos   = spx->pos * rc.right / 100;
		    if (spx->flags & CCSP_POS)	    sp->xpos   = spx->pos;
		    if (sp->xpos <= 0)		    sp->xpos   = -1;

		    if (spx->flags & CCSP_FLOWER)   sp->xlower = spx->lower * rc.right / 100;
		    if (spx->flags & CCSP_LOWER)    sp->xlower = spx->lower;
		    if (sp->xlower <= 0)	    sp->xlower = 0;

		    if (spx->flags & CCSP_FUPPER)   sp->xupper = spx->upper * rc.right / 100;
		    if (spx->flags & CCSP_UPPER)    sp->xupper = spx->upper;
		    if (sp->xupper <= 0)	    sp->xupper = 0;

		    if (spx->flags & CCSP_WIDTH)    sp->xwidth = spx->width;
		    if (sp->xwidth < CCSP_DEFWIDTH) sp->xwidth = CCSP_DEFWIDTH;
		}

		if (spy) {
		    if (spy->flags & CCSP_FPOS)	    sp->ypos   = spy->pos * rc.bottom / 100;
		    if (spy->flags & CCSP_POS)	    sp->ypos   = spy->pos;
		    if (sp->ypos <= 0)		    sp->ypos   = -1;

		    if (spy->flags & CCSP_FLOWER)   sp->ylower = spy->lower * rc.bottom / 100;
		    if (spy->flags & CCSP_LOWER)    sp->ylower = spy->lower;
		    if (sp->ylower <= 0)	    sp->ylower = 0;

		    if (spy->flags & CCSP_FUPPER)   sp->yupper = spy->upper * rc.bottom / 100;
		    if (spy->flags & CCSP_UPPER)    sp->yupper = spy->upper;
		    if (sp->yupper <= 0)	    sp->yupper = 0;

		    if (spy->flags & CCSP_WIDTH)    sp->ywidth = spy->width;
		    if (sp->ywidth < CCSP_DEFWIDTH) sp->ywidth = CCSP_DEFWIDTH;
		}

		SpPositionViewWindows (hSplitter);
		SpNotifyParent (hSplitter, CCSPN_SPLITCHANGED, sp->xpos>=0, sp->ypos>=0);
		return TRUE;
    }

    return DefWindowProc (hSplitter, uMsg, wParam, lParam);
}

/********************************************************************************/
/* Private procedure definitions - utility functions				*/
/********************************************************************************/

static	int	SpGetRect (
	HWND	hSplitter,		// Splitter window handle
	int	id,			// Rectangle ID
	RECT   *prc)			// Rectangle data ptr
{
    RECT    rc;				// Window rectangle
    SPINFO *sp;				// Splitter info ptr

    if (prc == NULL)
	return FALSE;

    SetRect (prc, 0, 0, 0, 0);
    GetClientRect (hSplitter, &rc);

    sp = (SPINFO *)GetWindowLong (hSplitter, GWL_SPINFO);

    switch (id) {
	case CCSPV_TOPLEFT:
		prc->left   = 0;
		prc->top    = 0;
		prc->right  = sp->xpos>=0 ? SPLEFT (sp->xpos, sp->xwidth) : rc.right;
		prc->bottom = sp->ypos>=0 ? SPTOP  (sp->ypos, sp->ywidth) : rc.bottom;
		return TRUE;

	case CCSPV_TOPRIGHT:
		if (sp->xpos < 0)
		    break;
		prc->left   = SPRIGHT (sp->xpos, sp->xwidth);
		prc->top    = 0;
		prc->right  = rc.right;
		prc->bottom = sp->ypos>=0 ? SPTOP (sp->ypos, sp->ywidth) : rc.bottom;
		return TRUE;

	case CCSPV_BOTTOMLEFT:
		if (sp->ypos < 0)
		    break;
		prc->left   = 0;
		prc->top    = SPBOTTOM (sp->ypos, sp->ywidth);
		prc->right  = sp->xpos>=0 ? SPLEFT (sp->xpos, sp->xwidth) : rc.right;
		prc->bottom = rc.bottom;
		return TRUE;

	case CCSPV_BOTTOMRIGHT:
		if (sp->ypos < 0 || sp->xpos < 0)
		    break;
		prc->left   = SPRIGHT  (sp->xpos, sp->xwidth);
		prc->top    = SPBOTTOM (sp->ypos, sp->ywidth);
		prc->right  = rc.right;
		prc->bottom = rc.bottom;
		return TRUE;
    }

    // Rectangle does not exist...
    return FALSE;
}

static	void	SpNotifyParent (
	HWND	hSplitter,		// Splitter window handle
	int	code,			// Notification code
	int	xParam,			// X-parameter to notification
	int	yParam)			// Y-parameter to notification
{
    NMCCSPLIT	notify;			// Notification data

    notify.hdr.hwndFrom	= hSplitter;
    notify.hdr.idFrom	= GetWindowLong (hSplitter, GWL_ID);
    notify.hdr.code	= code;
    notify.x		= xParam;
    notify.y		= yParam;

    SendMessage (GetParent (hSplitter), WM_NOTIFY, notify.hdr.idFrom, (LPARAM)&notify);
}

static	void	SpPositionViewWindows (
	HWND	hSplitter)		// Splitter window handle
{
    SPINFO *sp;				// Splitter info ptr
    HDWP    hDefer;			// DeferWindowPos handle
    RECT    rc;				// View window rectangle
    int	    i;				// Loop counter

    sp = (SPINFO *)GetWindowLong (hSplitter, GWL_SPINFO);

    hDefer = BeginDeferWindowPos (NVIEWS);
    for (i=0; i<NVIEWS; i++) {
	if (sp->hView[i] == NULL)
	    continue;
	if (SpGetRect (hSplitter, i, &rc)) {
	    MapWindowPoints (hSplitter, GetParent (sp->hView[i]), (POINT *)&rc, 2);
	    hDefer = DeferWindowPos (hDefer, sp->hView[i], NULL, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
	} else {
	    ShowWindow (sp->hView[i], SW_HIDE);
	}
    }
    EndDeferWindowPos (hDefer);

    SpNotifyParent (hSplitter, CCSPN_POSCHANGED, sp->xpos, sp->ypos);
}

/********************************************************************************/
/* End of file									*/
/********************************************************************************/
