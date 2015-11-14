#pragma once
#include <emmintrin.h>

class Vec2
{
public:
	Vec2() : x(0), y(0)
	{
	}

	Vec2( float _x, float _y ) : x(_x), y(_y)
	{
	}

	Vec2 operator-(const Vec2& rh) const;
	Vec2 operator+(const Vec2& rh) const;
	Vec2 operator/(float fScalar) const;
	Vec2 operator*(float fScalar) const;
	void operator+=(const Vec2& rh);

	void set(float _x, float _y);

	float x;
	float y;
};

class Color4F;
class Vec3;
/*__declspec(align(16)) */class Vec4
{
public:
	Vec4();
	Vec4(float _x, float _y, float _z, float _w);
	Vec4(const Vec3& rh, float _w);

	void set(float _x, float _y, float _z, float _w);

	void operator*=(float fScalar);
	void operator/=(float fScalar);
	void operator+=(const Vec4& rh);

	Vec4 operator*(float fScalar) const;
	Vec4 operator+(const Vec4& rh) const;
	Vec4 operator-(const Vec4& rh) const;

	float& operator[](int iIndex);

	float Dot(const Vec4& rh) const;

	union
	{
		struct { float x, y, z, w; };
		//__m128 simd_data;
	};
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

	Vec3(const Vec4& rh)
	{
		x = rh.x;
		y = rh.y;
		z = rh.z;
	}

	void set(float _x, float _y, float _z);

	Vec3 operator+(const Vec3& rh) const;
	void operator+=(const Vec3& rh);

	Vec3 operator-(const Vec3& rh) const;
	void operator-=(const Vec3& rh);

	Vec3 operator*(float fScalar) const;
	void operator*=(float fScalar);

	Vec3 operator*(const Vec3& rh) const;

	Vec3 operator/(float fScalar) const;
	void operator/=(float fScalar);

	Vec3 operator-() const;

	bool operator==(const Vec3& rh) const;
	bool operator!=(const Vec3& rh) const;

	float Dot(const Vec3& rh) const;
	Vec3 Cross(const Vec3& rh) const;
	void Normalize();
	Vec3 GetNormalized();
	float GetLength() const;
	float Distance(const Vec3& rh) const;

	float x;
	float y;
	float z;
};

