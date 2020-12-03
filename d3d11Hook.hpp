#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "mem.h"
inline ID3D11Device* pDevice2 = nullptr;
inline IDXGISwapChain* pSwapchain2 = nullptr;
inline ID3D11DeviceContext* pContext2 = nullptr;
inline ID3D11RenderTargetView* pRenderTargetView2 = nullptr;
inline ID3D11VertexShader* pVertexShader = nullptr;
inline ID3D11InputLayout* pVertexLayout = nullptr;
inline ID3D11PixelShader* pPixelShader = nullptr;
inline ID3D11Buffer* pVertexBuffer = nullptr;
inline ID3D11Buffer* pIndexBuffer = nullptr;
inline ID3D11Buffer* pConstantBuffer = nullptr;
float fHeight = 0;
float fWidth = 0;


#define safe_release(p) if (p) { p->Release(); p = nullptr; } 

#include "shadez.h"
#include "D3D_VMT_Indices.h"
#define VMT_PRESENT (UINT)IDXGISwapChainVMT::Present
#define PRESENT_STUB_SIZE 5

// d3d11 related object ptrs
using namespace DirectX;


D3D11_SUBRESOURCE_DATA sr{ 0 };
D3D11_BUFFER_DESC bd{ 0 };

XMMATRIX mOrtho;

struct ConstantBuffer
{
	XMMATRIX mProjection;
};

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

bool WriteMem(void* pDst, char* pBytes, size_t size);
bool D3D11Hook();
bool CompileShader(const char* szShader, const char * szEntrypoint, const char * szTarget, ID3D10Blob ** pBlob);
bool InitD3DHook(IDXGISwapChain* pSwapchain2);
void CleanupD3D();
void Render();
HRESULT __stdcall hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);

using fnPresent = HRESULT(__stdcall*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
void* ogPresent;					// Pointer to the original Present function
fnPresent ogPresentTramp;			// Function pointer that calls the Present stub in our trampoline
void* pTrampoline = nullptr;		// Pointer to jmp instruction in our trampoline that leads to hkPresent
char ogBytes[PRESENT_STUB_SIZE];	// Buffer to store original bytes from Present

bool WriteMem(void* pDst, char* pBytes, size_t size)
{
	DWORD dwOld;
	if (!VirtualProtect(pDst, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return false;

	memcpy(pDst, pBytes, PRESENT_STUB_SIZE);

	VirtualProtect(pDst, size, dwOld, &dwOld);
	return true;
}

bool D3D11Hook()
{
	// Create a dummy device, get swapchain vmt, hook present.
	D3D_FEATURE_LEVEL featLevel;
	DXGI_SWAP_CHAIN_DESC sd{ 0 };
	sd.BufferCount = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Height = 800;
	sd.BufferDesc.Width = 600;
	sd.BufferDesc.RefreshRate = { 60, 1 };
	sd.OutputWindow = GetForegroundWindow();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwapchain2, &pDevice2, &featLevel, nullptr);
	if (FAILED(hr))
		return false;

	// Get swapchain vmt
	void** pVMT = *(void***)pSwapchain2;

	// Get Present's address out of vmt
	ogPresent = (fnPresent)(pVMT[VMT_PRESENT]);

	// got what we need, we can release device and swapchain now
	// we'll be stealing the game's.
	safe_release(pSwapchain2);
	safe_release(pDevice2);

	// Create a code cave to trampoline to our hook
	// We'll try to do this close to present to make sure we'll be able to use a 5 byte jmp (important for x64)
	void* pLoc = (void*)((uintptr_t)ogPresent - 0x2000);
	void* pTrampLoc = nullptr;
	while (!pTrampLoc)
	{
		pTrampLoc = VirtualAlloc(pLoc, 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		pLoc = (void*)((uintptr_t)pLoc + 0x200);
	}
	ogPresentTramp = (fnPresent)pTrampLoc;

	// write original bytes to trampoline
	// write jmp to hook
	memcpy(ogBytes, ogPresent, PRESENT_STUB_SIZE);
	memcpy(pTrampLoc, ogBytes, PRESENT_STUB_SIZE);

	pTrampLoc = (void*)((uintptr_t)pTrampLoc + PRESENT_STUB_SIZE);

	// write the jmp back into present
	*(char*)pTrampLoc = (char)0xE9;
	pTrampLoc = (void*)((uintptr_t)pTrampLoc + 1);
	uintptr_t ogPresRet = (uintptr_t)ogPresent + 5;
	*(int*)pTrampLoc = ogPresRet - (int)(uintptr_t)pTrampLoc - 4;

	// write the jmp to our hook
	pTrampoline = pTrampLoc = (void*)((uintptr_t)pTrampLoc + 4);
	#ifdef _WIN64
	// if x64, gazzillion byte absolute jmp
	char pJmp[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
	WriteMem(pTrampLoc, pJmp, ARRAYSIZE(pJmp));
	pTrampLoc = (void*)((uintptr_t)pTrampLoc + ARRAYSIZE(pJmp));
	*(uintptr_t*)pTrampLoc = (uintptr_t)hkPresent;
	#else
	// if x86, normal 0xE9 jmp
	*(char*)pTrampLoc = (char)0xE9;
	pTrampLoc = (void*)((uintptr_t)pTrampLoc + 1);
	*(int*)pTrampLoc = (uintptr_t)hkPresent - (uintptr_t)pTrampLoc - 4;
	#endif

	// hook present, place a normal mid-function at the beginning of the Present function
	Hook(ogPresent, pTrampoline, PRESENT_STUB_SIZE);
}

bool CompileShader(const char* szShader, const char * szEntrypoint, const char * szTarget, ID3D10Blob ** pBlob)
{
	ID3D10Blob* pErrorBlob = nullptr;

	auto hr = D3DCompile(szShader, strlen(szShader), 0, nullptr, nullptr, szEntrypoint, szTarget, D3DCOMPILE_ENABLE_STRICTNESS, 0, pBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			char szError[256]{ 0 };
			memcpy(szError, pErrorBlob->GetBufferPointer(), pErrorBlob->GetBufferSize());
			MessageBoxA(nullptr, szError, "Error", MB_OK);
		}
		return false;
	}
	return true;
}
float left;
float top;

void DrawLineB(int x1, int y1, int x2, int y2, int tickness)
{
	//int NewY2 = (600 - y2) - 100;

	Vertex pVerts[6] = {



		{ XMFLOAT3(left + x1,				top - y1,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x1,	top - y1 + tickness,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x2,				top - y2,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },

		{ XMFLOAT3(left + x2,				top - y2,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x1,	top - y1 + tickness,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x2,	top - y2 + tickness,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },

	};
	D3D11_BUFFER_DESC bd{ 0 };
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 6 * sizeof(Vertex);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sr{ 0 };

	ZeroMemory(&sr, sizeof(sr));
	sr.pSysMem = &pVerts;
	pDevice2->CreateBuffer(&bd, &sr, &pVertexBuffer);

	ConstantBuffer cb;
	cb.mProjection = XMMatrixTranspose(mOrtho);
	pContext2->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
	pContext2->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	// Make sure the input assembler knows how to process our verts/indices
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext2->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pContext2->IASetInputLayout(pVertexLayout);
	pContext2->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pContext2->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the shaders we need to render our triangle
	pContext2->VSSetShader(pVertexShader, nullptr, 0);
	pContext2->PSSetShader(pPixelShader, nullptr, 0);

	// Draw our triangle
	pContext2->Draw(6, 0);
}

void DrawLineB2(int x1, int y1, int x2, int y2, int tickness)
{

	Vertex pVerts[6] = {



		{ XMFLOAT3(left + x1,				top - y1,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x1 + tickness,	top - y1,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x2,				top - y2,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },

		{ XMFLOAT3(left + x2,				top - y2,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x1 + tickness,	top - y1,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x2 + tickness,	top - y2,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },

	};
	D3D11_BUFFER_DESC bd{ 0 };
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 6 * sizeof(Vertex);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sr{ 0 };

	ZeroMemory(&sr, sizeof(sr));
	sr.pSysMem = &pVerts;
	pDevice2->CreateBuffer(&bd, &sr, &pVertexBuffer);

	ConstantBuffer cb;
	cb.mProjection = XMMatrixTranspose(mOrtho);
	pContext2->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
	pContext2->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	// Make sure the input assembler knows how to process our verts/indices
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext2->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pContext2->IASetInputLayout(pVertexLayout);
	pContext2->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pContext2->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the shaders we need to render our triangle
	pContext2->VSSetShader(pVertexShader, nullptr, 0);
	pContext2->PSSetShader(pPixelShader, nullptr, 0);

	// Draw our triangle
	pContext2->Draw(6, 0);
}

void DrawLine(int x1, int y1, int x2, int y2, int tickness)
{
	int NewY2 = (fHeight - y2);
	
	Vertex pVerts[3] = {



		{ XMFLOAT3(left + x1,				top - y1,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x1 + tickness,	top - y1,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(left + x2,				top - NewY2,	1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },

	};
	D3D11_BUFFER_DESC bd{ 0 };
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 3 * sizeof(Vertex);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sr{ 0 };

	ZeroMemory(&sr, sizeof(sr));
	sr.pSysMem = &pVerts;
	pDevice2->CreateBuffer(&bd, &sr, &pVertexBuffer);

	ConstantBuffer cb;
	cb.mProjection = XMMatrixTranspose(mOrtho);
	pContext2->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
	pContext2->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	// Make sure the input assembler knows how to process our verts/indices
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext2->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pContext2->IASetInputLayout(pVertexLayout);
	pContext2->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pContext2->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the shaders we need to render our triangle
	pContext2->VSSetShader(pVertexShader, nullptr, 0);
	pContext2->PSSetShader(pPixelShader, nullptr, 0);

	// Draw our triangle
	pContext2->Draw(3, 0);
}

void DrawBox(int xCenter, int yCenter)
{
	int tickness = 8;
	int NewY2 = (fHeight - yCenter);
	DrawLineB(xCenter - 30, NewY2 - 63, xCenter + 30, NewY2 - 63, tickness);
	DrawLineB2(xCenter - 30, NewY2 - 63, xCenter - 30, NewY2, tickness);

	DrawLineB(xCenter - 30, NewY2, xCenter + 30, NewY2, tickness);
	DrawLineB2(xCenter + 30, NewY2 - 70, xCenter + 30, NewY2, tickness);

	///`````

	
}



bool InitD3DHook(IDXGISwapChain * pSwapchain2)
{
	HRESULT hr = pSwapchain2->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice2);
	if (FAILED(hr))
		return false;

	pDevice2->GetImmediateContext(&pContext2);
	pContext2->OMGetRenderTargets(1, &pRenderTargetView2, nullptr);

	// If for some reason we fail to get a render target, create one.
	// This will probably never happen with a real game but maybe certain test environments... :P
	if (!pRenderTargetView2)
	{
		// Get a pointer to the back buffer for the render target view
		ID3D11Texture2D* pBackbuffer = nullptr;
		hr = pSwapchain2->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackbuffer));
		if (FAILED(hr))
			return false;

		// Create render target view
		hr = pDevice2->CreateRenderTargetView(pBackbuffer, nullptr, &pRenderTargetView2);
		pBackbuffer->Release();
		if (FAILED(hr))
			return false;

		// Make sure our render target is set.
		pContext2->OMSetRenderTargets(1, &pRenderTargetView2, nullptr);
	}

	// initialize shaders
	ID3D10Blob* pBlob = nullptr;

	// create vertex shader
	if (!CompileShader(szShadez, "VS", "vs_5_0", &pBlob))
		return false;

	hr = pDevice2->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	if (FAILED(hr))
		return false;

	// Define/create the input layout for the vertex shader
	D3D11_INPUT_ELEMENT_DESC layout[2] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = pDevice2->CreateInputLayout(layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexLayout);
	if (FAILED(hr))
		return false;

	safe_release(pBlob);

	// create pixel shader
	if (!CompileShader(szShadez, "PS", "ps_5_0", &pBlob))
		return false;

	hr = pDevice2->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	if (FAILED(hr))
		return false;

	D3D11_VIEWPORT vp{ 0 };
	UINT numViewports = 1;
	// 
	
	pContext2->RSGetViewports(&numViewports, &vp);
	// Usually this won't fail but if it does...
	if (!numViewports)
	{
		// This should be retrieved dynamically
		fWidth = 800.0f;
		fHeight = 600.0f;

		WidthX = fWidth;
		HeightY = fWidth;
		// Setup viewport
		D3D11_VIEWPORT vp{ 0 };
		vp.Width = (float)fWidth;
		vp.Height = (float)fHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		// Set viewport to context
		pContext2->RSSetViewports(1, &vp);
	}
	fWidth = (float)vp.Width;
	fHeight = (float)vp.Height;

	// Create the constant buffer

	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.Usage = D3D11_USAGE_DEFAULT;

	// Setup orthographic projection
	mOrtho = XMMatrixOrthographicLH(fWidth, fHeight, 0.0f, 1.0f);
	ConstantBuffer cb;
	cb.mProjection = mOrtho;


	sr.pSysMem = &cb;
	hr = pDevice2->CreateBuffer(&bd, &sr, &pConstantBuffer);
	if (FAILED(hr))
		return false;

	// Create a triangle to render
	// Create a vertex buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 6 * sizeof(Vertex);
	bd.StructureByteStride = sizeof(Vertex);

	// left and top edge of window
	left = fWidth / -2;
	top = fHeight / 2;

	// Create an index buffer
	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&sr, sizeof(sr));

	UINT pIndices[6] = { 0, 1, 2,3,4,5 };
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * 6;
	bd.StructureByteStride = sizeof(UINT);

	sr.pSysMem = &pIndices;
	hr = pDevice2->CreateBuffer(&bd, &sr, &pIndexBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

void CleanupD3D()
{
	safe_release(pVertexBuffer);
	safe_release(pIndexBuffer);
	safe_release(pConstantBuffer);
	safe_release(pPixelShader);
	safe_release(pVertexShader);
	safe_release(pVertexLayout);
}

void Render()
{
	if (bESP || bBoxESP)
	{
		for (int i = 0; i < 10; i++)
		{
			
			if (enemyPosInScreen[i].x != 0 && enemyPosInScreen[i].y != 0)
			{
				if (bESP)
				{
					DrawLine(fWidth / 2, 5 , enemyPosInScreen[i].x, enemyPosInScreen[i].y, 7);
				}
				if (bBoxESP)
				{
					DrawBox(enemyPosInScreen[i].x, enemyPosInScreen[i].y);
				}

				
			}
			
		}
	}


	// Update view
	
}

