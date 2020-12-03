#include "stdafx.h"
#include "Dump.h"
#include "Vectors.h"
#include "Global.h"
#include "mem.h"
#include "HacksFunctions.h"
#include <d3d9.h>
#include "d3d11Hook.hpp"

void LoadHook();

void Unload();

void Setup();

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef Vector3(*tw2s)(void* instance, Vector3 pos);

typedef void*(*tGet_MainCamera)();

typedef bool(*tGetAmiBanned)(void* instance);

typedef GameData_PlayerInfo_o*(*tGetPlayerById)(void* instance, BYTE id);

typedef Vector2(*tGetTruePosition)(void* playercontrol);

typedef void(*tget_IsGameStarted)(void* instace);

typedef void(*tUpdateMeetingHud)(void* instace);

typedef bool(*tget_IsGameOver)(void* intance);

typedef unsigned int(*tgetPlayerCount)(void* instance);

tgetPlayerCount ogetPlayerCount = (tgetPlayerCount)(GameAssemblyModBaseAddr + getPlayerCount);

tw2s ow2s = (tw2s)(GameAssemblyModBaseAddr + w2s);

//tDrawLine oDrawLine = (tDrawLine)(OverlayModBase + Drawline);

tGet_MainCamera oGet_MainCamera = (tGet_MainCamera)(GameAssemblyModBaseAddr + get_MainCamera);

tGetAmiBanned oGetAmiBanned = (tGetAmiBanned)(GameAssemblyModBaseAddr + getAmIBanned);

tget_IsGameOver oget_IsGameOver = (tget_IsGameOver)(GameAssemblyModBaseAddr + get_IsGameOver);

 tGetPlayerById oGetPlayerById = (tGetPlayerById)(GameAssemblyModBaseAddr + getPlayerById);


tGetTruePosition oGetTruePosition = (tGetTruePosition)(GameAssemblyModBaseAddr + getTruePosition);

 tget_IsGameStarted oget_IsGameStarted = (tget_IsGameStarted)(GameAssemblyModBaseAddr + get_IsGameStarted);

 tUpdateMeetingHud oUpdateMeetingHud = (tUpdateMeetingHud)(GameAssemblyModBaseAddr + UpdateMeetingHud);

 void* hGet_MainCamera()
 {

	 MainCameraInstance = (void*)oGet_MainCamera();

	 

	 return MainCameraInstance;
 }

 Vector2 hkGetTruePosition(void* PlayerControl)
{
	 CleanLAndBInLobby = false;
	 
	 Vector2 me = oGetTruePosition(PlayerControl);
		
	 MainCameraInstance = oGet_MainCamera();
	 IsExecutingSomeHookFunc = true;
	 if (isReady != false &&
		 status != 0 && isGameOver != true)
	 {
		 if (AllControls != NULL)
		 {
			 EntityListIndex = ogetPlayerCount(gamedataInstance);//*(unsigned int *)(*(unsigned int *)(((uintptr_t)gamedataInstance) + 0x24) + 0xc);

			 if (EntityListIndex != 1)
			 {
				 for (unsigned int i = 0; i < 10; i++)
				 {
					 PlayersInfo[i] = (GameData_PlayerInfo_o*)oGetPlayerById(gamedataInstance, i);
					 
					 if (PlayersInfo[i] != nullptr)
					 {
						 
						
						 if (PlayersInfo[i]->fields.IsDead == false || PlayersInfo[i]->fields.Disconnected == false)
						 {
							 
							 enemyPos[i] = oGetTruePosition((void*)PlayersInfo[i]->fields._object);
							 IsImpostor[i] = PlayersInfo[i]->fields.IsImpostor;
							 colors[i] = (int)PlayersInfo[i]->fields.ColorId;
							 died[i] = PlayersInfo[i]->fields.IsDead;

							 enemyPosvec3[i].x = enemyPos[i].x;
							 enemyPosvec3[i].y = enemyPos[i].y;
							 enemyPosvec3[i].z = 0;
							 if (died[i] == true)
							 {
								 enemyPosvec3[i].x = 0;
								 enemyPosvec3[i].y = 0;
								 enemyPosvec3[i].z = 0;
							 }

							 if (PlayersInfo[i]->fields.Disconnected == true)
							 {
								 enemyPos[i].x = 0;
								 enemyPos[i].y = 0;
								 enemyPosInScreen[i].x = 0;
								 enemyPosInScreen[i].y = 0;
							 }

							 if (status != 0)
							 {
								 enemyPosInScreen[i] = (Vector3)ow2s(MainCameraInstance, enemyPosvec3[i]);
							 }
							 else
							 {
								 enemyPos[i].x = 0;
								 enemyPos[i].y = 0;
								 enemyPosInScreen[i].x = 0;
								 enemyPosInScreen[i].y = 0;
							 }
							 
							 //enemyPosInScreen[i] = (Vector3)

							 if (enemyPos[i].x == me.x)
							 {
								 MyCoords.x = enemyPosInScreen[i].x;
								 MyCoords.y = enemyPosInScreen[i].y;
								 enemyPosInScreen[i].x = 0;
								 enemyPosInScreen[i].y = 0;
							 }

						 }
						 else
						 {

							 enemyPos[i].x = 0;
							 enemyPos[i].y = 0;
							 enemyPosInScreen[i].x = 0;
							 enemyPosInScreen[i].y = 0;
						 }
					 }
					 else
					 {
						 enemyPos[i].x = 0;
						 enemyPos[i].y = 0;
						 enemyPosInScreen[i].x = 0;
						 enemyPosInScreen[i].y = 0;
					 }
				 }
			 }
			 else
			 {
				 isReady = false;
			 }


		 }
		 isReady = true;
	 }
	 else
	 {
		 gamedataInstance = NULL;
		 gamedataInstance = (void*)(mem::FindDMAAddy((GameAssemblyModBaseAddr + 0x14B2E9C), { 0x5c, 0x0, 0x0 }));

		 if (gamedataInstance != NULL)
		 {
			 isReady = true;
		 }


	 }

	 IsExecutingSomeHookFunc = false;

	 return me;

}

 void hget_IsGameStarted(void* instance) {

	
	 
	

	 if (unload == true)
	 {
		 Unload();
		 CreateThread(0, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, pHandle, 0, 0);
	 }
	IsExecutingSomeHookFunc = true;
	
	if (instance)
	{
		status = *(int*)((uintptr_t)instance + 0x64);
		isGameOver = oget_IsGameOver(instance);

		if (status == 0 && CleanLAndBInLobby != true)
		{
			int i = 0;
			Sleep(0);
			while (i < 10)
			{
				enemyPos[i].x = 0;
				enemyPos[i].y = 0;
				enemyPosInScreen[i].x = 0;
				enemyPosInScreen[i].y = 0;
				i++;
			}
			CleanLAndBInLobby = true;
		}
	}
	IsExecutingSomeHookFunc = false;
	return oget_IsGameStarted(instance);
	
}

 GameData_PlayerInfo_o* hGetPlayerById(void* instance, BYTE Playerid)
{
	gamedataInstance = instance;
	return oGetPlayerById(instance, Playerid);
	
}

 bool hGetAmiBanned(void* instance)
 {
	 StatsManagerInstance = instance;
	 return false;
 }

 void LoadHook()
 {
	 //oGet_MainCamera = (tGet_MainCamera)TrampHook((BYTE*)oGet_MainCamera, (BYTE*)hGet_MainCamera, 5);
	 oGetPlayerById = (tGetPlayerById)TrampHook((BYTE*)oGetPlayerById, (BYTE*)hGetPlayerById, 10);
	 oGetTruePosition = (tGetTruePosition)TrampHook((BYTE*)oGetTruePosition, (BYTE*)hkGetTruePosition, 6);
	 oget_IsGameStarted = (tget_IsGameStarted)TrampHook((BYTE*)oget_IsGameStarted, (BYTE*)hget_IsGameStarted, 5);
	 oGetAmiBanned = (tGetAmiBanned)TrampHook((BYTE*)oGetAmiBanned, (BYTE*)hGetAmiBanned, 5);
 }

 void Unload()
 {
	 //unhook getPlayer By ID
	 mem::Patch((BYTE*)(GameAssemblyModBaseAddr + getPlayerById), (BYTE*)"\x55\x8b\xec\x80\x3d\x89\xb1\xd7\x05\x00", 10);

	 //unhook meetenghood update
	 mem::Patch((BYTE*)(GameAssemblyModBaseAddr + UpdateMeetingHud), (BYTE*)"\x55\x8b\xec\x83\xec\x30", 6);

	 //unhook oGetTruePosition
	 mem::Patch((BYTE*)(GameAssemblyModBaseAddr + getTruePosition), (BYTE*)"\x55\x8b\xec\x83\xec\x14", 6);

	 //unhook isGameStarted
	 mem::Patch((BYTE*)(GameAssemblyModBaseAddr + get_IsGameStarted), (BYTE*)"\x55\x8b\xec\x6a\xff", 5);

	 //unhook isGameStarted
	 mem::Patch((BYTE*)(GameAssemblyModBaseAddr + getAmIBanned), (BYTE*)"\x55\x8b\xec\x6a\x00", 5);

 }

 //imguiHook

 LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	 if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		 return true;

	 _hWnd = hWnd;

	 return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

 }



 HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
 {
	 if (!init)
	 {
		 if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		 {
			 pSwapchain2 = pSwapChain;

			 
			
			 pDevice->GetImmediateContext(&pContext);
			 DXGI_SWAP_CHAIN_DESC sd;
			 pSwapChain->GetDesc(&sd);
			 window = sd.OutputWindow;
			 ID3D11Texture2D* pBackBuffer;
			 pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			 pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			 pBackBuffer->Release();
			 oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			 InitImGui();
			 InitD3DHook(pSwapChain);
			 init = true;

		 }

		 else
			 return oPresent(pSwapChain, SyncInterval, Flags);
	 }


	 

	 if (GetAsyncKeyState(VK_INSERT) & 1)
	 {
		 show = !show;
	 }

	 // menu start :0

	 if (show)
	 {
		 ImGui_ImplDX11_NewFrame();
		 ImGui_ImplWin32_NewFrame();
		 ImGui::NewFrame();
		 ImGui::SetNextWindowSize({ 325, 0 });
		 ImGui::Begin("Among Us Hack");
		 ImGui::SetWindowSize({ 325, 0 }, ImGuiCond_Always);
		 ImGui::SetNextWindowSize({ 325, 0 });
		 auto& style = ImGui::GetStyle();
		 style.Colors[ImGuiCol_TitleBg] = ImVec4{ 164.f / 255.f, 130.f / 255.f, 21.f / 255.f, 1.f };
		 style.Colors[ImGuiCol_TitleBgActive] = ImVec4{ 164.f / 255.f, 130.f / 255.f, 21.f / 255.f, 1.f };
		 style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 164.f / 255.f, 130.f / 255.f, 21.f / 255.f, 1.f };
		 style.Colors[ImGuiCol_Button] = ImVec4{ 164.f / 255.f, 130.f / 255.f, 21.f / 255.f, 1.f };
		 // tabs
		 if (ImGui::Button("Table", ImVec2(100.0f, 0.0f)))
			 switchTabs = 0;
		 ImGui::SameLine(0.0, 2.0f);

		 switch (switchTabs)
		 {
		 case 0: //Main TAB

			 ImGui::Checkbox("ESP TriangleLine", &bESP);
			 ImGui::SameLine(0.0, 6.0f);
			 ImGui::Checkbox("ESP Box", &bBoxESP);
			 ImGui::SameLine(0.0, 6.0f);


			 break;
		 }

		 ImGui::End();

		 ImGui::Render();
		 Render();

		 pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		 ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	 }
	 if (GetAsyncKeyState(VK_END)) {



		 kiero::shutdown();
		 WriteMem(ogPresent, ogBytes, PRESENT_STUB_SIZE);
		 CleanupD3D();
		 VirtualFree((void*)ogPresentTramp, 0x1000, MEM_RELEASE);
		 unload = true;
		 
	 }
	 return oPresent(pSwapChain, SyncInterval, Flags);
 }




 void Setup()
 {
	 LoadHook();
	 _Setup();
 }

