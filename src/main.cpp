#include "main.cpp"

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
