#pragma once

#define MATH_TOLERANCE 2e-37f
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180
#define RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180

class Vec2
{
public:
	Vec2()
		: x(0), y(0)
	{
	}
	
	float x, y;
};

class Vec3
{
public:
	Vec3();
	Vec3(float _x, float _y, float _z);

	Vec3 operator+(const Vec3& rh) const;
	Vec3 operator-(const Vec3& rh) const;
	Vec3 operator*(float fScalar) const;

	float Dot(const Vec3& rh);
	Vec3 Cross(const Vec3& rh);
	void set(float xx, float yy, float zz);

	void normalize();

	float x, y, z;
};

class Vec4
{
public:
	Vec4()
		: x(0), y(0), z(0), w(0)
	{
	}

	void set(float xx, float yy, float zz, float ww);

	float x, y, z, w;
};