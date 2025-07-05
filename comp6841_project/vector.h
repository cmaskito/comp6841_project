#pragma once
#include <ostream>
#include <cmath>
#include <numbers>

class Vector {
public:
	float x, y, z;

	Vector(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	~Vector() {

	}

	Vector add(Vector v) {
		this->x += v.x; 
		this->y += v.y; 
		this->z += v.z;
		return *this;
	}

	Vector subtract(Vector v) {
		this->x -= v.x; 
		this->y -= v.y; 
		this->z -= v.z;
		return *this;
	}
	
	Vector copy() {
		return Vector(this->x, this->y, this->z);
	}

	float horizontalAngle() {
		float angle_rad = std::atan2(this->y, this->x);
		return angle_rad * 180 / std::numbers::pi;
	}

	float verticalAngle() {
		float angle_rad = std::atan2(this->z, std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2)));
		return angle_rad * 180 / std::numbers::pi;
	}

};

inline std::ostream& operator<<(std::ostream& os, const Vector& v) {
	return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}