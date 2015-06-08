#include "Mesh.h"
#include "Utility.h"
#include "Camera.h"
#include "Director.h"
#include "Image/ImageManager.h"
#include <algorithm>
#include <iostream>

using namespace std;

CMesh::CMesh()
	: m_Sampler(0)
	, m_theProgram(0)
	, m_bEnableLight(false)
{
}

CMesh::~CMesh()
{
}

void CMesh::InitFromFile( const char* pMeshFileName )
{
	FILE* pMeshFile = fopen((MESH_FILE_DIR + pMeshFileName).c_str(), "rb");
	if ( !pMeshFile )
		return;

	m_data.ReadFromFile(pMeshFile);
	m_vSubMeshVisibility.resize(m_data.m_vSubMesh.size(), true);
	m_MV.resize(m_data.m_vSubMesh.size());
	m_animator.SetTarget(this);

	InitMaterial();
	InitVBOAndVAO();
	InitSkeleton();
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

	for ( auto& rBoneData : m_data.m_skeleton.m_vBone )
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

	for (auto& rBoneIndex : m_data.m_skeleton.m_vSkinBone)
	{
		CBone* pBoneFound = FindCBoneByIndex(rBoneIndex);
		if ( pBoneFound )
		{
			m_skeleton.m_vSkinBone.push_back(pBoneFound);
		}
	}

	STransform temp;
	temp.m_rotation.y = 180;
	temp.m_pos.x += 50;
	temp.m_pos.z -= 80;
	CMeshSocket sock(this, "b_Missle_L", temp);
	m_vSocket.push_back(sock);
}

void CMesh::Update(float dt)
{
	m_animator.Update(dt);

	for ( int i = 0; i < m_data.m_vSubMesh.size(); ++i )
	{
		m_MV[i] = m_transform.GetTransformMat() * m_data.m_vSubMesh[i].m_MeshMatrix;
	}
}

void CMesh::Render()
{
	if ( m_bEnableCullFace )
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	if ( m_bDrawWireFrame )
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_theProgram);

	for ( int i = 0; i < m_data.m_vSubMesh.size(); ++i )
	{
		if ( !m_vSubMeshVisibility[i] )
			continue;

		glBindVertexArray(m_vertexAttributeObj[i]);

		UpdateMaterialUniform(i);
		UpdateLightUniform();

		GLint modelMatrixUnif = glGetUniformLocation(m_theProgram, "modelMatrix");
		if ( modelMatrixUnif >= 0 )
			glUniformMatrix4fv(modelMatrixUnif, 1, GL_FALSE, m_MV[i].m);

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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj[i]);
		glDrawElements(GL_TRIANGLES, m_data.m_vSubMesh[i].m_vFace.size() * 3, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		if ( m_bDrawWireFrame )
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glUseProgram(0);
}

void CMesh::SetMaterial( const CMaterial& rMaterial, int iIndex )
{
	m_vMaterial[iIndex] = rMaterial;
}

void CMesh::InitMaterial()
{
	int iSubMeshCount = m_data.m_vSubMesh.size();
	m_vMaterial.resize(iSubMeshCount);

	for ( int i = 0; i < iSubMeshCount; ++i )
	{
		std::string sTextureFile;
		if ( !m_data.m_vSubMesh[i].m_cMaterial.m_SubTextureVec.empty() )
			sTextureFile = m_data.m_vSubMesh[i].m_cMaterial.m_SubTextureVec[0].m_sFileName;

		if ( !sTextureFile.empty() )
		{
			CMaterial newMaterial;
			newMaterial.SetBaseColorTexture(sTextureFile);
			SetMaterial(newMaterial, i);
		}
	}

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void CMesh::InitVBOAndVAO()
{
	int iSubMeshCount = m_data.m_vSubMesh.size();
	m_vertexDataObj.resize(iSubMeshCount);
	m_vertexIndexObj.resize(iSubMeshCount);
	m_vertexAttributeObj.resize(iSubMeshCount);

	for ( int i = 0; i < iSubMeshCount; ++i )
	{
		glGenBuffers(1, &m_vertexDataObj[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj[i]);
		glBufferData(GL_ARRAY_BUFFER, m_data.m_vSubMesh[i].m_vVectex.size() * sizeof(SSkinMeshVertex), &m_data.m_vSubMesh[i].m_vVectex.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &m_vertexIndexObj[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_data.m_vSubMesh[i].m_vFace.size() * sizeof(SFace), &m_data.m_vSubMesh[i].m_vFace.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &m_vertexAttributeObj[i]);
		glBindVertexArray(m_vertexAttributeObj[i]);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj[i]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*) offsetof(SSkinMeshVertex, m_position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*) offsetof(SSkinMeshVertex, m_texCoord));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*) offsetof(SSkinMeshVertex, m_boneIndex));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*) offsetof(SSkinMeshVertex, m_blendWeight));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*) offsetof(SSkinMeshVertex, m_normal));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(SSkinMeshVertex), (GLvoid*) offsetof(SSkinMeshVertex, m_tangent));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void CMesh::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
}

void CMesh::SetVisible(bool bVisible, const std::string& sSubMeshName)
{
	int iIndex = -1;
	for (int i = 0; i < m_data.m_vSubMesh.size(); ++i)
	{
		if ( m_data.m_vSubMesh[i].m_MeshName == sSubMeshName )
		{
			iIndex = i;
			break;
		}
	}

	if ( iIndex >= 0 )
	{
		m_vSubMeshVisibility[iIndex] = bVisible;
	}
}

void CMesh::PlayAnim( int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback )
{
	m_animator.PlayAnim(iStartFrameIndex, iEndFrameIndex, bLoop, callback);
}

void CMesh::SetLightEnable( bool bEnable )
{
	m_bEnableLight = bEnable;
}

void CMesh::UpdateLightUniform()
{
	GLint enableLightUnif = glGetUniformLocation(m_theProgram, "u_enableLight");
	if ( enableLightUnif >= 0 )
	{
		glUniform1i(enableLightUnif, m_bEnableLight);
	}

	if ( m_bEnableLight )
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
					pDirLight->m_lightDir = -pDirLight->m_pDebugMesh->m_transform.m_pos;
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
					pPointLight->m_lightPos = pPointLight->m_pDebugMesh->m_transform.m_pos;
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
					pSpotLight->m_lightPos = pSpotLight->m_pDebugMesh->m_transform.m_pos;
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
				glUniform1f(unif, DEGREES_TO_RADIANS(pSpotLight->fInnerAngle));

			unif = glGetUniformLocation(m_theProgram, (oss.str() + ".outerCutoff").c_str());
			if ( unif >= 0 )
				glUniform1f(unif, DEGREES_TO_RADIANS(pSpotLight->fOuterAngle));
		}
	}
}

void CMesh::UpdateMaterialUniform( int i )
{
	if ( m_vMaterial[i].GetBaseColorTex() != -1 )
	{
		GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_Material.baseColorTex");
		if ( colorTextureUnif >= 0 )
		{
			glUniform1i(colorTextureUnif, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_vMaterial[i].GetBaseColorTex());
			glBindSampler(0, m_Sampler);
		}
	}

	if ( m_vMaterial[i].GetNormalMapTex() != -1 )
	{
		GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_Material.normalMapTex");
		if ( colorTextureUnif >= 0 )
		{
			glUniform1i(colorTextureUnif, 1);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, m_vMaterial[i].GetNormalMapTex());
			glBindSampler(1, m_Sampler);
		}
	}

	GLint bHasNormalMapUnif = glGetUniformLocation(m_theProgram, "u_Material.bHasNormalMap");
	if ( bHasNormalMapUnif >= 0 )
	{
		glUniform1i(bHasNormalMapUnif, int(m_vMaterial[i].GetNormalMapTex() != -1));
	}

	GLint shininessUnif = glGetUniformLocation(m_theProgram, "u_Material.shininess");
	if ( shininessUnif >= 0 )
	{
		glUniform1f(shininessUnif, m_vMaterial[i].GetShininess());
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
