#include "Vector.h"

Vec3::Vec3()
	: x(0), y(0), z(0)
{
}

Vec3::Vec3( float _x, float _y, float _z )
{
	x = _x; y = _y; z = _z;
}

Vec3 Vec3::operator+( const Vec3& rh )
{
	return Vec3( x + rh.x, y + rh.y, z + rh.z );
}

Vec3 Vec3::operator-( const Vec3& rh )
{
	return Vec3( x - rh.x, y - rh.y, z - rh.z );
}

float Vec3::Dot( const Vec3& rh )
{
	return x * rh.x + y * rh.y + z * rh.z;
}

Vec3 Vec3::Cross( const Vec3& rh )
{
	float x = (this->y * rh.z) - (this->z * rh.y);
	float y = (this->z * rh.x) - (this->x * rh.z);
	float z = (this->x * rh.y) - (this->y * rh.x);

	return Vec3(x, y, z);
}

