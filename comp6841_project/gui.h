#pragma once
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

namespace gui {
	inline bool isOpen = true;

	// Data
	inline ID3D11Device* g_pd3dDevice = nullptr;
	inline ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
	inline IDXGISwapChain* g_pSwapChain = nullptr;
	inline bool                     g_SwapChainOccluded = false;
	inline UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
	inline ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

	// Forward declarations of helper functions
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int doGui();
}