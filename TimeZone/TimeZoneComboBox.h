#pragma once


#include "..\StdAfx.h"
#include "TimeZone.h"


class CTimeZoneComboBox
{
public:
  CTimeZoneComboBox(HWND hComboWnd) : hComboWnd_(hComboWnd) {};
  ~CTimeZoneComboBox(void) {}

  void Fill();
  const CTimeZone* GetCurrentTZInfo() const;

protected:
  HWND GetComboWnd() const { return hComboWnd_; }

private:
  HWND hComboWnd_;

  typedef std::deque<CTimeZone> TimeZoneList;
  TimeZoneList tzis_;
};
