#pragma once
#include <ostream>
#include <cmath>
#include <numbers>

class Vector {
public:
	double x, y, z;
	Vector() = default;
	Vector(double x, double y, double z) {
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

	double horizontalAngle() {
		double angle_rad = std::atan2(this->y, this->x);
		return angle_rad * 180 / std::numbers::pi;
	}

	double verticalAngle() {
		double angle_rad = std::atan2(this->z, std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2)));
		return angle_rad * 180 / std::numbers::pi;
	}

	double distanceTo(Vector v) {
		Vector resultant_vec = v.copy().subtract(*this);
		double distance = std::sqrt(std::pow(resultant_vec.x, 2) + std::pow(resultant_vec.y, 2) + std::pow(resultant_vec.z, 2));
		return distance;
	}
};

class Vector2 {
public:
	float x, y;
	Vector2() = default;
	Vector2(float x, float y) {
		this->x = x;
		this->y = y;
	}
	~Vector2() {

	}

	Vector2 add(Vector2 v) {
		this->x += v.x;
		this->y += v.y;
		return *this;
	}

	Vector2 subtract(Vector2 v) {
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}

	Vector2 copy() {
		return Vector2(this->x, this->y);
	}
};

inline std::ostream& operator<<(std::ostream& os, const Vector& v) {
	return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

inline std::ostream& operator<<(std::ostream& os, const Vector2& v) {
	return os << '(' << v.x << ", " << v.y << ')';
}