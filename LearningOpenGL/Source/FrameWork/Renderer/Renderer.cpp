#include "FrameWork/Renderer/Renderer.h"

void CRenderer::AddDrawCallBack(DrawCallBack cb)
{
	m_renderingQueue.push_back(cb);
}

void CRenderer::ClearDrawCallBack()
{
	m_renderingQueue.clear();
}

void CRenderer::Draw()
{
	for (auto& cb : m_renderingQueue)
	{
		cb();
	}
}

CRenderer* CRenderer::GetInstance()
{
	if ( !s_instance )
	{
		s_instance = new CRenderer;
	}

	return s_instance;
}

CRenderer* CRenderer::s_instance = nullptr;
