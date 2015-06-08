#include "Matrix.h"
#include "Quaternion.h"

#include <string>

#define MATH_PIOVER2                1.57079632679489661923f
#define MATH_EPSILON                0.000001f

Matrix4X4::Matrix4X4()
{
	*this = ZERO;
}

Matrix4X4::Matrix4X4( float a00, float a01, float a02, float a03,
					 float a10, float a11, float a12, float a13,
					 float a20, float a21, float a22, float a23,
					 float a30, float a31, float a32, float a33 )
{
	m[0] = a00; m[4] = a01; m[8] = a02; m[12] = a03;
	m[1] = a10; m[5] = a11; m[9] = a12; m[13] = a13;
	m[2] = a20; m[6] = a21; m[10] = a22; m[14] = a23;
	m[3] = a30; m[7] = a31; m[11] = a32; m[15] = a33;
}

void Matrix4X4::Identity()
{
	*this = IDENTITY;
}

Matrix4X4 Matrix4X4::Inverse()
{
	float a0 = m[0] * m[5] - m[1] * m[4];
	float a1 = m[0] * m[6] - m[2] * m[4];
	float a2 = m[0] * m[7] - m[3] * m[4];
	float a3 = m[1] * m[6] - m[2] * m[5];
	float a4 = m[1] * m[7] - m[3] * m[5];
	float a5 = m[2] * m[7] - m[3] * m[6];
	float b0 = m[8] * m[13] - m[9] * m[12];
	float b1 = m[8] * m[14] - m[10] * m[12];
	float b2 = m[8] * m[15] - m[11] * m[12];
	float b3 = m[9] * m[14] - m[10] * m[13];
	float b4 = m[9] * m[15] - m[11] * m[13];
	float b5 = m[10] * m[15] - m[11] * m[14];

	// Calculate the determinant.
	float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

	// Close to zero, can't invert.
	if (fabs(det) <= MATH_TOLERANCE)
		return ZERO;

	// Support the case where m == dst.
	Matrix4X4 inverse;
	inverse.m[0]  = m[5] * b5 - m[6] * b4 + m[7] * b3;
	inverse.m[1]  = -m[1] * b5 + m[2] * b4 - m[3] * b3;
	inverse.m[2]  = m[13] * a5 - m[14] * a4 + m[15] * a3;
	inverse.m[3]  = -m[9] * a5 + m[10] * a4 - m[11] * a3;

	inverse.m[4]  = -m[4] * b5 + m[6] * b2 - m[7] * b1;
	inverse.m[5]  = m[0] * b5 - m[2] * b2 + m[3] * b1;
	inverse.m[6]  = -m[12] * a5 + m[14] * a2 - m[15] * a1;
	inverse.m[7]  = m[8] * a5 - m[10] * a2 + m[11] * a1;

	inverse.m[8]  = m[4] * b4 - m[5] * b2 + m[7] * b0;
	inverse.m[9]  = -m[0] * b4 + m[1] * b2 - m[3] * b0;
	inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
	inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

	inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
	inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
	inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
	inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

	inverse = inverse * ( 1.0f / det );

	return inverse;
}

Matrix4X4 Matrix4X4::Transpose()
{
	return Matrix4X4( m[0], m[1], m[2], m[3],
						m[4], m[5], m[6], m[7],
						m[8], m[9], m[10], m[11],
						m[12], m[13], m[14], m[15]);
}

Matrix4X4 Matrix4X4::operator*( const Matrix4X4& rh ) const
{
	Matrix4X4 ret;

	ret.m[0]  = this->m[0] * rh.m[0]  + this->m[4] * rh.m[1] + this->m[8]   * rh.m[2]  + this->m[12] * rh.m[3];
	ret.m[1]  = this->m[1] * rh.m[0]  + this->m[5] * rh.m[1] + this->m[9]   * rh.m[2]  + this->m[13] * rh.m[3];
	ret.m[2]  = this->m[2] * rh.m[0]  + this->m[6] * rh.m[1] + this->m[10]  * rh.m[2]  + this->m[14] * rh.m[3];
	ret.m[3]  = this->m[3] * rh.m[0]  + this->m[7] * rh.m[1] + this->m[11]  * rh.m[2]  + this->m[15] * rh.m[3];

	ret.m[4]  = this->m[0] * rh.m[4]  + this->m[4] * rh.m[5] + this->m[8]   * rh.m[6]  + this->m[12] * rh.m[7];
	ret.m[5]  = this->m[1] * rh.m[4]  + this->m[5] * rh.m[5] + this->m[9]   * rh.m[6]  + this->m[13] * rh.m[7];
	ret.m[6]  = this->m[2] * rh.m[4]  + this->m[6] * rh.m[5] + this->m[10]  * rh.m[6]  + this->m[14] * rh.m[7];
	ret.m[7]  = this->m[3] * rh.m[4]  + this->m[7] * rh.m[5] + this->m[11]  * rh.m[6]  + this->m[15] * rh.m[7];

	ret.m[8]  = this->m[0] * rh.m[8]  + this->m[4] * rh.m[9] + this->m[8]   * rh.m[10] + this->m[12] * rh.m[11];
	ret.m[9]  = this->m[1] * rh.m[8]  + this->m[5] * rh.m[9] + this->m[9]   * rh.m[10] + this->m[13] * rh.m[11];
	ret.m[10] = this->m[2] * rh.m[8]  + this->m[6] * rh.m[9] + this->m[10]  * rh.m[10] + this->m[14] * rh.m[11];
	ret.m[11] = this->m[3] * rh.m[8]  + this->m[7] * rh.m[9] + this->m[11]  * rh.m[10] + this->m[15] * rh.m[11];

	ret.m[12] = this->m[0] * rh.m[12] + this->m[4] * rh.m[13] + this->m[8]  * rh.m[14] + this->m[12] * rh.m[15];
	ret.m[13] = this->m[1] * rh.m[12] + this->m[5] * rh.m[13] + this->m[9]  * rh.m[14] + this->m[13] * rh.m[15];
	ret.m[14] = this->m[2] * rh.m[12] + this->m[6] * rh.m[13] + this->m[10] * rh.m[14] + this->m[14] * rh.m[15];
	ret.m[15] = this->m[3] * rh.m[12] + this->m[7] * rh.m[13] + this->m[11] * rh.m[14] + this->m[15] * rh.m[15];

	return ret;
}

Matrix4X4 Matrix4X4::operator*( float scalar )
{
	Matrix4X4 ret;

	ret.m[0]  = m[0]  * scalar;
	ret.m[1]  = m[1]  * scalar;
	ret.m[2]  = m[2]  * scalar;
	ret.m[3]  = m[3]  * scalar;
	ret.m[4]  = m[4]  * scalar;
	ret.m[5]  = m[5]  * scalar;
	ret.m[6]  = m[6]  * scalar;
	ret.m[7]  = m[7]  * scalar;
	ret.m[8]  = m[8]  * scalar;
	ret.m[9]  = m[9]  * scalar;
	ret.m[10] = m[10] * scalar;
	ret.m[11] = m[11] * scalar;
	ret.m[12] = m[12] * scalar;
	ret.m[13] = m[13] * scalar;
	ret.m[14] = m[14] * scalar;
	ret.m[15] = m[15] * scalar;

	return ret;
}

Vec4 Matrix4X4::operator*( const Vec4& vec ) const
{
	Vec4 ret;
	ret.x = m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12] * vec.w;
	ret.y = m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13] * vec.w;
	ret.z = m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14] * vec.w;
	ret.w = vec.w;

	return ret;
}

Vec3 Matrix4X4::operator*( const Vec3& vec ) const
{
	Vec3 ret;
	ret.x = m[0] * vec.x + m[4] * vec.y + m[8] * vec.z;
	ret.y = m[1] * vec.x + m[5] * vec.y + m[9] * vec.z;
	ret.z = m[2] * vec.x + m[6] * vec.y + m[10] * vec.z;

	return ret;
}

Matrix4X4 Matrix4X4::operator+( const Matrix4X4& rh )
{
	Matrix4X4 r;
	for (int i = 0; i < 16; ++i)
	{
		r.m[i] = this->m[i] + rh.m[i];
	}

	return r;
}

Matrix4X4 Matrix4X4::CreateFromTranslation( float x, float y, float z )
{
	Matrix4X4 ret = IDENTITY;
	ret.m[12] = x;
	ret.m[13] = y;
	ret.m[14] = z;
	return ret;
}

Matrix4X4 Matrix4X4::CreateFromRotation( const Quaternion& q )
{
	Matrix4X4 dst;

	float x2 = q.x + q.x;
	float y2 = q.y + q.y;
	float z2 = q.z + q.z;

	float xx2 = q.x * x2;
	float yy2 = q.y * y2;
	float zz2 = q.z * z2;
	float xy2 = q.x * y2;
	float xz2 = q.x * z2;
	float yz2 = q.y * z2;
	float wx2 = q.w * x2;
	float wy2 = q.w * y2;
	float wz2 = q.w * z2;

	dst.m[0] = 1.0f - yy2 - zz2;
	dst.m[1] = xy2 + wz2;
	dst.m[2] = xz2 - wy2;
	dst.m[3] = 0.0f;

	dst.m[4] = xy2 - wz2;
	dst.m[5] = 1.0f - xx2 - zz2;
	dst.m[6] = yz2 + wx2;
	dst.m[7] = 0.0f;

	dst.m[8] = xz2 + wy2;
	dst.m[9] = yz2 - wx2;
	dst.m[10] = 1.0f - xx2 - yy2;
	dst.m[11] = 0.0f;

	dst.m[12] = 0.0f;
	dst.m[13] = 0.0f;
	dst.m[14] = 0.0f;
	dst.m[15] = 1.0f;

	return dst;
}

Matrix4X4 Matrix4X4::CreateFromRotation( float degX, float degY, float degZ )
{
	return CreateFromRotationX(degX) * CreateFromRotationY(degY) * CreateFromRotationZ(degZ);
}

Matrix4X4 Matrix4X4::CreateFromScale( float x, float y, float z )
{
	Matrix4X4 ret = IDENTITY;
	ret.m[0] *= x;
	ret.m[5] *= y;
	ret.m[10] *= z;
	return ret;
}

Matrix4X4 Matrix4X4::CreateFromRotationX( float deg )
{
	Matrix4X4 ret = IDENTITY;

	float angle = DEGREES_TO_RADIANS(deg);

	float c = cos(angle);
	float s = sin(angle);

	ret.m[5]  = c;
	ret.m[6]  = s;
	ret.m[9]  = -s;
	ret.m[10] = c;

	return ret;
}

Matrix4X4 Matrix4X4::CreateFromRotationY( float deg )
{
	Matrix4X4 ret = IDENTITY;

	float angle = DEGREES_TO_RADIANS(deg);

	float c = cos(angle);
	float s = sin(angle);

	ret.m[0]  = c;
	ret.m[2]  = -s;
	ret.m[8]  = s;
	ret.m[10] = c;

	return ret;
}

Matrix4X4 Matrix4X4::CreateFromRotationZ( float deg )
{
	Matrix4X4 ret = IDENTITY;

	float angle = DEGREES_TO_RADIANS(deg);

	float c = cos(angle);
	float s = sin(angle);

	ret.m[0]  = c;
	ret.m[1]  = s;
	ret.m[4]  = -s;
	ret.m[5] = c;

	return ret;
}

Matrix4X4 Matrix4X4::createPerspective( float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane )
{
	Matrix4X4 ret = ZERO;

	float f_n = 1.0f / (zFarPlane - zNearPlane);
	float theta = DEGREES_TO_RADIANS(fieldOfView) * 0.5f;
	float divisor = tan(theta);
	float factor = 1.0f / divisor;

	ret.m[0] = (1.0f / aspectRatio) * factor;
	ret.m[5] = factor;
	ret.m[10] = (-(zFarPlane + zNearPlane)) * f_n;
	ret.m[11] = -1.0f;
	ret.m[14] = -2.0f * zFarPlane * zNearPlane * f_n;

	return ret;
}

Matrix4X4 Matrix4X4::createOrthographic( float width, float height, float zNearPlane, float zFarPlane )
{
	Matrix4X4 dst;
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	createOrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNearPlane, zFarPlane, &dst);

	return dst;
}

void Matrix4X4::createOrthographicOffCenter( float left, float right, float bottom, float top, float zNearPlane, float zFarPlane, Matrix4X4* dst )
{
	memset(dst, 0,  sizeof(float) * 16);
	dst->m[0] = 2 / (right - left);
	dst->m[5] = 2 / (top - bottom);
	dst->m[10] = 2 / (zNearPlane - zFarPlane);

	dst->m[12] = (left + right) / (left - right);
	dst->m[13] = (top + bottom) / (bottom - top);
	dst->m[14] = (zNearPlane + zFarPlane) / (zNearPlane - zFarPlane);
	dst->m[15] = 1;
}

Matrix4X4 Matrix4X4::createLookAt( const Vec3& eyePos, const Vec3& lookAtDir, Vec3 up )
{
	Matrix4X4 ret = ZERO;

	Vec3 forwardDir(-lookAtDir.x, -lookAtDir.y, -lookAtDir.z);
	forwardDir.normalize();
	
	Vec3 rightDir = up.Cross(forwardDir);
	rightDir.normalize();

	up = forwardDir.Cross(rightDir);
	up.normalize();

	ret.m[0] = rightDir.x;
	ret.m[1] = rightDir.y;
	ret.m[2] = rightDir.z;
	ret.m[3] = 0;

	ret.m[4] = up.x;
	ret.m[5] = up.y;
	ret.m[6] = up.z;
	ret.m[7] = 0;

	ret.m[8] = forwardDir.x;
	ret.m[9] = forwardDir.y;
	ret.m[10] = forwardDir.z;
	ret.m[11] = 0;

	ret.m[12] = eyePos.x;
	ret.m[13] = eyePos.y;
	ret.m[14] = eyePos.z;
	ret.m[15] = 1.0f;

	ret = ret.Inverse();

	return ret;
}

void Matrix4X4::SetRight( float a00, float a10, float a20 )
{
	m[0] = a00;
	m[1] = a10;
	m[2] = a20;
}

void Matrix4X4::SetUp( float a01, float a11, float a21 )
{
	m[4] = a01;
	m[5] = a11;
	m[6] = a21;
}

void Matrix4X4::SetForward( float a02, float a12, float a22 )
{
	m[8] = a02;
	m[9] = a12;
	m[10] = a22;
}

Vec3 Matrix4X4::GetUp()
{
	return Vec3( m[4], m[5], m[6] );
}

Vec3 Matrix4X4::GetRight()
{
	return Vec3( m[0], m[1], m[2] );
}

Vec3 Matrix4X4::GetForward()
{
	return Vec3( m[8], m[9], m[10] );
}

Vec3 Matrix4X4::TransformPoint( const Vec3& point ) const
{
	Vec4 pp(point.x, point.y, point.z, 1.0f);
	pp = *this * pp;
	return Vec3(pp.x, pp.y, pp.z);
}

Vec3 Matrix4X4::TransformVector( const Vec3& vec ) const
{
	return *this * vec;
}

bool Matrix4X4::operator==( const Matrix4X4& rh )
{
	return !memcmp(this->m, rh.m, sizeof(this->m));
}

bool Matrix4X4::operator!=( const Matrix4X4& rh )
{
	return !this->operator==(rh);
}

Matrix4X4 Matrix4X4::ZERO= Matrix4X4(0, 0, 0, 0, 
									 0, 0, 0, 0, 
									 0, 0, 0, 0, 
									 0, 0, 0, 0);

Matrix4X4 Matrix4X4::IDENTITY = Matrix4X4( 1, 0, 0, 0, 
										  0, 1, 0, 0, 
										  0, 0, 1, 0, 
										  0, 0, 0, 1);

