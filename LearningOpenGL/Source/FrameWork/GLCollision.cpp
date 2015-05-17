#include "GLCollision.h"


CAABB::CAABB()
{
}

CAABB::CAABB( const Vec3& minPoint, const Vec3& maxPoint )
{
	SetData(minPoint, maxPoint);
}

void CAABB::SetData( const Vec3& minPoint, const Vec3& maxPoint )
{
	m_minPoint = minPoint;
	m_maxPoint = maxPoint;
}

bool CAABB::IsIntersect( const CAABB& rh )
{
	return ContainsPoint(rh.m_minPoint) || ContainsPoint(rh.m_maxPoint);
}

bool CAABB::ContainsPoint( const Vec3& point )
{
	if ( point.x >= m_minPoint.x && point.x <= m_maxPoint.x 
		&& point.y >= m_minPoint.y && point.y <= m_maxPoint.y
		&& point.z >= m_minPoint.z && point.z <= m_maxPoint.z )
	{
		return true;
	}

	return false;
}

Vec3 CAABB::GetCenter()
{
	float x = m_minPoint.x + (m_maxPoint.x - m_minPoint.x) / 2;
	float y = m_minPoint.y + (m_maxPoint.y - m_minPoint.y) / 2;
	float z = m_minPoint.z + (m_maxPoint.z - m_minPoint.z) / 2;

	return Vec3( x, y, z );
}
