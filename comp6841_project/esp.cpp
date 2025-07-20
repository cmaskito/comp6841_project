#include <Windows.h>

#include "esp.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#include <d3d11.h>
#include <iostream>
#include "globals.h"
#include "entity.h"
#include <thread>

bool esp::entityOnScreen(Entity entity) {
    return (entity.originPos2D.x > 0 && entity.originPos2D.x < 1920 &&
        entity.originPos2D.y > 0 && entity.originPos2D.y < 1080);
}
 
class ViewMatrix {
public:
    float matrix[4][4] = {0};

    ViewMatrix() = default;
    ViewMatrix(Memory &mem) {
		auto client_dll_addr = mem.GetModuleAddress("client.dll");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                this->matrix[i][j] = mem.Read<float>(client_dll_addr + offsets::dwViewMatrix + (i * 4 + j) * sizeof(float));
            }
		}
    }
    ~ViewMatrix() {
    }
};

Vector2 esp::worldToScreen(float matrix[4][4], Vector pos, Vector2 screenSize) {
    Vector2 screenPos;

	// Calculate x and y coordinates in clip space
    screenPos.x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
    screenPos.y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];

    // use screenW for perspective division
    float screenW = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

    if (screenW < 0.001f) {
        return { -1, -1 }; // behind the camera
    }
	// Normalised Device Coordinates (range of [-1, 1] for both x and y)
	float ndcX = screenPos.x / screenW;
	float ndcY = screenPos.y / screenW;


	// Convert normalised device coordinates to actual screen coordinates
	screenPos.x = (screenSize.x * 0.5f * ndcX) + (ndcX + screenSize.x * 0.5f);
    screenPos.y = -(screenSize.y * 0.5f * ndcY) + (ndcY + screenSize.y * 0.5f);

	std::cout << "screenPos: " << screenPos.x << ", " << screenPos.y << std::endl;
    return screenPos;
}

void esp::doEsp(Memory& mem) {
    if (!globals::espActive) {
        return; // if esp is not enabled, do nothing
	}
    const auto client_dll_addr = mem.GetModuleAddress("client.dll");

	const auto local_player_addr = mem.Read<uintptr_t>(client_dll_addr + offsets::dwLocalPlayer);
    //const auto view_matrix_addr = mem.Read<float*>(client_dll_addr + offsets::dwViewMatrix);
	ViewMatrix view_matrix = ViewMatrix(mem);
    if (local_player_addr) {

        Entity player = Entity(mem, local_player_addr);
        player.originPos.add(Vector(0, 0, player.viewOffset)); // get player camera position
        
        for (int i = 0; i < 64; i++) {
            const auto entity_ptr = mem.Read<uintptr_t>(client_dll_addr + offsets::entityList + i * 0x10);
            if (entity_ptr) {
                Entity entity = Entity(mem, entity_ptr);
                if (entity.team == player.team || entity.lifeState || entity.isDormant) {
                    continue; // skip same team
                }

                entity.originPos2D = worldToScreen(view_matrix.matrix, entity.originPos, { 1920, 1080 });
                entity.viewOffsetPos2D = worldToScreen(view_matrix.matrix, entity.originPos.copy().add(Vector(0, 0, entity.viewOffset)), {1920, 1080});

                if (entityOnScreen(entity)) {
					float entityHeight = entity.viewOffsetPos2D.y - entity.originPos2D.y;
                    float boxWidth = entityHeight * 0.3f;

                    // Draw box around entity
                    ImGui::GetBackgroundDrawList()->AddRect(
                        { entity.viewOffsetPos2D.x - boxWidth, entity.viewOffsetPos2D.y + entityHeight * 0.1f },
                        { entity.originPos2D.x + boxWidth, entity.originPos2D.y },
                        ImColor(1.f, 0.f, 0.f),
                        0.f,
                        ImDrawFlags_RoundCornersAll,
                        1.f
                    );
                }
            }
            else {
                break;
            }
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

