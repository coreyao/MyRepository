#pragma once

#include "Vector.h"

class Quaternion;
class Matrix4X4
{
public:
	Matrix4X4();
	Matrix4X4(float a00, float a01, float a02, float a03,
		float a10, float a11, float a12, float a13,
		float a20, float a21, float a22, float a23,
		float a30, float a31, float a32, float a33);

	void Identity();
	Matrix4X4 Inverse();
	Matrix4X4 Transpose();

	void SetRight(float a00, float a10, float a20);
	Vec3 GetRight();

	void SetUp(float a01, float a11, float a21);
	Vec3 GetUp();

	void SetForward(float a02, float a12, float a22);
	Vec3 GetForward();

	Matrix4X4 operator*(const Matrix4X4& rh) const;
	Matrix4X4 operator*(float fScalar);
	Matrix4X4 operator+(const Matrix4X4& rh);
	bool operator==(const Matrix4X4& rh);
	bool operator!=(const Matrix4X4& rh);

	Vec4 operator*(const Vec4& vec) const;
	Vec3 operator*(const Vec3& vec) const;

	Vec3 TransformPoint(const Vec3& point) const;
	Vec3 TransformVector(const Vec3& vec) const;

	static Matrix4X4 createPerspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane);
	static Matrix4X4 createOrthographic(float width, float height, float zNearPlane, float zFarPlane);
	static void createOrthographicOffCenter(float left, float right, float bottom, float top,
		float zNearPlane, float zFarPlane, Matrix4X4* dst);
	static Matrix4X4 createLookAt(const Vec3& eyePos, const Vec3& lookAtDir, Vec3 up);
	static Matrix4X4 CreateFromTranslation(float x, float y, float z);
	static Matrix4X4 CreateFromRotation(const Quaternion& quat);
	static Matrix4X4 CreateFromRotation(float degX, float degY, float degZ);
	static Matrix4X4 CreateFromRotationX(float deg);
	static Matrix4X4 CreateFromRotationY(float deg);
	static Matrix4X4 CreateFromRotationZ(float deg);
	static Matrix4X4 CreateFromScale(float x, float y, float z);

	static Matrix4X4 IDENTITY;
	static Matrix4X4 ZERO;

	float m[16];
};