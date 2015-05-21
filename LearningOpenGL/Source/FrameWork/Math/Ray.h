#pragma once

#include "../Utility.h"

class CRay
{
public:
	CRay(const Vec3& rOrigin, const Vec3& rDir);

	void Transform(const Mat4& mat);

	Vec3 m_origin;
	Vec3 m_direction;
};