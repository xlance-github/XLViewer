#ifndef _DWGLAYERINFO_
#define _DWGLAYERINFO_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor      
// Purpose:   
//
//
//
// Date:	  05-26-2011    
//
// NOTES:
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <TChar.h>
//#include "..\LibreDWG\dwg.h"

// Layer Entry Object Definition
typedef struct _DWGLAYERINFO
{
private:
	TCHAR *ptcLayerName;

public:
	BOOL bEnabled;
	//Dwg_Object_LAYER *pdwgobjLayer;

	/**
	 * Default constructor
	 */
	_DWGLAYERINFO()
	{
		// initialize members to their defaults
		bEnabled = FALSE;
		//pdwgobjLayer = NULL;
		ptcLayerName = NULL;
	}
	
	/**
	 * Constructor which accepts the layer's name and visibility state.
	 */
	_DWGLAYERINFO(TCHAR *ptcLayerNameIn, BOOL bVisible = TRUE)
	{
		// attempt to create/store layer name
		if(ptcLayerNameIn)
		{
			ptcLayerName = new TCHAR[lstrlen(ptcLayerNameIn) + 1];
			if(ptcLayerName)
				_tcscpy(ptcLayerName, ptcLayerNameIn);
		}

		// store visibility state
		bEnabled = bVisible;
	}

	///**
	// * Constructor which accepts a layer as an argument.
	// */
	//_DWGLAYERINFO(Dwg_Object_LAYER *pdwgobjInLayer)
	//{
	//	// initialize members
	//	bEnabled = TRUE;
	//	pdwgobjLayer = pdwgobjInLayer;
	//}
	
	/**
	 * Destructor, performs any final garbage collection
	 */
	~_DWGLAYERINFO()
	{
		// release layer name
		if(ptcLayerName)
		{
			delete[] ptcLayerName;
			ptcLayerName = NULL;
		}
	}

	/**
	 * Sets the layer name to the one specified.
	 */
	BOOL setLayerName(TCHAR *ptcLayerNameIn)
	{
		// validate layer name, create if applicable
		if(ptcLayerName == NULL)
		{	
			// attempt to allocate storate
			ptcLayerName = new TCHAR[MAX_PATH + 1];

			// validate, copy
			if(ptcLayerName)
				_tcscpy(ptcLayerName, ptcLayerNameIn);
			else
				return FALSE;
		}

		// return success
		return TRUE;
	}

	/**
	 * Gets the layer name.
	 */
	TCHAR *getLayerName() {return ptcLayerName;}

}DWGLAYERINFO, *PDWGLAYERINFO;

#endif // End DWGLAYERINFO module