#include "includes.h"
#include <codecvt>
#include "Hook.hpp"
#include "HacksFunctions.h"
#include "mem.h"
#include "Global.h"
#include "Enums.h"
#include <d3d11.h>
//#include "d3d11Hook.hpp"

#define SAFE_RELEASE(p) if (p) { p->Release(); p = nullptr; }

void HookD3D()
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	pHandle = lpReserved;

	CreateThread(nullptr, 0,(LPTHREAD_START_ROUTINE)HookD3D, NULL, 0, nullptr);

	LoadHook();

	Setup();

	while (true)
	{
		Update();
	}
	
	
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{



    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
       // DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        kiero::shutdown();
        break;
    }
    return TRUE;
}