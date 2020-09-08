#include "pch.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <Windows.h>
#include "detours.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

typedef HRESULT(__stdcall* endScene)(LPDIRECT3DDEVICE9 d3ddev); // calling convention
endScene pEndScene;

LPD3DXFONT font; // font for drawing text

void PrintText(LPDIRECT3DDEVICE9 d3ddev, LPCWSTR toPrint, D3DXCOLOR color, int y,int x)
{
    if (!font)
        D3DXCreateFont(d3ddev, 16, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Tahoma", &font);
    RECT textRectangle;
    SetRect(&textRectangle, y, x, 1000, 1000);
    font->DrawTextW(NULL, toPrint, -1, &textRectangle, DT_NOCLIP | DT_LEFT, color); //draw text;
}

HRESULT __stdcall hookedEndScene(LPDIRECT3DDEVICE9 d3ddev)
{
    PrintText(d3ddev,L"Hello JackSkellington :p",D3DXCOLOR(D3DCOLOR_ARGB(255,252, 236, 3)),500,500); 
    return pEndScene(d3ddev); // call original endScene
}

void hookEndScene()
{
    LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION); // Create a Direct3D Interface
    LPDIRECT3DDEVICE9 d3ddev;
    if (!d3d)
        return;
    D3DPRESENT_PARAMETERS pD3D; // Create a struct to hold various device information
    pD3D.Windowed = true;
    pD3D.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pD3D.hDeviceWindow = GetForegroundWindow();
    HRESULT result = d3d->CreateDevice(0, D3DDEVTYPE_HAL, pD3D.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pD3D, &d3ddev);
    if (FAILED(result) || !d3ddev) {
        d3d->Release();
        return;
    }
    
    void** vTable = *reinterpret_cast<void***>(d3ddev); // creating our virtual Table

    pEndScene = (endScene)DetourFunction((PBYTE)vTable[42], (PBYTE)hookedEndScene); // delete original code for putting our directx9 stuff

    d3ddev->Release();
    d3d->Release();
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    hookEndScene();
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, LPTHREAD_START_ROUTINE(MainThread), hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

