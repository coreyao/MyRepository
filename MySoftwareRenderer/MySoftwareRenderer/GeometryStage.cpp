#include "GeometryStage.h"
#include "Director.h"

bool GeometryStage::TransformCameraToScreen(SFaceRuntime& face)
{
	bool bInFrustrum1 = TransformCameraToScreen(face.m_vertex1, face.m_bUseNormalizedPos);
	bool bInFrustrum2 = TransformCameraToScreen(face.m_vertex2, face.m_bUseNormalizedPos);
	bool bInFrustrum3 = TransformCameraToScreen(face.m_vertex3, face.m_bUseNormalizedPos);
	if (!bInFrustrum1 && !bInFrustrum2 && !bInFrustrum3)
		return false;

	return true;
}

bool GeometryStage::TransformCameraToScreen(SVertexRuntime& vertex, bool bUseNormalizedPos)
{
	const Mat4& rProjMat = CDirector::GetInstance()->GetCurProjectionMat();
	Vec4 clippingPos;
	if (!bUseNormalizedPos)
		clippingPos = rProjMat * Vec4(vertex.m_pos.x, vertex.m_pos.y, vertex.m_pos.z, 1.0f);
	else
		clippingPos = Vec4(vertex.m_normalizePos.x, vertex.m_normalizePos.y, vertex.m_normalizePos.z, 1.0f);

	float rhw = 1.0f / clippingPos.w;
	clippingPos.x *= rhw;
	clippingPos.y *= rhw;
	clippingPos.z *= rhw;

	vertex.m_pos.x = (clippingPos.x * 0.5f + 0.5f) * (SCREEN_WIDTH)-0.5f;
	vertex.m_pos.y = (-clippingPos.y * 0.5f + 0.5f) * (SCREEN_HEIGHT)-0.5f;
	vertex.m_pos.z = clippingPos.z;
	vertex.m_inverseZ = rhw;
	vertex.m_UV.x *= vertex.m_inverseZ;
	vertex.m_UV.y *= vertex.m_inverseZ;
	vertex.m_color *= vertex.m_inverseZ;

	if (!IsVertexInFrustrum(clippingPos))
		return false;

	return true;
}

void GeometryStage::TransformWorldToCamera(SFaceRuntime& face)
{
	const Mat4& rViewMat = CDirector::GetInstance()->GetCurViewMat();

	Vec4 cameraPos = rViewMat * Vec4(face.m_vertex1.m_pos.x, face.m_vertex1.m_pos.y, face.m_vertex1.m_pos.z, 1.0f);
	face.m_vertex1.m_pos = Vec3(cameraPos.x, cameraPos.y, cameraPos.z);

	cameraPos = rViewMat * Vec4(face.m_vertex2.m_pos.x, face.m_vertex2.m_pos.y, face.m_vertex2.m_pos.z, 1.0f);
	face.m_vertex2.m_pos = Vec3(cameraPos.x, cameraPos.y, cameraPos.z);

	cameraPos = rViewMat * Vec4(face.m_vertex3.m_pos.x, face.m_vertex3.m_pos.y, face.m_vertex3.m_pos.z, 1.0f);
	face.m_vertex3.m_pos = Vec3(cameraPos.x, cameraPos.y, cameraPos.z);
}

bool GeometryStage::NearPlaneCulling(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace)
{
	float fNear = -CDirector::GetInstance()->GetPerspectiveCamera()->GetNearZ();

	int iCount = 0;
	if (face.m_vertex1.m_pos.z > fNear)
		++iCount;
	if (face.m_vertex2.m_pos.z > fNear)
		++iCount;
	if (face.m_vertex3.m_pos.z > fNear)
		++iCount;

	if ( iCount == 0 )
	{
		return false;
	}
	else if (iCount < 3)
	{
		if (face.m_vertex1.m_pos.z > face.m_vertex2.m_pos.z)
			Helper::Swap(face.m_vertex1, face.m_vertex2);
		if (face.m_vertex1.m_pos.z > face.m_vertex3.m_pos.z)
			Helper::Swap(face.m_vertex1, face.m_vertex3);
		if (face.m_vertex2.m_pos.z > face.m_vertex3.m_pos.z)
			Helper::Swap(face.m_vertex2, face.m_vertex3);

		if (iCount == 1)
		{
			SVertexRuntime newVertex1;
			float t = (fNear - face.m_vertex1.m_pos.z) / (face.m_vertex3.m_pos.z - face.m_vertex1.m_pos.z);
			newVertex1.m_pos.x = face.m_vertex1.m_pos.x + t * (face.m_vertex3.m_pos.x - face.m_vertex1.m_pos.x);
			newVertex1.m_pos.y = face.m_vertex1.m_pos.y + t * (face.m_vertex3.m_pos.y - face.m_vertex1.m_pos.y);
			newVertex1.m_pos.z = fNear;
			newVertex1.m_UV = face.m_vertex1.m_UV + (face.m_vertex3.m_UV - face.m_vertex1.m_UV) * t;
			newVertex1.m_color = face.m_vertex1.m_color + (face.m_vertex3.m_color - face.m_vertex1.m_color) * t;

			SVertexRuntime newVertex2;
			t = (fNear - face.m_vertex2.m_pos.z) / (face.m_vertex3.m_pos.z - face.m_vertex2.m_pos.z);
			newVertex2.m_pos.x = face.m_vertex2.m_pos.x + t * (face.m_vertex3.m_pos.x - face.m_vertex2.m_pos.x);
			newVertex2.m_pos.y = face.m_vertex2.m_pos.y + t * (face.m_vertex3.m_pos.y - face.m_vertex2.m_pos.y);
			newVertex2.m_pos.z = fNear;
			newVertex2.m_UV = face.m_vertex2.m_UV + (face.m_vertex3.m_UV - face.m_vertex2.m_UV) * t;
			newVertex2.m_color = face.m_vertex2.m_color + (face.m_vertex3.m_color - face.m_vertex2.m_color) * t;

			bAddFace = true;
			addFace.m_vertex1 = face.m_vertex1;
			addFace.m_vertex2 = face.m_vertex2;
			addFace.m_vertex3 = newVertex2;
			addFace.m_fAlpha = face.m_fAlpha;

			face.m_vertex2 = newVertex1;
			face.m_vertex3 = newVertex2;
		}
		else if (iCount == 2)
		{
			float t = (fNear - face.m_vertex1.m_pos.z) / (face.m_vertex2.m_pos.z - face.m_vertex1.m_pos.z);
			face.m_vertex2.m_pos.x = face.m_vertex1.m_pos.x + (face.m_vertex2.m_pos.x - face.m_vertex1.m_pos.x) * t;
			face.m_vertex2.m_pos.y = face.m_vertex1.m_pos.y + (face.m_vertex2.m_pos.y - face.m_vertex1.m_pos.y) * t;
			face.m_vertex2.m_pos.z = fNear;
			face.m_vertex2.m_UV = face.m_vertex1.m_UV + (face.m_vertex2.m_UV - face.m_vertex1.m_UV) * t;
			face.m_vertex2.m_color = face.m_vertex1.m_color + (face.m_vertex2.m_color - face.m_vertex1.m_color) * t;

			t = (fNear - face.m_vertex1.m_pos.z) / (face.m_vertex3.m_pos.z - face.m_vertex1.m_pos.z);
			face.m_vertex3.m_pos.x = face.m_vertex1.m_pos.x + t * (face.m_vertex3.m_pos.x - face.m_vertex1.m_pos.x);
			face.m_vertex3.m_pos.y = face.m_vertex1.m_pos.y + t * (face.m_vertex3.m_pos.y - face.m_vertex1.m_pos.y);
			face.m_vertex3.m_pos.z = fNear;
			face.m_vertex3.m_UV = face.m_vertex1.m_UV + (face.m_vertex3.m_UV - face.m_vertex1.m_UV) * t;
			face.m_vertex3.m_color = face.m_vertex1.m_color + (face.m_vertex3.m_color - face.m_vertex1.m_color) * t;
		}

		return false;
	}
	else
	{
		return true;
	}
}

bool GeometryStage::IsVertexInFrustrum(const Vec4& pos)
{
	//float fFOV = DEG_TO_RAD(CDirector::GetInstance()->GetPerspectiveCamera()->GetFOV() / 2);
	//float fAspectRatio = CDirector::GetInstance()->GetPerspectiveCamera()->GetAspectRatio();
	//float fNear = -CDirector::GetInstance()->GetPerspectiveCamera()->GetNearZ();
	//float fFar = -CDirector::GetInstance()->GetPerspectiveCamera()->GetFarZ();
	
	//// - check top & bottom plane
	//if (tan(fFOV) * -vertex.m_pos.z < fabs(vertex.m_pos.y))
	//	return false;

	//// - check left & right plane
	//if (fAspectRatio * tan(fFOV) * -vertex.m_pos.z < fabs(vertex.m_pos.x))
	//	return false;

	//// - check near & far plane
	//if (vertex.m_pos.z > fNear || vertex.m_pos.z < fFar)
	//	return false;
	//
	//return true;

	if (   pos.x <= -pos.w || pos.x >= pos.w
		|| pos.y <= -pos.w || pos.y >= pos.w
		|| pos.z <= -pos.w || pos.z >= pos.w )
	{
		return false;
	}

	return true;
}

