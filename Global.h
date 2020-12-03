#pragma once
#include <Windows.h>
#include "Dump.h"
#include "Vectors.h"
#include "includes.h"

//offsets
inline uintptr_t getPlayerCount = 0xDA2780;
inline uintptr_t getPlayerById = 0xD9F300;
inline uintptr_t getTruePosition = 0x43C030;
inline uintptr_t get_IsGameStarted = 0x7F6500;
inline uintptr_t playerControlClassAddr = 0xDA5A84;
inline uintptr_t get_IsGameOver = 0x7F6BA0;
inline uintptr_t UpdateMeetingHud = 0x1B4290;
inline uintptr_t getAmIBanned = 0xA3C1B0;
inline uintptr_t get_MainCamera = 0x6CAAE0;
inline uintptr_t w2s = 0x6CA830;
inline uintptr_t Drawline = 0x582950;
inline uintptr_t pDeviceoffs = 0x203b4;



//maxCMD x 72 y 24

//declaration
inline int status;
inline bool visibleDisabledBy = true;
inline bool radarOn = false;
inline bool unload = false;
inline unsigned int EntityListIndex;
inline uintptr_t* playercontrolAddrPTR;

//PlayersVariables
inline Vector3 onScreenPos;
inline PlayerControl_c* AllControls;
inline int xCMD = 78, yCMD = 23;
inline int myPlayerPosX = xCMD / 2, myPlayerPosY = yCMD / 2;
inline GameData_PlayerInfo_o* PlayersInfo[10];
inline Vector3 enemyPosInScreen[10];
inline Vector3 enemyPosvec3[10];
inline Vector2 enemyPos[10];
inline IntVec2 enemyPosCMDRadar[10];
inline bool IsImpostor[10];
inline int colors[10];
inline bool died[10];
inline IntVec2 distanceToMe[10];
inline int maxPlayers;
inline uintptr_t* playerinfoPtr;
inline Players playerInfoToEsp;
inline Vector2 MyCoords;

inline  Present oPresent;
inline  HWND window;
inline WNDPROC oWndProc;
inline ID3D11Device* pDevice;
inline IDXGISwapChain* pSwapChainL;
inline ID3D11DeviceContext* pContext;
inline ID3D11RenderTargetView* mainRenderTargetView;
inline ID3D11Texture2D* pBackBuffer;
inline HANDLE process_handle;



inline HWND _hWnd;
//inline Graphics gfx;
//inline RenderWindow render_window;

//aux
inline int currentplayercount;
inline int currentStatus;
inline bool isAllCleaned;


//declaration + definition
inline const float RadarCMDCalcMultiplierFor78 = 10.68493;
inline const float RadarCMDCalcMultiplierFor1 = 0.04391;
inline int Width = GetSystemMetrics(SM_CXSCREEN);
inline int Height = GetSystemMetrics(SM_CYSCREEN);
inline uintptr_t GameAssemblyModBaseAddr = (uintptr_t)GetModuleHandle("GameAssembly.dll");
inline uintptr_t unityGameAssemblyModBaseAddrAddr = (uintptr_t)GetModuleHandle("UnityPlayer.dll");

inline void* MainCameraInstance = nullptr;
inline void* obJectToWorldPos = NULL;
inline void* gamedataInstance = nullptr;
inline void* MeetingHudInstance = nullptr;
inline void* StatsManagerInstance = nullptr;

//inline DWORD OverlayPID = GetProcId("DirectX Overlay.exe");
//inline uintptr_t OverlayModBase = GetModuleBaseAddress(OverlayPID, "DirectX Overlay.exe");
//inline void* pDevice = (void*)(OverlayModBase + pDeviceoffs);
inline bool init = false;
inline bool show = true;
inline bool switchTabs = 7;
inline bool bESP = false;
inline bool bBoxESP = false;
inline bool settedParameterStatus = NULL;
inline bool OnWaitMatchParameterAplied = false;
inline bool visible = false;
inline bool onetime = false;
inline bool isReady = false;
inline bool isGameOver = false;
inline bool eject = false;
inline bool IsExecutingSomeHookFunc = false;
inline bool CleanLAndBInLobby = false;
inline uintptr_t* enemyposptr;
inline LPVOID pHandle;
inline int WidthX = 0, HeightY = 0;
