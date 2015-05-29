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
	, m_colorTexUnit(0)
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

	Mat4 viewMatrix = CDirector::GetInstance()->GetPerspectiveCamera()->GetViewMat();
	for ( int i = 0; i < m_data.m_vSubMesh.size(); ++i )
	{
		m_MV[i] = viewMatrix * m_transform.GetTransformMat() * m_data.m_vSubMesh[i].m_MeshMatrix;
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

		if ( m_vTexture[i] >= 0 )
		{
			glActiveTexture(GL_TEXTURE0 + m_colorTexUnit);
			glBindTexture(GL_TEXTURE_2D, m_vTexture[i]);
			glBindSampler(m_colorTexUnit, m_Sampler);
		}

		GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
		if ( modelViewMatrixUnif >= 0 )
		{
			glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, m_MV[i].m);
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
		if ( m_bDrawWireFrame )
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glUseProgram(0);
}

void CMesh::SetTexture( const char* pTextureFileName, int iIndex )
{
	unsigned char* pData = nullptr;
	float fWidth = 0;
	float fHeight = 0;

	CImageManager::GetInstance()->Load(pTextureFileName, pData, fWidth, fHeight);
	if ( pData )
	{
		glGenTextures(1, &m_vTexture[iIndex]);
		glBindTexture(GL_TEXTURE_2D, m_vTexture[iIndex]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fWidth, fHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);

		glBindTexture(GL_TEXTURE_2D, 0);

		delete [] pData;
	}
}

void CMesh::InitUniform()
{
	glUseProgram(m_theProgram);

	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_colorTexture");
	if ( colorTextureUnif >= 0 )
	{
		glUniform1i(colorTextureUnif, m_colorTexUnit);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetProjMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	glUseProgram(0);
}

void CMesh::InitMaterial()
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

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void CMesh::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
	InitUniform();
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
