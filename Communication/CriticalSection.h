#pragma once
const int MAX_SEM_COUNT = 1;
const int MAX_WAIT_COUNT = 60000*2;
class CMaxSemaphore;
class CAutoCriticalSection;
class CAutoSemaphore;

class CMaxCriticalSection
{
public:
	CMaxCriticalSection();
	virtual ~CMaxCriticalSection();

	void Lock();
	void Unlock();

private:
	CRITICAL_SECTION m_CritSect;
};

class CMaxSemaphore
{
public:
	CMaxSemaphore(int nMaxCount = MAX_SEM_COUNT);
	virtual ~CMaxSemaphore();

	void Lock();
	void Unlock();

private:
	HANDLE m_hSemaphore;;
	int m_nMaxCount;
};

class CAutoCriticalSection
{
public:
	CAutoCriticalSection(CMaxCriticalSection& rCritSect);
	virtual ~CAutoCriticalSection();

private:
	CMaxCriticalSection& m_rCritSect;
};

class CAutoSemaphore
{
public:
	CAutoSemaphore(CMaxSemaphore& rSemaphore);
	virtual ~CAutoSemaphore();

private:
	CMaxSemaphore& m_rSemaphore;
};