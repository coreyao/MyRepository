#pragma once

class Vec3
{
public:
	Vec3();
	Vec3(float _x, float _y, float _z);

	Vec3 operator+(const Vec3& rh);
	Vec3 operator-(const Vec3& rh);

	float Dot(const Vec3& rh);
	Vec3 Cross(const Vec3& rh);

private:
	float x, y, z;
};