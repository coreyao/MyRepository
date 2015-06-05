#pragma once

#include "Vector.h"

class Quaternion
{
public:
    float w;
    float x;
    float y;
    float z;

	Quaternion();
	Quaternion(float xx, float yy, float zz, float ww);
	Quaternion(const Vec3& axis, float angle);
	Quaternion operator*(float fScale);
	Quaternion operator*(const Quaternion& rh);
	Quaternion operator+(const Quaternion& rh);

	void normalize();
	static void slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion* dst);

private:
	static void slerp(float q1x, float q1y, float q1z, float q1w, float q2x, float q2y, float q2z, float q2w, float t, float* dstx, float* dsty, float* dstz, float* dstw);
};