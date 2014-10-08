// MTSDiskSyncOverlayIcon.h : Declaration of the CMTSDiskSyncOverlayIcon

#pragma once
#include "resource.h"       // main symbols



#include "OverlayIcon.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CMTSDiskSyncOverlayIcon

class ATL_NO_VTABLE CMTSDiskSyncOverlayIcon :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMTSDiskSyncOverlayIcon, &CLSID_MTSDiskSyncOverlayIcon>,
	public IDispatchImpl<IMTSDiskSyncOverlayIcon, &IID_IMTSDiskSyncOverlayIcon, &LIBID_OverlayIconLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellIconOverlayIdentifier
{
public:
	CMTSDiskSyncOverlayIcon()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MTSDISKSYNCOVERLAYICON)


BEGIN_COM_MAP(CMTSDiskSyncOverlayIcon)
	COM_INTERFACE_ENTRY(IMTSDiskSyncOverlayIcon)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellIconOverlayIdentifier)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	// IShellIconOverlayIdentifier Methods
	STDMETHOD(GetOverlayInfo)(LPWSTR pwszIconFile,int cchMax,int *pIndex,DWORD* pdwFlags);
	STDMETHOD(GetPriority)(int* pPriority);
	STDMETHOD(IsMemberOf)(LPCWSTR pwszPath,DWORD dwAttrib);

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



};

OBJECT_ENTRY_AUTO(__uuidof(MTSDiskSyncOverlayIcon), CMTSDiskSyncOverlayIcon)