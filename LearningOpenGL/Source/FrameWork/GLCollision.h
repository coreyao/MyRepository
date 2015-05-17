#pragma once

#include "Utility.h"

class CAABB
{
public:
	CAABB();
	CAABB(const Vec3& minPoint, const Vec3& maxPoint);

	void SetData(const Vec3& minPoint, const Vec3& maxPoint);
	bool ContainsPoint(const Vec3& point);
	bool IsIntersect(const CAABB& rh);

private:
	Vec3 m_minPoint;
	Vec3 m_maxPoint;
};