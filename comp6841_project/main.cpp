#include "memory.h"
#include <iostream>
#include <thread>

namespace offsets {
	constexpr std::ptrdiff_t dwLocalPlayerController = 0x1A50AD0;
	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x18560D0;
	constexpr std::ptrdiff_t m_fFlags = 0x3EC; // uint8
	constexpr std::ptrdiff_t jump = 0x184EE00;
}

int main() {
	auto mem = Memory("cs2.exe");

	std::cout << "Process id: " << mem.GetProcessId() << std::endl;

	const auto client_dll_addr = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client_dll_addr << std::dec << std::endl;

	while (true) {
		const auto dwLocalPlayerController = mem.Read<uintptr_t>(client_dll_addr + offsets::dwLocalPlayerPawn);

		if (dwLocalPlayerController) {
			const auto onGround = mem.Read<bool>(dwLocalPlayerController + offsets::m_fFlags);
			//std::cout << "jump" << std::endl;
			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0)) {
				//std::cout << onGround << std::endl;
				mem.Write<uint32_t>(client_dll_addr + offsets::jump, 65537);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				mem.Write<uint32_t>(client_dll_addr + offsets::jump, 16777472);



			}
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(2));

	}
}