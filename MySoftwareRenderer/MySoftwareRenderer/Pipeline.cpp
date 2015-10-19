#include "Pipeline.h"
#include "Shaders/Shader.h"

CPipeline* CPipeline::s_pInstance;

void CPipeline::Draw()
{
	RasterizationStage::CRasterizer::GetInstance()->ClearDepthBuffer(1.0f);
	RasterizationStage::CRasterizer::GetInstance()->ClearColorBuffer(Color4F::BLACK);

	list<CFaceRuntime> vAddFace;
	for (auto& curFace : m_vRenderList)
	{
		curFace->m_pRenderState->m_pVertexShader->ProcessVertex(&curFace->m_vertex1);
		curFace->m_pRenderState->m_pVertexShader->ProcessVertex(&curFace->m_vertex2);
		curFace->m_pRenderState->m_pVertexShader->ProcessVertex(&curFace->m_vertex3);

		bool bAddFace = false;
		CFaceRuntime newFace;
		if (GeometryStage::DoClipInClipSpace(*curFace, bAddFace, newFace))
			continue;

		if (bAddFace)
			vAddFace.push_back(newFace);

		GeometryStage::TransformClipToScreen(*curFace);
		RasterizationStage::CRasterizer::GetInstance()->DrawAnyTriangle(curFace->m_vertex1, curFace->m_vertex2, curFace->m_vertex3, curFace->m_fAlpha, curFace->m_pRenderState);
	}

	for (auto& curFace : vAddFace)
	{
		GeometryStage::TransformClipToScreen(curFace);
		RasterizationStage::CRasterizer::GetInstance()->DrawAnyTriangle(curFace.m_vertex1, curFace.m_vertex2, curFace.m_vertex3, curFace.m_fAlpha, curFace.m_pRenderState);
	}

	m_vRenderList.clear();
}

void CPipeline::AddFace(CFaceRuntime* rFace)
{
	m_vRenderList.push_back(rFace);
}

CPipeline* CPipeline::GetInstance()
{
	if ( !s_pInstance )
		s_pInstance = new CPipeline;

	return s_pInstance;
}
