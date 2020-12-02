#pragma once
#include "GetModuleBase.h"
namespace memtools
{
	//external
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

	//internal
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
	uintptr_t FindDMAAddyInternal(uintptr_t ptr, std::vector<unsigned int> offsets);
}