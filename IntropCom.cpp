#include "IntropCom.h"

CIntropCom::CIntropCom()
{
	m_pIComIntrop = NULL;
}

CIntropCom::~CIntropCom()
{
	if(NULL != m_pIComIntrop)
	{
		m_pIComIntrop->Release();
	}
}

bool CIntropCom::CreateComcomponant()
{
	char szDes[100] = {0};

	try{
		CoInitialize(NULL);
		HRESULT hRes = m_pIComIntrop.CreateInstance(__uuidof(XLanceDLL::XLanceClass));
		if (hRes == S_OK)
		{
			return true;
		}
		else
		{
			ErrorDescription(hRes, szDes);
			strLastError += szDes;
			strLastError += "\n";
		}
		CoUninitialize();
	}
	catch(_com_error &e)
	{
		strLastError = _com_error(e).ErrorMessage();
		strLastError += "\n";
		OutputDebugString(strLastError.c_str());	
		
		//AddLogEntry(L"CreateDBInstance..._com_error:%s",(LPCTSTR)bstr);
	}
	return false;
}

void CIntropCom::ReleaseComponant()
{
	if(NULL != m_pIComIntrop)
	{
		m_pIComIntrop->Release();
	}
}