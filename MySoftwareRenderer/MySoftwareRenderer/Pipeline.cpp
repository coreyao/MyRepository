#include "Pipeline.h"

CPipeline* CPipeline::s_pInstance;

void CPipeline::Run()
{

}

void CPipeline::AddFace(const SFaceRuntime& rFace)
{
	m_vRenderList.push_back(rFace);
}

CPipeline* CPipeline::GetInstance()
{
	if ( !s_pInstance )
		s_pInstance = new CPipeline;

	return s_pInstance;
}
