#include "stdafx.h"
#include "CriticalSection.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*--------------------------------------------------------------------------------------
Function       : CMaxCriticalSection
In Parameters  :
Out Parameters :
Description    : C'tor Initializes the critical section
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CMaxCriticalSection::CMaxCriticalSection()
{
	InitializeCriticalSection(&m_CritSect);
}

/*--------------------------------------------------------------------------------------
Function       : ~CMaxCriticalSection
In Parameters  :
Out Parameters :
Description    : The critical section is automatically destryed during stack clearance
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CMaxCriticalSection::~CMaxCriticalSection()
{
	DeleteCriticalSection(&m_CritSect);
}

/*--------------------------------------------------------------------------------------
Function       : Lock
In Parameters  :
Out Parameters : void
Description    : Function for locking on demand (Auto mode is not used)
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CMaxCriticalSection::Lock()
{
	EnterCriticalSection(&m_CritSect);
}

/*--------------------------------------------------------------------------------------
Function       : Unlock
In Parameters  :
Out Parameters : void
Description    : Function for unlocking on demand (Auto mode is not used)
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CMaxCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_CritSect);
}

/*--------------------------------------------------------------------------------------
Function       : CAutoCriticalSection
In Parameters  : CMaxCriticalSection& rCritSect,
Out Parameters :
Description    : Autocritical section c'tor for wrapping the critical section
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CAutoCriticalSection::CAutoCriticalSection(CMaxCriticalSection& rCritSect)
: m_rCritSect(rCritSect)
{
	m_rCritSect.Lock();
}

/*--------------------------------------------------------------------------------------
Function       : ~CAutoCriticalSection
In Parameters  :
Out Parameters :
Description    : D'tor automatically unlocks CS
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CAutoCriticalSection::~CAutoCriticalSection()
{
	m_rCritSect.Unlock();
}

/*--------------------------------------------------------------------------------------
Function       : CMaxSemaphore
In Parameters  : int nMaxCount,
Out Parameters :
Description    : C'tor Wrapping the Semaphore objects
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CMaxSemaphore::CMaxSemaphore(int nMaxCount)
{
	m_nMaxCount = nMaxCount;
	m_hSemaphore = CreateSemaphore(
		NULL,           // default security attributes
		nMaxCount,  // initial count
		nMaxCount,  // maximum count
		NULL);
}

/*--------------------------------------------------------------------------------------
Function       : ~CMaxSemaphore
In Parameters  :
Out Parameters :
Description    : Auto Release of Semaphore Object
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CMaxSemaphore::~CMaxSemaphore()
{
	ReleaseSemaphore(
		m_hSemaphore,  // handle to semaphore
		1,            // increase count by one
		NULL);       // not interested in previous count
	CloseHandle(m_hSemaphore);

}

/*--------------------------------------------------------------------------------------
Function       : Lock
In Parameters  :
Out Parameters : void
Description    : locking of semphhore object
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CMaxSemaphore::Lock()
{
	WaitForSingleObject(m_hSemaphore, MAX_WAIT_COUNT);
}

/*--------------------------------------------------------------------------------------
Function       : Unlock
In Parameters  :
Out Parameters : void
Description    : Unlocking of Sempahore object
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CMaxSemaphore::Unlock()
{
	ReleaseSemaphore(
		m_hSemaphore,  // handle to semaphore
		1,            // increase count by one
		NULL);       // not interested in previous count
}

/*--------------------------------------------------------------------------------------
Function       : CAutoSemaphore
In Parameters  : CMaxSemaphore& rSemaphore,
Out Parameters :
Description    : C'tor Autolocking of Semaphore object
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CAutoSemaphore::CAutoSemaphore(CMaxSemaphore& rSemaphore)
: m_rSemaphore(rSemaphore)
{
	m_rSemaphore.Lock();
}

/*--------------------------------------------------------------------------------------
Function       : ~CAutoSemaphore
In Parameters  :
Out Parameters :
Description    : Auto Unlocking of Semaphore object through d'tor
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CAutoSemaphore::~CAutoSemaphore()
{
	m_rSemaphore.Unlock();
}

