#pragma once
#include <iostream>
#include <windows.h>
#include <TLHelp32.h>
#include <vector>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include "Memory.h"

DWORD GetProcId(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
