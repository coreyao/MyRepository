#include "GeometryStage.h"
#include "Director.h"

void GeometryStage::TransformWorldToCamera(SFaceRuntime& face)
{
	const Mat4& rViewMat = CDirector::GetInstance()->GetCurViewMat();

	face.m_vertex1.m_pos = rViewMat * face.m_vertex1.m_pos;
	face.m_vertex2.m_pos = rViewMat * face.m_vertex2.m_pos;
	face.m_vertex3.m_pos = rViewMat * face.m_vertex3.m_pos;
}

void GeometryStage::TransformCameraToClip(SFaceRuntime& face)
{
	const Mat4& rProjMat = CDirector::GetInstance()->GetCurProjectionMat();
	if (!face.m_bUseNormalizedPos)
	{
		face.m_vertex1.m_pos = rProjMat * face.m_vertex1.m_pos;
		face.m_vertex2.m_pos = rProjMat * face.m_vertex2.m_pos;
		face.m_vertex3.m_pos = rProjMat * face.m_vertex3.m_pos;
	}
}

bool GeometryStage::DoClipInClipSpace(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace)
{
	if (!IsVertexInCVV(face.m_vertex1.m_pos)
		&& !IsVertexInCVV(face.m_vertex2.m_pos)
		&& !IsVertexInCVV(face.m_vertex3.m_pos))
	{
		bAddFace = false;
		return true;
	}
	else
	{
		float fNear = -CDirector::GetInstance()->GetPerspectiveCamera()->GetNearZ();

		int iCount = 0;
		if (-face.m_vertex1.m_pos.w > fNear)
			++iCount;
		if (-face.m_vertex2.m_pos.w > fNear)
			++iCount;
		if (-face.m_vertex3.m_pos.w > fNear)
			++iCount;

		if (iCount == 0)
		{
			bAddFace = false;
			return false;
		}
		else if (iCount < 3)
		{
			if (face.m_vertex1.m_pos.w < face.m_vertex2.m_pos.w)
				Helper::Swap(face.m_vertex1, face.m_vertex2);
			if (face.m_vertex1.m_pos.w < face.m_vertex3.m_pos.w)
				Helper::Swap(face.m_vertex1, face.m_vertex3);
			if (face.m_vertex2.m_pos.w < face.m_vertex3.m_pos.w)
				Helper::Swap(face.m_vertex2, face.m_vertex3);

			if (iCount == 1)
			{
				SVertexRuntime newVertex1;
				float t = (fNear - (-face.m_vertex1.m_pos.w)) / ((-face.m_vertex3.m_pos.w) - (-face.m_vertex1.m_pos.w));
				newVertex1.m_pos.x = face.m_vertex1.m_pos.x + t * (face.m_vertex3.m_pos.x - face.m_vertex1.m_pos.x);
				newVertex1.m_pos.y = face.m_vertex1.m_pos.y + t * (face.m_vertex3.m_pos.y - face.m_vertex1.m_pos.y);
				newVertex1.m_pos.z = face.m_vertex1.m_pos.z + t * (face.m_vertex3.m_pos.z - face.m_vertex1.m_pos.z);
				newVertex1.m_pos.w = face.m_vertex1.m_pos.w + t * (face.m_vertex3.m_pos.w - face.m_vertex1.m_pos.w);
				newVertex1.m_UV = face.m_vertex1.m_UV + (face.m_vertex3.m_UV - face.m_vertex1.m_UV) * t;
				newVertex1.m_color = face.m_vertex1.m_color + (face.m_vertex3.m_color - face.m_vertex1.m_color) * t;

				SVertexRuntime newVertex2;
				t = (fNear - (-face.m_vertex2.m_pos.w)) / ((-face.m_vertex3.m_pos.w) - (-face.m_vertex2.m_pos.w));
				newVertex2.m_pos.x = face.m_vertex2.m_pos.x + t * (face.m_vertex3.m_pos.x - face.m_vertex2.m_pos.x);
				newVertex2.m_pos.y = face.m_vertex2.m_pos.y + t * (face.m_vertex3.m_pos.y - face.m_vertex2.m_pos.y);
				newVertex2.m_pos.z = face.m_vertex2.m_pos.z + t * (face.m_vertex3.m_pos.z - face.m_vertex2.m_pos.z);
				newVertex2.m_pos.w = face.m_vertex2.m_pos.w + t * (face.m_vertex3.m_pos.w - face.m_vertex2.m_pos.w);
				newVertex2.m_UV = face.m_vertex2.m_UV + (face.m_vertex3.m_UV - face.m_vertex2.m_UV) * t;
				newVertex2.m_color = face.m_vertex2.m_color + (face.m_vertex3.m_color - face.m_vertex2.m_color) * t;

				bAddFace = true;
				addFace.m_vertex1 = face.m_vertex1;
				addFace.m_vertex2 = face.m_vertex2;
				addFace.m_vertex3 = newVertex2;
				addFace.m_fAlpha = face.m_fAlpha;
				addFace.m_pRenderState = face.m_pRenderState;

				face.m_vertex2 = newVertex1;
				face.m_vertex3 = newVertex2;
			}
			else if (iCount == 2)
			{
				float t = (fNear - (-face.m_vertex1.m_pos.w)) / ((-face.m_vertex2.m_pos.w) - (-face.m_vertex1.m_pos.w));
				face.m_vertex2.m_pos.x = face.m_vertex1.m_pos.x + (face.m_vertex2.m_pos.x - face.m_vertex1.m_pos.x) * t;
				face.m_vertex2.m_pos.y = face.m_vertex1.m_pos.y + (face.m_vertex2.m_pos.y - face.m_vertex1.m_pos.y) * t;
				face.m_vertex2.m_pos.z = face.m_vertex1.m_pos.z + (face.m_vertex2.m_pos.z - face.m_vertex1.m_pos.z) * t;
				face.m_vertex2.m_pos.w = face.m_vertex1.m_pos.w + (face.m_vertex2.m_pos.w - face.m_vertex1.m_pos.w) * t;
				face.m_vertex2.m_UV = face.m_vertex1.m_UV + (face.m_vertex2.m_UV - face.m_vertex1.m_UV) * t;
				face.m_vertex2.m_color = face.m_vertex1.m_color + (face.m_vertex2.m_color - face.m_vertex1.m_color) * t;

				t = (fNear - (-face.m_vertex1.m_pos.w)) / ((-face.m_vertex3.m_pos.w) - (-face.m_vertex1.m_pos.w));
				face.m_vertex3.m_pos.x = face.m_vertex1.m_pos.x + t * (face.m_vertex3.m_pos.x - face.m_vertex1.m_pos.x);
				face.m_vertex3.m_pos.y = face.m_vertex1.m_pos.y + t * (face.m_vertex3.m_pos.y - face.m_vertex1.m_pos.y);
				face.m_vertex3.m_pos.z = face.m_vertex1.m_pos.z + t * (face.m_vertex3.m_pos.z - face.m_vertex1.m_pos.z);
				face.m_vertex3.m_pos.w = face.m_vertex1.m_pos.w + t * (face.m_vertex3.m_pos.w - face.m_vertex1.m_pos.w);
				face.m_vertex3.m_UV = face.m_vertex1.m_UV + (face.m_vertex3.m_UV - face.m_vertex1.m_UV) * t;
				face.m_vertex3.m_color = face.m_vertex1.m_color + (face.m_vertex3.m_color - face.m_vertex1.m_color) * t;

				bAddFace = false;
			}

			return false;
		}
		else
		{
			return true;
		}
	}
}

void GeometryStage::TransformClipToScreen(SFaceRuntime& face)
{
	if (!face.m_bUseNormalizedPos)
	{
		{
			SVertexRuntime& vertex = face.m_vertex1;
			float rhw = 1.0f / vertex.m_pos.w;
			vertex.m_pos *= rhw;
			vertex.m_pos.x = (vertex.m_pos.x * 0.5f + 0.5f) * (SCREEN_WIDTH)-0.5f;
			vertex.m_pos.y = (-vertex.m_pos.y * 0.5f + 0.5f) * (SCREEN_HEIGHT)-0.5f;
			vertex.m_pos.w = rhw;
			vertex.m_UV.x *= rhw;
			vertex.m_UV.y *= rhw;
			vertex.m_color *= rhw;
		}

		{
			SVertexRuntime& vertex = face.m_vertex2;
			float rhw = 1.0f / vertex.m_pos.w;
			vertex.m_pos *= rhw;
			vertex.m_pos.x = (vertex.m_pos.x * 0.5f + 0.5f) * (SCREEN_WIDTH)-0.5f;
			vertex.m_pos.y = (-vertex.m_pos.y * 0.5f + 0.5f) * (SCREEN_HEIGHT)-0.5f;
			vertex.m_pos.w = rhw;
			vertex.m_UV.x *= rhw;
			vertex.m_UV.y *= rhw;
			vertex.m_color *= rhw;
		}

		{
			SVertexRuntime& vertex = face.m_vertex3;
			float rhw = 1.0f / vertex.m_pos.w;
			vertex.m_pos *= rhw;
			vertex.m_pos.x = (vertex.m_pos.x * 0.5f + 0.5f) * (SCREEN_WIDTH)-0.5f;
			vertex.m_pos.y = (-vertex.m_pos.y * 0.5f + 0.5f) * (SCREEN_HEIGHT)-0.5f;
			vertex.m_pos.w = rhw;
			vertex.m_UV.x *= rhw;
			vertex.m_UV.y *= rhw;
			vertex.m_color *= rhw;
		}
	}
}

bool GeometryStage::IsVertexInCVV(const Vec4& pos)
{
	if (pos.x <= -pos.w || pos.x >= pos.w
		|| pos.y <= -pos.w || pos.y >= pos.w
		|| pos.z <= -pos.w || pos.z >= pos.w)
	{
		return false;
	}

	return true;

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
}

