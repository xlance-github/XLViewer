#include <stdafx.h>

#include <algorithm>
#include <cstdlib>

#include "TimeZone.h"


// MS defined array for the registry
typedef struct tagTZREG {
    LONG Bias;
    LONG StandardBias;
    LONG DaylightBias;
    SYSTEMTIME StandardDate;
    SYSTEMTIME DaylightDate;
} TZREG;


void CTimeZone::EnumAll( std::deque<CTimeZone>& tzis )
{
	HKEY hKey;
  DWORD dwKeyCount = 0;

	// Open the registry key where MS hides the Time Zones
	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"), 0, KEY_READ, &hKey) == ERROR_SUCCESS )
	{
		// Get the number of time zones
		if( RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwKeyCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL) == ERROR_SUCCESS )
		{
      TIME_ZONE_INFORMATION tzi;

			// Fill in the Standard Names
			for( UINT iIndex = 0; iIndex < dwKeyCount; ++iIndex )
			{
        DWORD dwLen = _countof( tzi.StandardName );
				if (RegEnumKeyExW(hKey, iIndex, tzi.StandardName, &dwLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
			    TZREG	tzr;
          HKEY hKeyItem;
				   // Open the TZREG structure
				  if( RegOpenKeyExW(hKey, tzi.StandardName, 0, KEY_READ, &hKeyItem) == ERROR_SUCCESS )
				  {
					   // Get the TZREG structure
					  DWORD dwLen = sizeof(tzi.DaylightName);
					  DWORD dwType = REG_SZ;
					  RegQueryValueEx(hKeyItem, _T("Dlt"), NULL, &dwType, (LPBYTE)tzi.DaylightName, &dwLen);
					  dwLen = sizeof(TZREG);
					  dwType = REG_BINARY;
					  RegQueryValueEx(hKeyItem, _T("TZI"), NULL, &dwType, (LPBYTE)&tzr, &dwLen);

					  // Copy the TZREG to the TZI
					  tzi.Bias = tzr.Bias;
					  tzi.DaylightBias = tzr.DaylightBias;
					  tzi.DaylightDate = tzr.DaylightDate;
					  tzi.StandardBias = tzr.StandardBias;
					  tzi.StandardDate = tzr.StandardDate;

					  // Close the item
					  RegCloseKey(hKeyItem);

            tzis.push_back(tzi);
				  }
        }
			}
    }
    // Close the registry
		RegCloseKey(hKey);
  }

  std::sort(tzis.begin(), tzis.end(), CompareByBias);
}


bool CTimeZone::CompareByBias(const CTimeZone& l, const CTimeZone& r)
{
  return l.GetData().Bias > r.GetData().Bias;
}


const tstring& CTimeZone::ToString(tstring& res) const 
{
  WToTChar(GetData().StandardName, res);

  TCHAR bias_str[10];
  int hour_bias = GetData().Bias / 60;
  _stprintf( bias_str, _T("(UTC%c%d)"), ((hour_bias<=0) ? _T('+'):_T('-')), abs(hour_bias) );

  res = _T(" ") + res;
  res = bias_str + res;

  return res;
}


bool CTimeZoneFind::operator()( const CTimeZone& tz ) const
{
  tstring tz_name;
  WToTChar(tz.GetData().StandardName, tz_name);

  return (hour_bias_ == tz.GetHourBias() ) && 
    ( !use_name_ || (use_name_ && ( standart_name_ == tz_name )));
}
