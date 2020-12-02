#include "Memory.h"

void memtools::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

void memtools::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}



// MultiLeveled Pointer Resolver, more information here --> https://guidedhacking.com/threads/finddmaaddy-c-multilevel-pointer-function.6292/
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}












//interal versons below!

void memtools::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect( dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

	memcpy(dst, src, size);

	VirtualProtect( dst, size, oldprotect, &oldprotect);
}

void memtools::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

	memset(dst, 0x90, size);

	VirtualProtect(dst, size, oldprotect, &oldprotect);
}



// MultiLeveled Pointer Resolver, more information here --> https://guidedhacking.com/threads/finddmaaddy-c-multilevel-pointer-function.6292/
uintptr_t memtools::FindDMAAddyInternal(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}