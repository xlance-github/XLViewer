#ifndef _CDWGRENDERENGINE_
#define _CDWGRENDERENGINE_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor    
// Purpose:   CDWGRenderEngine object interface. 
//		
//		
//		
// Date:      
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include <windows.h>
#include <string>
//#include "..\LibreDWG\dwg.h"
#include <cad.h>
#include "..\LinkedList.h"
#include "..\DWGLayerInfo.h"
#include "ACColorTable.h"

// Render engine object definition
class CDWGRenderEngine
{
private:
	///////////////////////////////////////////////////////////////////////////
	// Fields
	///////////////////////////////////////////////////////////////////////////

	CADENUM CADEnum;
	CADCREATE CADCreate;
	CADCLOSE CADClose;
	CADIS3D CADIs3D;
	CADGETLASTERROR CADGetLastError;
	CADPROHIBITCURESASPOLY CADProhibitCurvesAsPoly;
	CADLAYOUTCURRENT CADLayoutCurrent;
	CADLAYOUTNAME CADLayoutName;
	CADLAYOUTCOUNT CADLayoutCount;
	CADGETBOX CADGetBox;
	CADSETSHXOPTIONS CADSetSHXOptions;
	CADDRAW CADDraw;
	CADLAYER CADLayer;
	CADLAYERCOUNT CADLayerCount;
	CADVISIBLE CADVisible;
	CADGETSECTION CADGetSection;
	//Dwg_Data m_dwgdatThis;

	LinkedListEx<ACCOLOR_ENTRY> *m_pllstACColorTable;

	LinkedListEx<DWGLAYERINFO> *m_pllstLayers;
	
	HANDLE m_hCADImporterDrawing;

	HMODULE m_hmodCADImporter;

	HWND m_hwndOutputControl,
		 m_hwndProgressControl;
	
	tstring m_strFilename,
				m_strMessage,
				m_strLastError;

	BOOL m_bDrawingUsesBlack;

	long m_lEntityCount;

	int m_iZoomFactor;
				
	///////////////////////////////////////////////////////////////////////////
	// Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Parses and stores the AutoCad color table contained in the binary data.
	 */
	BOOL parseACColorTable(TCHAR *ptcData, DWORD dwSize);

	/**
	 * Loads the DWG specified and if necessary, the color table as well.
	 */
	BOOL loadDrawing(TCHAR *tstrDWGFilename);

	/**
	 * Loads the CADImporter library.
	 */
	BOOL loadCADImporterLibrary();

	/**
	 * Manages the process of loading and parsing the AutoCad Color Table from
	 * this application's resource file.
	 */
	BOOL loadACColorTable();

	/**
	 * Checks the specified drawing file's version. Returns TRUE if the version
	 * is supported, otherwise FALSE.
	 */
	BOOL isSupportedVersion(TCHAR *tstrDWGFilename);

	/**
	 * Retrieves the version of AutoCad used to create the active drawing
	 * file.
	 */
	BOOL getVersion(TCHAR *&ptcOutVersion);

	/**
	 * Loads all layers in the active drawing.
	 */
	BOOL loadLayers();

	/**
	 * Waits until all painting (more or less) has been completed before
	 * returning.
	 */
	BOOL doEvents();

	///**
	// * Creates a pen using the proper color for the object specified
	// */
	//HPEN createObjectPen(Dwg_Object_Entity *pdwgentObject); 

	///**
	// * Returns the color for the specified layer. If the layer isn't found or
	// * an error occurs, white (255,255,255) is returned.
	// */
	//COLORREF getLayerColor(Dwg_Object_LAYER *pdwgobjLayer);

	///////////////////////////////////////////////////////////////////////////
	// Draw Methods
	///////////////////////////////////////////////////////////////////////////

	///**
	// * Returns whether or not the layer that the specified object is on is
	// * active.
	// */
	//BOOL layerIsActive(Dwg_Object_Entity *pdwgentObject);

	///**
	// * Renders the line object specified.
	// */
	//BOOL renderLine(Dwg_Object_Entity *pdwgentLine, HDC hdcOutput, 
	//	float fOutputScale, POINT ptAbsoluteOffset);

	///**
	// * Renders the line object specified.
	// */
	//BOOL renderMultiLine(Dwg_Object_Entity *pdwgentMultiLine, HDC hdcOutput, 
	//	float fOutputScale, POINT ptAbsoluteOffset);

	///**
	// * Renders the circle object specified.
	// */
	//BOOL renderCircle(Dwg_Object_Entity *pdwgentCircle, HDC hdcOutput, 
	//	float fOutputScale, POINT ptAbsoluteOffset);

	///**
	// * Renders the point object specified.
	// */
	//BOOL renderPoint(Dwg_Object_Entity *pdwgentPoint, HDC hdcOutput, 
	//	float fOutputScale, POINT ptAbsoluteOffset);

	///**
	// * Renders the text object specified.
	// */
	//BOOL renderText(Dwg_Object_Entity *pdwgentText, HDC hdcOutput, 
	//	float fOutputScale, POINT ptAbsoluteOffset);

public:

	//////////////////////////////////////////////////////////////////////////////
	// constructor(s) / destructor
	//////////////////////////////////////////////////////////////////////////////

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CDWGRenderEngine();

	/**
	 * Default constructor, initializes all fields to their defaults.
	 */
	CDWGRenderEngine(TCHAR *tstrDWGFilename);

	/**
	 * Destructor, performs clean-up.
	 */
	~CDWGRenderEngine();

	///////////////////////////////////////////////////////////////////////////
	// Rendering Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Renders the active DWG into the active control. NOTE: to set the active
	 * control, use setOutputControl().
	 */
	BOOL renderDrawing();

	///////////////////////////////////////////////////////////////////////////
	// Getter Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Returns the last error encountered, if any.
	 */
	TCHAR *getLastError() {return (TCHAR *)m_strLastError.data();}

	/**
	 * Returns the filename of the currently active DWG, if any.
	 */
	TCHAR *getFilename() {return (TCHAR *)m_strFilename.data();}

	/**
	 * Returns the current message from the last operation. Differs from 
	 * getLastError() in that this method returns any informational messages
	 * instead of error conditions.
	 */
	TCHAR *getMessage() {return (TCHAR *)m_strMessage.data();}

	/**
	 * Returns a pointer to the active drawing's layer list.
	 */
	LinkedListEx<DWGLAYERINFO> *getLayers() {return m_pllstLayers;}

	/**
	 * Returns the current zoom.
	 */
	int getZoom() {return m_iZoomFactor;}

	/**
	 * Returns whether or not there is an active drawing.
	 */
	BOOL hasActiveDrawing();

	/**
	 * Returns the handle of the control that is the output control for the
	 * rendered drawing.
	 */
	HWND getOutputControl() {return m_hwndOutputControl;}
	
	///////////////////////////////////////////////////////////////////////////
	// Setter Methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Sets the handle for the output control to the one specified.
	 */
	VOID setOutputControl(HWND hwndNewControl) 
		{m_hwndOutputControl = hwndNewControl;}

	/**
	 * Sets the handle for the progressbar control to the one specified.
	 */
	VOID setProgressbarControl(HWND hwndNewControl)
		{m_hwndProgressControl = hwndNewControl;}

	/**
	 * Clears the current drawing and attempts to load the one specified.
	 */
	BOOL setFilename(TCHAR *tstrNewFilename);

	/**
	 * Sets the current zoom.
	 */
	VOID setZoom(int iNewZoom) {m_iZoomFactor = iNewZoom;}

	///////////////////////////////////////////////////////////////////////////
	// Output (String) Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Outputs the active DWG's information to a string. The calling function
	 * method, etc. is responsible for freeing the allocated storage.
	 */
	BOOL getDWGInformation(TCHAR *&ptcOutString);

	///////////////////////////////////////////////////////////////////////////
	// Other Methods
	///////////////////////////////////////////////////////////////////////////
	
	/**
	 * Increments the current zoom by one.
	 */
	BOOL zoomIn();

	/**
	 * Decrements the current zoom by one.
	 */
	BOOL zoomOut();
};

#endif // End _CDWGRENDERENGINE_
