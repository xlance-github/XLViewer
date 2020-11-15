#pragma once

#include <list>
#include "XlvNamedPipeListener.h"
#include "CriticalSection.h"

typedef std::list<CXLVNamedPipeListener*> TMaxNamedPipeListenerList;
typedef void (*CallBackFunctionPtr)(LPVOID lpParam);

class CXlvCommunicatorServer : public IMaxNamedPipeData
{
public:
	CXlvCommunicatorServer(const TCHAR* tchPipeName, CallBackFunctionPtr fnPtrCallBack, DWORD dwSize);
	virtual ~CXlvCommunicatorServer();

	bool Run(bool bMonitorConnections = false,bool bSingleThreaded = false);
	virtual bool SendResponse(LPVOID lpData);
	virtual bool StopServer();
protected:
	virtual void OnIncomingData(LPVOID sMaxPipeData);
	virtual void OnConnectingPipe();
	virtual void OnDisConnectingPipe(CXLVNamedPipeListener* pListener);
	virtual TCHAR* GetPipeName(void);
	virtual DWORD GetStructSize(void);
private:
	bool RunPipeReader(void);
	TMaxNamedPipeListenerList	m_PipeListenerList;
	CMaxCriticalSection			m_CriticalSectionData;
	CMaxCriticalSection			m_CriticalSectionSys;
	CallBackFunctionPtr			m_fnPtrCallBack;
	TCHAR						m_tchPipe[MAX_PATH];
	DWORD						m_dwStructSize;
	bool m_bMonitorConnections;
	bool m_bServerRunning;
	HANDLE m_hServerStopEvent;
	HANDLE m_hLastClientDisconnectEvent;

};