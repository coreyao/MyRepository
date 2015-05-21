#include "Collision.h"


CAABB::CAABB()
{
	Reset();
}

CAABB::CAABB( const Vec3& minPoint, const Vec3& maxPoint )
{
	Init(minPoint, maxPoint);
}

void CAABB::Init( const Vec3& minPoint, const Vec3& maxPoint )
{
	m_minPoint = minPoint;
	m_maxPoint = maxPoint;
}

void CAABB::Init( const std::vector<Vec3>& vData )
{
	for (int i = 0; i < vData.size(); ++i)
	{
		if ( vData[i].x < m_minPoint.x)
			m_minPoint.x = vData[i].x;

		if ( vData[i].y < m_minPoint.y)
			m_minPoint.y = vData[i].y;

		if ( vData[i].z < m_minPoint.z)
			m_minPoint.z = vData[i].z;

		if ( vData[i].x > m_maxPoint.x)
			m_maxPoint.x = vData[i].x;

		if ( vData[i].y > m_maxPoint.y)
			m_maxPoint.y = vData[i].y;

		if ( vData[i].z > m_maxPoint.z)
			m_maxPoint.z = vData[i].z;
	}
}

void CAABB::Merge( const CAABB& rh )
{
	if ( rh.m_minPoint.x < m_minPoint.x)
		m_minPoint.x = rh.m_minPoint.x;

	if ( rh.m_minPoint.y < m_minPoint.y)
		m_minPoint.y = rh.m_minPoint.y;

	if ( rh.m_minPoint.z < m_minPoint.z)
		m_minPoint.z = rh.m_minPoint.z;

	if ( rh.m_maxPoint.x > m_maxPoint.x)
		m_maxPoint.x = rh.m_maxPoint.x;

	if ( rh.m_maxPoint.y > m_maxPoint.y)
		m_maxPoint.y = rh.m_maxPoint.y;

	if ( rh.m_maxPoint.z > m_maxPoint.z)
		m_maxPoint.z = rh.m_maxPoint.z;
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

Vec3 CAABB::GetMaxPoint() const
{
	return m_maxPoint;
}

Vec3 CAABB::GetMinPoint() const
{
	return m_minPoint;
}

CAABB CAABB::Transform( const Mat4& mat ) const
{
	CAABB ret;
	ret.m_minPoint = mat.TransformPoint(m_minPoint);
	ret.m_maxPoint = mat.TransformPoint(m_maxPoint);
	return ret;
}

void CAABB::Reset()
{
	m_minPoint.set(99999.0f, 99999.0f, 99999.0f);
	m_maxPoint.set(-99999.0f, -99999.0f, -99999.0f);
}
