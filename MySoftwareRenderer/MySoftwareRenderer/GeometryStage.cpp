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
		bool bOut[3] = {false, false, false};
		if (-face.m_vertex1.m_pos.w > fNear)
		{
			bOut[0] = true;
			++iCount;
		}
		if (-face.m_vertex2.m_pos.w > fNear)
		{
			bOut[1] = true;
			++iCount;
		}
		if (-face.m_vertex3.m_pos.w > fNear)
		{
			bOut[2] = true;
			++iCount;
		}

		if (iCount == 0)
		{
			bAddFace = false;
			return false;
		}
		else if (iCount < 3)
		{
			if (iCount == 1)
			{
				SFaceRuntime tempFace = face;
				if ( bOut[0] )
				{
					float t = (fNear - (-tempFace.m_vertex1.m_pos.w)) / ((-tempFace.m_vertex2.m_pos.w) - (-tempFace.m_vertex1.m_pos.w));
					tempFace.m_vertex1.m_pos.x = tempFace.m_vertex1.m_pos.x + (tempFace.m_vertex2.m_pos.x - tempFace.m_vertex1.m_pos.x) * t;
					tempFace.m_vertex1.m_pos.y = tempFace.m_vertex1.m_pos.y + (tempFace.m_vertex2.m_pos.y - tempFace.m_vertex1.m_pos.y) * t;
					tempFace.m_vertex1.m_pos.w = tempFace.m_vertex1.m_pos.w + (tempFace.m_vertex2.m_pos.w - tempFace.m_vertex1.m_pos.w) * t;

					if (tempFace.m_pRenderState->m_bEnableCullFace && IsBackFace(tempFace, tempFace.m_pRenderState->m_eVertexOrder))
						return true;

					SVertexRuntime newVertex1;
					t = (fNear - (-face.m_vertex1.m_pos.w)) / ((-face.m_vertex3.m_pos.w) - (-face.m_vertex1.m_pos.w));
					newVertex1.m_pos.x = face.m_vertex1.m_pos.x + t * (face.m_vertex3.m_pos.x - face.m_vertex1.m_pos.x);
					newVertex1.m_pos.y = face.m_vertex1.m_pos.y + t * (face.m_vertex3.m_pos.y - face.m_vertex1.m_pos.y);
					newVertex1.m_pos.z = face.m_vertex1.m_pos.z + t * (face.m_vertex3.m_pos.z - face.m_vertex1.m_pos.z);
					newVertex1.m_pos.w = face.m_vertex1.m_pos.w + t * (face.m_vertex3.m_pos.w - face.m_vertex1.m_pos.w);
					newVertex1.m_UV = face.m_vertex1.m_UV + (face.m_vertex3.m_UV - face.m_vertex1.m_UV) * t;
					newVertex1.m_color = face.m_vertex1.m_color + (face.m_vertex3.m_color - face.m_vertex1.m_color) * t;

					SVertexRuntime newVertex2;
					t = (fNear - (-face.m_vertex1.m_pos.w)) / ((-face.m_vertex2.m_pos.w) - (-face.m_vertex1.m_pos.w));
					newVertex2.m_pos.x = face.m_vertex1.m_pos.x + t * (face.m_vertex2.m_pos.x - face.m_vertex1.m_pos.x);
					newVertex2.m_pos.y = face.m_vertex1.m_pos.y + t * (face.m_vertex2.m_pos.y - face.m_vertex1.m_pos.y);
					newVertex2.m_pos.z = face.m_vertex1.m_pos.z + t * (face.m_vertex2.m_pos.z - face.m_vertex1.m_pos.z);
					newVertex2.m_pos.w = face.m_vertex1.m_pos.w + t * (face.m_vertex2.m_pos.w - face.m_vertex1.m_pos.w);
					newVertex2.m_UV = face.m_vertex1.m_UV + (face.m_vertex2.m_UV - face.m_vertex1.m_UV) * t;
					newVertex2.m_color = face.m_vertex1.m_color + (face.m_vertex2.m_color - face.m_vertex1.m_color) * t;

					if (face.m_pRenderState->m_eVertexOrder == EVertexOrder_Counter_ClockWise)
					{
						addFace.m_vertex1 = face.m_vertex2;
						addFace.m_vertex2 = face.m_vertex3;
						addFace.m_vertex3 = newVertex2;

						face.m_vertex1 = face.m_vertex3;
						face.m_vertex2 = newVertex1;
						face.m_vertex3 = newVertex2;
					}
					else
					{
						addFace.m_vertex1 = face.m_vertex2;
						addFace.m_vertex2 = face.m_vertex3;
						addFace.m_vertex3 = newVertex1;

						face.m_vertex1 = face.m_vertex2;
						face.m_vertex2 = newVertex1;
						face.m_vertex3 = newVertex2;
					}
				}
				else if ( bOut[1] )
				{
					float t = (fNear - (-tempFace.m_vertex1.m_pos.w)) / ((-tempFace.m_vertex2.m_pos.w) - (-tempFace.m_vertex1.m_pos.w));
					tempFace.m_vertex2.m_pos.x = tempFace.m_vertex1.m_pos.x + (tempFace.m_vertex2.m_pos.x - tempFace.m_vertex1.m_pos.x) * t;
					tempFace.m_vertex2.m_pos.y = tempFace.m_vertex1.m_pos.y + (tempFace.m_vertex2.m_pos.y - tempFace.m_vertex1.m_pos.y) * t;
					tempFace.m_vertex2.m_pos.w = tempFace.m_vertex1.m_pos.w + (tempFace.m_vertex2.m_pos.w - tempFace.m_vertex1.m_pos.w) * t;

					if (tempFace.m_pRenderState->m_bEnableCullFace && IsBackFace(tempFace, tempFace.m_pRenderState->m_eVertexOrder))
						return true;

					SVertexRuntime newVertex1;
					t = (fNear - (-face.m_vertex2.m_pos.w)) / ((-face.m_vertex3.m_pos.w) - (-face.m_vertex2.m_pos.w));
					newVertex1.m_pos.x = face.m_vertex2.m_pos.x + t * (face.m_vertex3.m_pos.x - face.m_vertex2.m_pos.x);
					newVertex1.m_pos.y = face.m_vertex2.m_pos.y + t * (face.m_vertex3.m_pos.y - face.m_vertex2.m_pos.y);
					newVertex1.m_pos.z = face.m_vertex2.m_pos.z + t * (face.m_vertex3.m_pos.z - face.m_vertex2.m_pos.z);
					newVertex1.m_pos.w = face.m_vertex2.m_pos.w + t * (face.m_vertex3.m_pos.w - face.m_vertex2.m_pos.w);
					newVertex1.m_UV = face.m_vertex2.m_UV + (face.m_vertex3.m_UV - face.m_vertex2.m_UV) * t;
					newVertex1.m_color = face.m_vertex2.m_color + (face.m_vertex3.m_color - face.m_vertex2.m_color) * t;

					SVertexRuntime newVertex2;
					t = (fNear - (-face.m_vertex2.m_pos.w)) / ((-face.m_vertex1.m_pos.w) - (-face.m_vertex2.m_pos.w));
					newVertex2.m_pos.x = face.m_vertex2.m_pos.x + t * (face.m_vertex1.m_pos.x - face.m_vertex2.m_pos.x);
					newVertex2.m_pos.y = face.m_vertex2.m_pos.y + t * (face.m_vertex1.m_pos.y - face.m_vertex2.m_pos.y);
					newVertex2.m_pos.z = face.m_vertex2.m_pos.z + t * (face.m_vertex1.m_pos.z - face.m_vertex2.m_pos.z);
					newVertex2.m_pos.w = face.m_vertex2.m_pos.w + t * (face.m_vertex1.m_pos.w - face.m_vertex2.m_pos.w);
					newVertex2.m_UV = face.m_vertex2.m_UV + (face.m_vertex1.m_UV - face.m_vertex2.m_UV) * t;
					newVertex2.m_color = face.m_vertex2.m_color + (face.m_vertex1.m_color - face.m_vertex2.m_color) * t;

					if (face.m_pRenderState->m_eVertexOrder == EVertexOrder_Counter_ClockWise)
					{
						addFace.m_vertex1 = face.m_vertex1;
						addFace.m_vertex2 = newVertex1;
						addFace.m_vertex3 = face.m_vertex3;

						face.m_vertex1 = face.m_vertex1;
						face.m_vertex2 = newVertex2;
						face.m_vertex3 = newVertex1;
					}
					else
					{
						addFace.m_vertex1 = face.m_vertex3;
						addFace.m_vertex2 = face.m_vertex1;
						addFace.m_vertex3 = newVertex2;

						face.m_vertex1 = face.m_vertex3;
						face.m_vertex2 = newVertex2;
						face.m_vertex3 = newVertex1;
					}
				}
				else if (bOut[2])
				{
					float t = (fNear - (-tempFace.m_vertex1.m_pos.w)) / ((-tempFace.m_vertex3.m_pos.w) - (-tempFace.m_vertex1.m_pos.w));
					tempFace.m_vertex3.m_pos.x = tempFace.m_vertex1.m_pos.x + (tempFace.m_vertex3.m_pos.x - tempFace.m_vertex1.m_pos.x) * t;
					tempFace.m_vertex3.m_pos.y = tempFace.m_vertex1.m_pos.y + (tempFace.m_vertex3.m_pos.y - tempFace.m_vertex1.m_pos.y) * t;
					tempFace.m_vertex3.m_pos.w = tempFace.m_vertex1.m_pos.w + (tempFace.m_vertex3.m_pos.w - tempFace.m_vertex1.m_pos.w) * t;

					if (tempFace.m_pRenderState->m_bEnableCullFace && IsBackFace(tempFace, tempFace.m_pRenderState->m_eVertexOrder))
						return true;

					SVertexRuntime newVertex1;
					t = (fNear - (-face.m_vertex3.m_pos.w)) / ((-face.m_vertex2.m_pos.w) - (-face.m_vertex3.m_pos.w));
					newVertex1.m_pos.x = face.m_vertex3.m_pos.x + t * (face.m_vertex2.m_pos.x - face.m_vertex3.m_pos.x);
					newVertex1.m_pos.y = face.m_vertex3.m_pos.y + t * (face.m_vertex2.m_pos.y - face.m_vertex3.m_pos.y);
					newVertex1.m_pos.z = face.m_vertex3.m_pos.z + t * (face.m_vertex2.m_pos.z - face.m_vertex3.m_pos.z);
					newVertex1.m_pos.w = face.m_vertex3.m_pos.w + t * (face.m_vertex2.m_pos.w - face.m_vertex3.m_pos.w);
					newVertex1.m_UV = face.m_vertex3.m_UV + (face.m_vertex2.m_UV - face.m_vertex3.m_UV) * t;
					newVertex1.m_color = face.m_vertex3.m_color + (face.m_vertex2.m_color - face.m_vertex3.m_color) * t;

					SVertexRuntime newVertex2;
					t = (fNear - (-face.m_vertex3.m_pos.w)) / ((-face.m_vertex1.m_pos.w) - (-face.m_vertex3.m_pos.w));
					newVertex2.m_pos.x = face.m_vertex3.m_pos.x + t * (face.m_vertex1.m_pos.x - face.m_vertex3.m_pos.x);
					newVertex2.m_pos.y = face.m_vertex3.m_pos.y + t * (face.m_vertex1.m_pos.y - face.m_vertex3.m_pos.y);
					newVertex2.m_pos.z = face.m_vertex3.m_pos.z + t * (face.m_vertex1.m_pos.z - face.m_vertex3.m_pos.z);
					newVertex2.m_pos.w = face.m_vertex3.m_pos.w + t * (face.m_vertex1.m_pos.w - face.m_vertex3.m_pos.w);
					newVertex2.m_UV = face.m_vertex3.m_UV + (face.m_vertex1.m_UV - face.m_vertex3.m_UV) * t;
					newVertex2.m_color = face.m_vertex3.m_color + (face.m_vertex1.m_color - face.m_vertex3.m_color) * t;

					if (face.m_pRenderState->m_eVertexOrder == EVertexOrder_Counter_ClockWise)
					{
						addFace.m_vertex1 = face.m_vertex2;
						addFace.m_vertex2 = newVertex2;
						addFace.m_vertex3 = face.m_vertex1;

						face.m_vertex1 = face.m_vertex2;
						face.m_vertex2 = newVertex1;
						face.m_vertex3 = newVertex2;
					}
					else
					{
						addFace.m_vertex1 = face.m_vertex1;
						addFace.m_vertex2 = face.m_vertex2;
						addFace.m_vertex3 = newVertex1;

						face.m_vertex1 = face.m_vertex1;
						face.m_vertex2 = newVertex1;
						face.m_vertex3 = newVertex2;
					}
				}
				
				bAddFace = true;
				addFace.m_fAlpha = face.m_fAlpha;
				addFace.m_pRenderState = face.m_pRenderState;
			}
			else if (iCount == 2)
			{
				SVertexRuntime* pIn = nullptr;
				SVertexRuntime* pOut1 = nullptr;
				SVertexRuntime* pOut2 = nullptr;

				if ( !bOut[0] )
				{
					pIn = &face.m_vertex1;
					pOut1 = &face.m_vertex2;
					pOut2 = &face.m_vertex3;
				}
				else if ( !bOut[1] )
				{
					pIn = &face.m_vertex2;
					pOut1 = &face.m_vertex1;
					pOut2 = &face.m_vertex3;
				}
				else if ( !bOut[2] )
				{
					pIn = &face.m_vertex3;
					pOut1 = &face.m_vertex1;
					pOut2 = &face.m_vertex2;
				}

				float t = (fNear - (-pIn->m_pos.w)) / ((-pOut1->m_pos.w) - (-pIn->m_pos.w));
				pOut1->m_pos.x = pIn->m_pos.x + (pOut1->m_pos.x - pIn->m_pos.x) * t;
				pOut1->m_pos.y = pIn->m_pos.y + (pOut1->m_pos.y - pIn->m_pos.y) * t;
				pOut1->m_pos.z = pIn->m_pos.z + (pOut1->m_pos.z - pIn->m_pos.z) * t;
				pOut1->m_pos.w = pIn->m_pos.w + (pOut1->m_pos.w - pIn->m_pos.w) * t;
				pOut1->m_UV = pIn->m_UV + (pOut1->m_UV - pIn->m_UV) * t;
				pOut1->m_color = pIn->m_color + (pOut1->m_color - pIn->m_color) * t;

				t = (fNear - (-pIn->m_pos.w)) / ((-pOut2->m_pos.w) - (-pIn->m_pos.w));
				pOut2->m_pos.x = pIn->m_pos.x + (pOut2->m_pos.x - pIn->m_pos.x) * t;
				pOut2->m_pos.y = pIn->m_pos.y + (pOut2->m_pos.y - pIn->m_pos.y) * t;
				pOut2->m_pos.z = pIn->m_pos.z + (pOut2->m_pos.z - pIn->m_pos.z) * t;
				pOut2->m_pos.w = pIn->m_pos.w + (pOut2->m_pos.w - pIn->m_pos.w) * t;
				pOut2->m_UV = pIn->m_UV + (pOut2->m_UV - pIn->m_UV) * t;
				pOut2->m_color = pIn->m_color + (pOut2->m_color - pIn->m_color) * t;

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

bool GeometryStage::IsBackFace(SFaceRuntime& face, EVertexOrder eOrder /*= EVertexOrder_ClockWise*/)
{
	Vec3 p1;
	Vec3 p2;
	if (eOrder == EVertexOrder_ClockWise)
	{
		p1 = Vec3(face.m_vertex1.m_pos) / face.m_vertex1.m_pos.w - Vec3(face.m_vertex2.m_pos) / face.m_vertex2.m_pos.w;
		p2 = Vec3(face.m_vertex3.m_pos) / face.m_vertex3.m_pos.w - Vec3(face.m_vertex2.m_pos) / face.m_vertex2.m_pos.w;
	}
	else if (eOrder == EVertexOrder_Counter_ClockWise)
	{
		p1 = Vec3(face.m_vertex2.m_pos) / face.m_vertex2.m_pos.w - Vec3(face.m_vertex1.m_pos) / face.m_vertex1.m_pos.w;
		p2 = Vec3(face.m_vertex3.m_pos) / face.m_vertex3.m_pos.w - Vec3(face.m_vertex2.m_pos) / face.m_vertex2.m_pos.w;
	}

	p1.z = 0;
	p2.z = 0;

	Vec3 v = p1.Cross(p2);
	return v.z < 0;
}

