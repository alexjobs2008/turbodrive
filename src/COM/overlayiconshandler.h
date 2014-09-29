#ifndef OVERLAYICONSHANDLER_H
#define OVERLAYICONSHANDLER_H

#include <QtCore/QObject>
#include <QtWidgets/QWidget>
#include <ActiveQt/QAxAggregated>

#include <comdef.h>
#include <Shlobj.h>

// Icon overlay handler
class OverlayIconsHandler :
        public QAxAggregated,
        public IShellIconOverlayIdentifier
{
    int iconIndex;

    // Read state from file ::$DATA stream
    int getState(char *fileName);

public:
    OverlayIconsHandler(int iconIndex);

    long queryInterface(const QUuid &iid, void **iface);

    // IUnknown
    QAXAGG_IUNKNOWN

    // IShellIconOverlayIdentifier
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IsMemberOf(PCWSTR pwszPath, DWORD);
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetOverlayInfo(PWSTR pwszIconFile, int cchMax, int *pIndex, DWORD *pdwFlags);
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPriority(int *pIPriority);

};

#endif // OVERLAYICONSHANDLER_H
