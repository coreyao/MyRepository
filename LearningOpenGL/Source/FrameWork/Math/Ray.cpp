#include "Ray.h"

CRay::CRay( const Vec3& rOrigin, const Vec3& rDir )
	: m_origin(rOrigin)
	, m_direction(rDir)
{
	m_direction.normalize();
}

void CRay::Transform( const Mat4& mat )
{
	m_origin = mat.TransformPoint(m_origin);
	m_direction = mat.TransformVector(m_direction);
	m_direction.normalize();
}

