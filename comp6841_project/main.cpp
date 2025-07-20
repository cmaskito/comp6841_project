#include "memory.h"
#include <iostream>
#include <thread>
#include "vector.h"
#include "entity.h"
#include <vector>
#include <algorithm>
#include "imguiHandler.h"
#include "esp.h"

void doAimbot(Memory& mem);
void doBHop(Memory& mem);
void doGlow(Memory& mem);

    
int main() {
	
	auto mem = Memory("csgo.exe");

	std::cout << "Process id: " << mem.GetProcessId() << std::endl;

	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client_dll_addr << std::dec << std::endl;
	std::thread t1(doAimbot, std::ref(mem));
	std::thread t2(doBHop, std::ref(mem));
	std::thread t3(doGlow, std::ref(mem));

	imguiHandler::doGui(mem);

	t1.join();
	t2.join();
	t3.join();
	return 0;
}

void doGlow(Memory& mem) {
	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	const auto engine_dll_addr = mem.GetModuleAddress("engine.dll");

	while (imguiHandler::isOpen) {
		if (!globals::glowActive) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		const auto local_player_addr = mem.Read<uintptr_t>(client_dll_addr + offsets::dwLocalPlayer);
		const auto glow_obj_manager = mem.Read<uintptr_t>(client_dll_addr + offsets::dwGlowObjectManager);
		if (local_player_addr) {

			Entity player = Entity(mem, local_player_addr);
			player.originPos.add(Vector(0, 0, player.viewOffset)); // get player camera position

			for (int i = 0; i < 64; i++) {
				const auto entity_ptr = mem.Read<uintptr_t>(client_dll_addr + offsets::entityList + i * 0x10);
				if (entity_ptr) {
					Entity entity = Entity(mem, entity_ptr);
					if ( entity.team == player.team ) {
						continue; // skip same team
					}
					const auto glowIndex = mem.Read<std::int32_t>(entity_ptr + offsets::m_iGlowIndex);
					mem.Write<float>(glow_obj_manager + glowIndex * 0x38 + 0x8, 1.f);
					mem.Write<float>(glow_obj_manager + glowIndex * 0x38 + 0xC, 0.f);
					mem.Write<float>(glow_obj_manager + glowIndex * 0x38 + 0x10, 0.f);
					mem.Write<float>(glow_obj_manager + glowIndex * 0x38 + 0x14, 1.f);

					mem.Write<bool>(glow_obj_manager + glowIndex * 0x38 + 0x27, true);
					mem.Write<bool>(glow_obj_manager + glowIndex * 0x38 + 0x28, true);

				}
				else {
					break;
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void doBHop(Memory& mem) {
	const auto client_dll_addr = mem.GetModuleAddress("client.dll");

	while (imguiHandler::isOpen) {
		if (!globals::bHopActive) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue; 
		}
		
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
	// get module addresses
	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	const auto engine_dll_addr = mem.GetModuleAddress("engine.dll");
	bool toggle_was_pressed = false;

	while (imguiHandler::isOpen) {
		// check if the toggle button is pressed
		bool toggle_pressed = GetAsyncKeyState(VK_XBUTTON1); // Check if the button is pressed
		if (toggle_pressed && !toggle_was_pressed) {
			std::cout << "Toggling aimbot: " << GetAsyncKeyState(VK_XBUTTON1) << std::endl;
			globals::aimbotActive = !globals::aimbotActive;
		}
		toggle_was_pressed = toggle_pressed; // Update the toggle state

		// check if the aimbot is active
		if (!globals::aimbotActive) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

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
						continue; // skip local player, same team, dead or dormant entities
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

			// On key press, aim at the closest entity
			if (GetAsyncKeyState(VK_LBUTTON) && !entity_list.empty()) {
				Entity entity = entity_list[0]; // get closest entity
				const float required_yaw = entity.headBonePos.copy().subtract(player.originPos).horizontalAngle();
				const float required_pitch = -entity.headBonePos.copy().subtract(player.originPos).verticalAngle();

				const auto client_state = mem.Read<uintptr_t>(engine_dll_addr + offsets::clientState);
				const auto yaw_addr = client_state + offsets::yaw;
				const auto pitch_addr = client_state + offsets::pitch;

				mem.Write<float>(yaw_addr, required_yaw);
				mem.Write<float>(pitch_addr, required_pitch);
			}

		}
	}
}