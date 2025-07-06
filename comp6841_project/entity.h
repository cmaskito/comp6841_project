#pragma once
#include <cstdint>
#include "vector.h"
#include "offsets.h"

class Entity
{
public:
	uintptr_t addr;
	Vector pos;
	int32_t health;
	float distance;
	uint32_t lifeState;

	// Constructor and destructor
	Entity(Memory& mem, uintptr_t addr) {
		const auto xPos = mem.Read<float>(addr + offsets::xPos);
		const auto yPos = mem.Read<float>(addr + offsets::yPos);
		const auto zPos = mem.Read<float>(addr + offsets::zPos);
		this->pos = Vector(xPos, yPos, zPos);
		this->health = mem.Read<int32_t>(addr + offsets::m_iHealth);
		this->lifeState = mem.Read<uint8_t>(addr + offsets::m_lifeState);
		this->addr = addr;
		this->distance = 0.0f; // Initialize distance to 0, can be calculated later
	}

	~Entity() {
	};

	//// Getters
	//const Vector& getPos() const { return pos; }
	//int getHealth() const { return health; }
	//float getDistance() const { return distance; }
	//uint32_t getLifeState() const { return lifeState; }
	//uintptr_t getAddr() const { return addr; }
}; //Size: 0x0248