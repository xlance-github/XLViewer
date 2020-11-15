#include "StdAfx.h"

#ifdef _DEBUG
#import "..\\Debug\\XLanceDLL.tlb" 
#else
#import "..\\Release\\XLanceDLL.tlb" 
#endif


class CIntropCom
{
public:
	CIntropCom();
	~CIntropCom();
	bool CreateComcomponant();
	void ReleaseComponant();

public:
		XLanceDLL::IMyClassPtr m_pIComIntrop;	
		tstring strLastError;
};