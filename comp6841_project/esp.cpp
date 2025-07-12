#include <Windows.h>

#include "esp.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#include <d3d11.h>
#include <iostream>


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK esp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    if (msg == WM_DESTROY) {
        ::PostQuitMessage(0);
        return 0;

    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void esp::CreateHWindow(const char* windowName, const char* className) {
    wc = {
        sizeof(wc),
        CS_HREDRAW | CS_VREDRAW,
        WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        className,
        nullptr
    };

    //RegisterClassExA(&wc);
    if (!RegisterClassExA(&wc)) {
        std::cerr << "RegisterClassExA failed. Error: " << GetLastError() << std::endl;
    }
    hWnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        wc.lpszClassName,
        windowName,
        WS_POPUP,
        0,
        0,
        1920,
        1080,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );
    std::cout << "wc? " << wc.cbSize << std::endl;

    std::cout << "hWnd? " << hWnd << std::endl;


    SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    RECT clientArea = {};
    GetClientRect(hWnd, &clientArea);

    RECT windowArea = {};
    GetWindowRect(hWnd, &windowArea);

    POINT diff{};
    ClientToScreen(hWnd, &diff);

    const MARGINS margins = {
        diff.x,
        diff.y,
        clientArea.right,
        clientArea.bottom
    };

    DwmExtendFrameIntoClientArea(hWnd, &margins);
    
}

void esp::DestroyHWindow() {
    DestroyWindow(hWnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);
}

bool esp::CreateDeviceD3D()
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, 
        D3D_DRIVER_TYPE_HARDWARE, 
        nullptr, 
        createDeviceFlags, 
        featureLevelArray, 
        2, 
        D3D11_SDK_VERSION, 
        &sd, 
        &g_pSwapChain, 
        &g_pd3dDevice, 
        &featureLevel, 
        &g_pd3dDeviceContext
    );
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;
    
    CreateRenderTarget();
    return true;
}

void esp::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void esp::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);
}

void esp::CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

void esp::CreateImGui() {
    IMGUI_CHECKVERSION();
    imguiContextDX11 = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContextDX11);


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

void esp::DestroyImGui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void esp::Render() {
    ImGui::SetCurrentContext(imguiContextDX11);
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    std::cout << "esp " << ImGui::GetCurrentContext() << std::endl;

    ImGui::NewFrame();
    ImGui::GetBackgroundDrawList()->AddCircleFilled({ 500, 500 }, 10.f, ImColor(1.f, 0.f, 0.f));

    ImGui::Render();

    const float clear_color_with_alpha[4] = { 0, 0, 0, 0 };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1U, 0U);
}

//void esp::RenderHelper() {
//    ImGui::EndFrame();
//    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
//    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
//    g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
//    if (g_pd3dDevice->BeginScene() >= 0)
//    {
//        ImGui::Render();
//        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
//        g_pd3dDevice->EndScene();
//    }
//    HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
//    if (result == D3DERR_DEVICELOST)
//        g_DeviceLost = true;
//}