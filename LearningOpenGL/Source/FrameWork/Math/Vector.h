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

	Vec2( float _x, float _y )
	{
		x = _x;
		y = _y;
	}

	bool operator==( const Vec2& rh );
	bool operator!=( const Vec2& rh );
	const Vec2 operator-(const Vec2& rh) const;
	
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
	Vec3 operator/(float fScalar) const;
	Vec3 operator*(const Vec3& rh) const;
	Vec3 operator+=(const Vec3& rh);
	const Vec3 operator-() const;

	float Dot(const Vec3& rh) const;
	Vec3 Cross(const Vec3& rh) const;
	void set(float xx, float yy, float zz);
	float Distance( const Vec3& rh );

	void normalize();
	float length() const;

	float x, y, z;
};

class Vec4
{
public:
	Vec4()
		: x(0), y(0), z(0), w(0)
	{
	}

	Vec4(float xx, float yy, float zz, float ww)
	{
		set(xx, yy, zz, ww);
	}

	Vec4 operator*(float fScalar) const;
	Vec4 operator+(const Vec4& rh) const;
	float& operator[](int iIndex);

	void set(float xx, float yy, float zz, float ww);

	float x, y, z, w;
};