#pragma once

#define PI 3.141592f
#define DEG_TO_RAD(x) ((PI) * ((180.0f) / (x))) 
#define RAD_TO_DEG(x) ((180) * ((PI) / (x)) )
class Vec2
{
public:
	Vec2(): x(0), y(0)
	{
	}

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

	Vec3 operator+(const Vec3& rh);
	void operator+=(const Vec3& rh);

	Vec3 operator-(const Vec3& rh);
	void operator-=(const Vec3& rh);

	Vec3 operator*(float fScalar);
	void operator*=(float fScalar);

	Vec3 operator/(float fScalar);
	void operator/=(float fScalar);

	float Dot(const Vec3& rh);
	Vec3 Cross(const Vec3& rh);
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

class Mat4
{
public:
	static Mat4 CreateTranslationMat(const Vec3& trans);
	static Mat4 CreateRotationMat(float x, float y, float z);
	static Mat4 CreateScaleMat(float x, float y, float z);
	static Mat4 CreatePerspectiveMat(float fVerticleFov, float whRatio, float n, float f);
	static Mat4 CreateOrthegraphicsMat(float l, float r, float t, float b, float n, float f);

	Mat4(const Vec4& xAxis, const Vec4& yAxis, const Vec4& zAxis, const Vec4& origin);

	Mat4 operator*(const Mat4& rh);
	void operator*=(const Mat4& rh);

	Mat4();
	float m[16];

	static Mat4 IDENTITY;
	static Mat4 ZERO;
};