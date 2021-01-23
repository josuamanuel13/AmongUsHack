#pragma once
#include <iostream>
#include "Global.h"
#include "mem.h"
#include "Enums.h"
#include "Vectors.h"


void _Setup();

void WaitingMatchParametter();

void inMatchParameter();

void OnLobbyParameter();

void Cast();

void Update();

void InitImGui();

//void DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font);

//void DrawLine(ID3D11Device Device_Interface, int x1, int y1, int x2, int y2, int tickness, D3DCOLOR COLOR);

void Render();

int getClousesEnemies();