#pragma once


#include "..\StdAfx.h"
#include <deque>


class CTimeZone
{
public:
  CTimeZone(const TIME_ZONE_INFORMATION& tzi) :tzi_(tzi) {}
  virtual ~CTimeZone(void) {}

  typedef std::deque<CTimeZone> TZList;
  static void EnumAll( TZList& tzis );
  const TIME_ZONE_INFORMATION& GetData() const    { return tzi_; }
  void SetData(const TIME_ZONE_INFORMATION& tzi)  { tzi_ = tzi; }

  const tstring& ToString(tstring& res) const;
  const int GetHourBias() const { return GetData().Bias / 60; }

protected:
  static bool CompareByBias(const CTimeZone& l, const CTimeZone& r);

private:
  TIME_ZONE_INFORMATION tzi_;
};


class CTimeZoneFind
{
public:
  CTimeZoneFind(long hour_bias, const TCHAR* standart_name )
    : hour_bias_(hour_bias)
    , standart_name_(standart_name)
  {}
  bool operator()( const CTimeZone& tz ) const;
  void UseName(bool use_name) { use_name_ = use_name; }
  void SetHourBias(long hour_bias) { hour_bias_ = hour_bias; }

private:
  long hour_bias_;
  tstring standart_name_;
  bool use_name_;
};
