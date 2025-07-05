#pragma once
#include <cstdint>

class clientInfo
{
public:
	class ent* entPtr; //0x0000
	int32_t padding; //0x0004
	class clientInfo* backLink; //0x0008
	class clientInfo* forwardLink; //0x000C
}; //Size: 0x0010

class CBaseEntityList : public clientInfo
{
public:
	class clientInfo entList[64]; //0x0010
	char pad_0410[2096]; //0x0410
}; //Size: 0x0C40

class ent
{
public:
	char pad_0000[160]; //0x0000
	float xPos; //0x00A0
	float yPos; //0x00A4
	float zPos; //0x00A8
	char pad_00AC[84]; //0x00AC
	int32_t health; //0x0100
	char pad_0104[324]; //0x0104
}; //Size: 0x0248