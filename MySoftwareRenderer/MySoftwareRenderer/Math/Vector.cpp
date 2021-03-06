#include "Vector.h"
#include "../Utility.h"
#include "../DataTypes.h"

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

float Vec3::Dot(const Vec3& rh) const
{
	return x * rh.x + y * rh.y + z * rh.z;
}

Vec3 Vec3::Cross(const Vec3& rh) const
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

float Vec3::GetLength() const
{
	return sqrt(x * x + y * y + z * z);
}

Vec2 Vec2::operator-(const Vec2& rh)const
{ 
	return Vec2( this->x - rh.x, this->y - rh.y );
}

Vec2 Vec2::operator+(const Vec2& rh) const
{
	return Vec2(this->x + rh.x, this->y + rh.y);
}

Vec2 Vec2::operator/(float fScalar)const
{
	return Vec2( this->x / fScalar, this->y / fScalar );
}

Vec2 Vec2::operator*(float fScalar)const
{
	return Vec2(this->x * fScalar, this->y * fScalar);
}

void Vec2::operator+=(const Vec2& rh)
{
	this->x += rh.x;
	this->y += rh.y;
}

void Vec2::set(float _x, float _y)
{
	x = _x;
	y = _y;
}

Vec4::Vec4()
{
#if USE_SIMD
	simd_data = _mm_setzero_ps();
#else
	x = y = z = w = 0;
#endif
}

Vec4::Vec4(float _x, float _y, float _z, float _w)
{
#if USE_SIMD
	simd_data = _mm_setr_ps(_x, _y, _z, _w);
#else
	x = _x;
	y = _y;
	z = _z;
	w = _w;
#endif
}

Vec4::Vec4(const Vec3& rh, float _w)
{
	this->x = rh.x;
	this->y = rh.y;
	this->z = rh.z;
	this->w = _w;
}

Vec4::Vec4(const Color4F& rh)
{
	x = rh.r;
	y = rh.g;
	z = rh.b;
	w = rh.a;
}

void Vec4::operator/=(float fScalar)
{
#if USE_SIMD
	simd_data = _mm_div_ps( simd_data, _mm_set1_ps(fScalar) );
#else
	float fInv = 1.0f / fScalar;
	this->x *= fInv;
	this->y *= fInv;
	this->z *= fInv;
	this->w *= fInv;
#endif
}

void Vec4::operator*=(float fScalar)
{
#if USE_SIMD
	simd_data = _mm_mul_ps( simd_data, _mm_set1_ps(fScalar) );
#else
	this->x *= fScalar;
	this->y *= fScalar;
	this->z *= fScalar;
	this->w *= fScalar;
#endif
}

void Vec4::set(float _x, float _y, float _z, float _w)
{
#if USE_SIMD
	simd_data = _mm_setr_ps(_x, _y, _z, _w);
#else
	x = _x;
	y = _y;
	z = _z;
	w = _w;
#endif
}

Vec4 Vec4::operator*(float fScalar) const
{
#if USE_SIMD
	Vec4 ret;
	ret.simd_data = _mm_mul_ps( simd_data, _mm_set1_ps(fScalar) );
	return ret;
#else
	return Vec4(x * fScalar, y * fScalar, z * fScalar, w * fScalar);
#endif
}

Vec4 Vec4::operator+(const Vec4& rh) const
{
#if USE_SIMD
	Vec4 ret;
	ret.simd_data = _mm_add_ps(simd_data, rh.simd_data);
	return ret;
#else
	return Vec4(x + rh.x, y + rh.y, z + rh.z, w + rh.w);
#endif
}

Vec4 Vec4::operator-(const Vec4& rh) const
{
#if USE_SIMD
	Vec4 ret;
	ret.simd_data = _mm_sub_ps(simd_data, rh.simd_data);
	return ret;
#else
	return Vec4(x - rh.x, y - rh.y, z - rh.z, w - rh.w);
#endif
}

void Vec4::operator+=(const Vec4& rh)
{
#if USE_SIMD
	simd_data = _mm_add_ps(simd_data, rh.simd_data);
#else
	x += rh.x;
	y += rh.y;
	z += rh.z;
	w += rh.w;
#endif
}

float Vec4::Dot(const Vec4& rh) const
{
#if USE_SIMD
	__m128 tmp = _mm_mul_ps(simd_data, rh.simd_data);
	return tmp.m128_f32[0] + tmp.m128_f32[1] + tmp.m128_f32[2] + tmp.m128_f32[3];

#else
	return x * rh.x + y * rh.y + z * rh.z + w * rh.w;
#endif
}
