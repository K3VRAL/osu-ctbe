#include "read.hpp"

bool read(uint32_t *value, int32_t offset, HANDLE h_process)
{
    return ReadProcessMemory(h_process, (LPCVOID)(*value + offset), value, sizeof(*value), NULL);
}
