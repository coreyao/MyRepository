#include "Ray.h"

CRay::CRay( const Vec3& rOrigin, const Vec3& rDir )
	: m_origin(rOrigin)
	, m_direction(rDir)
{
	m_direction.Normalize();
}

void CRay::Transform( const Mat4& mat )
{
	m_origin = mat * Vec4(m_origin, 1.0f);
	m_direction = mat * Vec4(m_direction, 0.0f);
	m_direction.Normalize();
}

