#include "main.hpp"

// https://github.com/UnnamedOrange/osu-memory/blob/main/native/include/osu_memory/readable_process.hpp#L92
uintptr_t find(vector<uint8_t> bin, string mask, HANDLE h_process)
{
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    PVOID min_address = sys_info.lpMinimumApplicationAddress;
    PVOID max_address = sys_info.lpMaximumApplicationAddress;

    PVOID crt_address = min_address;
    while (crt_address < max_address)
    {
        MEMORY_BASIC_INFORMATION mem_info;
        if (!VirtualQueryEx(h_process, crt_address, &mem_info, sizeof(mem_info)))
        {
            return (uintptr_t)NULL;
        }

        if ((mem_info.Protect & PAGE_EXECUTE_READWRITE) && mem_info.State == MEM_COMMIT)
        {
            vector<uint8_t> ret(mem_info.RegionSize);
            SIZE_T read{};
            if (ReadProcessMemory(h_process, (LPCVOID)(mem_info.BaseAddress), ret.data(), mem_info.RegionSize, &read) == 0
                || read != mem_info.RegionSize)
            {
                continue;
            }

            for (size_t i = 0; i + bin.size() - 1 < ret.size(); i++)
            {
                bool ok = true;
                for (size_t j = 0; j < bin.size(); j++)
                {
                    if (!(mask[j] == '?' || ret[i + j] == bin[j]))
                    {
                        ok = false;
                        break;
                    }
                }
                if (ok)
                {
                    return uintptr_t(mem_info.BaseAddress) + i;
                }
            }
        }
        crt_address = PVOID(uintptr_t(crt_address) + mem_info.RegionSize);
    }
    return (uintptr_t)NULL;
}
