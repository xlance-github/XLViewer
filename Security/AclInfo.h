#pragma once

#ifndef _ACL_INFO_CLASS_H
#define _ACL_INFO_CLASS_H

#include <StdAfx.h>

#include <AccCtrl.h>
#include <AclApi.H>
#include <iostream>
#include <comdef.h>
#include <TChar.h>
//#include <string>

using std::ostream;

struct ace_list
{
	ACE_HEADER*		pAce;
	BOOL			bAllowed;
	ace_list*		next;
};

/**
 * Rights structure used by ACERIGHTS structure.
 */
struct RIGHTS
{
	BYTE btRead;
	BYTE btWrite;
	BYTE btExecute;
	BYTE btModify;

	/**
	 * Default constructor, initializes members to their defaults
	 */
	RIGHTS()
	{
		btRead = btWrite = btExecute = btModify = (BYTE)0;
	}

	/**
	 * Assignment operator
	 */
	RIGHTS &operator= (const RIGHTS &rightsRValue)
	{
		// assign internals
		btRead = rightsRValue.btRead;
		btWrite = rightsRValue.btWrite;
		btExecute = rightsRValue.btExecute;
		btModify = rightsRValue.btModify;
		
		// return result
		return *this;
	}

	/**
	 * Bitwise OR operator
	 */
	RIGHTS &operator| (const RIGHTS &rightsRValue)
	{
		btRead |= rightsRValue.btRead;
		btWrite |= rightsRValue.btWrite;
		btExecute |= rightsRValue.btExecute;
		btModify |= rightsRValue.btModify;

		return *this;
	}
};

// ACE rights structure. Although expansive, I think it is overkill to
//	 make this into a class.
struct ACERIGHTS
{
private:	
	
	tstring m_strOutput;

public:
	RIGHTS rightsSystem,
		   rightsAdministrators,
		   rightsUsers,
		   rightsEveryone;

	/**
	 * Default constructor, initializes members to their defaults
	 */
	ACERIGHTS()
	{
		// initialize members
		m_strOutput = _T("");
	}

	/**
	 * Outputs rights to a string.
	 */
	const TCHAR *toString()
	{
		try
		{
			// initialize output buffer
			m_strOutput = _T("");
			
			// attempt to create rights... order is:
			//
			//		System-Administrators-Everyone-Users
			//
			
			// System
			if(rightsSystem.btRead)
				m_strOutput = _T("R");
			else
				m_strOutput = _T("_");
			if(rightsSystem.btWrite)
				m_strOutput += _T("W");
			else
				m_strOutput += _T("_");
			if(rightsSystem.btExecute)
				m_strOutput += _T("X");
			else
				m_strOutput += _T("_");

			// Add separator
			m_strOutput += _T("-");

			// Administrators
			if(rightsAdministrators.btRead)
				m_strOutput += _T("R");
			else
				m_strOutput += _T("_");
			if(rightsAdministrators.btWrite)
				m_strOutput += _T("W");
			else
				m_strOutput += _T("_");
			if(rightsAdministrators.btExecute)
				m_strOutput += _T("X");
			else
				m_strOutput += _T("_");

			// Add separator
			m_strOutput += _T("-");

			// Everyone
			if(rightsEveryone.btRead)
				m_strOutput += _T("R");
			else
				m_strOutput += _T("_");
			if(rightsEveryone.btWrite)
				m_strOutput += _T("W");
			else
				m_strOutput += _T("_");
			if(rightsEveryone.btExecute)
				m_strOutput += _T("X");
			else
				m_strOutput += _T("_");

			// Add separator
			m_strOutput += _T("-");

			// Users
			if(rightsUsers.btRead)
				m_strOutput += _T("R");
			else
				m_strOutput += _T("_");
			if(rightsUsers.btWrite)
				m_strOutput += _T("W");
			else
				m_strOutput += _T("_");
			if(rightsUsers.btExecute)
				m_strOutput += _T("X");
			else
				m_strOutput += _T("_");
		}
		catch(...)
		{
			// reset output buffer, jic...
			m_strOutput = _T("");
		}
		
		// return output buffer
		return (TCHAR *)m_strOutput.c_str();
	}

	/**
	 * Assignment operator overload
	 */
	ACERIGHTS &operator= (const ACERIGHTS &aceRValue)
	{
		rightsSystem = aceRValue.rightsSystem;
		rightsAdministrators = aceRValue.rightsAdministrators;
		rightsEveryone = aceRValue.rightsEveryone;
		rightsUsers = aceRValue.rightsUsers;

		return *this;
	}

	ACERIGHTS &operator| (const ACERIGHTS &aceRValue)
	{
		rightsSystem = rightsSystem | aceRValue.rightsSystem;
		rightsAdministrators = rightsAdministrators | aceRValue.rightsAdministrators;
		rightsEveryone = rightsEveryone | aceRValue.rightsEveryone;
		rightsUsers = rightsUsers | aceRValue.rightsUsers;

		return *this;		
	}

	ACERIGHTS &operator|= (const ACERIGHTS &aceRValue)
	{
		*this = *this | aceRValue;

		return *this;
	}
};

class CACLInfo
{
public:
	// construction / destruction

	// constructs a new CACLInfo object
	// bstrPath - path for which ACL info should be queried
	CACLInfo(_bstr_t bstrPath);
	virtual ~CACLInfo(void);

	// Queries NTFS for ACL Info of the file/directory
	HRESULT Query();

	// Outputs ACL info in Human-readable format
	// to supplied output stream
	BOOL Output(ostream& os);

	// Overload, outputs rights to rights structure
	BOOL Output(ACERIGHTS &acerightsOut);

	BOOL SetRights(ACERIGHTS *pacerNew);

	/**
	 *
	 */
	TCHAR *getLastError() {return (TCHAR *)m_strLastError.data();}

	/**
	 *
	 */
	BOOL setPath(TCHAR *tstrFullpath);

	/**
	 * Iterates through DACL and determines if an "Everyone" group
	 * is present at all.
	 */
	BOOL hasEveryoneGroup();

private:
	// Fields
	tstring m_strLastError;
	_bstr_t		m_bstrPath;		// path
	ace_list*	m_sAceList;		// list of Access Control Entries

	// Private methods
	void ClearAceList();

	HRESULT AddAceToList(ACE_HEADER* pAce);


	DWORD AddAceToObjectsSecurityDescriptor (
		LPVOID lpvTrustee,          // trustee for new ACE
		DWORD dwAccessRights,       // access mask for new ACE
		ACCESS_MODE AccessMode,     // type of ACE
		DWORD dwInheritance         // inheritance flags for new ACE
		);
};

#endif // _ACL_INFO_CLASS_H