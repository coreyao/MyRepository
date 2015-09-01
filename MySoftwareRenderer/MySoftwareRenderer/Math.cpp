#include "Math.h"
#include <cmath>

Vec3 Vec3::operator+(const Vec3& rh)
{
	return Vec3(x + rh.x, y + rh.y, z + rh.z);
}

void Vec3::operator+=(const Vec3& rh)
{
	x += rh.x;
	y += rh.y;
	z += rh.z;
}

Vec3 Vec3::operator-(const Vec3& rh)
{
	return Vec3(x - rh.x, y - rh.y, z - rh.z);
}

void Vec3::operator-=(const Vec3& rh)
{
	x -= rh.x;
	y -= rh.y;
	z -= rh.z;
}

Vec3 Vec3::operator*(float fScalar)
{
	return Vec3(x * fScalar, y * fScalar, z * fScalar);
}

void Vec3::operator*=(float fScalar)
{
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;
}

Vec3 Vec3::operator/(float fScalar)
{
	return Vec3(x / fScalar, y / fScalar, z / fScalar);
}

void Vec3::operator/=(float fScalar)
{
	x /= fScalar;
	y /= fScalar;
	z /= fScalar;
}

float Vec3::Dot(const Vec3& rh)
{
	return x * rh.x + y * rh.y;
}

Vec3 Vec3::Cross(const Vec3& rh)
{
	float nx = y * rh.z - rh.y * z;
	float ny = x * rh.z - rh.x * z;
	float nz = x * rh.y - rh.x * y;

	return Vec3(nx, ny, nz);
}

void Vec3::Normalize()
{
	float n = 1.0f / GetLength();
	*this *= n;
}

Vec3 Vec3::GetNormalized()
{
	float n = 1.0f / GetLength();
	return *this * n;
}

float Vec3::GetLength()
{
	return sqrt(x * x + y * y + z * z);
}

Mat4 Mat4::operator*(const Mat4& rh)
{
	Mat4 ret;
	ret.m[0] = m[0] * rh.m[0] + m[4] * rh.m[1] + m[8] * rh.m[2] + m[12] * rh.m[3];
	ret.m[4] = m[0] * rh.m[4] + m[4] * rh.m[5] + m[8] * rh.m[6] + m[12] * rh.m[7];
	ret.m[8] = m[0] * rh.m[8] + m[4] * rh.m[9] + m[8] * rh.m[10] + m[12] * rh.m[11];
	ret.m[12] = m[0] * rh.m[12] + m[4] * rh.m[13] + m[8] * rh.m[14] + m[12] * rh.m[15];

	ret.m[1] = m[1] * rh.m[0] + m[5] * rh.m[1] + m[9] * rh.m[2] + m[13] * rh.m[3];
	ret.m[5] = m[1] * rh.m[4] + m[5] * rh.m[5] + m[9] * rh.m[6] + m[13] * rh.m[7];
	ret.m[9] = m[1] * rh.m[8] + m[5] * rh.m[9] + m[9] * rh.m[10] + m[13] * rh.m[11];
	ret.m[13] = m[1] * rh.m[12] + m[5] * rh.m[13] + m[9] * rh.m[14] + m[13] * rh.m[15];

	//ret.m[2] = m[2] * rh.m[0] + m[6] * rh.m[1] + m[8] * rh.m[2] + m[12] * rh.m[3];
	//ret.m[6] = m[2] * rh.m[4] + m[4] * rh.m[5] + m[8] * rh.m[6] + m[12] * rh.m[7];
	//ret.m[10] = m[2] * rh.m[8] + m[4] * rh.m[9] + m[8] * rh.m[10] + m[12] * rh.m[11];
	//ret.m[14] = m[2] * rh.m[12] + m[4] * rh.m[13] + m[8] * rh.m[14] + m[12] * rh.m[15];

	//ret.m[0] = m[0] * rh.m[0] + m[4] * rh.m[1] + m[8] * rh.m[2] + m[12] * rh.m[3];
	//ret.m[4] = m[0] * rh.m[4] + m[4] * rh.m[5] + m[8] * rh.m[6] + m[12] * rh.m[7];
	//ret.m[8] = m[0] * rh.m[8] + m[4] * rh.m[9] + m[8] * rh.m[10] + m[12] * rh.m[11];
	//ret.m[12] = m[0] * rh.m[12] + m[4] * rh.m[13] + m[8] * rh.m[14] + m[12] * rh.m[15];
}

void Mat4::operator*=(const Mat4& rh)
{

}

Mat4 Mat4::CreateTranslationMat(const Vec3& trans)
{

}

Mat4 Mat4::CreateRotationMat(float x, float y, float z)
{

}

Mat4 Mat4::CreateScaleMat(float x, float y, float z)
{

}

Mat4 Mat4::CreatePerspectiveMat(float fVerticleFov, float whRatio, float n, float f)
{

}

Mat4 Mat4::CreateOrthegraphicMat(float l, float r, float t, float b, float n, float f)
{

}
