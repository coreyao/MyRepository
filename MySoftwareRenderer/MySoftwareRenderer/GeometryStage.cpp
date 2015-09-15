#include "GeometryStage.h"
#include "Director.h"

void GeometryStage::TransformWorldToScreen(SVertex& rVertex)
{
	Vec4 cameraPos = CDirector::GetInstance()->GetCurViewMat() * Vec4(rVertex.m_pos.x, rVertex.m_pos.y, rVertex.m_pos.z, 1);
	Vec4 clippingPos = CDirector::GetInstance()->GetCurProjectionMat() * cameraPos;
	if (clippingPos.w != 0)
	{
		clippingPos.x /= clippingPos.w;
		clippingPos.y /= clippingPos.w;
		clippingPos.z /= clippingPos.w;

		Vec2 screenPos;
		screenPos.x = (clippingPos.x * 0.5f + 0.5f) * SCREEN_WIDTH;
		screenPos.y = SCREEN_HEIGHT - (clippingPos.y * 0.5f + 0.5f) * SCREEN_HEIGHT;

		rVertex.m_pos.x = screenPos.x;
		rVertex.m_pos.y = screenPos.y;
		rVertex.m_pos.z = clippingPos.z;

		rVertex.m_inverseZ = 1.0f / clippingPos.w;

		rVertex.m_UV.x *= rVertex.m_inverseZ;
		rVertex.m_UV.y *= rVertex.m_inverseZ;
	}
}


