#pragma once
#include <cstdint>
#include "vector.h"
#include "globals.h"

class Entity
{
public:
	uintptr_t addr;
	Vector originPos;
	int32_t health;
	float distance;
	uint32_t lifeState;
	float viewOffset;
	Vector headBonePos;
	uint32_t team;
	bool isDormant;
	Vector2 originPos2D;
	Vector2 viewOffsetPos2D;

	// Constructor and destructor
	Entity(Memory& mem, uintptr_t addr) {
		const auto xPos = mem.Read<float>(addr + offsets::xPos);
		const auto yPos = mem.Read<float>(addr + offsets::yPos);
		const auto zPos = mem.Read<float>(addr + offsets::zPos);
		this->originPos = Vector(xPos, yPos, zPos);
		this->health = mem.Read<int32_t>(addr + offsets::m_iHealth);
		this->lifeState = mem.Read<uint8_t>(addr + offsets::m_lifeState);
		this->addr = addr;
		this->viewOffset = mem.Read<float>(addr + offsets::m_vecViewOffset + 0x8);
		this->distance = 0.0f; // Initialize distance to 0, can be calculated later
		const auto boneMatrixPtr = mem.Read<uintptr_t>(addr + offsets::m_dwBoneMatrix);
		const auto headBoneX = mem.Read<float>(boneMatrixPtr + 0xC + offsets::headBone);
		const auto headBoneY = mem.Read<float>(boneMatrixPtr + 0x1C + offsets::headBone);
		const auto headBoneZ = mem.Read<float>(boneMatrixPtr + 0x2C + offsets::headBone);
		this->headBonePos = Vector(headBoneX, headBoneY, headBoneZ);
		this->team = mem.Read<uint32_t>(addr + offsets::m_iTeamNum); 
		this->isDormant = mem.Read<bool>(addr + offsets::m_bDormant);
	}

	~Entity() {
	};
};