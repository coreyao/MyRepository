#include "RenderingDataStructure.h"
#include "../Utility.h"
#include "../Image/PNGReader.h"
#include <algorithm>

COGLMesh::COGLMesh()
	: m_vertexDataObj(0)
	, m_vertexIndexObj(0)
	, m_vertexAttributeObj(0)
	, m_Texture(0)
	, m_Sampler(0)
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

	InitProgram();
	InitMaterial();
	InitVBOAndVAO();
	InitSkeleton();
}

void COGLMesh::InitSkeleton()
{
	for ( auto& rBoneData : m_data.m_skeleton.m_vBone )
	{
		CBone newBone;
		newBone.m_data = rBoneData;
		newBone.m_localMat = rBoneData.m_originalBindMat;
		newBone.m_worldMat = rBoneData.m_originalBindMat;
		m_skeleton.m_vBone.push_back(newBone);
	}

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

void COGLMesh::Render()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glUseProgram(m_theProgram);
	glBindVertexArray(m_vertexAttributeObj);

	if ( m_Texture > 0 )
	{
		glActiveTexture(GL_TEXTURE0 + m_colorTexUnit);
		glBindTexture(GL_TEXTURE_2D, m_Texture);
		glBindSampler(m_colorTexUnit, m_Sampler);
	}

	Mat4 viewMatrix;
	cml::matrix_translation(viewMatrix, Vec3(0.0f, 0.0f, -100.0f));

	Mat4 ScaleMatrix;
	cml::matrix_scale(ScaleMatrix, m_scale);

	Mat4 RotationMatrix;
	cml::matrix_rotation_euler(RotationMatrix, m_rotation[0], m_rotation[1], m_rotation[2], cml::euler_order_xyz);

	Mat4 TranslationMatrix;
	cml::matrix_translation(TranslationMatrix, m_worldPos);

	Mat4 ModelViewMatrix;
	ModelViewMatrix = viewMatrix * TranslationMatrix * ScaleMatrix * RotationMatrix;

	GLuint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
	glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, ModelViewMatrix.data());

	GLuint matrixPaletteUnif = glGetUniformLocation(m_theProgram, "u_matrixPalette");
	glUniform4fv( matrixPaletteUnif, (GLsizei)m_skeleton.m_vBone.size() * 3, (const float*)m_skeleton.GetMatrixPalette() );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, m_data.m_vFace.size() * 3, GL_UNSIGNED_INT, 0);

	glBindSampler(m_colorTexUnit, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void COGLMesh::SetTexture( const char* pTextureFileName )
{
	CPNGReader pngReader(pTextureFileName);
	if ( pngReader.GetData() )
	{
		glGenTextures(1, &m_Texture);
		glBindTexture(GL_TEXTURE_2D, m_Texture);

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
	Mat4 perspectiveMatrix;
	cml::matrix_perspective_xfov_RH(perspectiveMatrix, 90.0f, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f, cml::z_clip_neg_one);
	glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, perspectiveMatrix.data());

	glUseProgram(0);
}

void COGLMesh::InitMaterial()
{
	std::string sTextureFile;
	if ( !m_data.m_cMaterial.m_SubTextureVec.empty() )
		sTextureFile = m_data.m_cMaterial.m_SubTextureVec[0].m_sFileName;

	if ( !sTextureFile.empty() )
		SetTexture(sTextureFile.c_str());

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void COGLMesh::InitVBOAndVAO()
{
	glGenBuffers(1, &m_vertexDataObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, m_data.m_vVectex.size() * sizeof(SVertex), &m_data.m_vVectex.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_vertexIndexObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_data.m_vFace.size() * sizeof(SFace), &m_data.m_vFace.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_vertexAttributeObj);
	glBindVertexArray(m_vertexAttributeObj);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)(12 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)(16 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


