#include "Quaternion.h"
#include <cmath>
#include <cassert>

Quaternion::Quaternion()
	: w(0), x(0), y(0), z(0)
{
}

Quaternion::Quaternion( float xx, float yy, float zz, float ww )
{
	x = xx; y = yy; z = zz; w = ww;
}

Quaternion::Quaternion( const Vec3& axis, float angle )
{
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = sinf(halfAngle);

	Vec3 normal(axis);
	normal.normalize();
	x = normal.x * sinHalfAngle;
	y = normal.y * sinHalfAngle;
	z = normal.z * sinHalfAngle;
	w = cosf(halfAngle);
}

void Quaternion::normalize()
{
	float n = x * x + y * y + z * z + w * w;
	// Already normalized.
	if (n == 1.0f)
		return;

	n = sqrt(n);
	// Too close to zero.
	if (n < 0.000001f)
		return;

	n = 1.0f / n;
	x *= n;
	y *= n;
	z *= n;
	w *= n;
}

void Quaternion::slerp( const Quaternion& q1, const Quaternion& q2, float t, Quaternion* dst )
{
	slerp(q1.x, q1.y, q1.z, q1.w, q2.x, q2.y, q2.z, q2.w, t, &dst->x, &dst->y, &dst->z, &dst->w);
}

void Quaternion::slerp( float q1x, float q1y, float q1z, float q1w, float q2x, float q2y, float q2z, float q2w, float t, float* dstx, float* dsty, float* dstz, float* dstw )
{
	// Fast slerp implementation by kwhatmough:
	// It contains no division operations, no trig, no inverse trig
	// and no sqrt. Not only does this code tolerate small constraint
	// errors in the input quaternions, it actually corrects for them.
	assert(dstx && dsty && dstz && dstw);
	assert(!(t < 0.0f || t > 1.0f));

	if (t == 0.0f)
	{
		*dstx = q1x;
		*dsty = q1y;
		*dstz = q1z;
		*dstw = q1w;
		return;
	}
	else if (t == 1.0f)
	{
		*dstx = q2x;
		*dsty = q2y;
		*dstz = q2z;
		*dstw = q2w;
		return;
	}

	if (q1x == q2x && q1y == q2y && q1z == q2z && q1w == q2w)
	{
		*dstx = q1x;
		*dsty = q1y;
		*dstz = q1z;
		*dstw = q1w;
		return;
	}

	float halfY, alpha, beta;
	float u, f1, f2a, f2b;
	float ratio1, ratio2;
	float halfSecHalfTheta, versHalfTheta;
	float sqNotU, sqU;

	float cosTheta = q1w * q2w + q1x * q2x + q1y * q2y + q1z * q2z;

	// As usual in all slerp implementations, we fold theta.
	alpha = cosTheta >= 0 ? 1.0f : -1.0f;
	halfY = 1.0f + alpha * cosTheta;

	// Here we bisect the interval, so we need to fold t as well.
	f2b = t - 0.5f;
	u = f2b >= 0 ? f2b : -f2b;
	f2a = u - f2b;
	f2b += u;
	u += u;
	f1 = 1.0f - u;

	// One iteration of Newton to get 1-cos(theta / 2) to good accuracy.
	halfSecHalfTheta = 1.09f - (0.476537f - 0.0903321f * halfY) * halfY;
	halfSecHalfTheta *= 1.5f - halfY * halfSecHalfTheta * halfSecHalfTheta;
	versHalfTheta = 1.0f - halfY * halfSecHalfTheta;

	// Evaluate series expansions of the coefficients.
	sqNotU = f1 * f1;
	ratio2 = 0.0000440917108f * versHalfTheta;
	ratio1 = -0.00158730159f + (sqNotU - 16.0f) * ratio2;
	ratio1 = 0.0333333333f + ratio1 * (sqNotU - 9.0f) * versHalfTheta;
	ratio1 = -0.333333333f + ratio1 * (sqNotU - 4.0f) * versHalfTheta;
	ratio1 = 1.0f + ratio1 * (sqNotU - 1.0f) * versHalfTheta;

	sqU = u * u;
	ratio2 = -0.00158730159f + (sqU - 16.0f) * ratio2;
	ratio2 = 0.0333333333f + ratio2 * (sqU - 9.0f) * versHalfTheta;
	ratio2 = -0.333333333f + ratio2 * (sqU - 4.0f) * versHalfTheta;
	ratio2 = 1.0f + ratio2 * (sqU - 1.0f) * versHalfTheta;

	// Perform the bisection and resolve the folding done earlier.
	f1 *= ratio1 * halfSecHalfTheta;
	f2a *= ratio2;
	f2b *= ratio2;
	alpha *= f1 + f2a;
	beta = f1 + f2b;

	// Apply final coefficients to a and b as usual.
	float w = alpha * q1w + beta * q2w;
	float x = alpha * q1x + beta * q2x;
	float y = alpha * q1y + beta * q2y;
	float z = alpha * q1z + beta * q2z;

	// This final adjustment to the quaternion's length corrects for
	// any small constraint error in the inputs q1 and q2 But as you
	// can see, it comes at the cost of 9 additional multiplication
	// operations. If this error-correcting feature is not required,
	// the following code may be removed.
	f1 = 1.5f - 0.5f * (w * w + x * x + y * y + z * z);
	*dstw = w * f1;
	*dstx = x * f1;
	*dsty = y * f1;
	*dstz = z * f1;
}

Quaternion Quaternion::operator*( float fScale )
{
	return Quaternion(this->x * fScale, this->y * fScale, this->z * fScale, this->w * fScale);
}

Quaternion Quaternion::operator+( const Quaternion& rh )
{
	return Quaternion( this->x + rh.x, this->y + rh.y, this->z + rh.z, this->w + rh.w );
}

