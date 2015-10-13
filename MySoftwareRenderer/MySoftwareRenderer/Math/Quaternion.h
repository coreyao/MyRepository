#pragma once

#include "Vector.h"

class Quaternion
{
public:
	Quaternion() : w(0), x(0), y(0), z(0) {}
	Quaternion(float _x, float _y, float _z, float _w);
	Quaternion(float fDeg, const Vec3& Axis);

	Quaternion operator*(const Quaternion& rh);
	void operator*(float fScalar);
	Vec3 operator*(const Vec3& rh);

	float GetLength();
	Quaternion GetInverse();
	void Normalize();

	static Quaternion Slerp(const Quaternion& v1, const Quaternion& v2, float fRatio);
	static Quaternion IDENTITY;

	float w;
	float x;
	float y;
	float z;
};