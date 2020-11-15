#ifndef STDAFX_H
#define STDAFX_H

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#include <Windows.h>
#include <Windowsx.h>
#include <TChar.h>
#include <assert.h>

#ifdef __cplusplus
#include <deque>
#include <string>
#include <vector>
#include "Constants.h"

#define ID_UPDATE_TAB_CONTROL       WM_USER + 100

enum TokenConstant
{
NONE = 0,
XLV = 7,
LOAD = 8,
ALL = 14,
W = 16,
F = 21,
UNLOAD = 44,
TABZERO = 67,
TABONE = 28,
TABTWO = 32,
TABTHREE = 255
};

typedef std::basic_string<TCHAR> tstring;

void ErrorDescription(HRESULT hr, char * szRetErrMsg);
void AToWChar(const char* source, std::wstring& res);
void WToAChar(const wchar_t* source, std::string& res, bool best_fit_chars=false);

void AToTChar(const char* source, tstring& res);
void TToAChar(const TCHAR* source, std::string& res, bool best_fit_chars = false );
void TToWChar(const TCHAR* source, std::wstring& res);
void WToTChar(const WCHAR* source, tstring& res, bool best_fit_chars = false );
void StringTrim(std::string& str);
#endif

#endif

