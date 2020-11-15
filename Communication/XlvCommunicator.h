#pragma once
class CXlvCommunicator
{
public:
	// Constructor
	CXlvCommunicator(const TCHAR* tchPipeName, bool bRetryConnection = false);
	// Destructor
	~CXlvCommunicator();
	// Member Functions
	bool SendData(LPVOID lpMaxData, DWORD dwSize);
	bool ReadData(LPVOID lpMaxData, DWORD dwSize);
	void Close();
private:
	// To connect to a given named pipe
	bool Connect(void);
	// Handle to the Named Pipe
	HANDLE m_hPipe;
	// Pipe Name
	TCHAR m_tchPipe[MAX_PATH];
	bool m_bRetryConnection;
};