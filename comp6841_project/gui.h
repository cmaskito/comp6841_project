#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>

namespace gui {
	const int WIDTH = 300;
	const int HEIGHT = 200;

	inline WNDCLASSEXA wc = {};
	inline HWND hWnd = {};

	inline bool isOpen = true;
	//inline bool aimbotActive = false;

	// Data
	inline LPDIRECT3D9              g_pD3D = nullptr;
	inline LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
	inline bool                     g_DeviceLost = false;
	inline UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
	inline POINTS					position;
	inline D3DPRESENT_PARAMETERS    g_d3dpp = {};
	
	// Forward declarations of helper functions
	bool CreateDeviceD3D();
	void CleanupDeviceD3D();
	void ResetDevice();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	void CreateHWindow(const char* windowName, const char* className);
	void DestroyHWindow();

	void CreateImGui();
	void DestroyImGui();

	void Render();
	void RenderHelper();
}
