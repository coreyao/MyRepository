#include "ApplicationStage.h"
#include "Director.h"

void ApplicationStage::TransformLocalToWorld(SFaceRuntime& face, const Mat4& worldTransform)
{
	face.m_vertex1.m_pos = worldTransform * face.m_vertex1.m_pos;
	face.m_vertex2.m_pos = worldTransform * face.m_vertex2.m_pos;
	face.m_vertex3.m_pos = worldTransform * face.m_vertex3.m_pos;
}

//bool ApplicationStage::IsBackFace(SFaceRuntime& face, EVertexOrder eOrder /*= EVertexOrder_ClockWise*/)
//{
//	Vec3 eyePos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
//	Vec3 normal;
//	if (eOrder == EVertexOrder_ClockWise)
//	{
//		Vec3 p1 = Vec3(face.m_vertex1.m_pos) - Vec3(face.m_vertex2.m_pos);
//		Vec3 p2 = Vec3(face.m_vertex3.m_pos) - Vec3(face.m_vertex2.m_pos);
//		normal = p1.Cross(p2);
//	}
//	else if (eOrder == EVertexOrder_Counter_ClockWise)
//	{
//		Vec3 p1 = Vec3(face.m_vertex1.m_pos) - Vec3(face.m_vertex3.m_pos);
//		Vec3 p2 = Vec3(face.m_vertex2.m_pos) - Vec3(face.m_vertex3.m_pos);
//		normal = p1.Cross(p2);
//	}
//
//	Vec3 dir = eyePos - face.m_vertex1.m_pos;
//	return dir.Dot(normal) < 0;
//}
