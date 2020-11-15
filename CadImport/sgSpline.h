#ifndef _SGSPLINE_
#define _SGSPLINE_
	#include <cad.h>
	#include <sgAdditional.h>

	double N(int n, int i, float t, FPOINT *Knot);
	FPOINT NURBS_3(FPOINT *DP, FPOINT *Knot, int j, float t);
	void DrawNURBS(FPOINT *DP, FPOINT *Knot, int Count, LPPARAM param);
	void DrawSpline(LPFPOINT CP, LPFPOINT FP, LPFPOINT Knot,
					int FCount, int CCount, LPPARAM Param);
#endif

