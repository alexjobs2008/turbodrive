#include "overlayiconshandler.h"
#include "FileUtils.h"
#include <ActiveQt/QAxFactory>

#include <windows.h>

int OverlayIconsHandler::getState(char *fileName)
{
    // Stream name
    QString fileNameStr(fileName);
    fileNameStr += ":Stream";
    const wchar_t *fileNameCStr = fileNameStr.toStdWString().c_str();

    // Open stream for exclusive read
    HANDLE hStream = CreateFile(
        fileNameCStr,            // Filename
        GENERIC_READ,            // Desired access
        0,                       // Share flags
        NULL,                    // Security Attributes
        OPEN_EXISTING,           // Creation Disposition
        0,                       // Flags and Attributes
        NULL);                   // OVERLAPPED pointer

    if (hStream == INVALID_HANDLE_VALUE)
    {
        return Drive::FOLDER_STATE_NOT_SET;
    }
    else
    {
        char buf[1] = {Drive::FOLDER_STATE_NOT_SET};
        int state = Drive::FOLDER_STATE_NOT_SET;

        BOOL readResult = ReadFile(
            hStream,            // _In_         HANDLE hFile,
            buf,                // _Out_        LPVOID lpBuffer,
            1,                  // _In_         DWORD nNumberOfBytesToRead,
            NULL,               // _Out_opt_    LPDWORD lpNumberOfBytesRead,
            NULL                //_Inout_opt_  LPOVERLAPPED lpOverlapped
        );

        if (readResult)
        {
            state = (int) buf[0];
        }

        CloseHandle(hStream);

        return state;
    }

    return Drive::FOLDER_STATE_NOT_SET;
}

OverlayIconsHandler::OverlayIconsHandler(int iconIndex) :
    iconIndex(iconIndex)
{
}

long OverlayIconsHandler::queryInterface(const QUuid &iid, void **iface)
{
    *iface = 0;

    if (iid == QUuid(IID_IShellIconOverlayIdentifier))
    {
        *iface = (void *)(IShellIconOverlayIdentifier *)this;
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


// CMyOverlayIcon
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE OverlayIconsHandler::GetOverlayInfo(
             PWSTR pwszIconFile,
             int cchMax,
             int* pIndex,
             DWORD* pdwFlags)
{
    // Get our module's full path
    GetModuleFileNameW(
        GetModuleHandleW(QAxFactory::serverFilePath().toStdWString().c_str()),
        pwszIconFile, cchMax);

    // Use first icon in the resource
    *pIndex = iconIndex;

    *pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;
    return S_OK;
}

// IShellIconOverlayIdentifier::GetPriority
// returns the priority of this overlay 0 being the highest.
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE OverlayIconsHandler::GetPriority(int* pPriority)
{
    // we want highest priority
    *pPriority=0;
    return S_OK;
}

// IShellIconOverlayIdentifier::IsMemberOf
// Returns whether the object should have this overlay or not
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE OverlayIconsHandler::IsMemberOf(PCWSTR pwszPath, DWORD /*dwAttrib*/)
{
    wchar_t *s = _wcsdup(pwszPath);
    HRESULT r = S_FALSE;

    _wcslwr(s);

    // Criteria
    if (wcsstr(s, L"codeproject") != 0)
    r = S_OK;

    free(s);

    return r;
}
