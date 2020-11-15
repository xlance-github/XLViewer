
#include "stdafx.h"
#include "XlvNamedPipeListener.h"
#include <iostream>
#include <tchar.h>
#include <stdlib.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CXLVNamedPipeListener::m_bSingleThreaded = false;
bool CXLVNamedPipeListener::m_bStopListener = false;
/*--------------------------------------------------------------------------------------
Function       : CXLVNamedPipeListener
In Parameters  : IMaxNamedPipeData* pDest, 
Out Parameters : 
Description    : uses the IMaxNamedPipeData interface for callbacks
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CXLVNamedPipeListener::CXLVNamedPipeListener(IMaxNamedPipeData* pDest)
{
	m_nID = 0;
	m_hThread = NULL;
	m_pDest = pDest;
	m_hPipe = NULL;
	m_hOverlap[0] = NULL;
	m_hOverlap[1] = NULL;
	m_hServerStopEvent = NULL;
	m_hLastClientDisconnectEvent = NULL;

	m_bMonitorConnections = false;
}

/*--------------------------------------------------------------------------------------
Function       : ~CXLVNamedPipeListener
In Parameters  : 
Out Parameters : 
Description    : D'tor 
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CXLVNamedPipeListener::~CXLVNamedPipeListener()
{
	if(m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

}

/*--------------------------------------------------------------------------------------
Function       : Cleanup
In Parameters  : 
Out Parameters : void 
Description    : Closing Pipe and Overlap IO structure cleanup
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CXLVNamedPipeListener::Cleanup()
{
	if(m_hOverlap[1])
	{
		CloseHandle(m_hOverlap[1]);
		m_hOverlap[1] = NULL;
	}

	if(m_hPipe)
	{
		CloseHandle(m_hPipe);
		m_hPipe = NULL;
	}
}

bool CXLVNamedPipeListener::ReadPipe()
{
	OVERLAPPED  op;
	SecureZeroMemory(&op, sizeof(OVERLAPPED));
	m_hOverlap[0] = m_hServerStopEvent;	
	m_hOverlap[1] = op.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!m_hOverlap[1])
		return false;

	/*******************************************/
	// Security Attributes
	/*******************************************/
	BYTE  sd[SECURITY_DESCRIPTOR_MIN_LENGTH]={0};
	SECURITY_ATTRIBUTES  sa={0};

	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, (PACL) 0, FALSE);

	m_hPipe = CreateNamedPipe(
		m_pDest->GetPipeName(),					// __in      LPCTSTR lpName,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,		// __in      DWORD dwOpenMode,
		PIPE_TYPE_BYTE | PIPE_WAIT, // __in      DWORD dwPipeMode,
		PIPE_UNLIMITED_INSTANCES,	// __in      DWORD nMaxInstances,
		0,							// __in      DWORD nOutBufferSize,
		0,							// __in      DWORD nInBufferSize,
		0,							// __in      DWORD nDefaultTimeOut,
		&sa							// __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes
		);
	if(m_hPipe == INVALID_HANDLE_VALUE) 
	{
		return false;
	}

	ConnectNamedPipe(m_hPipe, &op);
	switch (WaitForMultipleObjects(2, m_hOverlap, FALSE, INFINITE))
	{
	case WAIT_OBJECT_0:
		Cleanup();
		return false;
		break;
	case WAIT_OBJECT_0 + 1:
		if(m_bStopListener)
		{
			Cleanup();
			return false;
		}
		ResetEvent(m_hOverlap);
		break;
	default:
		Cleanup();
		return false;
		break;
	}

	DWORD dwSize = 0;
	if (m_pDest)
	{
		if(false == CXLVNamedPipeListener::m_bSingleThreaded)
		{
			//Do not Launch the child thread;
			m_pDest->OnConnectingPipe();
		}
		dwSize = m_pDest->GetStructSize();
	}	
	DWORD nReaded = 0;
	BYTE buffer[MAX_PIPE_LENGTH];
	SecureZeroMemory(&buffer,MAX_PIPE_LENGTH);
	while (ReadFile(m_hPipe, &buffer, dwSize, &nReaded, NULL) && !m_bStopListener)
	{
		if (m_pDest)
			m_pDest->OnIncomingData(buffer);
	}
	if(m_bStopListener)
	{
		return false;
	}
	if(m_bMonitorConnections)
	{
		if(GetLastError() == ERROR_BROKEN_PIPE)
		{
			//Beware Client code should handle NULL value!!
			if (m_pDest)
				m_pDest->OnIncomingData(NULL);
		}
	}
	Cleanup();
	return true;
}

DWORD WINAPI CXLVNamedPipeListener::NamedPipeListenerThread(LPVOID lParam)
{
	CoInitializeEx(NULL,COINIT_MULTITHREADED);
	{
		__try{
			CXLVNamedPipeListener* pThis = (CXLVNamedPipeListener*)lParam;

			pThis->ReadPipe();
			IMaxNamedPipeData* pPipeDataDest = pThis->m_pDest;
			if (pPipeDataDest)
				pPipeDataDest->OnDisConnectingPipe(pThis);
		}__except(0)
		{

		}
	}
	CoUninitialize();
	return 0;
}

bool CXLVNamedPipeListener::StartReader(void)
{
	if(m_bStopListener)
	{
		return false;
	}
	m_hThread = CreateThread(NULL, 0, NamedPipeListenerThread, this, 0, &m_nID);
	return m_hThread != NULL;
}

bool CXLVNamedPipeListener::SendResponse(LPVOID lpResponse)
{
	BOOL bReturn = FALSE;
	__try{
		if(m_hPipe != INVALID_HANDLE_VALUE)
		{
			DWORD nWritten = 0;
			OVERLAPPED  op;
			SecureZeroMemory(&op, sizeof(OVERLAPPED));
			op.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			bReturn = WriteFile(
				m_hPipe,			// __in          HANDLE hFile,
				(LPCVOID)lpResponse,		// __in          LPCVOID lpBuffer,
				m_pDest->GetStructSize(),			// __in          DWORD nNumberOfBytesToWrite,
				&nWritten,		// __out         LPDWORD lpNumberOfBytesWritten,
				&op				// __in          LPOVERLAPPED lpOverlapped
				);
			if(!bReturn)
			{
				if (GetLastError() == ERROR_IO_PENDING)
				{
					bReturn = GetOverlappedResult((HANDLE)m_hPipe, &op, &nWritten, FALSE);
				}
			}
			if(op.hEvent)
				CloseHandle(op.hEvent);
		}
	}
	__except(0)
	{

	}

	return bReturn?true:false;
}