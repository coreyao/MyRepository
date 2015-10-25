#include "Shader.h"
#include "../Director.h"
#include "../RasterizationStage.h"
#include "../Light.h"

void CMeshVertexShader::ProcessVertex(CVertexRuntime* pVertex)
{
	const Mat4& viewMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetViewMat();
	const Mat4& projMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetProjMat();
	auto& outPos = pVertex->m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position];
	pVertex->m_vCustomVariable[0].v3 = ModelMat * outPos.v4;

	outPos.v4 = projMat * viewMat * ModelMat * outPos.v4;
}

Color4F CMeshFragmentShader::ProcessFragment(SFragment* pFragment)
{
	Color4F lightColor = Color4F(0, 0, 0, 1.0f);
	const Vec2& uv = pFragment->m_vVertexAttributeVar[EVertexAttributeVar_UV].v2;
	Color4F texColor = RasterizationStage::CRasterizer::GetInstance()->SampleTexture(m_vMaterial[0]->GetBaseColorTex(), uv);
	if (EnableLight)
	{
		Vec3 Normal = pFragment->m_vVertexAttributeVar[EVertexAttributeVar_Normal].v3;
		if (EnableNormalMap)
		{
			Normal.Normalize();
			Vec3 Tangent = pFragment->m_vVertexAttributeVar[EVertexAttributeVar_Tangent].v3;
			Tangent = Tangent - Normal * Tangent.Dot(Normal);
			Tangent.Normalize();
			Mat4 TBN(Tangent, Tangent.Cross(Normal).GetNormalized(), Normal, Vec3(0, 0, 0));
			Color4F normalColor = RasterizationStage::CRasterizer::GetInstance()->SampleTexture(m_vMaterial[0]->GetNormalMapTex(), uv);
			normalColor = normalColor * 2.0f - 1.0f;
			Normal = TBN * normalColor;
		}

		Normal = ModelMat * Vec4(Normal, 0.0f);
		Normal.Normalize();

		const Vec3& FragPos = pFragment->m_vCustomVariable[0].v3;
		const Vec3& EyePos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
		const Vec3& FragToEyeDir = (EyePos - FragPos).GetNormalized();

		const std::vector<CDirectionalLight>& vDirLights = CLightManager::GetInstance()->GetAllDirectionalLights();
		for (int i = 0; i < vDirLights.size(); ++i)
		{
			const CDirectionalLight& rLight = vDirLights[i];
			Color4F DirlightColor = Color4F(0, 0, 0, 1.0f);

			DirlightColor += rLight.m_ambientColor;

			float fCos = rLight.m_lightDir.Dot(Normal);
			Helper::Clamp(fCos, 0.0f, 1.0f);
			DirlightColor += rLight.m_diffuseColor * fCos;

			Vec3 halfDir = (rLight.m_lightDir + FragToEyeDir).GetNormalized();
			float spec = halfDir.Dot(Normal);
			Helper::Clamp(spec, 0.0f, 1.0f);
			DirlightColor += rLight.m_specularColor * pow(spec, m_vMaterial[0]->GetShininess());

			lightColor += DirlightColor;
		}

		const std::vector<CPointLight>& vPointLights = CLightManager::GetInstance()->GetAllPointLights();
		for (int i = 0; i < vPointLights.size(); ++i)
		{
			const CPointLight& rLight = vPointLights[i];
			Color4F PointlightColor = Color4F(0, 0, 0, 1.0f);

			Vec3 lightDir = (rLight.m_lightPos - FragPos).GetNormalized();
			float distance = lightDir.GetLength();
			lightDir.Normalize();
			float attenuation = 1.0f / (rLight.m_attenuation_constant + rLight.m_attenuation_linear * distance + rLight.m_attenuation_quadratic * (distance * distance));

			PointlightColor += rLight.m_ambientColor;

			float fCos = lightDir.Dot(Normal);
			Helper::Clamp(fCos, 0.0f, 1.0f);
			PointlightColor += rLight.m_diffuseColor * fCos;

			Vec3 halfDir = (lightDir + FragToEyeDir).GetNormalized();
			float spec = halfDir.Dot(Normal);
			Helper::Clamp(spec, 0.0f, 1.0f);
			PointlightColor += rLight.m_specularColor * pow(spec, m_vMaterial[0]->GetShininess());

			lightColor += PointlightColor * attenuation;
		}

		const std::vector<CSpotLight>& vSpotLights = CLightManager::GetInstance()->GetAllSpotLights();
		for (int i = 0; i < vSpotLights.size(); ++i)
		{
			const CSpotLight& rLight = vSpotLights[i];
			Color4F SpotlightColor = Color4F(0, 0, 0, 1.0f);

			Vec3 lightDir = (rLight.m_lightPos - FragPos).GetNormalized();
			float distance = lightDir.GetLength();
			lightDir.Normalize();
			float attenuation = 1.0f / (rLight.m_attenuation_constant + rLight.m_attenuation_linear * distance + rLight.m_attenuation_quadratic * (distance * distance));

			float fTheta = RAD_TO_DEG(acosf(lightDir.Dot(rLight.m_lightDir)));
			float epsilon = rLight.fInnerAngle - rLight.fOuterAngle;
			float intensity = (fTheta - rLight.fOuterAngle) / epsilon;
			Helper::Clamp(intensity, 0.0f, 1.0f);

			SpotlightColor += rLight.m_ambientColor;

			float fCos = lightDir.Dot(Normal);
			Helper::Clamp(fCos, 0.0f, 1.0f);
			SpotlightColor += rLight.m_diffuseColor * fCos;

			Vec3 halfDir = (lightDir + FragToEyeDir).GetNormalized();
			float spec = halfDir.Dot(Normal);
			Helper::Clamp(spec, 0.0f, 1.0f);
			SpotlightColor += rLight.m_specularColor * pow(spec, m_vMaterial[0]->GetShininess());

			lightColor += SpotlightColor * attenuation * intensity;
		}
	}
	else
	{
		lightColor = Color4F::WHITE;
	}

	Color4F& rVertexColor = pFragment->m_vVertexAttributeVar[EVertexAttributeVar_Color].color;
	Color4F finalColor = texColor * rVertexColor * lightColor;
	finalColor.a = pFragment->fAlpha;
	return finalColor;
}

void CSkinMeshVertexShader::ProcessVertex(CVertexRuntime* pVertex)
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

		auto& outPos = pVertex->m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
		Vec4 skinnedPos;
		skinnedPos.x = outPos.Dot(matrixPalette1);
		skinnedPos.y = outPos.Dot(matrixPalette2);
		skinnedPos.z = outPos.Dot(matrixPalette3);
		skinnedPos.w = outPos.w;

		const Mat4& viewMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetViewMat();
		const Mat4& projMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetProjMat();
		outPos = projMat * viewMat * ModelMat * skinnedPos;
	}
}
