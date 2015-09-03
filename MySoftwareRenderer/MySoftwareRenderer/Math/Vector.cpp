#include "Vector.h"

void Vec3::set(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

Vec3 Vec3::operator+(const Vec3& rh) const
{
	return Vec3(x + rh.x, y + rh.y, z + rh.z);
}

void Vec3::operator+=(const Vec3& rh)
{
	x += rh.x;
	y += rh.y;
	z += rh.z;
}

Vec3 Vec3::operator-(const Vec3& rh) const
{
	return Vec3(x - rh.x, y - rh.y, z - rh.z);
}

void Vec3::operator-=(const Vec3& rh)
{
	x -= rh.x;
	y -= rh.y;
	z -= rh.z;
}

Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator*(float fScalar) const
{
	return Vec3(x * fScalar, y * fScalar, z * fScalar);
}

void Vec3::operator*=(float fScalar)
{
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;
}

Vec3 Vec3::operator/(float fScalar) const
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
	return x * rh.x + y * rh.y + z * rh.z;
}

Vec3 Vec3::Cross(const Vec3& rh)
{
	float nx = y * rh.z - rh.y * z;
	float ny = z * rh.x - rh.z * x;
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