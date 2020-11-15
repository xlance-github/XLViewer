#include <stdafx.h>
#include <sgAdditional.h>

sgFloat BoxLeft, BoxRight, BoxTop, BoxBottom;

LONG Round(float a)
{
	LONG fa = (LONG)a;
	
	float b = a - fa;
	if (b < 0)
		return (b <= -0.5) ? (fa - 1):fa;
	else
		return (b >= 0.5) ? (fa + 1):fa;
}

POINT GetPoint(FPOINT Point, POINT offset, sgFloat Scale)
{
	POINT P;
	P.x = Round((float)((Point.x - BoxLeft) * Scale)) + offset.x;
	P.y = Round((float)((-Point.y + BoxTop) * Scale)) + offset.y;
	return P;
}

void sgMoveTo(HDC hDC, POINT Point)
{
	MoveToEx(hDC, Point.x, Point.y, 0);  
}

void sgLineTo(HDC hDC, POINT Point)
{
	LineTo(hDC, Point.x, Point.y);
}

void sgSetPixel(HDC hDC, POINT Point, COLORREF Color)
{
	SetPixel(hDC, Point.x, Point.y, Color);
}

void DrawPolyInsteadSpline(LPFPOINT DP, int Count, LPPARAM Param)
{
int i;
POINT *Pts;
	Pts = new POINT [Count];
	for (i = 0; i < Count; i++)
		Pts[i] = GetPoint(DP[i], Param->offset, Param->Scale);
	Polyline(Param->hDC, Pts, Count);
	delete [] Pts;
}

