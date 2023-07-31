#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <tlhelp32.h>

using namespace std;

DWORD open(wstring);
uintptr_t find(vector<uint8_t>, string, HANDLE);
bool read(uint32_t *, int32_t, HANDLE);
uintptr_t get_addr_offset(uintptr_t, vector<int32_t>, HANDLE);
