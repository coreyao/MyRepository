#include "Pipeline.h"

CPipeline* CPipeline::s_pInstance;

void CPipeline::Draw()
{
	RasterizationStage::CRasterizer::GetInstance()->ClearDepthBuffer(1.0f);

	for (auto& curFace : m_vRenderList)
	{
		ApplicationStage::TransformLocalToWorld(*curFace, curFace->m_pRenderState->m_worldTransform);
		if (!curFace->m_pRenderState->m_bEnableCullFace || !ApplicationStage::IsBackFace(*curFace, curFace->m_pRenderState->m_eVertexOrder))
		{
			GeometryStage::TransformWorldToCamera(*curFace);

			bool bAddFace = false;
			SFaceRuntime newFace;
			bool bClip = GeometryStage::FrustrumCulling(*curFace, bAddFace, newFace);
			if (bClip)
				continue;

			if (bAddFace)
			{
				GeometryStage::TransformCameraToScreen(newFace);
				RasterizationStage::CRasterizer::GetInstance()->DrawAnyTriangle(newFace.m_vertex1, newFace.m_vertex2, newFace.m_vertex3, curFace->m_pRenderState->m_bDrawWireFrame);
			}

			GeometryStage::TransformCameraToScreen(*curFace);
			RasterizationStage::CRasterizer::GetInstance()->DrawAnyTriangle(curFace->m_vertex1, curFace->m_vertex2, curFace->m_vertex3, curFace->m_pRenderState->m_bDrawWireFrame);
		}

		delete curFace;
	}

	m_vRenderList.clear();
}

void CPipeline::AddFace(SFaceRuntime* rFace)
{
	m_vRenderList.push_back(rFace);
}

CPipeline* CPipeline::GetInstance()
{
	if ( !s_pInstance )
		s_pInstance = new CPipeline;

	return s_pInstance;
}
