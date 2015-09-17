#pragma once

#include "../Utility.h"

class Vec2
{
public:
	Vec2() : x(0), y(0)
	{
	}

	Vec2( float _x, float _y ) : x(_x), y(_y)
	{
	}

	Vec2 operator-(const Vec2& rh);
	Vec2 operator+(const Vec2& rh);
	Vec2 operator/(float fScalar);
	Vec2 operator*(float fScalar);
	void operator+=(const Vec2& rh);

	void set(float _x, float _y);

	float x;
	float y;
};

class Vec3
{
public:
	Vec3() : x(0), y(0), z(0)
	{
	}

	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{
	}

	void set(float _x, float _y, float _z);

	Vec3 operator+(const Vec3& rh) const;
	void operator+=(const Vec3& rh);

	Vec3 operator-(const Vec3& rh) const;
	void operator-=(const Vec3& rh);

	Vec3 operator*(float fScalar) const;
	void operator*=(float fScalar);

	Vec3 operator/(float fScalar) const;
	void operator/=(float fScalar);

	Vec3 operator-() const;

	float Dot(const Vec3& rh) const;
	Vec3 Cross(const Vec3& rh) const;
	void Normalize();
	Vec3 GetNormalized();
	float GetLength();

	float x;
	float y;
	float z;
};

class Vec4
{
public:
	Vec4() : x(0), y(0), z(0), w(0)
	{
	}

	Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
	{
	}

	float x;
	float y;
	float z;
	float w;
};
