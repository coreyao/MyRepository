#include "Quaternion.h"
#include "../Utility.h"

Quaternion::Quaternion(float fDeg, const Vec3& Axis)
{
	float fAlpha = DEG_TO_RAD(fDeg / 2);
	w = cosf(fAlpha);
	x = Axis.x * sinf(fAlpha);
	y = Axis.y * sinf(fAlpha);
	z = Axis.z * sinf(fAlpha);

	this->Normalize();
}

Quaternion::Quaternion(float _w, float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

Quaternion Quaternion::operator*(const Quaternion& rh)
{
	Quaternion ret;

	Vec3 v1 = Vec3(x, y, z);
	Vec3 v2 = Vec3(rh.x, rh.y, rh.z);

	ret.w = w * rh.w - v1.Dot(v2);

	Vec3 t = v2 * w + v1 * rh.w + v1.Cross(v2);
	ret.x = t.x;
	ret.y = t.y;
	ret.z = t.z;

	return ret;
}

void Quaternion::operator*(float fScalar)
{
	w *= fScalar;
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;
}

Vec3 Quaternion::operator*(const Vec3& rh)
{
	Quaternion q( 0, rh.x, rh.y, rh.z );
	Quaternion iv = GetInverse();
	Quaternion ret = (*this) * q * iv;
	return Vec3(ret.x, ret.y, ret.z);
}

void Quaternion::Normalize()
{
	float fLen = GetLength();
	if ( fLen != 0 )
	{
		fLen = 1.0f / fLen;
		w *= fLen;
		x *= fLen;
		y *= fLen;
		z *= fLen;
	}
}

Quaternion Quaternion::Slerp(const Quaternion& v1, const Quaternion& v2, float fRatio)
{
	Quaternion ret;

	float w0 = v1.w;
	float x0 = v1.x;
	float y0 = v1.y;
	float z0 = v1.z;

	float w1 = v2.w;
	float x1 = v2.x;
	float y1 = v2.y;
	float z1 = v2.z;

	float fCosOmega = w0 * w1 + x0 * x1 + y0 * y1 + z0 * z1;
	if ( fCosOmega < 0.0f )
	{
		w1 = -w1;
		x1 = -x1;
		y1 = -y1;
		z1 = -z1;
		fCosOmega = -fCosOmega;
	}

	float k0, k1;
	if ( fCosOmega > 0.9999f )
	{
		k0 = 1.0f - fRatio;
		k1 = fRatio;
	}
	else
	{
		float sinOmega = sqrtf( 1.0f - fCosOmega * fCosOmega );
		float omega = atan2f(sinOmega, fCosOmega);
		float oneOverSinOmega = 1.0f / sinOmega;
		k0 = sinf((1.0f - fRatio) * omega) * oneOverSinOmega;
		k1 = sinf(fRatio * omega) * oneOverSinOmega;
	}

	ret.w = w0 * k0 + w1 * k1;
	ret.x = x0 * k0 + x1 * k1;
	ret.y = y0 * k0 + y1 * k1;
	ret.z = z0 * k0 + z1 * k1;

	return ret;
}

Quaternion Quaternion::GetInverse()
{
	float fLen = GetLength();
	if (fLen != 0)
	{
		fLen = 1.0f / fLen;
		return Quaternion(w * fLen, -x * fLen, -y * fLen, -z * fLen);
	}

	return Quaternion();
}

float Quaternion::GetLength()
{
	return sqrtf(w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::IDENTITY = Quaternion(1.0f, Vec3(0, 0, 0));
