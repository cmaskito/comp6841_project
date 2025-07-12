#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d11.h>
#include <tchar.h>

namespace esp {
	inline WNDCLASSEXA wc = {};
	inline HWND hWnd = {};
	inline ImGuiContext* imguiContextDX11 = nullptr;

	inline bool isOpen = true;
	//inline bool aimbotActive = false;

	// Data
	static ID3D11Device* g_pd3dDevice = nullptr;
	static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
	static IDXGISwapChain* g_pSwapChain = nullptr;
	static bool                     g_SwapChainOccluded = false;
	static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
	static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

	// Forward declarations of helper functions
	bool CreateDeviceD3D();
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void CreateHWindow(const char* windowName, const char* className);
	void DestroyHWindow();

	void CreateImGui();
	void DestroyImGui();

	void Render();
	void RenderHelper();

}
