#include "stdafx.h"
#include <string>
#include <fstream>

//
// BEGIN Turbodrive-desktop file/folder state
//

int getState(wchar_t *fileName)
{
    // Stream name
    std::wstring fileNameStr(fileName);

	if (fileNameStr == L"c:\\tmp\\myoverlayicon.txt")
	{
		return FOLDER_STATE_NOT_SET;
	}

    fileNameStr += L":Stream:$DATA";
    const wchar_t *fileNameCStr = fileNameStr.c_str();
	DWORD error = 0;

	std::wofstream log;

    HANDLE hStream = CreateFile(
        fileNameCStr,            // Filename
        GENERIC_READ,            // Desired access
        FILE_SHARE_READ | FILE_SHARE_WRITE,         // Share flags
        NULL,                    // Security Attributes
        OPEN_EXISTING,           // Creation Disposition
        0,						// Flags and Attributes
        NULL);                   // OVERLAPPED pointer

	error = GetLastError();

    if (hStream == INVALID_HANDLE_VALUE)
    {
		//log.open(L"c:/tmp/MyOverlayIcon.txt", std::ios_base::app);
		//log << "File [" << fileNameStr << "], failed to CreateFile, GetLastError() = " << error << std::endl;
		//log.close();
        return FOLDER_STATE_NOT_SET;
    }
    else
    {
        char buf[1] = {FOLDER_STATE_NOT_SET};
        int state = FOLDER_STATE_NOT_SET;
		DWORD numberOfBytesRead = 0;

        BOOL readResult = ReadFile(
            hStream,            // _In_         HANDLE hFile,
            buf,                // _Out_        LPVOID lpBuffer,
            1,                  // _In_         DWORD nNumberOfBytesToRead,
            &numberOfBytesRead,        // _Out_opt_    LPDWORD lpNumberOfBytesRead,
            NULL                //_Inout_opt_  LPOVERLAPPED lpOverlapped
        );

		error = GetLastError();

        if (readResult)
        {
            state = (int) buf[0];
			//log.open(L"c:/tmp/MyOverlayIcon.txt", std::ios_base::app);
			//log << "File " << fileNameStr << ", state " << state << std::endl;
			//log.close();
        }
		else
		{
			//log.open(L"c:/tmp/MyOverlayIcon.txt", std::ios_base::app);
			//log << "File [" << fileNameStr << "], failed to ReadFile, GetLastError() = " << error << std::endl;
			//log.close();
		}

        CloseHandle(hStream);

        return state;
    }

	error = GetLastError();

	//log.open(L"c:/tmp/MyOverlayIcon.txt", std::ios_base::app);
	//log << "File [" << fileNameStr << "], GetLastError() = " << error << std::endl;
	//log.close();

    return FOLDER_STATE_NOT_SET;
}

//
// Turbodrive-desktop file/folder state
//

