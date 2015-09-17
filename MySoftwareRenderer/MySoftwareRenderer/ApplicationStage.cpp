#include "ApplicationStage.h"
#include "Director.h"

void ApplicationStage::TransformLocalToWorld(SFaceRuntime& face, const Mat4& worldTransform)
{
	Vec4 worldPos = worldTransform * Vec4(face.m_vertex1.m_pos.x, face.m_vertex1.m_pos.y, face.m_vertex1.m_pos.z, 1.0f);
	face.m_vertex1.m_pos = Vec3(worldPos.x, worldPos.y, worldPos.z);

	worldPos = worldTransform * Vec4(face.m_vertex2.m_pos.x, face.m_vertex2.m_pos.y, face.m_vertex2.m_pos.z, 1.0f);
	face.m_vertex2.m_pos = Vec3(worldPos.x, worldPos.y, worldPos.z);

	worldPos = worldTransform * Vec4(face.m_vertex3.m_pos.x, face.m_vertex3.m_pos.y, face.m_vertex3.m_pos.z, 1.0f);
	face.m_vertex3.m_pos = Vec3(worldPos.x, worldPos.y, worldPos.z);
}

bool ApplicationStage::IsBackFace(SFaceRuntime& face, EVertexOrder eOrder /*= EVertexOrder_ClockWise*/)
{
	Vec3 eyePos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
	Vec3 normal;
	if ( eOrder == EVertexOrder_ClockWise )
	{
		Vec3 p1 = face.m_vertex1.m_pos - face.m_vertex2.m_pos;
		Vec3 p2 = face.m_vertex3.m_pos - face.m_vertex2.m_pos;
		normal = p1.Cross(p2);
	}
	else if ( eOrder == EVertexOrder_Counter_ClockWise )
	{
		Vec3 p1 = face.m_vertex1.m_pos - face.m_vertex3.m_pos;
		Vec3 p2 = face.m_vertex2.m_pos - face.m_vertex3.m_pos;
		normal = p1.Cross(p2);
	}

	Vec3 dir = eyePos - face.m_vertex1.m_pos;
	return dir.Dot(normal) < 0;
}

