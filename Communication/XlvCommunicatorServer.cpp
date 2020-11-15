#include "stdafx.h"
#include "XlvCommunicatorServer.h"
#include "XlvNamedPipeListener.h"

const int MAX_CLOSE_TIMEOUT = 5000;
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*--------------------------------------------------------------------------------------
Function       : CXlvCommunicatorServer
In Parameters  : const TCHAR* tchPipeName, CallBackFunctionPtr fnPtrCallBack, DWORD dwSize,
Out Parameters :
Description    : C'tor Uses the Pipe Name for server and Callback ptr for Data handling
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CXlvCommunicatorServer::CXlvCommunicatorServer(const TCHAR* tchPipeName, CallBackFunctionPtr fnPtrCallBack, DWORD dwSize)
{
	_tcscpy_s(m_tchPipe, tchPipeName);
	m_fnPtrCallBack = fnPtrCallBack;
	m_dwStructSize = dwSize;
	m_bMonitorConnections = false;
	m_hServerStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hLastClientDisconnectEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bServerRunning = false;
}

/*--------------------------------------------------------------------------------------
Function       : ~CXlvCommunicatorServer
In Parameters  :
Out Parameters :
Description    : D'tor
Author         : Parth Software
--------------------------------------------------------------------------------------*/
CXlvCommunicatorServer::~CXlvCommunicatorServer()
{
	if(m_bServerRunning)
	{
		StopServer();
		m_bServerRunning = false;
	}

	if(m_hServerStopEvent)
	{
		::CloseHandle(m_hServerStopEvent);
		m_hServerStopEvent = NULL;
	}

	if(m_hLastClientDisconnectEvent)
	{
		::CloseHandle(m_hLastClientDisconnectEvent);
		m_hLastClientDisconnectEvent = NULL;
	}


	CXLVNamedPipeListener::m_bStopListener = false;


}

/*--------------------------------------------------------------------------------------
Function       : StopServer
In Parameters  :
Out Parameters : bool
Description    : Send stop request to all connections and then perform a graceful shutdown
Author         : Parth Software
--------------------------------------------------------------------------------------*/
bool CXlvCommunicatorServer::StopServer()
{
	__try{
		CXLVNamedPipeListener::m_bStopListener = true;
		if(m_hServerStopEvent)
		{
			SetEvent(m_hServerStopEvent);
		}

		::WaitForSingleObject(m_hLastClientDisconnectEvent, MAX_CLOSE_TIMEOUT);

		m_bMonitorConnections = false;
	}
	__except(0)
	{
	}
	return true;
}

/*--------------------------------------------------------------------------------------
Function       : Run
In Parameters  : bool bMonitorConnections, bool bSingleThreaded,
Out Parameters : bool
Description    : Start the Comm server
Author         : Parth Software
--------------------------------------------------------------------------------------*/
bool CXlvCommunicatorServer::Run(bool bMonitorConnections,bool bSingleThreaded)
{
	m_bServerRunning = true;
	m_bMonitorConnections = bMonitorConnections;
	CXLVNamedPipeListener::m_bSingleThreaded = bSingleThreaded;
	return RunPipeReader();
}

/*--------------------------------------------------------------------------------------
Function       : RunPipeReader
In Parameters  :
Out Parameters : bool
Description    : Starts the Pipe reader
Author         : Parth Software
--------------------------------------------------------------------------------------*/
bool CXlvCommunicatorServer::RunPipeReader()
{
	CAutoCriticalSection cas(m_CriticalSectionSys);
	if(CXLVNamedPipeListener::m_bStopListener)
	{
		return false;
	}
	CXLVNamedPipeListener* pPipeListener = new CXLVNamedPipeListener(this);
	pPipeListener->m_bMonitorConnections = m_bMonitorConnections;
	pPipeListener->m_hServerStopEvent = m_hServerStopEvent;
	pPipeListener->m_hLastClientDisconnectEvent = m_hLastClientDisconnectEvent;

	m_PipeListenerList.push_back(pPipeListener);

	return pPipeListener->StartReader();
}

/*--------------------------------------------------------------------------------------
Function       : OnConnectingPipe
In Parameters  :
Out Parameters : void
Description    : Called on every new connection which also spawns the new connection thread
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CXlvCommunicatorServer::OnConnectingPipe()
{
	__try{
		RunPipeReader();
	}
	__except(0)
	{
	}
}

/*--------------------------------------------------------------------------------------
Function       : OnDisConnectingPipe
In Parameters  : CXLVNamedPipeListener* pListener,
Out Parameters : void
Description    : Callback on pipe disconnect
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CXlvCommunicatorServer::OnDisConnectingPipe(CXLVNamedPipeListener* pListener)
{
	if(NULL == pListener)
	{
		return;
	}
	CAutoCriticalSection cas(m_CriticalSectionSys);

	if(m_PipeListenerList.size() > 0)
	{
		m_PipeListenerList.remove(pListener);
		delete pListener;
		pListener = NULL;
	}

	if((CXLVNamedPipeListener::m_bStopListener && m_PipeListenerList.size() == 0) || CXLVNamedPipeListener::m_bSingleThreaded)
	{
		OutputDebugString("SetEvent m_hLastClientDisconnectEvent");
		if(m_hLastClientDisconnectEvent)
		{
			::SetEvent(m_hLastClientDisconnectEvent);
		}
	}

}

/*--------------------------------------------------------------------------------------
Function       : OnIncomingData
In Parameters  : LPVOID lpParam,
Out Parameters : void
Description    : callback from server for Data receive.Calls the user registerd callback function
Author         : Parth Software
--------------------------------------------------------------------------------------*/
void CXlvCommunicatorServer::OnIncomingData(LPVOID lpParam)
{
	__try{
		m_fnPtrCallBack(lpParam);
	}
	__except(0)
	{

	}
}

/*--------------------------------------------------------------------------------------
Function       : GetPipeName
In Parameters  : void,
Out Parameters : TCHAR*
Description    : Returns the Pipe Name
Author         : Parth Software
--------------------------------------------------------------------------------------*/
TCHAR* CXlvCommunicatorServer::GetPipeName(void)
{
	return m_tchPipe;
}

/*--------------------------------------------------------------------------------------
Function       : GetStructSize
In Parameters  : void,
Out Parameters : DWORD
Description    : Returns the struct size that is used for communication
Author         : Parth Software
--------------------------------------------------------------------------------------*/
DWORD CXlvCommunicatorServer::GetStructSize(void)
{
	return m_dwStructSize;
}

/*--------------------------------------------------------------------------------------
Function       : SendResponse
In Parameters  : LPVOID lpData,
Out Parameters : bool
Description    : Sending a response from the communication server to the MaxComunicator
object
Author         : Parth Software
--------------------------------------------------------------------------------------*/
bool CXlvCommunicatorServer::SendResponse(LPVOID lpData)
{
	CAutoCriticalSection cas(m_CriticalSectionSys);
	for (TMaxNamedPipeListenerList::iterator it = m_PipeListenerList.begin(); it != m_PipeListenerList.end(); it++)
	{
		CXLVNamedPipeListener* pPipeListener = (*it);
		if(pPipeListener)
		{
			if(pPipeListener->m_nID == GetCurrentThreadId())
			{
				return pPipeListener->SendResponse(lpData);
			}
		}
	}
	return false;
}