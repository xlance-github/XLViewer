#include "stdafx.h"
#include "XlvCommunicator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAX_CONNECT_RETRY = 15;
const int MAX_CONNECT_TIMEOUT = 3000;

/*--------------------------------------------------------------------------------------
Function       : CXlvCommunicator
In Parameters  : const TCHAR* tchPipeName, bool bRetryConnection,
Out Parameters :
Description    : C'tor
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CXlvCommunicator::CXlvCommunicator(const TCHAR* tchPipeName, bool bRetryConnection)
{
	_tcscpy_s(m_tchPipe, tchPipeName);
	m_hPipe = INVALID_HANDLE_VALUE;
	m_bRetryConnection = bRetryConnection;
}

/*--------------------------------------------------------------------------------------
Function       : ~CXlvCommunicator
In Parameters  :
Out Parameters :
Description    : C'tor
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CXlvCommunicator::~CXlvCommunicator()
{
	Close();
}

/*--------------------------------------------------------------------------------------
Function       : Close
In Parameters  :
Out Parameters : void
Description    : Closing the client connection
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CXlvCommunicator::Close()
{
	__try{
		if(m_hPipe != INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(m_hPipe);
			DisconnectNamedPipe(m_hPipe);
			CloseHandle(m_hPipe);
			m_hPipe = INVALID_HANDLE_VALUE;
		}
	}
	__except(0)
	{

	}
}

/*--------------------------------------------------------------------------------------
Function       : Connect
In Parameters  : void,
Out Parameters : bool
Description    : Connecting to the server
Author         : Parth Software
--------------------------------------------------------------------------------------*/
bool CXlvCommunicator::Connect(void)
{
	int nTryCount = 0;
	bool bConnected = false;
	__try{
		do{
			m_hPipe = CreateFile(
				m_tchPipe,		// __in          LPCTSTR lpFileName,
				GENERIC_WRITE | GENERIC_READ,					// __in          DWORD dwDesiredAccess,
				0,								// __in          DWORD dwShareMode,
				NULL,							// __in          LPSECURITY_ATTRIBUTES lpSecurityAttributes,
				OPEN_EXISTING,					// __in          DWORD dwCreationDisposition,
				FILE_ATTRIBUTE_NORMAL,			// DWORD dwFlagsAndAttributes,
				NULL							// __in          HANDLE hTemplateFile
				);

			if(m_hPipe == INVALID_HANDLE_VALUE)
			{
				if(m_bRetryConnection)
				{
					OutputDebugString("Retry Count");
					if(nTryCount >= MAX_CONNECT_RETRY)
					{
						m_bRetryConnection = false;
						break;
					}
					nTryCount ++;
					Sleep(MAX_CONNECT_TIMEOUT);
				}
			}
			else
			{
				bConnected = true;
				m_bRetryConnection = false;
			}
		}while ((!bConnected) && m_bRetryConnection);
	}
	__except(0)
	{
		bConnected = false;
	}
	return bConnected;
}

/*--------------------------------------------------------------------------------------
Function       : SendData
In Parameters  : LPVOID lpMaxData, DWORD dwSize,
Out Parameters : bool
Description    : Sending Data to the server using the Max Structures
Author         : Parth Software
--------------------------------------------------------------------------------------*/
bool CXlvCommunicator::SendData(LPVOID lpMaxData, DWORD dwSize)
{
	BOOL bReturn = FALSE;
	__try{
		if(m_hPipe == INVALID_HANDLE_VALUE)
		{
			if(false == Connect())
			{
				return false;
			}
		}

		DWORD nWritten = 0;

		bReturn = WriteFile(
			m_hPipe,			// __in          HANDLE hFile,
			(LPCVOID)lpMaxData,		// __in          LPCVOID lpBuffer,
			dwSize,			// __in          DWORD nNumberOfBytesToWrite,
			&nWritten,		// __out         LPDWORD lpNumberOfBytesWritten,
			NULL				// __in          LPOVERLAPPED lpOverlapped
			);
		if(!bReturn)
		{
			Close();
		}
	}
	__except(0)
	{

	}

	return (bReturn == FALSE ? false : true); 
}

/*--------------------------------------------------------------------------------------
Function       : ReadData
In Parameters  : LPVOID lpMaxData, DWORD dwSize,
Out Parameters : bool
Description    : Getting a response from the server sent using SendResponse
Author         : Parth Software
--------------------------------------------------------------------------------------*/
bool CXlvCommunicator::ReadData(LPVOID lpMaxData, DWORD dwSize)
{
	BOOL bReturn = FALSE;
	__try{
		if(m_hPipe == INVALID_HANDLE_VALUE)
			return false;

		DWORD nRead = 0;

		bReturn = ReadFile(
			m_hPipe,			// __in          HANDLE hFile,
			lpMaxData,		// __in          LPCVOID lpBuffer,
			dwSize,			// __in          DWORD nNumberOfBytesToWrite,
			&nRead,		// __out         LPDWORD lpNumberOfBytesWritten,
			NULL				// __in          LPOVERLAPPED lpOverlapped
			);
	}
	__except(0)
	{

	}

	return (bReturn == FALSE ? false : true); 
}