#include "GeometryStage.h"
#include "Director.h"

bool GeometryStage::DoClipInClipSpace(CFaceRuntime& face, bool& bAddFace, CFaceRuntime& addFace)
{
	auto& outPos1 = face.m_vertex1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
	auto& outPos2 = face.m_vertex2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
	auto& outPos3 = face.m_vertex3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;

	if (!IsVertexInCVV(outPos1)
		&& !IsVertexInCVV(outPos2)
		&& !IsVertexInCVV(outPos3))
	{
		bAddFace = false;
		return true;
	}
	else
	{
		float fNear = -CDirector::GetInstance()->GetPerspectiveCamera()->GetNearZ();

		int iCount = 0;
		bool bOut[3] = {false, false, false};
		if (-outPos1.w > fNear)
		{
			bOut[0] = true;
			++iCount;
		}
		if (-outPos2.w > fNear)
		{
			bOut[1] = true;
			++iCount;
		}
		if (-outPos3.w > fNear)
		{
			bOut[2] = true;
			++iCount;
		}

		if (iCount == 0)
		{
			bAddFace = false;
			return GeometryStage::IsBackFace(face, face.m_pRenderState->m_eVertexOrder);
		}
		else if (iCount < 3)
		{
			if (iCount == 1)
			{
				CFaceRuntime tempFace = face;
				if (bOut[0])
				{
					auto& outTempPos1 = tempFace.m_vertex1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
					auto& outTempPos2 = tempFace.m_vertex2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
					float t = (fNear - (-outTempPos1.w)) / ((-outTempPos2.w) - (-outTempPos1.w));
					outTempPos1 = Helper::Lerp(outTempPos1, outTempPos2, t);
					if (IsBackFace(tempFace, tempFace.m_pRenderState->m_eVertexOrder))
						return true;

					CVertexRuntime newVertex1;
					t = (fNear - (-outPos1.w)) / ((-outPos3.w) - (-outPos1.w));
					Helper::LerpVertex(&face.m_vertex1, &face.m_vertex3, t, &newVertex1);

					CVertexRuntime newVertex2;
					t = (fNear - (-outPos1.w)) / ((-outPos2.w) - (-outPos1.w));
					Helper::LerpVertex(&face.m_vertex1, &face.m_vertex2, t, &newVertex2);
					
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
					auto& outTempPos1 = tempFace.m_vertex1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
					auto& outTempPos2 = tempFace.m_vertex2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
					float t = (fNear - (-outTempPos1.w)) / ((-outTempPos2.w) - (-outTempPos1.w));
					outTempPos2 = Helper::Lerp(outTempPos1, outTempPos2, t);
					if (IsBackFace(tempFace, tempFace.m_pRenderState->m_eVertexOrder))
						return true;

					CVertexRuntime newVertex1;
					t = (fNear - (-outPos2.w)) / ((-outPos3.w) - (-outPos2.w));
					Helper::LerpVertex(&face.m_vertex2, &face.m_vertex3, t, &newVertex1);

					CVertexRuntime newVertex2;
					t = (fNear - (-outPos2.w)) / ((-outPos1.w) - (-outPos2.w));
					Helper::LerpVertex(&face.m_vertex2, &face.m_vertex1, t, &newVertex2);

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
					auto& outTempPos1 = tempFace.m_vertex1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
					auto& outTempPos3 = tempFace.m_vertex3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
					float t = (fNear - (-outTempPos1.w)) / ((-outTempPos3.w) - (-outTempPos1.w));
					outTempPos3 = Helper::Lerp(outTempPos1, outTempPos3, t);
					if (IsBackFace(tempFace, tempFace.m_pRenderState->m_eVertexOrder))
						return true;

					CVertexRuntime newVertex1;
					t = (fNear - (-outPos3.w)) / ((-outPos2.w) - (-outPos3.w));
					Helper::LerpVertex(&face.m_vertex3, &face.m_vertex2, t, &newVertex1);

					CVertexRuntime newVertex2;
					t = (fNear - (-outPos3.w)) / ((-outPos1.w) - (-outPos3.w));
					Helper::LerpVertex(&face.m_vertex3, &face.m_vertex1, t, &newVertex2);
				
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
				CVertexRuntime* pIn = nullptr;
				CVertexRuntime* pOut1 = nullptr;
				CVertexRuntime* pOut2 = nullptr;

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

				float t = (fNear - (-pIn->m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.w)) / 
					((-pOut1->m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.w) - (-pIn->m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.w));
				Helper::LerpVertex(pIn, pOut1, t, pOut1);

				t = (fNear - (-pIn->m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.w)) / 
					((-pOut2->m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.w) - (-pIn->m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.w));
				Helper::LerpVertex(pIn, pOut2, t, pOut2);

				bAddFace = false;

				if (IsBackFace(face, face.m_pRenderState->m_eVertexOrder))
					return true;
			}

			return false;
		}
		else
		{
			return true;
		}
	}
}

void GeometryStage::TransformClipToScreen(CFaceRuntime& face)
{
	{
		CVertexRuntime& vertex = face.m_vertex1;
		auto& outPos = vertex.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position];
		float rhw = 1.0f / outPos.v4.w;

		for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
			vertex.m_vVertexAttributeVar[eVar] *= rhw;

		for (int i = 0; i < conMaxCustomVar; ++i)
			vertex.m_vCustomVariable[i] *= rhw;

		outPos.v4.x = (outPos.v4.x * 0.5f + 0.5f) * (SCREEN_WIDTH)-0.5f;
		outPos.v4.y = (-outPos.v4.y * 0.5f + 0.5f) * (SCREEN_HEIGHT)-0.5f;
		outPos.v4.w = rhw;
	}

	{
	CVertexRuntime& vertex = face.m_vertex2;
	auto& outPos = vertex.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position];
	float rhw = 1.0f / outPos.v4.w;

	for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
		vertex.m_vVertexAttributeVar[eVar] *= rhw;

	for (int i = 0; i < conMaxCustomVar; ++i)
		vertex.m_vCustomVariable[i] *= rhw;

	outPos.v4.x = (outPos.v4.x * 0.5f + 0.5f) * (SCREEN_WIDTH)-0.5f;
	outPos.v4.y = (-outPos.v4.y * 0.5f + 0.5f) * (SCREEN_HEIGHT)-0.5f;
	outPos.v4.w = rhw;
	}

	{
		CVertexRuntime& vertex = face.m_vertex3;
		auto& outPos = vertex.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position];
		float rhw = 1.0f / outPos.v4.w;

		for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
			vertex.m_vVertexAttributeVar[eVar] *= rhw;

		for (int i = 0; i < conMaxCustomVar; ++i)
			vertex.m_vCustomVariable[i] *= rhw;

		outPos.v4.x = (outPos.v4.x * 0.5f + 0.5f) * (SCREEN_WIDTH)-0.5f;
		outPos.v4.y = (-outPos.v4.y * 0.5f + 0.5f) * (SCREEN_HEIGHT)-0.5f;
		outPos.v4.w = rhw;
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

bool GeometryStage::IsBackFace(CFaceRuntime& face, EVertexOrder eOrder /*= EVertexOrder_ClockWise*/)
{
	if (!face.m_pRenderState->m_bEnableCullFace)
		return false;

	auto& outPos1 = face.m_vertex1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
	auto& outPos2 = face.m_vertex2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
	auto& outPos3 = face.m_vertex3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;

	Vec3 p1;
	Vec3 p2;
	if (eOrder == EVertexOrder_ClockWise)
	{
		p1 = Vec3(outPos1) / outPos1.w - Vec3(outPos2) / outPos2.w;
		p2 = Vec3(outPos3) / outPos3.w - Vec3(outPos2) / outPos2.w;
	}
	else if (eOrder == EVertexOrder_Counter_ClockWise)
	{
		p1 = Vec3(outPos2) / outPos2.w - Vec3(outPos1) / outPos1.w;
		p2 = Vec3(outPos3) / outPos3.w - Vec3(outPos2) / outPos2.w;
	}

	p1.z = 0;
	p2.z = 0;

	Vec3 v = p1.Cross(p2);
	return v.z < 0;
}
