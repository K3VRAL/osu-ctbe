#include "get_addr_offset.hpp"

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
