#include "Math.h"
#include <cmath>
#include <string>

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

	ret.m[2] = m[2] * rh.m[0] + m[6] * rh.m[1] + m[10] * rh.m[2] + m[14] * rh.m[3];
	ret.m[6] = m[2] * rh.m[4] + m[6] * rh.m[5] + m[10] * rh.m[6] + m[14] * rh.m[7];
	ret.m[10] = m[2] * rh.m[8] + m[6] * rh.m[9] + m[10] * rh.m[10] + m[14] * rh.m[11];
	ret.m[14] = m[2] * rh.m[12] + m[6] * rh.m[13] + m[10] * rh.m[14] + m[14] * rh.m[15];

	ret.m[3] = m[3] * rh.m[0] + m[7] * rh.m[1] + m[11] * rh.m[2] + m[15] * rh.m[3];
	ret.m[7] = m[3] * rh.m[4] + m[7] * rh.m[5] + m[11] * rh.m[6] + m[15] * rh.m[7];
	ret.m[11] = m[3] * rh.m[8] + m[7] * rh.m[9] + m[11] * rh.m[10] + m[15] * rh.m[11];
	ret.m[15] = m[3] * rh.m[12] + m[7] * rh.m[13] + m[11] * rh.m[14] + m[15] * rh.m[15];

	return ret;
}

Mat4 Mat4::operator*(float fScalar)
{
	Mat4 ret;
	for (int i = 0; i < 16; ++i)
		ret.m[i] = m[i] * fScalar;

	return ret;
}

void Mat4::operator*=(const Mat4& rh)
{
	*this = *this * rh;
}

void Mat4::operator*=(float fScalar)
{
	*this = *this * fScalar;
}

Mat4::Mat4(const Vec4& xAxis, const Vec4& yAxis, const Vec4& zAxis, const Vec4& origin)
{
	m[0] = xAxis.x;
	m[1] = xAxis.y;
	m[2] = xAxis.z;
	m[3] = xAxis.w;

	m[4] = yAxis.x;
	m[5] = yAxis.y;
	m[6] = yAxis.z;
	m[7] = yAxis.w;

	m[8] = zAxis.x;
	m[9] = zAxis.y;
	m[10] = zAxis.z;
	m[11] = zAxis.w;

	m[12] = origin.x;
	m[13] = origin.y;
	m[14] = origin.z;
	m[15] = 1.0f;
}

Mat4::Mat4()
{
	memset((void*)m, 0, sizeof(m));
}

Mat4 Mat4::IDENTITY = Mat4(Vec4(1, 0, 0, 0),
							 Vec4(0, 1, 0, 0), 
							 Vec4(0, 0, 1, 0), 
							 Vec4(0, 0, 0, 1) );

Mat4 Mat4::ZERO = Mat4(	Vec4(0, 0, 0, 0),
						Vec4(0, 0, 0, 0),
						Vec4(0, 0, 0, 0),
						Vec4(0, 0, 0, 0));


Mat4 Mat4::CreateTranslationMat(const Vec3& trans)
{
	Mat4 ret = ZERO;
	ret.m[12] = trans.x;
	ret.m[13] = trans.y;
	ret.m[14] = trans.z;

	return ret;
}

Mat4 Mat4::CreateRotationMat(float x, float y, float z)
{
	Mat4 rotateZMat( Vec4(cosf(z), sinf(z), 0, 0),
		Vec4(-sinf(z), cosf(z), 0, 0),
		Vec4(0, 0, 1, 0),
		Vec4(0, 0, 0, 1) );

	Mat4 rotateXMat(Vec4(1, 0, 0, 0),
		Vec4(0, cosf(x), sinf(x), 0),
		Vec4(0, -sinf(x), cosf(x), 0),
		Vec4(0, 0, 0, 1));

	Mat4 rotateYMat(
		Vec4(cosf(x), 0, -sinf(x), 0),
		Vec4(1, 0, 0, 0),
		Vec4(sinf(x), 0, cosf(x), 0),
		Vec4(0, 0, 0, 1));

	return rotateXMat * rotateYMat * rotateZMat;
}

Mat4 Mat4::CreateScaleMat(float x, float y, float z)
{
	Mat4 ret = IDENTITY;
	ret.m[0] = x;
	ret.m[5] = y;
	ret.m[10] = z;

	return ret;
}

Mat4 Mat4::CreatePerspectiveMat(float fVerticleFov, float whRatio, float n, float f)
{
	Mat4 ret;
	ret.m[0] = (1.0f / whRatio) * (1.0f / tanf(DEG_TO_RAD(fVerticleFov)));
	ret.m[5] = 1.0f / tanf(DEG_TO_RAD(fVerticleFov));
	ret.m[10] = (f + n) / (f - n);
	ret.m[14] = -2 * f * n / (f - n);
	ret.m[11] = 1.0f;

	return ret;
}

Mat4 Mat4::CreateOrthegraphicsMat(float l, float r, float t, float b, float n, float f)
{
	return Mat4::ZERO;
}

Mat4 Mat4::operator+(const Mat4& rh)
{
	Mat4 ret;
	for (int i = 0; i < 16; ++i)
		ret.m[i] = m[i] + rh.m[i];

	return ret;
}

void Mat4::operator+=(const Mat4& rh)
{
	*this = *this + rh;
}

Mat4 Mat4::GetTransposed()
{
	Mat4 ret;
	ret.m[0] = m[0];
	ret.m[1] = m[4];
	ret.m[2] = m[8];
	ret.m[3] = m[12];

	ret.m[4] = m[1];
	ret.m[5] = m[5];
	ret.m[6] = m[9];
	ret.m[7] = m[13];

	ret.m[8] = m[2];
	ret.m[9] = m[6];
	ret.m[10] = m[10];
	ret.m[11] = m[14];

	ret.m[12] = m[3];
	ret.m[13] = m[7];
	ret.m[14] = m[11];
	ret.m[15] = m[15];

	return ret;
}

void Mat4::Transpose()
{
	*this = GetTransposed();
}

Mat4 Mat4::GetInversed()
{
	Mat4 ret;
	return ret;
}

void Mat4::Inverse()
{

}
