#include <stdafx.h>
#include "TimeZoneComboBox.h"


void CTimeZoneComboBox::Fill()
{
  if ( tzis_.empty() )
  {
    CTimeZone::EnumAll( tzis_ );
  }

  // clear combo and fill it with tz strings
  ComboBox_ResetContent( GetComboWnd() );
  tstring buf;
  for ( TimeZoneList::const_iterator tz_ci = tzis_.begin();
    tz_ci != tzis_.end();
    ++tz_ci )
  {
    ComboBox_AddString( GetComboWnd(), tz_ci->ToString(buf).c_str() );
  }
}


const CTimeZone* CTimeZoneComboBox::GetCurrentTZInfo() const
{
  const int cur_sel = ComboBox_GetCurSel( GetComboWnd() );
  if (( cur_sel == -1 ) || ( (unsigned int)cur_sel >= tzis_.size()))
  {
    return NULL;
  }

  return &tzis_[cur_sel];
}
