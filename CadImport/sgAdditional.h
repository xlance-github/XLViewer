#ifndef _SGADD_
#define _SGADD_

#include <windows.h>
#include "cad.h"

typedef struct _PARAM
{
	HWND hWnd;
	HDC hDC;
	POINT offset;
	double Scale;
	LONG DrawMode;
	LONG CircleDrawMode;
	bool GetArcsCurves;
	bool GetTextsCurves;
	int *IsInsideInsert;
	VOID *pvList;
} PARAM, *LPPARAM;

typedef struct _LAYER
{
	char * Name;
	int Count;	
} LAYER, *LPLAYER;

LONG Round(float a);
POINT GetPoint(FPOINT Point, POINT offset, double Scale);
void sgMoveTo(HDC hDC, POINT Point);
void sgLineTo(HDC hDC, POINT Point);
void sgSetPixel(HDC hDC, POINT Point, COLORREF Color);
void DrawPolyInsteadSpline(LPFPOINT DP, int Count, LPPARAM Param);

extern double BoxLeft, BoxRight, BoxTop, BoxBottom;

#endif
