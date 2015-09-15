#include "ApplicationStage.h"
#include "Director.h"

bool ApplicationStage::IsBackFace(const SVertex& v1, const SVertex& v2, const SVertex& v3, EVertexOrder eOrder /*= EVertexOrder_ClockWise*/)
{
	Vec3 eyePos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
	Vec3 normal;
	if ( eOrder == EVertexOrder_ClockWise )
	{
		Vec3 p1 = v1.m_pos - v2.m_pos;
		Vec3 p2 = v3.m_pos - v2.m_pos;
		normal = p1.Cross(p2);
	}
	else if ( eOrder == EVertexOrder_Counter_ClockWise )
	{
		Vec3 p1 = v1.m_pos - v3.m_pos;
		Vec3 p2 = v2.m_pos - v3.m_pos;
		normal = p1.Cross(p2);
	}

	Vec3 dir = eyePos - v1.m_pos;
	return dir.Dot(normal) < 0;
}

