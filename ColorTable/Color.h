#pragma once


#include "..\StdAfx.h"


class CColor
{
public:
  CColor()  : color_(0) {}
  CColor(COLORREF color)  : color_(color) {}
  virtual ~CColor(void) {}

  COLORREF Color() const
  {
    return color_;
  }

  void Color(COLORREF color)
  {
    color_ = color;
  }

private:
  COLORREF color_;
};
