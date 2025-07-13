#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d11.h>
#include <tchar.h>
#include "memory.h"
#include "entity.h"
#include "vector.h"

namespace esp {
	inline WNDCLASSEXA wc = {};
	inline HWND hWnd = {};
	inline ImGuiContext* imguiContextDX11 = nullptr;

	bool entityOnScreen(Entity entity);
	Vector2 worldToScreen(float matrix[4][4], Vector pos, Vector2 screenSize);
	void doEsp(Memory& mem);
}
