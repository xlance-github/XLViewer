/*
                      EASY SPLITTERS
          Draggable splitter window custom control

             For Pelles C, By L. D. Blake 2005

*/
#include <stdafx.h>
#include "easysplit.h"

// Return coordinates of a child window inside it's
// parent window's client area.

BOOL WINAPI GetChildRect(HWND hWin, PRECT pRect)
{ 
  POINT pr = {0,0};
  if (!(GetWindowLong(hWin,GWL_STYLE) & WS_CHILD))
    return 0;
  // parent window's client offsets
  ClientToScreen(GetParent(hWin),&pr);
  // now we need the child rect
  GetWindowRect(hWin,pRect);
  pRect->left   -= pr.x;
  pRect->top    -= pr.y;
  pRect->right  -= pr.x;
  pRect->bottom -= pr.y;
  return 1; 
}
