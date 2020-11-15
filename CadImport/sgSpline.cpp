#include <stdafx.h>
#include <sgSpline.h>

double N(int n, int i, sgFloat t, FPOINT *Knot)
{
	double v1, d1, v2, d2;

	if (!n)
		return ((Knot[i].x <= t) && (t < Knot[i + 1].x)) ? (float)1:(float)0;
	d1 = (Knot[i + n].x - Knot[i].x);
	v1 = (t - Knot[i].x) * N(n - 1, i, t, Knot);
	v1 = (d1 == 0) ? 0:(v1 / d1);

	d2 = (Knot[i + n + 1].x - Knot[i + 1].x);
	v2 = (Knot[i + n + 1].x - t) * N(n - 1, i + 1, t, Knot);
	v2 = (d2 == 0) ? 0:(v2 / d2);
	return v1 + v2;
}

FPOINT NURBS_3(FPOINT *DP, FPOINT *Knot, int j, sgFloat t)
{
	FPOINT R;
	int i;
	double Ni;

	R.x = 0;
	R.y = 0;
	R.z = 0;

	for (i = j - 3; i < j + 1; i++)
	{
		Ni = N(3, i, t, Knot);
		R.x = R.x + DP[i].x * Ni;
		R.y = R.y + DP[i].y * Ni;
		R.z = R.z + DP[i].z * Ni;
	}
	return R;
}

void DrawNURBS(FPOINT *DP, FPOINT *Knot, int Count, LPPARAM param)
{
	sgFloat t;
	sgFloat StepT;
	int j;
	POINT P;
	
	P = GetPoint(DP[0], param->offset, param->Scale);
	sgMoveTo(param->hDC, P);
	for (j = 3; j < Count; j++)
	{
		StepT = (Knot[j + 1].x - Knot[j].x) / 25;
		for (t = Knot[j].x; StepT && t < Knot[j + 1].x; t += StepT)
		{
			P = GetPoint(NURBS_3(DP, Knot, j, t), param->offset, param->Scale);
			sgLineTo(param->hDC, P);
		}
	}
	P = GetPoint(DP[Count - 1], param->offset, param->Scale);
	sgLineTo(param->hDC, P);
}

void DrawSpline(LPFPOINT CP, LPFPOINT FP, LPFPOINT Knot,
				int FCount, int CCount, LPPARAM Param)
{
	if (CCount == 0)
	  DrawPolyInsteadSpline(FP, FCount, Param);
	else
      DrawNURBS(CP, Knot, CCount, Param);
}
