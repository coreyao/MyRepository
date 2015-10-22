#pragma once

#include "Vector.h"
#include "Quaternion.h"

class Mat4
{
public:
	static Mat4 CreateTranslationMat(float x, float y, float z);
	static Mat4 CreateRotationMat(float x, float y, float z);
	static Mat4 CreateRotationMat(const Quaternion& quat);
	static Mat4 CreateScaleMat(float x, float y, float z);
	static Mat4 CreatePerspectiveMat(float fVerticleFov, float whRatio, float n, float f);
	static Mat4 CreateOrthegraphicsMat(float l, float r, float t, float b, float n, float f);
	static Mat4 CreateLookAt(const Vec3& eyePos, const Vec3& lookDir, Vec3 upVec = Vec3(0, 1, 0));

	Mat4(const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis, const Vec3& origin);

	Mat4 operator*(const Mat4& rh) const;
	void operator*=(const Mat4& rh);

	Mat4 operator*(float fScalar);
	void operator*=(float fScalar);

	Vec4 operator*(const Vec4& rh) const;

	Mat4 operator+(const Mat4& rh);
	void operator+=(const Mat4& rh);

	bool operator==(const Mat4& rh);
	bool operator!=(const Mat4& rh);

	Mat4 GetTransposed();
	void Transpose();

	Mat4 GetInversed();
	void Inverse();

	float Determinant();

	Vec3 ConvertToEuler() const;

	Vec3 GetForward() const;
	Vec3 GetRight() const;
	Vec3 GetUp() const;

	Mat4();

	float m[16];

	static Mat4 IDENTITY;
	static Mat4 ZERO;
};