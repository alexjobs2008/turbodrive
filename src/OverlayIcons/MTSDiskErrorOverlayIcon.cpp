// MTSDiskErrorOverlayIcon.cpp : Implementation of CMTSDiskErrorOverlayIcon

#include "stdafx.h"
#include "MTSDiskErrorOverlayIcon.h"
#include <fstream>


// CMTSDiskErrorOverlayIcon

// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CMTSDiskErrorOverlayIcon::GetOverlayInfo(
  LPWSTR pwszIconFile,
  int cchMax,int* pIndex,
  DWORD* pdwFlags)
{
  GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);

  *pIndex = 1;
  *pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;

  return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 

// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CMTSDiskErrorOverlayIcon::GetPriority(int* pPriority)
{
  // highest priority
  *pPriority=0;
  return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CMTSDiskErrorOverlayIcon::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
  wchar_t *s = _wcsdup(pwszPath);
  HRESULT r = S_FALSE;
  std::wofstream log;
  
  _wcslwr(s);

  // Criteria
  // if (wcsstr(s, L"codeproject") != 0)
  //   r = S_OK;

  int state = getState(s);

  if (state == FOLDER_ICON_ERROR) r = S_OK;

  free(s);

  //log.open(L"c:/tmp/CMTSDiskErrorOverlayIcon.txt", std::ios_base::app);
  //log << "File [" << pwszPath << "], state [" << state << "], result [" << r << "]" << std::endl;
  //log.close();

  return r;
}
