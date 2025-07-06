#include "memory.h"
#include <iostream>
#include <thread>
#include "entityList.h"
#include "vector.h"
#include "entity.h"
#include <vector>
#include <algorithm>

//namespace offsets {
//	constexpr std::ptrdiff_t dwLocalPlayerController = 0x1A50AD0;
//	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x18560D0;
//	constexpr std::ptrdiff_t m_fFlags = 0x3EC; // uint8
//	constexpr std::ptrdiff_t jump = 0x184EE00;
//}


void testFunction();

int main() {

	auto mem = Memory("csgo.exe");

	std::cout << "Process id: " << mem.GetProcessId() << std::endl;

	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client_dll_addr << std::dec << std::endl;
	std::thread t1(testFunction);

	while (true) {
		const auto dwLocalPlayerPawn = mem.Read<uint32_t>(client_dll_addr + offsets::dwLocalPlayer);

		if (dwLocalPlayerPawn) {
			const auto onGround = mem.Read<bool>(dwLocalPlayerPawn + offsets::m_fFlags);
			const auto health = mem.Read<int32_t>(dwLocalPlayerPawn + offsets::m_iHealth);
			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0)) {
				mem.Write<uint32_t>(client_dll_addr + offsets::dwForceJump, 6);
			}
		}
	}


	t1.join();
}

void doAimbot() {

}

void testFunction() {
	//std::cout << "IN test thread" << std::endl;

	auto mem = Memory("csgo.exe");
	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	const auto engine_dll_addr = mem.GetModuleAddress("engine.dll");


	while (true) {
		const auto local_player_addr = mem.Read<uintptr_t>(client_dll_addr + offsets::dwLocalPlayer);
		if (local_player_addr) {

			Entity player = Entity(mem, local_player_addr);
			player.originPos.add(Vector(0, 0, player.viewOffset)); // add 64 to Z axis to avoid hitting the ground
			// read entity list
			std::vector<Entity> entity_list;
			for (int i = 0; i < 64; i++) {

				const auto entity_ptr = mem.Read<uintptr_t>(client_dll_addr + offsets::entityList + i * 0x10);
				if (entity_ptr) {
					Entity entity = Entity(mem, entity_ptr);
					if (entity_ptr == local_player_addr) {
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

			if (GetAsyncKeyState(VK_XBUTTON2) && !entity_list.empty()) {
				Entity entity = entity_list[0]; // get closest entity
				const float required_yaw = entity.headBonePos.copy().subtract(player.originPos).horizontalAngle();
				const float required_pitch = -entity.headBonePos.copy().subtract(player.originPos).verticalAngle();

				const auto client_state = mem.Read<uintptr_t>(engine_dll_addr + offsets::clientState);
				const auto yaw_addr = client_state + offsets::yaw;
				const auto pitch_addr = client_state + offsets::pitch;

				//std::cout << "engine.dll -> " << "0x" << std::hex << engine_dll_addr << std::dec << std::endl;

				std::cout << "headbone pos -> " <<  entity.headBonePos << std::endl;

				mem.Write<float>(yaw_addr, required_yaw);
				mem.Write<float>(pitch_addr, required_pitch);
			}

			//std::cout << "closest entity health: " << entity_list[0].health << std::endl;
		}

		//}



		//const auto pitch_addr = 

		//std::cout << "===============" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}