#include "Vector.h"
#include <cmath>

Vec3::Vec3()
	: x(0), y(0), z(0)
{
}

Vec3::Vec3( float _x, float _y, float _z )
{
	x = _x; y = _y; z = _z;
}

Vec3 Vec3::operator+( const Vec3& rh ) const
{
	return Vec3( x + rh.x, y + rh.y, z + rh.z );
}

Vec3 Vec3::operator-( const Vec3& rh ) const
{
	return Vec3( x - rh.x, y - rh.y, z - rh.z );
}

float Vec3::Dot( const Vec3& rh )
{
	return x * rh.x + y * rh.y + z * rh.z;
}

Vec3 Vec3::Cross( const Vec3& rh ) const
{
	float x = (this->y * rh.z) - (this->z * rh.y);
	float y = (this->z * rh.x) - (this->x * rh.z);
	float z = (this->x * rh.y) - (this->y * rh.x);

	return Vec3(x, y, z);
}

void Vec3::normalize()
{
	float n = x * x + y * y + z * z;
	// Already normalized.
	if (n == 1.0f)
		return;

	n = sqrt(n);
	// Too close to zero.
	if (n < MATH_TOLERANCE)
		return;

	n = 1.0f / n;
	x *= n;
	y *= n;
	z *= n;
}

Vec3 Vec3::operator*( float fScalar ) const
{
	return Vec3( x * fScalar, y * fScalar, z * fScalar );
}

void Vec3::set( float xx, float yy, float zz )
{
	x = xx; y = yy; z = zz;
}

Vec3 Vec3::operator+=( const Vec3& rh )
{
	*this = *this + rh;
	return *this;
}

void Vec4::set( float xx, float yy, float zz, float ww )
{
	x = xx; y = yy; z = zz; w = ww;
}

float& Vec4::operator[]( int iIndex )
{
	if ( iIndex == 0 )
	{
		return x;
	}
	else if ( iIndex == 1 )
	{
		return y;
	}
	else if ( iIndex == 2 )
	{
		return z;
	}
	else if ( iIndex == 3 )
	{
		return w;
	}

	return x;
}
