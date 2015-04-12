#include "GLMesh.h"
#include "Utility.h"
#include "Image/PNGReader.h"
#include "Camera.h"
#include "Director.h"
#include <algorithm>
#include <iostream>

using namespace std;

COGLMesh::COGLMesh()
	: m_Sampler(0)
	, m_theProgram(0)
	, m_colorTexUnit(0)
{
}

COGLMesh::~COGLMesh()
{
}

void COGLMesh::InitFromFile( const char* pMeshFileName )
{
	FILE* pMeshFile = fopen((MESH_FILE_DIR + pMeshFileName).c_str(), "rb");
	if ( !pMeshFile )
		return;

	m_data.ReadFromFile(pMeshFile);
	m_vSubMeshVisibility.resize(m_data.m_vSubMesh.size(), true);
	m_animator.SetTarget(this);

	InitMaterial();
	InitVBOAndVAO();
	InitSkeleton();
}

void COGLMesh::InitSkeleton()
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
}

void COGLMesh::Update(float dt)
{
	m_animator.Update(dt);
}

void COGLMesh::Render()
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

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glUseProgram(m_theProgram);

	Mat4 viewMatrix = CDirector::GetInstance()->GetCurCamera()->GetViewMat();
	for ( int i = 0; i < m_data.m_vSubMesh.size(); ++i )
	{
		if ( !m_vSubMeshVisibility[i] )
			continue;

		glBindVertexArray(m_vertexAttributeObj[i]);

		if ( m_vTexture[i] >= 0 )
		{
			glActiveTexture(GL_TEXTURE0 + m_colorTexUnit);
			glBindTexture(GL_TEXTURE_2D, m_vTexture[i]);
			glBindSampler(m_colorTexUnit, m_Sampler);
		}

		GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
		if ( modelViewMatrixUnif >= 0 )
		{
			Mat4 temp = m_transform.GetTransformMat();

			Mat4 BillboardMatrix = Mat4::IDENTITY;
			Vec3 forward = m_transform.GetTransformMat().Inverse() * CDirector::GetInstance()->GetCurCamera()->GetEyePos();
			forward.normalize();
			Vec3 up(0, 0, 1);
			Vec3 right = forward.Cross(up);
			right.normalize();
			up = right.Cross(forward);
			up.normalize();
			BillboardMatrix.SetRight(right.x, right.y, right.z);
			BillboardMatrix.SetForward(up.x, up.y, up.z);
			BillboardMatrix.SetUp(forward.x, forward.y, forward.z);

			Mat4 ModelViewMatrix = BillboardMatrix * m_transform.GetTransformMat() * m_data.m_vSubMesh[i].m_MeshMatrix;
			ModelViewMatrix = viewMatrix * ModelViewMatrix;
			glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, ModelViewMatrix.m);
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

		glBindSampler(m_colorTexUnit, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	
	glUseProgram(0);
}

void COGLMesh::SetTexture( const char* pTextureFileName, int iIndex )
{
	CPNGReader pngReader(pTextureFileName);
	if ( pngReader.GetData() )
	{
		glGenTextures(1, &m_vTexture[iIndex]);
		glBindTexture(GL_TEXTURE_2D, m_vTexture[iIndex]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pngReader.GetWidth(), pngReader.GetHeight(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pngReader.GetData());

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void COGLMesh::InitUniform()
{
	glUseProgram(m_theProgram);

	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "colorTexture");
	if ( colorTextureUnif >= 0 )
	{
		glUniform1i(colorTextureUnif, m_colorTexUnit);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetCurCamera()->GetProjMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	glUseProgram(0);
}

void COGLMesh::InitMaterial()
{
	int iSubMeshCount = m_data.m_vSubMesh.size();
	m_vTexture.resize(iSubMeshCount);

	for ( int i = 0; i < iSubMeshCount; ++i )
	{
		std::string sTextureFile;
		if ( !m_data.m_vSubMesh[i].m_cMaterial.m_SubTextureVec.empty() )
			sTextureFile = m_data.m_vSubMesh[i].m_cMaterial.m_SubTextureVec[0].m_sFileName;

		if ( !sTextureFile.empty() )
			SetTexture(sTextureFile.c_str(), i);
	}

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void COGLMesh::InitVBOAndVAO()
{
	int iSubMeshCount = m_data.m_vSubMesh.size();
	m_vertexDataObj.resize(iSubMeshCount);
	m_vertexIndexObj.resize(iSubMeshCount);
	m_vertexAttributeObj.resize(iSubMeshCount);

	for ( int i = 0; i < iSubMeshCount; ++i )
	{
		glGenBuffers(1, &m_vertexDataObj[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj[i]);
		glBufferData(GL_ARRAY_BUFFER, m_data.m_vSubMesh[i].m_vVectex.size() * sizeof(SVertex), &m_data.m_vSubMesh[i].m_vVectex.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &m_vertexIndexObj[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_data.m_vSubMesh[i].m_vFace.size() * sizeof(SFace), &m_data.m_vSubMesh[i].m_vFace.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &m_vertexAttributeObj[i]);
		glBindVertexArray(m_vertexAttributeObj[i]);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj[i]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(SVertex, m_position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(SVertex, m_texCoord));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(SVertex, m_boneIndex));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(SVertex, m_blendWeight));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void COGLMesh::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
	InitUniform();
}

void COGLMesh::SetVisible(bool bVisible, const std::string& sSubMeshName)
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


