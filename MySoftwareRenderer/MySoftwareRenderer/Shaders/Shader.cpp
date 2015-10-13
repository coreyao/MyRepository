#include "Shader.h"
#include "../Director.h"
#include "../RasterizationStage.h"

void CMeshVertexShader::ProcessVertex(SVertexRuntime* pVertex)
{
	const Mat4& viewMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetViewMat();
	const Mat4& projMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetProjMat();
	pVertex->m_pos =  projMat * viewMat * ModelMat * pVertex->m_pos;

	SVariable variable;
	variable.v4 = pVertex->m_pos;
	pVertex->m_vCustomVariable.push_back(variable);
}

Color4F CMeshFragmentShader::ProcessFragment(SFragment* pFragment)
{
	Color4F finalColor = RasterizationStage::CRasterizer::GetInstance()->SampleTexture(m_vMaterial[0]->GetBaseColorTex(), pFragment->m_UV);
	finalColor = finalColor * pFragment->m_color;
	return finalColor;
}

void CSkinMeshVertexShader::ProcessVertex(SVertexRuntime* pVertex)
{
	int matrixIndex = 0;
	Vec4 matrixPalette1 = Vec4(0.0, 0.0, 0.0, 0.0);
	Vec4 matrixPalette2 = Vec4(0.0, 0.0, 0.0, 0.0);
	Vec4 matrixPalette3 = Vec4(0.0, 0.0, 0.0, 0.0);

	if (pVertex->m_blendWeight.x != 0 
		|| pVertex->m_blendWeight.y != 0 
		|| pVertex->m_blendWeight.z != 0 
		|| pVertex->m_blendWeight.w != 0)
	{
		float blendWeight = pVertex->m_blendWeight.x;
		if (blendWeight > 0.0)
		{
			matrixIndex = int(pVertex->m_boneIndex.x) * 3;
			matrixPalette1 = m_vMatrixPallet[matrixIndex] * blendWeight;
			matrixPalette2 = m_vMatrixPallet[matrixIndex + 1] * blendWeight;
			matrixPalette3 = m_vMatrixPallet[matrixIndex + 2] * blendWeight;
		}

		blendWeight = pVertex->m_blendWeight.y;
		if (blendWeight > 0.0)
		{
			matrixIndex = int(pVertex->m_boneIndex.y) * 3;
			matrixPalette1 += m_vMatrixPallet[matrixIndex] * blendWeight;
			matrixPalette2 += m_vMatrixPallet[matrixIndex + 1] * blendWeight;
			matrixPalette3 += m_vMatrixPallet[matrixIndex + 2] * blendWeight;
		}

		blendWeight = pVertex->m_blendWeight.z;
		if (blendWeight > 0.0)
		{
			matrixIndex = int(pVertex->m_boneIndex.z) * 3;
			matrixPalette1 += m_vMatrixPallet[matrixIndex] * blendWeight;
			matrixPalette2 += m_vMatrixPallet[matrixIndex + 1] * blendWeight;
			matrixPalette3 += m_vMatrixPallet[matrixIndex + 2] * blendWeight;
		}

		blendWeight = pVertex->m_blendWeight.w;
		if (blendWeight > 0.0)
		{
			matrixIndex = int(pVertex->m_boneIndex.w) * 3;
			matrixPalette1 += m_vMatrixPallet[matrixIndex] * blendWeight;
			matrixPalette2 += m_vMatrixPallet[matrixIndex + 1] * blendWeight;
			matrixPalette3 += m_vMatrixPallet[matrixIndex + 2] * blendWeight;
		}

		Vec4 _skinnedposition;
		_skinnedposition.x = pVertex->m_pos.Dot(matrixPalette1);
		_skinnedposition.y = pVertex->m_pos.Dot(matrixPalette2);
		_skinnedposition.z = pVertex->m_pos.Dot(matrixPalette3);
		_skinnedposition.w = pVertex->m_pos.w;

		const Mat4& viewMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetViewMat();
		const Mat4& projMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetProjMat();
		pVertex->m_pos = projMat * viewMat * ModelMat * _skinnedposition;
	}
}
