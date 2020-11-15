#pragma once;

class CXLVNamedPipeListener;

interface IMaxNamedPipeData
{
	virtual void OnIncomingData(LPVOID lpParam) = 0;
	virtual void OnConnectingPipe() = 0;
	virtual void OnDisConnectingPipe(CXLVNamedPipeListener* pReader) = 0;
	virtual TCHAR* GetPipeName(void) = 0;
	virtual DWORD GetStructSize(void) = 0;
	virtual bool SendResponse(LPVOID lpData) = 0;
};

class CXLVNamedPipeListener
{
public:
	CXLVNamedPipeListener(IMaxNamedPipeData* pDest);
	~CXLVNamedPipeListener();

	bool StartReader(void);
	bool SendResponse(LPVOID lpResponse);
	static bool m_bStopListener;
	static bool m_bSingleThreaded;
	DWORD	m_nID;
	bool m_bMonitorConnections;
	HANDLE m_hServerStopEvent;
	HANDLE m_hLastClientDisconnectEvent;
private:
	static DWORD WINAPI NamedPipeListenerThread(LPVOID lParam);
	bool ReadPipe(void);
	void Cleanup();
	
	HANDLE	m_hPipe;
	HANDLE	m_hThread;
	HANDLE m_hOverlap[2];
	IMaxNamedPipeData* m_pDest;
};