#include "Matrix.h"
#include "../Utility.h"

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

Vec4 Mat4::operator*(const Vec4& rh) const
{
	Vec4 ret;
	ret.x = m[0] * rh.x + m[4] * rh.y + m[8] * rh.z + m[12] * rh.w;
	ret.y = m[1] * rh.x + m[5] * rh.y + m[9] * rh.z + m[13] * rh.w;
	ret.z = m[2] * rh.x + m[6] * rh.y + m[10] * rh.z + m[14] * rh.w;
	ret.w = m[3] * rh.x + m[7] * rh.y + m[11] * rh.z + m[15] * rh.w;

	return ret;
}

Mat4::Mat4(const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis, const Vec3& origin)
{
	m[0] = xAxis.x;
	m[1] = xAxis.y;
	m[2] = xAxis.z;
	m[3] = 0;

	m[4] = yAxis.x;
	m[5] = yAxis.y;
	m[6] = yAxis.z;
	m[7] = 0;

	m[8] = zAxis.x;
	m[9] = zAxis.y;
	m[10] = zAxis.z;
	m[11] = 0;

	m[12] = origin.x;
	m[13] = origin.y;
	m[14] = origin.z;
	m[15] = 1.0f;
}

Mat4::Mat4()
{
	memset((void*)m, 0, sizeof(m));
}

Mat4 Mat4::IDENTITY = Mat4(Vec3(1, 0, 0),
	Vec3(0, 1, 0),
	Vec3(0, 0, 1),
	Vec3(0, 0, 0));

Mat4 Mat4::ZERO = Mat4(Vec3(0, 0, 0),
	Vec3(0, 0, 0),
	Vec3(0, 0, 0),
	Vec3(0, 0, 0));


Mat4 Mat4::CreateTranslationMat(float x, float y, float z)
{
	Mat4 ret = IDENTITY;
	ret.m[12] = x;
	ret.m[13] = y;
	ret.m[14] = z;

	return ret;
}

Mat4 Mat4::CreateRotationMat(float x, float y, float z)
{
	x = DEG_TO_RAD(x);
	y = DEG_TO_RAD(y);
	z = DEG_TO_RAD(z);

	Mat4 rotateZMat(Vec3(cosf(z), sinf(z), 0),
		Vec3(-sinf(z), cosf(z), 0),
		Vec3(0, 0, 1),
		Vec3(0, 0, 0));

	Mat4 rotateXMat(Vec3(1, 0, 0),
		Vec3(0, cosf(x), sinf(x)),
		Vec3(0, -sinf(x), cosf(x)),
		Vec3(0, 0, 0));

	Mat4 rotateYMat(
		Vec3(cosf(y), 0, -sinf(y)),
		Vec3(0, 1, 0),
		Vec3(sinf(y), 0, cosf(y)),
		Vec3(0, 0, 0));

	return rotateYMat * rotateXMat * rotateZMat;
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
	ret.m[0] = (1.0f / whRatio) * (1.0f / tanf(DEG_TO_RAD(fVerticleFov / 2)));
	ret.m[5] = 1.0f / tanf(DEG_TO_RAD(fVerticleFov / 2));
	ret.m[10] = -(f + n) / (f - n);
	ret.m[14] = -2 * f * n / (f - n);
	ret.m[11] = -1.0f;

	return ret;
}

Mat4 Mat4::CreateOrthegraphicsMat(float l, float r, float t, float b, float n, float f)
{
	return Mat4::ZERO;
}

Mat4 Mat4::CreateLookAt(const Vec3& eyePos, const Vec3& lookDir, Vec3 upVec /*= Vec3(0, 1, 0)*/)
{
	Vec3 forward = lookDir;
	forward.Normalize();

	Vec3 rightDir = forward.Cross(upVec);
	rightDir.Normalize();

	Vec3 upDir = rightDir.Cross(forward);
	upDir.Normalize();

	Mat4 cameraWorld(rightDir, upDir, -forward, eyePos);
	return cameraWorld.GetInversed();
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

bool Mat4::operator==(const Mat4& rh)
{
	return !memcmp((void*)m, (void*)rh.m, sizeof(m));
}

bool Mat4::operator!=(const Mat4& rh)
{
	return !(*this == rh);
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
	float det = Determinant();
	if (!NEARLY_EQUAL(det, 0))
	{
		float det_inv = 1.0 / det;

		ret.m[0] = det_inv * ( m[5] * m[10] - m[9] * m[6] );
		ret.m[1] = -det_inv * ( m[1] * m[10] - m[9] * m[2] );
		ret.m[2] = det_inv * ( m[1] * m[6] - m[5] * m[2] );

		ret.m[4] = -det_inv * ( m[4] * m[10] - m[8] * m[6] );
		ret.m[5] = det_inv * ( m[0] * m[10] - m[8] * m[2] );
		ret.m[6] = -det_inv * ( m[0] * m[6] - m[4] * m[2] );

		ret.m[8] = det_inv * ( m[4] * m[9] - m[8] * m[5] );
		ret.m[9] = -det_inv * ( m[0] * m[9] - m[1] * m[8] );
		ret.m[10] = det_inv * ( m[0] * m[5] - m[4] * m[1] );

		ret.m[12] = -(m[12] * ret.m[0] + m[13] * ret.m[4] + m[14] * ret.m[8]);
		ret.m[13] = -(m[12] * ret.m[1] + m[13] * ret.m[5] + m[14] * ret.m[9]);
		ret.m[14] = -(m[12] * ret.m[2] + m[13] * ret.m[6] + m[14] * ret.m[10]);
		ret.m[15] = 1.0f;
	}

	return ret;
}

void Mat4::Inverse()
{
	*this = GetInversed();
}

float Mat4::Determinant()
{
	return m[0] * m[5] * m[10] + m[4] * m[9] * m[2] + m[8] * m[6] * m[1]
		- m[0] * m[6] * m[9] - m[4] * m[1] * m[10] - m[8] * m[5] * m[2];
}
