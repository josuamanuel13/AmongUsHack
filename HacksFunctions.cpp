#include "stdafx.h"
#include "HacksFunctions.h"

void WaitingMatchParametter()
{
	gamedataInstance = NULL;
	isReady = false;
	visible = false;
	EntityListIndex = 0;
	settedParameterStatus = false;
	OnWaitMatchParameterAplied = true;
}

void inMatchParameter()
{
	Sleep(200);
	visible = true;
	settedParameterStatus = true;
	OnWaitMatchParameterAplied = false;
}

void OnLobbyParameter()
{
	Sleep(200);
	visible = true;
	settedParameterStatus = true;
	OnWaitMatchParameterAplied = false;
}

void Cast()
{
	playercontrolAddrPTR = (uintptr_t*)(GameAssemblyModBaseAddr + playerControlClassAddr);
	AllControls = (PlayerControl_c*)*playercontrolAddrPTR;
}

void Update()
{

	if (GetAsyncKeyState(VK_DELETE) & 1)
	{
		unload = true;
	}

	switch (status)
	{
	case Match:
		if (settedParameterStatus != true && visible != true)
		{
			inMatchParameter();
		}

		break;

	case Lobby:
		if (settedParameterStatus != true && visible != true)
		{
			OnLobbyParameter();
		}
		break;

	case WaitingMatch:
	{
		if (OnWaitMatchParameterAplied != true)
		{
			WaitingMatchParametter();
		}

		
		break;
	}
	}
}

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);


}



void _Setup()
{
	//casting 
	Cast();
}

/*void DrawLine(ID3D11Device* p_Device, int x1, int y1, int x2, int y2, int tickness, D3DCOLOR COLOR)
{
	
	
		d3d10CreateLin
	D3DXCreateLine(p_Device, &LineL);
	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	LineL->SetWidth(tickness);
	LineL->Draw(Line, 2, COLOR);
	LineL->Release();
	LineL
}

void DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font)
{
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	if (font != (void*)0xCCCCCCCC)
		font->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

*/
