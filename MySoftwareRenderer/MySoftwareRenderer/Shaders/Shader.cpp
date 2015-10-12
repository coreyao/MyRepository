#include "Shader.h"
#include "../Director.h"

void CMeshVertexShader::ProcessVertex(SVertexRuntime* pVertex)
{
	const Mat4& viewMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetViewMat();
	const Mat4& projMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetProjMat();
	pVertex->m_pos =  projMat * viewMat * ModelMat * pVertex->m_pos;
}

Color4F CMeshFragmentShader::ProcessFragment(SFragment* pFragment)
{
	return Color4F::WHITE;
}
