#ifndef GRAPHICS_MODE_H
#define GRAPHICS_MODE_H


#include <windows.h>


struct GRAPHICS_MODE
{
public:
  DWORD dwResolution;
  WORD wSupportedBitsPerPixel;
  WORD wFrequency;

  int compare(GRAPHICS_MODE& gm)
  {
    bool bRes = (dwResolution > gm.dwResolution) && (gm.wFrequency > gm.wFrequency) && (wSupportedBitsPerPixel > gm.wSupportedBitsPerPixel);

    return bRes ? 1: (!bRes ? -1 : 0);
  }
};

#endif
