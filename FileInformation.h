#ifndef _FILEINFORMATION_
#define _FILEINFORMATION_

///////////////////////////////////////////////////////////////////////////////
// Author(s): Chad R. Hearn, chearn@dnet.net
// Legal:     ©2011 M.Sc. E. Victor
// Purpose:   FILE_INFORMATION structure implementation
//		
//		
//		
// Date:      04-27-2011
//
// NOTES: 
///////////////////////////////////////////////////////////////////////////////
#include <tchar.h>
#include "Security\ACLInfo.h"

struct FILE_INFORMATION
{
	ACERIGHTS *paceFileRights;
	WIN32_FIND_DATA *pwfdFileInfo;

	/**
	 * Default constructor, initializes fields to their defaults.
	 */
	FILE_INFORMATION()
	{
		// initialize members
		pwfdFileInfo = new WIN32_FIND_DATA();
		paceFileRights = new ACERIGHTS();
	}

	/**
	 * Constructor which accepts a WIN32_FIND_DATA structure as an
	 * argument.
	 */
	FILE_INFORMATION(WIN32_FIND_DATA wfdFile)
	{
		// initialize internal file information with parameter
		pwfdFileInfo = new WIN32_FIND_DATA();
		if(pwfdFileInfo)
			memcpy(pwfdFileInfo, &wfdFile, sizeof(WIN32_FIND_DATA));

		// initialize rights to default
		paceFileRights = new ACERIGHTS();
	}

	/**
	 * Constructor which accepts a WIN32_FIND_DATA structure and an
	 * ACERIGHTS structure as arguments.
	 */
	FILE_INFORMATION(WIN32_FIND_DATA wfdFile, ACERIGHTS aceFile)
	{
		// initialize internal file information with parameter
		pwfdFileInfo = new WIN32_FIND_DATA();
		if(pwfdFileInfo)
			memcpy(pwfdFileInfo, &wfdFile, sizeof(WIN32_FIND_DATA));

		// initialize rights to default
		paceFileRights = new ACERIGHTS();

		if(paceFileRights)
			// initialize internal rights structure with parameter
			*paceFileRights = aceFile;
	}
	
	/**
	 * Destructor, performs final garbage collection
	 */
	~FILE_INFORMATION()
	{
		if(pwfdFileInfo)
		{
			delete pwfdFileInfo;
			pwfdFileInfo = NULL;
		}
		if(paceFileRights)
		{
			delete paceFileRights;
			paceFileRights = NULL;
		}
	}
};

#endif