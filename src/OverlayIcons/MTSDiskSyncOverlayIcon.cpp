// MTSDiskSyncOverlayIcon.cpp : Implementation of CMTSDiskSyncOverlayIcon

#include "stdafx.h"
#include "MTSDiskSyncOverlayIcon.h"
#include <fstream>


// CMTSDiskSyncOverlayIcon
// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CMTSDiskSyncOverlayIcon::GetOverlayInfo(
  LPWSTR pwszIconFile,
  int cchMax,int* pIndex,
  DWORD* pdwFlags)
{
  GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);

  *pIndex = 3;
  *pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;

  return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 

// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CMTSDiskSyncOverlayIcon::GetPriority(int* pPriority)
{
  // highest priority
  *pPriority=0;
  return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CMTSDiskSyncOverlayIcon::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
  wchar_t *s = _wcsdup(pwszPath);
  HRESULT r = S_FALSE;
  std::wofstream log;
  
  _wcslwr(s);

  // Criteria
  // if (wcsstr(s, L"codeproject") != 0)
  //   r = S_OK;

  int state = getState(s);

  if (state == FOLDER_ICON_SYNC) r = S_OK;

  free(s);

  //log.open(L"c:/tmp/CMTSDiskSyncOverlayIcon.txt", std::ios_base::app);
  //log << "File [" << pwszPath << "], state [" << state << "], result [" << r << "]" << std::endl;
  //log.close();

  return r;
}

