#include "memory.h"
#include <iostream>
#include <thread>

//namespace offsets {
//	constexpr std::ptrdiff_t dwLocalPlayerController = 0x1A50AD0;
//	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x18560D0;
//	constexpr std::ptrdiff_t m_fFlags = 0x3EC; // uint8
//	constexpr std::ptrdiff_t jump = 0x184EE00;
//}

namespace offsets {

	constexpr ::std::ptrdiff_t m_fFlags = 0x104;
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEF97C;
	constexpr ::std::ptrdiff_t dwForceJump = 0x52C0F50;
	constexpr ::std::ptrdiff_t m_iHealth = 0x100;

}

int main() {

	auto mem = Memory("csgo.exe");

	std::cout << "Process id: " << mem.GetProcessId() << std::endl;

	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client_dll_addr << std::dec << std::endl;

	while (true) {

		const auto dwLocalPlayerPawn = mem.Read<uint32_t>(client_dll_addr + offsets::dwLocalPlayer);

		if (dwLocalPlayerPawn) {
			const auto onGround = mem.Read<bool>(dwLocalPlayerPawn + offsets::m_fFlags);
			const auto health = mem.Read<int32_t>(dwLocalPlayerPawn + offsets::m_iHealth);
			std::cout << onGround << std::endl;
			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0)) {
				std::cout << "jump" << std::endl;
				mem.Write<uint32_t>(client_dll_addr + offsets::dwForceJump, 6);


			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
}