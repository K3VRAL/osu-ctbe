#include "open.hpp"

// https://stackoverflow.com/a/35623208
DWORD open(wstring name)
{
    HANDLE h_process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (h_process_snap == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }

    PROCESSENTRY32W pe32{ sizeof(pe32) };
    if (!Process32FirstW(h_process_snap, &pe32))
    {
        CloseHandle(h_process_snap);
        return NULL;
    }

    do
    {
        if (pe32.szExeFile != name)
        {
            continue;
        }

        CloseHandle(h_process_snap);
        return pe32.th32ProcessID;
    } while (Process32NextW(h_process_snap, &pe32));

    CloseHandle(h_process_snap);
    return NULL;
}
