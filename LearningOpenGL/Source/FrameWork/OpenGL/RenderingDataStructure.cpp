#include "RenderingDataStructure.h"
#include "../Utility.h"
#include "../Image/PNGReader.h"
#include <algorithm>
#include <iostream>

using namespace std;

COGLMesh::COGLMesh()
	: m_Sampler(0)
	, m_theProgram(0)
	, m_colorTexUnit(0)
{
	m_scale.set(1.0f, 1.0f, 1.0f);
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
	m_animator.SetTarget(this);

	//for (auto& rVertex : m_data.m_vChildMesh[0].m_vVectex)
	//{
	//	for (int i = 0; i < 4; ++i)
	//	{
	//		cout << rVertex.m_blendWeight[i] << " "; 
	//	}
	//	cout << endl;

	//	for (int i = 0; i < 4; ++i)
	//	{
	//		cout << rVertex.m_boneIndex[i] << " "; 
	//	}
	//	cout << endl << endl;
	//}

	InitProgram();
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glUseProgram(m_theProgram);

	Mat4 viewMatrix = Mat4::CreateFromTranslation(0.0f, 0.0f, -100.0f);
	Mat4 ScaleMatrix = Mat4::CreateFromScale(m_scale.x, m_scale.y, m_scale.z);
	Mat4 RotationMatrix = Mat4::CreateFromRotation(m_rotation.x, m_rotation.y, m_rotation.z);
	Mat4 TranslationMatrix = Mat4::CreateFromTranslation(m_worldPos.x, m_worldPos.y, m_worldPos.z);

	for ( int i = 0; i < m_data.m_vChildMesh.size(); ++i )
	{
		glBindVertexArray(m_vertexAttributeObj[i]);

		if ( m_vTexture[i] > 0 )
		{
			glActiveTexture(GL_TEXTURE0 + m_colorTexUnit);
			glBindTexture(GL_TEXTURE_2D, m_vTexture[i]);
			glBindSampler(m_colorTexUnit, m_Sampler);
		}

		Mat4 ModelViewMatrix;
		ModelViewMatrix = viewMatrix * TranslationMatrix * ScaleMatrix * RotationMatrix * m_data.m_vChildMesh[i].m_MeshMatrix;

		GLuint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, ModelViewMatrix.m);

		GLuint matrixPaletteUnif = glGetUniformLocation(m_theProgram, "u_matrixPalette");
		glUniform4fv( matrixPaletteUnif, (GLsizei)m_skeleton.m_vBone.size() * 3, (const float*)m_skeleton.GetMatrixPalette() );

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj[i]);
		glDrawElements(GL_TRIANGLES, m_data.m_vChildMesh[i].m_vFace.size() * 3, GL_UNSIGNED_INT, 0);

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

void COGLMesh::InitProgram()
{
	std::vector<GLuint> shaderList;
	shaderList.push_back(OpenGLFramework::LoadShader(GL_VERTEX_SHADER, SHADER_FILE_DIR + "SkinMesh_Vertex_Shader.vert"));
	shaderList.push_back(OpenGLFramework::LoadShader(GL_FRAGMENT_SHADER, SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag"));
	m_theProgram = OpenGLFramework::CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	GLuint colorTextureUnif = glGetUniformLocation(m_theProgram, "colorTexture");
	glUseProgram(m_theProgram);
	glUniform1i(colorTextureUnif, m_colorTexUnit);

	GLuint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	Mat4 perspectiveMatrix = Mat4::createPerspective(90.0f, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f);
	glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, perspectiveMatrix.m);

	glUseProgram(0);
}

void COGLMesh::InitMaterial()
{
	int iSubMeshCount = m_data.m_vChildMesh.size();
	m_vTexture.resize(iSubMeshCount);

	for ( int i = 0; i < iSubMeshCount; ++i )
	{
		std::string sTextureFile;
		if ( !m_data.m_vChildMesh[i].m_cMaterial.m_SubTextureVec.empty() )
			sTextureFile = m_data.m_vChildMesh[i].m_cMaterial.m_SubTextureVec[0].m_sFileName;

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
	int iSubMeshCount = m_data.m_vChildMesh.size();
	m_vertexDataObj.resize(iSubMeshCount);
	m_vertexIndexObj.resize(iSubMeshCount);
	m_vertexAttributeObj.resize(iSubMeshCount);

	for ( int i = 0; i < iSubMeshCount; ++i )
	{
		glGenBuffers(1, &m_vertexDataObj[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj[i]);
		glBufferData(GL_ARRAY_BUFFER, m_data.m_vChildMesh[i].m_vVectex.size() * sizeof(SVertex), &m_data.m_vChildMesh[i].m_vVectex.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &m_vertexIndexObj[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_data.m_vChildMesh[i].m_vFace.size() * sizeof(SFace), &m_data.m_vChildMesh[i].m_vFace.front(), GL_STATIC_DRAW);
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


