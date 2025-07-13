#pragma once
#include <cstddef>

namespace offsets {
	// client.dll
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEF97C;
	constexpr ::std::ptrdiff_t m_fFlags = 0x104;
	constexpr ::std::ptrdiff_t dwForceJump = 0x52C0F50;
	constexpr ::std::ptrdiff_t m_iHealth = 0x100;
	constexpr ::std::ptrdiff_t entityList = 0x4E051DC;
	constexpr ::std::ptrdiff_t xPos = 0xA0;
	constexpr ::std::ptrdiff_t yPos = 0xA4;
	constexpr ::std::ptrdiff_t zPos = 0xA8;
	constexpr ::std::ptrdiff_t m_vecViewOffset = 0x108;
	constexpr ::std::ptrdiff_t m_lifeState = 0x25F;
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_bDormant = 0xED;
	constexpr ::std::ptrdiff_t m_iGlowIndex = 0x10488;
	constexpr ::std::ptrdiff_t dwGlowObjectManager = 0x535FCB8;
	constexpr ::std::ptrdiff_t dwViewMatrix = 0x4DF6024;



	constexpr ::std::ptrdiff_t m_dwBoneMatrix = 0x26A8;
	constexpr ::std::ptrdiff_t headBone = 0x180;

	//engine.dll
	constexpr ::std::ptrdiff_t clientState = 0x59F19C;
	constexpr ::std::ptrdiff_t pitch = 0x4D90;
	constexpr ::std::ptrdiff_t yaw = 0x4D94;
}

namespace globals {
	inline bool aimbotActive = true;
	inline bool bHopActive = true;
	inline bool espActive = true;
	inline bool glowActive = true;
}