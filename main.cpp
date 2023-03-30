#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>

using namespace std;

/* Thanks to all github projects and other helpful links:
 * UnnamedOrange / osu-memory           https://github.com/UnnamedOrange/osu-memory             | For being the entry point and a massive help of my journey in learning more about how memory works and how to find them easily (though other projects did teach me in other aspects).
 * l3lackShark / gosumemory             https://github.com/l3lackShark/gosumemory               | Providing a lot more memory signatures, a lot I experimented, and a lot didn't seemed to work (this could be the fault of mine though), I'm especially thankful for the Song Time memory signature.
 * OsuSync / OsuRTDataProvider          https://github.com/OsuSync/OsuRTDataProvider            | Provided a lot of working memory signatures (compared to gosumemory) that I had fun playing with, I'm especially thankful for the Client Status memory signature.
 * Piotrekol / ProcessMemoryDataFinder  https://github.com/Piotrekol/ProcessMemoryDataFinder    | Teaching me about getting data in an array, and providing a lot of other memory signatures that helped me when trying to solve a problem that I stopped using, which was getting the beatmap's full file/folder path in unicode, then using my own `libosu` to extract the last object's time (but I later thought it was overkill).
 * Azukee / osu-rx                      https://github.com/Azukee/osu-rx                        | Providing the final memory signatures, that being the Last Object Time, which I'm really thankful (probably one of the hardest parts for me in the entire project when trying to find the data).
 * and everyone else I forgot to mention!
 * If it weren't for you're help, I would've died trying to get the right data using Cheat Engine by now.
 */

// Client Status
#define STATUS_FIND             vector<uint8_t>{ 0x75, 0x07, 0x8B, 0x45, 0x90, 0xC6, 0x40, 0x2A, 0x00, 0x83, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x0F }   // https://github.com/OsuSync/OsuRTDataProvider/blob/master/Memory/OsuStatusFinder.cs#L8
#define STATUS_MASK             "xxxxxxxxxxx????x"                                                                                                  // https://github.com/OsuSync/OsuRTDataProvider/blob/master/Memory/OsuStatusFinder.cs#L10
#define STATUS_OFFSET           vector<int32_t>{ 0x0B, 0x00 }                                                                                       // https://github.com/OsuSync/OsuRTDataProvider/blob/master/Memory/OsuStatusFinder.cs#L24

// Song Time
#define TIME_FIND               vector<uint8_t>{ 0x5E, 0x5F, 0x5D, 0xC3, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x89, 0x00, 0x04 }                           // https://github.com/l3lackShark/gosumemory/blob/f80a655b5e2088da03f91e64d552b9e13540ceea/memory/read.go#L20
#define TIME_MASK               "xxxxx????x?x"                                                                                                      // https://github.com/l3lackShark/gosumemory/blob/f80a655b5e2088da03f91e64d552b9e13540ceea/memory/read.go#L20
#define TIME_OFFSET             vector<int32_t>{ 0x05, 0x00 }                                                                                       // https://github.com/l3lackShark/gosumemory/blob/f80a655b5e2088da03f91e64d552b9e13540ceea/memory/read.go#L123

// Last Object Time
#define HITOBJS_FIND            vector<uint8_t>{ 0xFF, 0x00, 0x0C, 0x8B, 0xD8, 0x8B, 0x15 }                                                         // https://github.com/Azukee/osu-rx/blob/master/osu!rx/osu/Memory/Signatures.cs#L33
#define HITOBJS_MASK            "x?xxxxx"                                                                                                           // https://github.com/Azukee/osu-rx/blob/master/osu!rx/osu/Memory/Signatures.cs#L33
#define HITOBJS_OFFSET          vector<int32_t>{ 0x07, 0x00, 0x40, 0x48 }                                                                           // https://github.com/Azukee/osu-rx/blob/master/osu!rx/osu/Memory/Signatures.cs#L34 & https://github.com/Azukee/osu-rx/blob/a5f578163e5737753419d605a5886cfc295ee3cb/osu!rx/osu/Memory/Objects/OsuPlayer.cs#L26 & https://github.com/Azukee/osu-rx/blob/a5f578163e5737753419d605a5886cfc295ee3cb/osu!rx/osu/Memory/Objects/OsuHitObjectManager.cs#L32
#define HITOBJS_LIST_OFFSET     vector<int32_t>{ 0x04 }                                                                                             // https://github.com/Azukee/osu-rx/blob/a5f578163e5737753419d605a5886cfc295ee3cb/osu!rx/osu/Memory/Objects/OsuHitObjectManager.cs#L33
#define HITOBJS_COUNT_OFFSET    vector<int32_t>{ 0x0C }                                                                                             // https://github.com/Azukee/osu-rx/blob/a5f578163e5737753419d605a5886cfc295ee3cb/osu!rx/osu/Memory/Objects/OsuHitObjectManager.cs#L34
#define HITOBJS_START_OFFSET    vector<int32_t>{ 0x10 }                                                                                             // https://github.com/Azukee/osu-rx/blob/a5f578163e5737753419d605a5886cfc295ee3cb/osu!rx/osu/Memory/Objects/OsuHitObjectManager.cs#L47

DWORD open(wstring name)                                                                                                                            // https://stackoverflow.com/a/35623208
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

uintptr_t find(vector<uint8_t> bin, string mask, HANDLE h_process)                                                                                  // https://github.com/UnnamedOrange/osu-memory/blob/main/native/include/osu_memory/readable_process.hpp#L92
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
            return NULL;
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
    return NULL;
}

bool read(uint32_t *value, int32_t offset, HANDLE h_process)
{
    return ReadProcessMemory(h_process, (LPCVOID)(*value + offset), value, sizeof(*value), NULL);
}

uintptr_t get_addr_offset(uintptr_t base_addr, vector<int32_t> offsets, HANDLE h_process)
{
    uint32_t value = base_addr;
    bool failed = false;
    for (int32_t offset : offsets)
    {
        if (!read(&value, offset, h_process))
        {
            failed = true;
            break;
        }
    }
    return failed ? 0 : value;
}

void press_key(char ch)                                                                                                                             // https://www.youtube.com/watch?v=L271J_lKyRA
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    input.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(ch)), 0);
    SendInput(1, &input, sizeof(input));
    Sleep(10);
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));
}

int main(int argc, char** argv)
{
    // Assign inputted PAUSE key to variable.
    if (argc != 2 && strlen(*(argv + 1)) != 1)
    {
        cout << "You need to provide a PAUSE letter before you can continue.\n";
        return 1;
    }
    const char key_pause = **(argv + 1);

    // Find window/process and get it's process id.
    DWORD p_id = open(L"osu!.exe");
    if (p_id == NULL)
    {
        cout << "Failed to find the process ID of osu!.exe.\n";
        return 1;
    }

    // Open process to read data from
    HANDLE h_process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, false, p_id);
    if (h_process == NULL)
    {
        cout << "Failed to open process of osu!.exe.\n";
        return 1;
    }

    // Refresh memory signatures
    while (true)
    {
        uintptr_t status_addr = find(STATUS_FIND, STATUS_MASK, h_process);
        if (status_addr == NULL) break;

        uintptr_t time_addr = find(TIME_FIND, TIME_MASK, h_process);
        if (time_addr == NULL) continue;

        uintptr_t hitobjs_addr = find(HITOBJS_FIND, HITOBJS_MASK, h_process);
        if (hitobjs_addr == NULL) continue;

        // Evaluate data and check if should pause
        bool found = false;
        for (uint16_t i = 0; i < UINT16_MAX; i++)
        {
            uint32_t status_value = get_addr_offset(status_addr, STATUS_OFFSET, h_process);
            if (status_value != 2) continue;

            uint32_t time_value = get_addr_offset(time_addr, TIME_OFFSET, h_process);

            uintptr_t hitobjs_value = get_addr_offset(hitobjs_addr, HITOBJS_OFFSET, h_process);
            uintptr_t hitobjs_list_value = get_addr_offset(hitobjs_value, HITOBJS_LIST_OFFSET, h_process);
            uint32_t hitobjs_count_value = get_addr_offset(hitobjs_value, HITOBJS_COUNT_OFFSET, h_process);
            uintptr_t lastobj_addr = get_addr_offset(hitobjs_list_value, vector<int32_t>{ 8 + (((int32_t)hitobjs_count_value - 1) * 4) }, h_process);
            uint32_t lastobj_time_value = get_addr_offset(lastobj_addr, HITOBJS_START_OFFSET, h_process);
            if (lastobj_time_value == 0) continue;

            float difference = (float)abs((int32_t)(time_value - lastobj_time_value)) / ((time_value + lastobj_time_value) / 2) * 100;
            if (difference >= 0.01) continue;

            found = true;
            break;
        }

        if (found) break;
    }

    press_key(key_pause);

    // Make a beep sound to tell the user that the application has closed
    Beep(523, 500);

    CloseHandle(h_process);
    return 0;
}
