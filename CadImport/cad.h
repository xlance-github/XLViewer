#ifndef _CAD_
#define _CAD_

#include <windows.h>

#define CAD_SEC_TABLES   0
#define CAD_SEC_BLOCKS   1
#define CAD_SEC_ENTITIES 2
#define CAD_SEC_LTYPE    3
#define CAD_SEC_LAYERS   4

#define CAD_UNKNOWN     0
#define CAD_TABLE       1
#define CAD_BLOCK       2
#define CAD_LTYPE       3
#define CAD_LAYER       4
#define CAD_VERTEX      5
#define CAD_LINE        6
#define CAD_SOLID       7
#define CAD_CIRCLE      8
#define CAD_ARC         9
#define CAD_POLYLINE   10
#define CAD_LWPOLYLINE 11
#define CAD_SPLINE     12
#define CAD_INSERT     13
#define CAD_DIMENSION  14
#define CAD_TEXT       15
#define CAD_MTEXT      16
#define CAD_ATTDEF     17
#define CAD_ELLIPSE    18
#define CAD_POINT      19
#define CAD_3DFACE     20
#define CAD_HATCH      21
#define CAD_IMAGE_ENT  22
#define CAD_ATTRIB	   23
#define CAD_BEGIN_POLYLINE   100
#define CAD_END_POLYLINE     101
#define CAD_BEGIN_INSERT     102
#define CAD_END_INSERT       103
#define CAD_BEGIN_VIEWPORT   104
#define CAD_END_VIEWPORT     105

#define CADERR_GENERAL 1000
#define CADERR_INVALID_HANDLE (CADERR_GENERAL + 1)
#define CADERR_INVALID_INDEX (CADERR_GENERAL + 2)
#define CADERR_FILE_NOT_FOUND (CADERR_GENERAL + 3)
#define CADERR_FILE_READ (CADERR_GENERAL + 4)

// Soft Gold float type (float in previous version)
typedef double sgFloat;

typedef struct _FPOINT
{
	sgFloat x;
	sgFloat y;
	sgFloat z;
} FPOINT, *LPFPOINT;

typedef union _FRECT
{
	struct
	{
		sgFloat Left;
		sgFloat Top;
		sgFloat Z1;
		sgFloat Right;
		sgFloat Bottom;
		sgFloat Z2;
	} Points;	
	struct
	{
		FPOINT TopLeft;
		FPOINT BottomRight;
	} Corners;	
} FRECT, *LPFRECT;

// Extended CAD data
typedef struct _CADEXTENDEDDATA
{
	LPVOID Param1;
	// ... fields below may be added in future versions
} CADEXTENDEDDATA, *LPCADEXTENDEDDATA;

#pragma pack(push,1)
typedef struct _CADDATA
{
	WORD Tag;
	WORD Count;
	WORD TickCount;
	BYTE Flags;
	BYTE Style;    
	int Dimension;
    LPFPOINT DashDots;
    int DashDotsCount;
	int Color;
	LPVOID Ticks;
	sgFloat Thickness;
	sgFloat Rotation;
	char* Layer;
	char* Text;	
    char* FontName;
    unsigned long Handle;
	int Undefined1;
	sgFloat Undefined2;
	sgFloat Undefined3;
    LPCADEXTENDEDDATA CADExtendedData;

	FPOINT Point1;
	FPOINT Point2;
	FPOINT Point3;
	FPOINT Point4;

	union
	{		
		struct
		{
			sgFloat Radius;
			sgFloat StartAngle;
			sgFloat EndAngle;
			sgFloat Ratio;
			BYTE EntityType;
		} Arc;
		struct
		{
			HANDLE Block;
			FPOINT Scale;
		} Blocks;
		struct
		{
			sgFloat FHeight;
			sgFloat FScale;
			sgFloat RWidth;
			sgFloat RHeight;
			BYTE HAlign;
			BYTE VAlign;
		} Text;
		LPFPOINT PolyPoints;
	} DATA;
} CADDATA, *LPCADDATA;
#pragma pack(pop)

typedef void	(CALLBACK *CADPROC)(LPCADDATA, LPARAM);

typedef HANDLE (WINAPI *CADCREATE)(HWND, LPCSTR);
typedef int	   (WINAPI *CADCLOSE)(HANDLE);
typedef int    (WINAPI *CADGETBOX)(HANDLE img, double *left, double *right, double *top, double *bottom);
typedef int    (WINAPI *CADIS3D)(HANDLE);
typedef HANDLE (WINAPI *CADGETSECTION)(HANDLE, int, LPCADDATA);
typedef HANDLE (WINAPI *CADGETCHILD)(HANDLE, int, LPCADDATA);
typedef int    (WINAPI *CADGETDATA)(HANDLE, LPCADDATA);
typedef int    (WINAPI *CADLAYERCOUNT)(HANDLE);
typedef HANDLE (WINAPI *CADLAYER)(HANDLE, int, LPCADDATA);
typedef int    (WINAPI *CADLAYOUTCOUNT)(HANDLE);
typedef int    (WINAPI *CADLAYOUTCURRENT)(HANDLE, int*, BOOL);
typedef int    (WINAPI *CADLAYOUTNAME)(HANDLE, DWORD, LPCSTR, DWORD);
typedef int    (WINAPI *CADDRAW)(HANDLE, HDC, LPRECT);
typedef int    (WINAPI *CADENUM)(HANDLE, int, CADPROC, LPVOID);
typedef int    (WINAPI *CADUNITS)(HANDLE, LPINT);
typedef int    (WINAPI *CADLTSCALE)(HANDLE, double*);
typedef int    (WINAPI *CADVISIBLE)(HANDLE, LPCSTR);
typedef int    (WINAPI *CADGETLASTERROR)(LPCSTR);
typedef int    (WINAPI *CADPROHIBITCURESASPOLY)(HANDLE,int);
typedef int    (WINAPI *CADSETSHXOPTIONS)(LPCSTR SearchSHXPaths, LPCSTR DefaultSHXPath, LPCSTR DefaultSHXFont, BOOL UseSHXFonts, BOOL UseACADPaths);

#endif
