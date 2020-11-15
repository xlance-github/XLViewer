#pragma once


#include "..\StdAfx.h"
#include "Color.h"


class CColorTable
{
public:
  CColorTable(const HWND hListWnd, COLORREF& begin, COLORREF& end);
  virtual ~CColorTable(void) {}

  void AddColor(const tstring& name);
  void AddItem(const tstring& name);

  void Refresh(bool refresh) { refresh_ = refresh; if (refresh_) Refresh(); }
  int Color(bool set, int control, int color, COLORREF& c);
  BOOL ProcessNotify(HWND hParent, LPARAM lParam);

  int InitialColumns();
  void ShowAll();

protected:
  LRESULT ProcessCustomDraw(LPARAM lParam);

  bool CheckControlColorVals(int control, int color);
  void Refresh();
  HWND ListWnd() const {return list_wnd_;}

  void ColorsCount(const size_t color_count);
  void NormalizeColorsCount();
  void ColumnCount(const size_t column_count);

  void ItemCount(const size_t item_count);

private:
  HWND list_wnd_;
  bool refresh_;
  int init_cols_count_;

  typedef std::deque<CColor> ColorList;
  typedef std::deque<ColorList> ColorListList;
  ColorListList colors_;

  typedef std::deque<tstring> ColorNamesList;
  ColorNamesList color_names_;

  typedef std::deque<tstring> ControlNamesList;
  ControlNamesList control_names_;

  COLORREF* custom_first_;
  COLORREF* custom_last_;
};
