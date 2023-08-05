#include "main.hpp"

// Client Status
#define STATUS_FIND             vector<uint8_t>{ 0x75, 0x07, 0x8B, 0x45, 0x90, 0xC6, 0x40, 0x2A, 0x00, 0x83, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x0F }   // https://github.com/OsuSync/OsuRTDataProvider/blob/master/Memory/OsuStatusFinder.cs#L8
#define STATUS_MASK             "xxxxxxxxxxx????x"                                                                                                  // https://github.com/OsuSync/OsuRTDataProvider/blob/master/Memory/OsuStatusFinder.cs#L10
#define STATUS_OFFSET           vector<int32_t>{ 0x0B, 0x00 }                                                                                       // https://github.com/OsuSync/OsuRTDataProvider/blob/master/Memory/OsuStatusFinder.cs#L24

// Accuracy
#define RULESETS_FIND           vector<uint8_t>{ 0x7D, 0x15, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x85, 0xC0 }                                             // https://github.com/l3lackShark/gosumemory/blob/master/memory/read.go#L23
#define RULESETS_MASK           "xxx????xx"                                                                                                         // https://github.com/l3lackShark/gosumemory/blob/master/memory/read.go#L23
#define RULESET_OFFSET          vector<int32_t>{ -0x0B, 0x04, 0x68, 0x38 }                                                                          // https://github.com/l3lackShark/gosumemory/blob/master/memory/read.go#L62
#define SCORE_OFFSET            vector<int32_t>{ 0x78 }                                                                                             // https://github.com/l3lackShark/gosumemory/blob/master/memory/read.go#L150
#define FRUITCOMBO_OFFSET       vector<int32_t>{ 0x94 }                                                                                             // https://github.com/l3lackShark/gosumemory/blob/master/memory/read.go#L144
#define DROPLET_OFFSET          vector<int32_t>{ 0x88 }                                                                                             // https://github.com/l3lackShark/gosumemory/blob/master/memory/read.go#L138
#define TINYDROPLET_OFFSET      vector<int32_t>{ 0x8C }                                                                                             // https://github.com/l3lackShark/gosumemory/blob/master/memory/read.go#L140

int main(void)
{
    while (true)
    {
        // Find window/process and get it's process id.
        DWORD p_id = open(L"osu!.exe");
        if (p_id == (DWORD)NULL)
        {
            cout << "Failed to find the process ID of osu!.exe.\n";
            continue;
        }

        // Open process to read data from
        HANDLE h_process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, false, p_id);
        if (h_process == (HANDLE)NULL)
        {
            cout << "Failed to open process of osu!.exe.\n";
            continue;
        }

        int16_t fc_old = 0;
        vector<uint16_t> fc_l{0};
        while (WaitForSingleObject(h_process, 0) == WAIT_TIMEOUT)
        {
            // Refresh memory signatures
            uintptr_t status_addr = find(STATUS_FIND, STATUS_MASK, h_process);
            if (status_addr == (uintptr_t)NULL) continue;

            uintptr_t rulesets_addr = find(RULESETS_FIND, RULESETS_MASK, h_process);
            if (rulesets_addr == (uintptr_t)NULL) continue;

            // Evaluate data and check if should pause
            for (uint16_t i = 0; i < UINT16_MAX; i++)
            {
                uint32_t status_value = get_addr_offset(status_addr, STATUS_OFFSET, h_process);
                if (status_value != 2) continue;

                uintptr_t ruleset_addr = get_addr_offset(rulesets_addr, RULESET_OFFSET, h_process);
                int32_t score = get_addr_offset(ruleset_addr, SCORE_OFFSET, h_process);
                int16_t fc = get_addr_offset(ruleset_addr, FRUITCOMBO_OFFSET, h_process);
                if (fc == 0 && fc_l[fc_l.size() - 1] != fc_old)
                {
                    fc_l.push_back(fc_old); // TODO
                }
                fc_old = fc;
                int16_t d = get_addr_offset(ruleset_addr, DROPLET_OFFSET, h_process);
                int16_t td = get_addr_offset(ruleset_addr, TINYDROPLET_OFFSET, h_process);
                int32_t b = (score - ((fc + accumulate(fc_l.begin(), fc_l.end(), 0, [](int16_t a, int16_t b){ return a * 300 + b; })) + d * 100 + td * 50)) / 1100;

                cout << b << "\t\t\r";

                Sleep(10);
            }
            Sleep(10);
        }

        CloseHandle(h_process);
        Sleep(10);
    }

    return 0;
}
