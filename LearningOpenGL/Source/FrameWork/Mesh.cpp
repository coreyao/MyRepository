#include "Mesh.h"
#include "Utility.h"
#include "Camera.h"
#include "Director.h"
#include "Image/ImageManager.h"
#include <algorithm>
#include <iostream>

using namespace std;

Mat4 CBone::UpdateWorldMat()
{
	if (m_pParent)
	{
		m_worldMat = m_pParent->UpdateWorldMat() * m_localMat;
	}
	else
	{
		m_worldMat = m_localMat;
	}

	return m_worldMat;
}

void CBone::CalcPalette(Vec4* matrixPalette)
{
	Mat4 t = UpdateWorldMat() * m_data.m_inverseBindMat;

	matrixPalette[0].set(t.m[0], t.m[4], t.m[8], t.m[12]);
	matrixPalette[1].set(t.m[1], t.m[5], t.m[9], t.m[13]);
	matrixPalette[2].set(t.m[2], t.m[6], t.m[10], t.m[14]);
}

Vec4* CSkeleton::GetMatrixPalette()
{
	if (!m_pMatrixPalette)
	{
		m_pMatrixPalette = new Vec4[m_vSkinBone.size() * 3];
	}

	int i = 0;
	for (auto& cBone : m_vSkinBone)
	{
		cBone->CalcPalette(&m_pMatrixPalette[i++ * 3]);
	}

	return m_pMatrixPalette;
}

CSkeletonAnimator::CSkeletonAnimator()
: m_pTarget(nullptr)
, m_fElapsedTime(0.0f)
, m_fBlendElapsedTime(0.0f)
, m_bLoop(true)
, m_iStartFrameIndex(0)
, m_iEndFrameIndex(-1)
{
}

void CSkeletonAnimator::SetTarget(CMesh* pMesh)
{
	m_pTarget = pMesh;
}

void CSkeletonAnimator::Update(float fDeltaTime)
{
	auto FindBoneByName = [this](const std::string& sBoneName)
	{
		for (auto& pBone : m_pTarget->m_skeleton.m_vSkinBone)
		{
			if (pBone->m_data.m_sName == sBoneName)
			{
				return pBone;
			}
		}

		return (CBone*)nullptr;
	};

	if (!m_pTarget || m_iEndFrameIndex < 0)
		return;

	m_fElapsedTime += fDeltaTime;

	bool bAllFinished = false;
	for (int i = m_iStartFrameIndex; i < m_iEndFrameIndex; ++i)
	{
		const SBoneFrame* pCurFrame = &m_pTarget->m_meshData.m_skeleton.m_vFrame[i];
		const SBoneFrame* pNextFrame = &m_pTarget->m_meshData.m_skeleton.m_vFrame[i + 1];

		float fStartTime = pCurFrame->m_fTime;
		float fEndTime = pNextFrame->m_fTime;
		float fCurTotalTime = fEndTime - fStartTime;

		if (m_fElapsedTime < fStartTime)
			return;

		if (m_fElapsedTime > fEndTime)
		{
			bool bLast = (i == m_iEndFrameIndex - 1);
			if (bLast)
			{
				bAllFinished = true;
				break;
			}
			continue;
		}

		float fElapsedPercent = (m_fElapsedTime - fStartTime) / fCurTotalTime;
		for (int iKeyIdx = 0; iKeyIdx < pCurFrame->m_vKey.size(); ++iKeyIdx)
		{
			CBone* pBone = FindBoneByName(pCurFrame->m_vKey[iKeyIdx].m_sBoneName);
			if (pBone)
			{
				Vec3 finalPos = pCurFrame->m_vKey[iKeyIdx].m_translation + (pNextFrame->m_vKey[iKeyIdx].m_translation - pCurFrame->m_vKey[iKeyIdx].m_translation) * fElapsedPercent;
				Vec3 finalScale = pCurFrame->m_vKey[iKeyIdx].m_scale + (pNextFrame->m_vKey[iKeyIdx].m_scale - pCurFrame->m_vKey[iKeyIdx].m_scale) * fElapsedPercent;
				Quaternion finalRotation = Quaternion::Slerp(pCurFrame->m_vKey[iKeyIdx].m_rotation, pNextFrame->m_vKey[iKeyIdx].m_rotation, fElapsedPercent);

				Mat4 translationMatrix = Mat4::CreateTranslationMat(finalPos.x, finalPos.y, finalPos.z);
				Mat4 scaleMatrix = Mat4::CreateScaleMat(finalScale.x, finalScale.y, finalScale.z);
				Mat4 rotationMatrix = Mat4::CreateRotationMat(finalRotation);

				pBone->m_localMat = translationMatrix * scaleMatrix * rotationMatrix;
			}
		}

		break;
	}

	if (bAllFinished)
	{
		if (m_callback)
			m_callback();

		if (m_bLoop)
		{
			Reset();
		}
	}
}

void CSkeletonAnimator::Reset()
{
	m_fElapsedTime = m_iStartFrameIndex / 30.0f;
	m_fBlendElapsedTime = 0.0f;
}

void CSkeletonAnimator::PlayAnim(int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback)
{
	m_iStartFrameIndex = iStartFrameIndex;
	m_iEndFrameIndex = iEndFrameIndex;
	m_bLoop = bLoop;
	m_callback = callback;

	Reset();
}

CMesh::CMesh()
	: m_Sampler(0)
	, m_bFlipNormal(false)
{
	m_color = Color4F::WHITE;
}

CMesh::~CMesh()
{
}

void CMesh::InitFromFile( const char* pMeshFileName )
{
	FILE* pMeshFile = fopen((MESH_FILE_DIR + pMeshFileName).c_str(), "rb");
	if ( !pMeshFile )
		return;

	m_meshData.ReadFromFile(pMeshFile);
	m_animator.SetTarget(this);

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	InitSubMesh();
	InitSkeleton();
}

void CMesh::InitSubMesh()
{
	for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
	{
		CSubMesh* pSubMesh = new CSubMesh;
		pSubMesh->m_pParent = this;
		pSubMesh->m_iIndex = i;
		pSubMesh->m_meshMat = m_meshData.m_vSubMesh[i].m_MeshMatrix;
		m_vSubMesh.push_back(pSubMesh);

		// - Init material
		std::string sTextureFile;
		if (!m_meshData.m_vSubMesh[i].m_cMaterial.m_SubTextureVec.empty())
			sTextureFile = m_meshData.m_vSubMesh[i].m_cMaterial.m_SubTextureVec[0].m_sFileName;

		if (!sTextureFile.empty())
		{
			CMaterial newMaterial;
			newMaterial.SetBaseColorTexture(sTextureFile);

			pSubMesh->m_material = newMaterial;
		}

		// - Init VBO & VAO
		glGenBuffers(1, &pSubMesh->m_vertexDataObj);
		glBindBuffer(GL_ARRAY_BUFFER, pSubMesh->m_vertexDataObj);
		glBufferData(GL_ARRAY_BUFFER, m_meshData.m_vSubMesh[i].m_vVectex.size() * sizeof(SSkinMeshVertex), &m_meshData.m_vSubMesh[i].m_vVectex.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &pSubMesh->m_vertexIndexObj);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pSubMesh->m_vertexIndexObj);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_meshData.m_vSubMesh[i].m_vFace.size() * sizeof(SFace), &m_meshData.m_vSubMesh[i].m_vFace.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &pSubMesh->m_vertexAttributeObj);
		glBindVertexArray(pSubMesh->m_vertexAttributeObj);

		glBindBuffer(GL_ARRAY_BUFFER, pSubMesh->m_vertexDataObj);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*)offsetof(SSkinMeshVertex, m_position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*)offsetof(SSkinMeshVertex, m_texCoord));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*)offsetof(SSkinMeshVertex, m_boneIndex));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*)offsetof(SSkinMeshVertex, m_blendWeight));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*)offsetof(SSkinMeshVertex, m_normal));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*)offsetof(SSkinMeshVertex, m_tangent));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*)offsetof(SSkinMeshVertex, m_color));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void CMesh::InitSkeleton()
{
	auto FindCBoneByIndex = [this](int iIndex)
	{
		for (int i = 0; i < m_skeleton.m_vBone.size(); ++i)
		{
			if ( m_skeleton.m_vBone[i].m_data.m_iIndex == iIndex )
			{
				return &m_skeleton.m_vBone[i];
			}
		}

		return (CBone*)nullptr;
	};

	for ( auto& rBoneData : m_meshData.m_skeleton.m_vBone )
	{
		CBone newBone;
		newBone.m_data = rBoneData;
		newBone.m_localMat = rBoneData.m_originalBindMat;
		newBone.m_worldMat = rBoneData.m_originalBindMat;
		m_skeleton.m_vBone.push_back(newBone);
	}

	for (int i = 0; i < m_skeleton.m_vBone.size(); ++i)
	{
		CBone* pCurBone = &m_skeleton.m_vBone[i];
		int iParentIndex = pCurBone->m_data.m_iParentIndex;
		if ( iParentIndex > 0 )
		{
			pCurBone->m_pParent = FindCBoneByIndex(iParentIndex);
		}

		for (auto& iChildIndex : pCurBone->m_data.m_vChildIndex)
		{
			if ( iChildIndex > 0 )
			{
				CBone* pBoneFound = FindCBoneByIndex(iChildIndex);
				if ( pBoneFound )
					pCurBone->m_vChildren.push_back( pBoneFound );
			}
		}
	}

	for (auto& rBoneIndex : m_meshData.m_skeleton.m_vSkinBone)
	{
		CBone* pBoneFound = FindCBoneByIndex(rBoneIndex);
		if ( pBoneFound )
		{
			m_skeleton.m_vSkinBone.push_back(pBoneFound);
		}
	}
}

void CMesh::Update(float dt)
{
	m_animator.Update(dt);
}

void CMesh::Render()
{
	CObject::Render();

	glUseProgram(m_theProgram);

	for ( int i = 0; i < m_vSubMesh.size(); ++i )
	{
		CSubMesh* pSubMesh = m_vSubMesh[i];
		if (!pSubMesh->m_bSubMeshVisibility)
			continue;

		glBindVertexArray(pSubMesh->m_vertexAttributeObj);

		UpdateMaterialUniform(pSubMesh);
		UpdateLightUniform();

		GLint modelMatrixUnif = glGetUniformLocation(m_theProgram, "modelMatrix");
		if ( modelMatrixUnif >= 0 )
			glUniformMatrix4fv(modelMatrixUnif, 1, GL_FALSE, (m_transform.GetTransformMat() * pSubMesh->m_meshMat).m);

		Mat4 viewMatrix = CDirector::GetInstance()->GetCurViewMat();
		GLint viewMatrixUnif = glGetUniformLocation(m_theProgram, "viewMatrix");
		if ( viewMatrixUnif >= 0 )
			glUniformMatrix4fv(viewMatrixUnif, 1, GL_FALSE, viewMatrix.m);

		GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
		if ( perspectiveMatrixUnif >= 0 )
		{
			const Mat4& projMat = CDirector::GetInstance()->GetCurProjectionMat();
			glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
		}

		GLint matrixPaletteUnif = glGetUniformLocation(m_theProgram, "u_matrixPalette");
		if ( matrixPaletteUnif >= 0 )
		{
			glUniform4fv( matrixPaletteUnif, (GLsizei)m_skeleton.m_vBone.size() * 3, (const float*)m_skeleton.GetMatrixPalette() );
		}

		GLint colorUnif = glGetUniformLocation(m_theProgram, "u_color");
		if ( colorUnif >= 0 )
		{
			glUniform4f( colorUnif, m_color.r, m_color.g, m_color.b, m_color.a );
		}

		GLint bFlipNormal = glGetUniformLocation(m_theProgram, "u_flipnormal");
		if (bFlipNormal >= 0)
		{
			glUniform1i(bFlipNormal, m_bFlipNormal);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pSubMesh->m_vertexIndexObj);
		glDrawElements(GL_TRIANGLES, m_meshData.m_vSubMesh[i].m_vFace.size() * 3, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	
	glUseProgram(0);
}

void CMesh::SetMaterial( const CMaterial& rMaterial, int iIndex )
{
	m_vSubMesh[iIndex]->m_material = rMaterial;
}

void CMesh::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
}

void CMesh::SetVisible(bool bVisible, const std::string& sSubMeshName)
{
	int iIndex = -1;
	for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
	{
		if (m_meshData.m_vSubMesh[i].m_MeshName == sSubMeshName)
		{
			iIndex = i;
			break;
		}
	}

	if ( iIndex >= 0 )
	{
		m_vSubMesh[iIndex]->m_bSubMeshVisibility = bVisible;
	}
}

void CMesh::PlayAnim( int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback )
{
	m_animator.PlayAnim(iStartFrameIndex, iEndFrameIndex, bLoop, callback);
}

void CMesh::SetLightEnable( bool bEnable )
{
	m_renderState.m_bEnableLight = bEnable;
}

void CMesh::UpdateLightUniform()
{
	GLint enableLightUnif = glGetUniformLocation(m_theProgram, "u_enableLight");
	if ( enableLightUnif >= 0 )
	{
		glUniform1i(enableLightUnif, m_renderState.m_bEnableLight);
	}

	if (m_renderState.m_bEnableLight)
	{
		GLint eyePosUnif = glGetUniformLocation(m_theProgram, "u_eyePos");
		if ( eyePosUnif >= 0 )
		{
			Vec3 eyePos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
			glUniform3f(eyePosUnif, eyePos.x, eyePos.y, eyePos.z);
		}

		const std::vector<CDirectionalLight>& vAllDirectionalLight = CLightManager::GetInstance()->GetAllDirectionalLights();
		for (int i = 0; i < vAllDirectionalLight.size(); ++i)
		{
			CDirectionalLight* pDirLight = const_cast<CDirectionalLight*>(&vAllDirectionalLight[i]);
			ostringstream oss;
			oss << "u_AllDirLight[" << i << "]";
			GLint unif = glGetUniformLocation(m_theProgram, (oss.str() + ".direction").c_str());
			if ( unif >= 0 )
			{
				if ( pDirLight->m_pDebugMesh )
				{
					pDirLight->m_lightDir = -pDirLight->m_pDebugMesh->m_transform.GetPosition();
				}
				glUniform3f(unif, pDirLight->m_lightDir.x, pDirLight->m_lightDir.y, pDirLight->m_lightDir.z);
			}

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".ambient").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pDirLight->m_ambientColor.x, pDirLight->m_ambientColor.y, pDirLight->m_ambientColor.z);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".diffuse").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pDirLight->m_diffuseColor.x, pDirLight->m_diffuseColor.y, pDirLight->m_diffuseColor.z);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".specular").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pDirLight->m_specularColor.x, pDirLight->m_specularColor.y, pDirLight->m_specularColor.z);
		}

		const std::vector<CPointLight>& vAllPointLight = CLightManager::GetInstance()->GetAllPointLights();
		for (int i = 0; i < vAllPointLight.size(); ++i)
		{
			CPointLight* pPointLight = const_cast<CPointLight*>(&vAllPointLight[i]);
			ostringstream oss;
			oss << "u_AllPointLight[" << i << "]";
			GLint unif = glGetUniformLocation(m_theProgram, (oss.str() + ".position").c_str());
			if ( unif >= 0 )
			{
				if ( pPointLight->m_pDebugMesh )
				{
					pPointLight->m_lightPos = pPointLight->m_pDebugMesh->m_transform.GetPosition();
				}
				glUniform3f(unif, pPointLight->m_lightPos.x, pPointLight->m_lightPos.y, pPointLight->m_lightPos.z);
			}

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".ambient").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pPointLight->m_ambientColor.x, pPointLight->m_ambientColor.y, pPointLight->m_ambientColor.z);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".diffuse").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pPointLight->m_diffuseColor.x, pPointLight->m_diffuseColor.y, pPointLight->m_diffuseColor.z);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".specular").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pPointLight->m_specularColor.x, pPointLight->m_specularColor.y, pPointLight->m_specularColor.z);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".constant").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, pPointLight->m_attenuation_constant);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".linear").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, pPointLight->m_attenuation_linear);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".quadratic").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, pPointLight->m_attenuation_quadratic);
		}

		const std::vector<CSpotLight>& vAllSpotLight = CLightManager::GetInstance()->GetAllSpotLights();
		for (int i = 0; i < vAllSpotLight.size(); ++i)
		{
			CSpotLight* pSpotLight = const_cast<CSpotLight*>(&vAllSpotLight[i]);
			ostringstream oss;
			oss << "u_AllSpotLight[" << i << "]";
			GLint unif = glGetUniformLocation(m_theProgram, (oss.str() + ".position").c_str());
			if ( unif >= 0 )
			{
				if ( pSpotLight->m_pDebugMesh )
				{
					pSpotLight->m_lightPos = pSpotLight->m_pDebugMesh->m_transform.GetPosition();
				}
				glUniform3f(unif, pSpotLight->m_lightPos.x, pSpotLight->m_lightPos.y, pSpotLight->m_lightPos.z);
			}

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".direction").c_str());
			if ( unif >= 0 )
			{
				glUniform3f(unif, pSpotLight->m_lightDir.x, pSpotLight->m_lightDir.y, pSpotLight->m_lightDir.z);
			}

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".ambient").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pSpotLight->m_ambientColor.x, pSpotLight->m_ambientColor.y, pSpotLight->m_ambientColor.z);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".diffuse").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pSpotLight->m_diffuseColor.x, pSpotLight->m_diffuseColor.y, pSpotLight->m_diffuseColor.z);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".specular").c_str());
			if ( unif >= 0 )
				glUniform3f(unif, pSpotLight->m_specularColor.x, pSpotLight->m_specularColor.y, pSpotLight->m_specularColor.z);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".constant").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, pSpotLight->m_attenuation_constant);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".linear").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, pSpotLight->m_attenuation_linear);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".quadratic").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, pSpotLight->m_attenuation_quadratic);

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".innerCutoff").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, DEG_TO_RAD(pSpotLight->fInnerAngle));

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".outerCutoff").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, DEG_TO_RAD(pSpotLight->fOuterAngle));
		}

		if (CDirector::GetInstance()->m_pShadowMap)
		{
			GLint unif = glGetUniformLocation(m_theProgram, "lightSpaceMatrix");
			if (unif >= 0)
			{
				glUniformMatrix4fv(unif, 1, GL_FALSE, (CDirector::GetInstance()->m_pShadowMap->m_lightProjMat * CDirector::GetInstance()->m_pShadowMap->m_lightViewMat).m);
			}

			unif = glGetUniformLocation(m_theProgram, "u_shadowMapTexture");
			if (unif >= 0)
			{
				glUniform1i(unif, 2);

				glActiveTexture(GL_TEXTURE0 + 2);
				glBindTexture(GL_TEXTURE_2D, CDirector::GetInstance()->m_pShadowMap->GetDepthMapTex());
			}
		}
	}
}

void CMesh::UpdateMaterialUniform(CSubMesh* pSubMesh)
{
	if (pSubMesh->m_material.GetBaseColorTex() != -1)
	{
		GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_Material.baseColorTex");
		if ( colorTextureUnif >= 0 )
		{
			glUniform1i(colorTextureUnif, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pSubMesh->m_material.GetBaseColorTex());
			glBindSampler(0, m_Sampler);
		}
	}

	if (pSubMesh->m_material.GetNormalMapTex() != -1)
	{
		GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_Material.normalMapTex");
		if ( colorTextureUnif >= 0 )
		{
			glUniform1i(colorTextureUnif, 1);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, pSubMesh->m_material.GetNormalMapTex());
			glBindSampler(1, m_Sampler);
		}
	}

	GLint bHasNormalMapUnif = glGetUniformLocation(m_theProgram, "u_Material.bHasNormalMap");
	if ( bHasNormalMapUnif >= 0 )
	{
		glUniform1i(bHasNormalMapUnif, int(pSubMesh->m_material.GetNormalMapTex() != -1));
	}

	GLint shininessUnif = glGetUniformLocation(m_theProgram, "u_Material.shininess");
	if ( shininessUnif >= 0 )
	{
		glUniform1f(shininessUnif, pSubMesh->m_material.GetShininess());
	}
}

CMeshSocket::CMeshSocket( const CMesh* pTarget, const std::string& sBoneName, const STransform& offset )
{
	m_sBoneName = sBoneName;
	m_offset = offset;
	m_pTarget = pTarget;
}

Mat4 CMeshSocket::GetWorldMat()
{
	for (auto& rBone : m_pTarget->m_skeleton.m_vBone)
	{
		if ( rBone.m_data.m_sName == m_sBoneName )
		{
			return rBone.m_worldMat * m_offset.GetTransformMat();
		}
	}

	return Mat4::IDENTITY;
}
