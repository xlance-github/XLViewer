#include "StdAfx.h"
#include <CommCtrl.h>
#include "ColorTable.h"


CColorTable::CColorTable(const HWND hListWnd
  , COLORREF& first
  , COLORREF& last)
    : list_wnd_(hListWnd)
{
  COLORREF* pb = &first;
  COLORREF* pe = &last;
  if ((pe > pb) && (pb-pe<=15))
  {
    custom_first_ = pb;
    custom_last_ = pe;
  }

  init_cols_count_ = InitialColumns();
}


int CColorTable::InitialColumns()
{
  HWND hListHdr = ListView_GetHeader(ListWnd());
  HDC hListDC = GetDC(hListHdr);
  const TCHAR* col_names[] = {_T("Text demo"), _T("Selecting demo")};
  int max_cx = 0;
  for (size_t col_it = 0; col_it < _countof(col_names); ++col_it)
  {
    LVCOLUMN col_data;
    memset(&col_data, 0, sizeof(col_data));

    SIZE text_sz;
    GetTextExtentPoint(hListDC, col_names[col_it], _tcslen(col_names[col_it]), &text_sz);

    col_data.cx = text_sz.cx;
    col_data.pszText = const_cast< LPTSTR >( col_names[col_it] );
    col_data.mask = LVCF_WIDTH | LVCF_TEXT;
    ListView_InsertColumn(ListWnd(), col_it, &col_data);
  }
  ReleaseDC(hListHdr, hListDC);
  return _countof(col_names);
}


void CColorTable::AddColor(const tstring& name)
{
  color_names_.push_back(name);
}


void CColorTable::NormalizeColorsCount()
{
  for (ColorListList::iterator cll_ci = colors_.begin();
    cll_ci != colors_.end();
    ++cll_ci)
  {
    cll_ci->resize(color_names_.size());
  }
  Refresh();
}


void CColorTable::AddItem(const tstring& name)
{
  control_names_.push_back(name);
  colors_.resize(control_names_.size());
  NormalizeColorsCount();
}


int CColorTable::Color(bool set, int control, int color, COLORREF& c)
{
  if (!CheckControlColorVals(control, color))
    return FALSE;
  if (set)
    colors_[control][color].Color(c);
  else
    c = colors_[control][color].Color();
  return TRUE;
}


BOOL CColorTable::ProcessNotify(HWND hParent, LPARAM lParam)
{
  LPNMLISTVIEW pnm = reinterpret_cast<LPNMLISTVIEW>(lParam);
  if ( (pnm->hdr.hwndFrom == ListWnd())
    &&  (pnm->hdr.code == NM_CUSTOMDRAW) )
  {
    SetWindowLong(hParent, DWL_MSGRESULT, ProcessCustomDraw(lParam));
    return TRUE;
  }
  if (pnm->hdr.code == NM_DBLCLK)
  {
    int item = pnm->iItem;
    int subitem = pnm->iSubItem;

    if (pnm->iSubItem < init_cols_count_)
      return TRUE;

    if (item == -1)
      item = ListView_GetSelectionMark(ListWnd());

    if (item == -1)
    {
      for (int i = 0; i < ListView_GetItemCount(ListWnd()); ++i )
      {
        RECT rct;
        ListView_GetItemRect(ListWnd(), i, &rct, LVIR_BOUNDS);

        if (PtInRect(&rct, pnm->ptAction))
        {
          item = i;
          break;
        }
      }
    }

    assert(item != -1);
    if (item != -1)
    {
      CHOOSECOLOR cc;                 // common dialog box structure 
      COLORREF acrCustClr[16]; // array of custom colors 
      std::copy(custom_first_, custom_last_+1, acrCustClr);
      HWND hwnd = ListWnd();                      // owner window
      HBRUSH hbrush;                  // brush handle
      static DWORD rgbCurrent;        // initial color selection

      // Initialize CHOOSECOLOR 
      ZeroMemory(&cc, sizeof(cc));
      cc.rgbResult = colors_[item][subitem-init_cols_count_].Color();
      cc.lStructSize = sizeof(cc);
      cc.hwndOwner = hwnd;
      cc.lpCustColors = (LPDWORD) acrCustClr;
      cc.rgbResult = rgbCurrent;
      cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
 
      if (ChooseColor(&cc)==TRUE) 
      {
        colors_[item][subitem-init_cols_count_].Color(cc.rgbResult);
        ListView_Update(ListWnd(), item);
      }
    }
  }
  return FALSE;
}


LRESULT CColorTable::ProcessCustomDraw(LPARAM lParam)
{
  LPNMLVCUSTOMDRAW lplvcd = reinterpret_cast< LPNMLVCUSTOMDRAW >( lParam );
  switch(lplvcd->nmcd.dwDrawStage) 
  {
  case CDDS_PREPAINT : //Before the paint cycle begins
    //request notifications for individual listview items
    return CDRF_NOTIFYITEMDRAW;

  case CDDS_ITEMPREPAINT: //Before an item is drawn
    {
      return CDRF_NOTIFYSUBITEMDRAW;
    }
    break;

  case CDDS_SUBITEM | CDDS_ITEMPREPAINT: //Before a subitem is drawn
    {
      switch(lplvcd->iSubItem)
      {
      case 0:
        {
          // TODO more optional, add callback function for color behaviour
          lplvcd->clrText = colors_[lplvcd->nmcd.dwItemSpec][0].Color();
          lplvcd->clrTextBk = colors_[lplvcd->nmcd.dwItemSpec][1].Color();
          return CDRF_NEWFONT;
        }
        break;
      case 1:
        {
          lplvcd->clrText = colors_[lplvcd->nmcd.dwItemSpec][2].Color();
          lplvcd->clrTextBk = colors_[lplvcd->nmcd.dwItemSpec][3].Color();
          return CDRF_NEWFONT;
        }
        break;
      default:
        {
          lplvcd->clrTextBk = colors_[lplvcd->nmcd.dwItemSpec][lplvcd->iSubItem-init_cols_count_].Color();
          return CDRF_NEWFONT;
        }
      }
      break;
    }
  }
  return CDRF_DODEFAULT;
}


bool CColorTable::CheckControlColorVals(int control, int color)
{
  return !((control<0) || ((unsigned int)control>=control_names_.size())
    || (color<0) || ((unsigned int)color >= color_names_.size()));
}


void CColorTable::Refresh()
{
  ColumnCount(color_names_.size()+2);
  ItemCount(control_names_.size());
  ShowAll();
}


void CColorTable::ItemCount(const size_t item_count)
{
  size_t item_count_now = ListView_GetItemCount(ListWnd());
  for (; item_count_now < item_count; ++item_count_now)
  {
    LVITEM lvi;
    memset(&lvi, 0, sizeof(lvi));
    lvi.iItem = item_count_now;
    lvi.pszText = const_cast<LPTSTR>( control_names_[item_count_now].c_str() );
    lvi.mask = LVIF_TEXT;

    ListView_InsertItem(ListWnd(), &lvi);
    ListView_SetItemText(ListWnd(), item_count_now, 1, const_cast<LPTSTR>( control_names_[item_count_now].c_str() ));
  }
  if (item_count_now == item_count)
    return;

  for (; item_count_now > item_count; --item_count_now)
  {
    ListView_DeleteItem(ListWnd(), item_count_now-1);
  }
  assert(item_count_now == item_count);
}


void CColorTable::ColumnCount(const size_t column_count)
{
  HWND hListHdr = ListView_GetHeader(ListWnd());
  size_t col_count_now = Header_GetItemCount(hListHdr);

  HDC hListDC = GetDC(hListHdr);
  // if need more columns
  for (; col_count_now < column_count; ++col_count_now)
  {
    LVCOLUMN col_data;
    memset(&col_data, 0, sizeof(col_data));

    SIZE text_sz;
    GetTextExtentPoint(hListDC, color_names_[col_count_now-init_cols_count_].c_str()
      , color_names_[col_count_now-init_cols_count_].length(), &text_sz);

    col_data.cx = text_sz.cx;
    col_data.pszText = const_cast< LPTSTR >( color_names_[col_count_now-init_cols_count_].c_str() );
    col_data.mask = LVCF_WIDTH | LVCF_TEXT;
    ListView_InsertColumn(ListWnd(), col_count_now, &col_data);
  }
  ReleaseDC(hListHdr, hListDC);
  if (col_count_now == column_count)
    return;

  // if need less columns
  for (; col_count_now > column_count; --col_count_now)
  {
    ListView_DeleteColumn(ListWnd(), col_count_now-1);
  }
  assert(col_count_now == column_count);
}


void CColorTable::ShowAll()
{
  HWND hListHdr = ListView_GetHeader(ListWnd());
  size_t item_count = ListView_GetItemCount(ListWnd());
  HDC hListDC = GetDC(ListWnd());
  for (size_t col_it = 0; col_it < Header_GetItemCount(hListHdr); ++col_it)
  {
    int cx = 0;
    TCHAR text[100];
    for (size_t item_it = 0; item_it < item_count; ++item_it)
    {
      ListView_GetItemText(ListWnd(), item_it, col_it, text, _countof(text));

      SIZE text_sz;
      GetTextExtentPoint(hListDC, text, lstrlen(text), &text_sz);
      if (text_sz.cx > cx)
        cx = text_sz.cx;
    }
    if (cx > ListView_GetColumnWidth(ListWnd(), col_it))
      ListView_SetColumnWidth(ListWnd(), col_it, cx);
  }
  ReleaseDC(hListHdr, hListDC);
}
