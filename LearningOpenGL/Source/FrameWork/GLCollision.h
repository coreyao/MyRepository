#pragma once

#include "Utility.h"

class CAABB
{
public:
	CAABB();
	CAABB(const Vec3& minPoint, const Vec3& maxPoint);

	void Init(const std::vector<Vec3>& vData);
	void Init(const Vec3& minPoint, const Vec3& maxPoint);
	void Merge(const CAABB& rh);
	Vec3 GetCenter();
	bool ContainsPoint(const Vec3& point);
	bool IsIntersect(const CAABB& rh);
	CAABB Transform( const Mat4& mat ) const;
	void Reset();

	Vec3 GetMaxPoint() const;
	Vec3 GetMinPoint() const;

private:
	Vec3 m_minPoint;
	Vec3 m_maxPoint;
};

class COBB
{

};