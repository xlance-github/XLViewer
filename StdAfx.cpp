#include "StdAfx.h"

void ErrorDescription(HRESULT hr, char * szRetErrMsg) 
{ 
     if(FACILITY_WINDOWS == HRESULT_FACILITY(hr)) 
         hr = HRESULT_CODE(hr); 

	TCHAR* szErrMsg; 

     if(FormatMessage( 
       FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 
       NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
       (LPTSTR)&szErrMsg, 0, NULL) != 0) 
     { 
		 sprintf(szRetErrMsg, TEXT("%s"), szErrMsg); 
         LocalFree(szErrMsg); 
     } else 
         sprintf(szRetErrMsg, TEXT("[Could not find a description for error # %#x.]\n"), hr); 
}

void AToWChar(const char* source, std::wstring& res)
{
  std::vector< wchar_t > buf;
  const size_t len = strlen( source );
  buf.resize( len+1 );
  BOOL bRes = FALSE;
  MultiByteToWideChar(CP_ACP, 0, source, len, &buf[0], len);
  res = &buf[0];
}


void WToAChar(const wchar_t* source, std::string& res, bool best_fit_chars)
{
  std::vector< char > buf;
  const size_t len = wcslen( source );
  buf.resize( len+1 );
  BOOL bRes = FALSE;
  WideCharToMultiByte(CP_ACP, best_fit_chars?0:WC_NO_BEST_FIT_CHARS, source, len, &buf[0], len, "?", &bRes);
  res = &buf[0];
}


void AToTChar(const char* source, tstring& res)
{
#ifdef _UNICODE
  AToWChar(source, res);
#else
  res = source;
#endif
}


void TToAChar( const TCHAR* source, std::string& res, bool best_fit_chars )
{
#ifdef _UNICODE
  WToAChar(source, res, best_fit_chars);
#else
  res = source;
#endif
}


void TToWChar(const TCHAR* source, std::wstring& res)
{
#ifdef _UNICODE
  res = source;
#else
  AToWChar(source, res);
#endif
}


void WToTChar( const WCHAR* source, tstring& res, bool best_fit_chars )
{
#ifdef _UNICODE
  res = source;
#else
  WToAChar(source, res, best_fit_chars);
#endif
}

void StringTrim(std::string& str)
{
	std::string::size_type pos = str.find_last_not_of(' ');
  if(pos != std::string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != std::string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

