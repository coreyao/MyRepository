#include "Plane.h"

void CPlane::Init( const Vec3& n, float dist )
{
	float len = n.length();
	m_normal = n / len;
	m_dist = dist / len;
}

CPlane::EPlaneSide CPlane::GetSide( const Vec3& rPoint ) const
{
	float ret = GetDistToPlane(rPoint);
	if ( ret > 0 )
		return EPlaneSide_Front;
	else if ( ret == 0 )
		return EPlaneSide_On;
	else
		return EPlaneSide_Back;
}

float CPlane::GetDistToPlane( const Vec3& rPoint ) const
{
	return m_normal.Dot(rPoint) - m_dist;
}

Vec3 CPlane::GetNormal() const
{
	return m_normal;
}

