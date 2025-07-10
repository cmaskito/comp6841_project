#include "memory.h"
#include <iostream>
#include <thread>
#include "entityList.h"
#include "vector.h"
#include "entity.h"
#include <vector>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include <d3d9.h>
#include "gui.h"

void doAimbot(Memory& mem);
void doBHop(Memory& mem);
    
int main() {
	
	auto mem = Memory("csgo.exe");

	std::cout << "Process id: " << mem.GetProcessId() << std::endl;

	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client_dll_addr << std::dec << std::endl;
	std::thread t1(doAimbot, std::ref(mem));
	std::thread t2(doBHop, std::ref(mem));
	//doStuff();

	gui::CreateHWindow("COMP6841 project", "Cheat menu");
	gui::CreateDeviceD3D();
	gui::CreateImGui();
	while (gui::isOpen) {
		gui::Render();
		gui::RenderHelper();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	gui::DestroyImGui();
	gui::CleanupDeviceD3D();
	gui::DestroyHWindow();

	/*while (true) {
		const auto dwLocalPlayerPawn = mem.Read<uint32_t>(client_dll_addr + offsets::dwLocalPlayer);

		if (dwLocalPlayerPawn) {
			const auto onGround = mem.Read<bool>(dwLocalPlayerPawn + offsets::m_fFlags);
			const auto health = mem.Read<int32_t>(dwLocalPlayerPawn + offsets::m_iHealth);
			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0)) {
				mem.Write<uint32_t>(client_dll_addr + offsets::dwForceJump, 6);
			}
		}
	}*/

	t1.join();
	t2.join();
	return 0;
}

void doBHop(Memory& mem) {
	const auto client_dll_addr = mem.GetModuleAddress("client.dll");

	while (gui::isOpen) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!globals::bHopActive) continue;
		
		const auto dwLocalPlayerPawn = mem.Read<uint32_t>(client_dll_addr + offsets::dwLocalPlayer);

		if (dwLocalPlayerPawn) {
			const auto onGround = mem.Read<bool>(dwLocalPlayerPawn + offsets::m_fFlags);
			const auto health = mem.Read<int32_t>(dwLocalPlayerPawn + offsets::m_iHealth);
			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0)) {
				mem.Write<uint32_t>(client_dll_addr + offsets::dwForceJump, 6);
			}
		}
	}
}

void doAimbot(Memory& mem) {
	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	const auto engine_dll_addr = mem.GetModuleAddress("engine.dll");
	bool toggle_was_pressed = false;

	while (gui::isOpen) {
		bool toggle_pressed = GetAsyncKeyState(VK_XBUTTON1); // Check if the button is pressed
		if (toggle_pressed && !toggle_was_pressed) {
			std::cout << "Toggling aimbot: " << GetAsyncKeyState(VK_XBUTTON1) << std::endl;
			globals::aimbotActive = !globals::aimbotActive;
		}
		toggle_was_pressed = toggle_pressed; // Update the toggle state

		const auto local_player_addr = mem.Read<uintptr_t>(client_dll_addr + offsets::dwLocalPlayer);
		if (local_player_addr) {

			Entity player = Entity(mem, local_player_addr);
			player.originPos.add(Vector(0, 0, player.viewOffset)); // get player camera position
			// read entity list
			std::vector<Entity> entity_list;
			for (int i = 0; i < 64; i++) {
				const auto entity_ptr = mem.Read<uintptr_t>(client_dll_addr + offsets::entityList + i * 0x10);
				if (entity_ptr) {
					Entity entity = Entity(mem, entity_ptr);
					if (entity_ptr == local_player_addr || entity.team == player.team || entity.lifeState || entity.isDormant) {
						continue; // skip local player
					}
					entity.distance = player.originPos.distanceTo(entity.headBonePos);
					entity_list.push_back(entity);
				}
				else {
					break;
				}
			}

			std::sort(entity_list.begin(), entity_list.end(), [](const Entity& a, const Entity& b) {
				return a.distance < b.distance; // sort by distance, ascending order
				});

			if (GetAsyncKeyState(VK_LBUTTON) && !entity_list.empty() && globals::aimbotActive) {
				Entity entity = entity_list[0]; // get closest entity
				const float required_yaw = entity.headBonePos.copy().subtract(player.originPos).horizontalAngle();
				const float required_pitch = -entity.headBonePos.copy().subtract(player.originPos).verticalAngle();

				const auto client_state = mem.Read<uintptr_t>(engine_dll_addr + offsets::clientState);
				const auto yaw_addr = client_state + offsets::yaw;
				const auto pitch_addr = client_state + offsets::pitch;
				std::cout << "origin pos -> " << entity.originPos << std::endl;

				std::cout << "headbone pos -> " <<  entity.headBonePos << std::endl;

				mem.Write<float>(yaw_addr, required_yaw);
				mem.Write<float>(pitch_addr, required_pitch);
			}

		}
	}
}