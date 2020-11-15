#include "StdAfx.h"
#include "ACLInfo.h"

using std::ends;

#define READ_PERMISSIONS_LIMITED	FILE_READ_ATTRIBUTES
#define READ_PERMISSIONS (FILE_READ_DATA | \
						  FILE_READ_ATTRIBUTES)
#define WRITE_PERMISSIONS (FILE_WRITE_DATA | \
						   FILE_APPEND_DATA | \
						   FILE_WRITE_ATTRIBUTES | \
						   FILE_WRITE_EA)
#define EXECUTE_PERMISSIONS (FILE_READ_DATA | \
							 FILE_EXECUTE)

#define EMPTY_STRING					_T("")
#define STRING_GROUPNAME_SYSTEM			_T("SYSTEM")
#define STRING_GROUPNAME_ADMINISTRATORS _T("Administrators")
#define STRING_GROUPNAME_EVERYONE		_T("Authenticated Users")
#define STRING_GROUPNAME_EVERYONE2		_T("Everyone")
#define STRING_GROUPNAME_USERS			_T("Users")

#define STRING_DOMAINNAME_NTAUTHORITY	_T("NT AUTHORITY")
#define STRING_DOMAINNAME_BUILTIN		_T("BUILTIN")

// Constructor
CACLInfo::CACLInfo(_bstr_t bstrPath)
{
	m_sAceList = NULL;
	m_bstrPath = bstrPath;
}

// Destructor
CACLInfo::~CACLInfo(void)
{
	// Free ace_list structure
	ClearAceList();
}

// Free the nodes of ace_list
void CACLInfo::ClearAceList()
{
	ace_list* pList = m_sAceList;
	ace_list* pNext;
	while(NULL != pList)
	{
		pNext = pList->next;
		free(pList);
		pList = pNext;
	}

	m_sAceList = NULL;
}

/**
 *
 *
 * @param tstrFullpath
 *
 * @return
 */
BOOL CACLInfo::setPath(TCHAR *tstrFullpath)
{
	BOOL bReturn = FALSE;

	try
	{
		// set fullpath
		m_bstrPath = tstrFullpath;

		// re-initialize ACL list
		ClearAceList();

		// Query
		Query();

		// set success val
		bReturn = TRUE;
	}
	catch(...)
	{

		// reset fail val
		bReturn = FALSE;
	}
	
	// return val
	return bReturn;
}

HRESULT CACLInfo::Query()
{
	BOOL bSuccess = TRUE;
	BYTE* pSecDescriptorBuf;
	DWORD dwSizeNeeded = 0;

	// clear any previously queried information
	ClearAceList();

	// Find out size of needed buffer for security descriptor with DACL
	// DACL = Discretionary Access Control List
	bSuccess = GetFileSecurityW((BSTR)m_bstrPath,
								DACL_SECURITY_INFORMATION,
								NULL,
								0,
								&dwSizeNeeded);

	if (0 == dwSizeNeeded)
	{
		return E_FAIL;
	}
	pSecDescriptorBuf = new BYTE[dwSizeNeeded];

	// Retrieve security descriptor with DACL information
	bSuccess = GetFileSecurityW((BSTR)m_bstrPath,
								DACL_SECURITY_INFORMATION,
								pSecDescriptorBuf,
								dwSizeNeeded,
								&dwSizeNeeded);

	// Check if we successfully retrieved security descriptor with DACL information
	if (!bSuccess)
	{
		DWORD dwError = GetLastError();
		m_strLastError =  _T("Failed to get file security information.");
		return E_FAIL;
	}
	
	// Getting DACL from Security Descriptor
	PACL pacl;
	BOOL bDaclPresent, bDaclDefaulted;
	bSuccess = GetSecurityDescriptorDacl((SECURITY_DESCRIPTOR*)pSecDescriptorBuf,
										 &bDaclPresent, &pacl, &bDaclDefaulted);
	
	// Check if we successfully retrieved DACL
	if (!bSuccess)
	{
		DWORD dwError = GetLastError();
		m_strLastError =  _T("Failed to retrieve DACL from security descriptor.");
		return E_FAIL;
	}
	
	// Check if DACL present in security descriptor
	if (!bDaclPresent)
	{
		m_strLastError =  _T("DACL was not found.\n");
		return E_FAIL;
	}

	// Validate ACL
	if(pacl == NULL)
	{
		m_strLastError = _T("Returned DACL was invalid.");
		return E_FAIL;
	}

	// DACL for specified file was retrieved successfully
	// Now, we should fill in the linked list of ACEs
	// Iterate through ACEs (Access Control Entries) of DACL
	for (USHORT i = 0; i < pacl->AceCount; i++)
	{
		LPVOID pAce;
		bSuccess = GetAce(pacl, i, &pAce);
		if (!bSuccess)
		{
			DWORD dwError = GetLastError();
			m_strLastError =  _T("Failed to get ace.");
			continue;
		}
		HRESULT hr = AddAceToList((ACE_HEADER*)pAce);
		if (FAILED(hr))
		{
			m_strLastError =  _T("Failed to add ace.");
			continue;
		}
	}
	return S_OK;
}

HRESULT CACLInfo::AddAceToList(ACE_HEADER* pAce)
{
	ace_list* pNewAce = (ace_list*)malloc(sizeof(ace_list));
	if (NULL == pNewAce)
	{
		return E_OUTOFMEMORY;
	}

	// Check Ace type and update new list entry accordingly
	switch (pAce->AceType)
	{
		case ACCESS_ALLOWED_ACE_TYPE:
		{
			pNewAce->bAllowed = TRUE;
			break;
		}
		case ACCESS_DENIED_ACE_TYPE:
		{
			pNewAce->bAllowed = FALSE;
			break;
		}
	}
	// Update the remaining fields
	// We add new entry to the head of list
	pNewAce->pAce = pAce;
	pNewAce->next = m_sAceList;

	m_sAceList = pNewAce;

	return S_OK;
}

BOOL CACLInfo::Output(ACERIGHTS &acerightsOut)
{
	BOOL bReturn = TRUE;

	try
	{
		ACE_HEADER* pAce;
		RIGHTS *prightsCurrent = NULL;
		SID* pAceSid;
		ACCESS_MASK maskPermissions;
		ace_list* pList = m_sAceList;

		// validate ACL list
		if(m_sAceList == NULL)
			return FALSE;

		for(; pList != NULL; pList = pList->next)
		{
			pAce = pList->pAce;
			if (pList->bAllowed)
			{
				ACCESS_ALLOWED_ACE* pAllowed = (ACCESS_ALLOWED_ACE*)pAce;
				pAceSid = (SID*)(&(pAllowed->SidStart));
				maskPermissions = pAllowed->Mask;
			}
			else
			{
				// Don't care about this case... next please!
				continue;
			}

			DWORD dwCbName = 0;
			DWORD dwCbDomainName = 0;
			SID_NAME_USE SidNameUse;
			TCHAR bufName[MAX_PATH];
			TCHAR bufDomain[MAX_PATH];
			dwCbName = sizeof(bufName);
			dwCbDomainName = sizeof(bufDomain);

			// Get account name for SID
			BOOL bSuccess = LookupAccountSid(NULL,
								pAceSid,
								bufName,
								&dwCbName,
								bufDomain,
								&dwCbDomainName,
								&SidNameUse);
			
			// verify lookup succeeded
			if(!bSuccess)
				return FALSE;
			
			// reset group pointer
			prightsCurrent = NULL;

			// set group pointer by group name
			if(lstrcmp(bufName, STRING_GROUPNAME_SYSTEM) == 0)
				prightsCurrent = &acerightsOut.rightsSystem;
			else if(lstrcmp(bufName, STRING_GROUPNAME_EVERYONE) == 0
					||
					lstrcmp(bufName, STRING_GROUPNAME_EVERYONE2) == 0)
				prightsCurrent = &acerightsOut.rightsEveryone;
			else if(lstrcmp(bufName, STRING_GROUPNAME_ADMINISTRATORS) == 0)
				prightsCurrent = &acerightsOut.rightsAdministrators;
			else if(lstrcmp(bufName, STRING_GROUPNAME_USERS) == 0)
				prightsCurrent = &acerightsOut.rightsUsers;

			// validate group name pointer
			if(prightsCurrent == NULL)
				continue;			// Nothing we can do, next...

			// Read Permissions
			if ((maskPermissions & READ_PERMISSIONS) == READ_PERMISSIONS)
			{
				prightsCurrent->btRead = 1;
			}
			else
			{
				prightsCurrent->btRead = 0;
			}
			// Write permissions
			if ((maskPermissions & WRITE_PERMISSIONS) == WRITE_PERMISSIONS)
			{
				prightsCurrent->btWrite = 1;
			}
			else
			{
				prightsCurrent->btWrite = 0;
			}
			// Execute Permissions
			if ((maskPermissions & EXECUTE_PERMISSIONS) == EXECUTE_PERMISSIONS)
			{
				prightsCurrent->btExecute = 1;
			}
			else
			{
				prightsCurrent->btExecute = 0;
			}
		}
	}
	catch(...)
	{
		// return fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

BOOL CACLInfo::Output(ostream& os)
{
	BOOL bReturn = TRUE;

	try
	{
		ACERIGHTS acerightsThis;

		bReturn = Output(acerightsThis);
		if(bReturn)
			// create final output string...
			os << acerightsThis.toString() << ends;
	}
	catch(...)
	{
		// return fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 *
 *
 *
 *
 */
BOOL CACLInfo::SetRights(ACERIGHTS *pacerNew)
{
	BOOL bReturn = FALSE;

	try
	{
		ACE_HEADER* pAce = NULL;
		RIGHTS *prightsNew = NULL;
		SID* pAceSid = NULL;
		ACCESS_MASK maskPermissions;
		ace_list* pList = m_sAceList;
		DWORD dwRights = (DWORD)0;
		BOOL bHasReadPermissions = FALSE,
			 bHasWritePermissions = FALSE,
			 bHasExecutePermissions = FALSE;

		// validate ACL list
		if(m_sAceList == NULL)
			return FALSE;

		// validate new access rights param
		if(pacerNew == NULL)
			return FALSE;

		for(; pList != NULL; pList = pList->next)
		{
			// reinitialize permissions flags
			bHasReadPermissions = FALSE;
			bHasWritePermissions = FALSE;
			bHasExecutePermissions = FALSE;
			dwRights = (DWORD)0;

			// prep ACE
			pAce = pList->pAce;
			if (pList->bAllowed)
			{
				ACCESS_ALLOWED_ACE* pAllowed = (ACCESS_ALLOWED_ACE*)pAce;
				pAceSid = (SID*)(&(pAllowed->SidStart));
				maskPermissions = pAllowed->Mask;
			}
			else
			{
				// Don't care about this case... next please!
				continue;
			}

			DWORD dwCbName = 0;
			DWORD dwCbDomainName = 0;
			SID_NAME_USE SidNameUse;
			TCHAR bufName[MAX_PATH];
			TCHAR bufDomain[MAX_PATH];
			dwCbName = sizeof(bufName);
			dwCbDomainName = sizeof(bufDomain);

			// Get account name for SID
			BOOL bSuccess = LookupAccountSid(NULL,
								pAceSid,
								bufName,
								&dwCbName,
								bufDomain,
								&dwCbDomainName,
								&SidNameUse);
			
			// verify lookup succeeded
			if(!bSuccess)
				return FALSE;			
			
			// reset group pointer
			prightsNew = NULL;

			// set group pointer by group name
			if(lstrcmp(bufName, STRING_GROUPNAME_SYSTEM) == 0)
				prightsNew = &pacerNew->rightsSystem;
			else if(lstrcmp(bufName, STRING_GROUPNAME_EVERYONE) == 0 
					||
					lstrcmp(bufName, STRING_GROUPNAME_EVERYONE2) == 0)
				prightsNew = &pacerNew->rightsEveryone;
			else if(lstrcmp(bufName, STRING_GROUPNAME_ADMINISTRATORS) == 0)
				prightsNew = &pacerNew->rightsAdministrators;
			else if(lstrcmp(bufName, STRING_GROUPNAME_USERS) == 0)
				prightsNew = &pacerNew->rightsUsers;

			// validate group name pointer
			if(prightsNew == NULL)
				continue;			// Nothing we can do, next...

			// Get new permissions
			if(prightsNew->btRead)
				dwRights = READ_PERMISSIONS;
			if(prightsNew->btWrite)
				dwRights = dwRights | WRITE_PERMISSIONS;
			if(prightsNew->btExecute)
				dwRights = dwRights | EXECUTE_PERMISSIONS;

			if(dwRights == (DWORD)0)
				dwRights = READ_PERMISSIONS_LIMITED;

			// Assign new permissions
			AddAceToObjectsSecurityDescriptor(pAceSid, dwRights, 
				SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
		}

		// if we made it here, return success
		bReturn = TRUE;
	}
	catch(...)
	{
		
		// reset fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}

/**
 * Iterates through DACL and determines if an "Everyone" group
 * is present at all.
 *
 * @return TRUE if this file has either of the "Everyone" groups and no
 * errors occur, otherwise FALSE.
 */
BOOL CACLInfo::hasEveryoneGroup()
{
	BOOL bReturn = FALSE;

	try
	{
		ACE_HEADER* pAce = NULL;
		SID* pAceSid = NULL;
		ace_list* pList = m_sAceList;

		for(; pList != NULL; pList = pList->next)
		{
			// prep ACE
			pAce = pList->pAce;
			if (pList->bAllowed)
			{
				ACCESS_ALLOWED_ACE* pAllowed = (ACCESS_ALLOWED_ACE*)pAce;
				pAceSid = (SID*)(&(pAllowed->SidStart));
			}
			else
			{
				// Don't care about this case... next please!
				continue;
			}

			DWORD dwCbName = 0;
			DWORD dwCbDomainName = 0;
			SID_NAME_USE SidNameUse;
			TCHAR bufName[MAX_PATH];
			TCHAR bufDomain[MAX_PATH];
			dwCbName = sizeof(bufName);
			dwCbDomainName = sizeof(bufDomain);

			// Get account name for SID
			BOOL bSuccess = LookupAccountSid(NULL,
								pAceSid,
								bufName,
								&dwCbName,
								bufDomain,
								&dwCbDomainName,
								&SidNameUse);
			
			// verify lookup succeeded
			if(!bSuccess)
				return FALSE;			

			// set group pointer by group name
			if(lstrcmp(bufName, STRING_GROUPNAME_EVERYONE) == 0 
				||
			   lstrcmp(bufName, STRING_GROUPNAME_EVERYONE2) == 0)
			{			
				// set found return val
				bReturn = TRUE;

				// we're outta here
				break;
			}
		}
	}
	catch(...)
	{
		
		// reset fail val
		bReturn = FALSE;
	}

	// return success / fail val
	return bReturn;
}


DWORD CACLInfo::AddAceToObjectsSecurityDescriptor (
    LPVOID lpvTrustee,          // trustee for new ACE
    DWORD dwAccessRights,       // access mask for new ACE
    ACCESS_MODE AccessMode,     // type of ACE
    DWORD dwInheritance         // inheritance flags for new ACE
) 
{
    DWORD dwRes = 0;
    PACL pOldDACL = NULL, pNewDACL = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;
    EXPLICIT_ACCESS ea;
    SE_OBJECT_TYPE ObjectType = SE_FILE_OBJECT;  // type of object
    TRUSTEE_FORM TrusteeForm = TRUSTEE_IS_SID;   // format of trustee structure

    if (m_bstrPath.length() == 0)
        return ERROR_INVALID_PARAMETER;

    // Get a pointer to the existing DACL.

    dwRes = GetNamedSecurityInfo(m_bstrPath, ObjectType, 
          DACL_SECURITY_INFORMATION,
          NULL, NULL, &pOldDACL, NULL, &pSD);
    if (ERROR_SUCCESS != dwRes) {
        printf( "GetNamedSecurityInfo Error %u\n", dwRes );
        goto Cleanup; 
    }  

    // Initialize an EXPLICIT_ACCESS structure for the new ACE. 

    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea.grfAccessPermissions = dwAccessRights;
    ea.grfAccessMode = AccessMode;
    ea.grfInheritance= dwInheritance;
    ea.Trustee.TrusteeForm = TrusteeForm;
    ea.Trustee.ptstrName = (LPTSTR)lpvTrustee;

    // Create a new ACL that merges the new ACE
    // into the existing DACL.

    dwRes = SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);
    if (ERROR_SUCCESS != dwRes)  {
        printf( "SetEntriesInAcl Error %u\n", dwRes );
        goto Cleanup; 
    }  

    // Attach the new ACL as the object's DACL.

    dwRes = SetNamedSecurityInfo(m_bstrPath, ObjectType, 
          DACL_SECURITY_INFORMATION,
          NULL, NULL, pNewDACL, NULL);
    if (ERROR_SUCCESS != dwRes)  {
        printf( "SetNamedSecurityInfo Error %u\n", dwRes );
        goto Cleanup; 
    }  

    Cleanup:

        if(pSD != NULL) 
            LocalFree((HLOCAL) pSD); 
        if(pNewDACL != NULL) 
            LocalFree((HLOCAL) pNewDACL); 

        return dwRes;
}