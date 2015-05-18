#pragma once

#include "../Utility.h"

class CPlane
{
public:
	enum EPlaneSide
	{
		EPlaneSide_On,
		EPlaneSide_Front,
		EPlaneSide_Back,
	};

	void Init(const Vec3& n, float dist);
	EPlaneSide GetSide( const Vec3& rPoint ) const;
	Vec3 GetNormal() const;
	float GetDistToPlane( const Vec3& rPoint ) const;

private:
	Vec3 m_normal;
	float m_dist;
};