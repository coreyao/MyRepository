#include "Frustrum.h"
#include "Camera.h"

bool CFrustrum::IsInFrustrum( const CAABB& worldRect ) const
{
	bool bRet = true;
	for (int i = 0; i < 6; ++i)
	{
		Vec3 point;
		point.x = m_vPlane[i].GetNormal().x < 0.0f ? worldRect.GetMaxPoint().x : worldRect.GetMinPoint().x;
		point.y = m_vPlane[i].GetNormal().y < 0.0f ? worldRect.GetMaxPoint().y : worldRect.GetMinPoint().y;
		point.z = m_vPlane[i].GetNormal().z < 0.0f ? worldRect.GetMaxPoint().z : worldRect.GetMinPoint().z;

		if ( m_vPlane[i].GetSide(point) == CPlane::EPlaneSide_Front )
		{
			bRet = false;
			break;
		}
	}

	return bRet;
}

void CFrustrum::Init( const CCamera& cam )
{
	const Mat4& mat = cam.GetProjMat() * cam.GetViewMat();

	m_vPlane[0].Init(-Vec3(mat.m[3] + mat.m[0], mat.m[7] + mat.m[4], mat.m[11] + mat.m[8]), (mat.m[15] + mat.m[12]));//left
	m_vPlane[1].Init(-Vec3(mat.m[3] - mat.m[0], mat.m[7] - mat.m[4], mat.m[11] - mat.m[8]), (mat.m[15] - mat.m[12]));//right
	m_vPlane[2].Init(-Vec3(mat.m[3] + mat.m[1], mat.m[7] + mat.m[5], mat.m[11] + mat.m[9]), (mat.m[15] + mat.m[13]));//bottom
	m_vPlane[3].Init(-Vec3(mat.m[3] - mat.m[1], mat.m[7] - mat.m[5], mat.m[11] - mat.m[9]), (mat.m[15] - mat.m[13]));//top
	m_vPlane[4].Init(-Vec3(mat.m[3] + mat.m[2], mat.m[7] + mat.m[6], mat.m[11] + mat.m[10]), (mat.m[15] + mat.m[14]));//near
	m_vPlane[5].Init(-Vec3(mat.m[3] - mat.m[2], mat.m[7] - mat.m[6], mat.m[11] - mat.m[10]), (mat.m[15] - mat.m[14]));//far
}

