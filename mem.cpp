#include "stdafx.h"
#include "mem.h"

//https://guidedhacking.com/threads/how-to-hack-any-game-first-internal-hack-dll-tutorial.12142/

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

void mem::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}

void Hook(void* src, void* dst, unsigned int len)
{
	if (len < 5) return;

	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(src, 0x90, len);

	uintptr_t relativeAddress = (uintptr_t)dst - (uintptr_t)src - 5;

	*(BYTE*)src = 0xE9;

	*(uintptr_t*)((BYTE*)src + 1) = relativeAddress;
	VirtualProtect(src, len, curProtection, &curProtection);
}

BYTE* TrampHook(void* src, void* dst, unsigned int len)
{
	if (len < 5) return 0;

	//Create Gateway.
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//Write stolen bytes.
	memcpy_s(gateway, len, src, len);

	//Get the gateway to destination address.
	uintptr_t gatewayRelativeAddr = (BYTE*)src - gateway - 5;
	*(gateway + len) = 0xE9;

	//Write address of gateway to jmp.
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

	//Perform HOOK.
	Hook(src, dst, len);
	return gateway;
}

bool Detour32(void * src, void * dst, int len)
{
	if (len < 5) return false;

	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(src, 0x90, len);

	uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;

	*(BYTE*)src = 0xE9;
	*(uintptr_t*)((uintptr_t)src + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect(src, len, curProtection, &temp);

	return true;
}