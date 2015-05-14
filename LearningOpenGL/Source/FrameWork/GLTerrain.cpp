#include "GLTerrain.h"

#include "Image/PNGReader.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"

const int conChunkSize = 32;

CGLTerrain::CGLTerrain( const std::string& sHeightMapFile )
	: m_bDrawWireFrame(false)
{
	CPNGReader pngReader(sHeightMapFile);
	if ( pngReader.GetData() )
	{
		unsigned char* pData = new unsigned char[ int(pngReader.GetWidth()) * int(pngReader.GetHeight()) ];
		int iBlockSize = 4;
		if ( pngReader.GetImageType() == CImageReader::EImageType_RGB888 )
			iBlockSize = 3;

		for ( int i = 0; i < int(pngReader.GetWidth()) * int(pngReader.GetHeight()); ++i )
		{
			pData[i] = pngReader.GetData()[i * iBlockSize];
		}

		InitTerrain(pData, pngReader.GetWidth(), pngReader.GetHeight());
		SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("Terrain"));
	}
}

void CGLTerrain::InitTerrain(const unsigned char* pHeightMapData, int iWidth, int iHeight)
{
	for (int i = 0; i < iHeight; ++i)
	{
		for (int j = 0; j < iWidth; ++j)
		{
			SCommonVertex newVertex;
			float fHeight = pHeightMapData[ i * iWidth + j ];
			newVertex.m_pos = Vec3( j, fHeight, i);
			newVertex.m_color = Color4F::WHITE;
		
			m_vGlobalVertex.push_back(newVertex);
		}
	}

	m_iChunkCountX = iWidth / conChunkSize;
	m_iChunkCountY = iHeight / conChunkSize;

	m_vChunk = new SChunk**[m_iChunkCountY]();
	for (int i = 0; i < m_iChunkCountY; ++i)
	{
		m_vChunk[i] = new SChunk*[m_iChunkCountX]();
		for (int j = 0; j < m_iChunkCountX; ++j)
		{
			SChunk* pNewChunk = new SChunk;
			m_vChunk[i][j] = pNewChunk;
			pNewChunk->m_iCurLOD = RANDOM_0_1() * conMaxLOD;
			glGenBuffers(1, &pNewChunk->m_vertexIndexObj);

			int iBase = i * conChunkSize * iWidth + j * conChunkSize;
			for ( int iLOD = 0; iLOD < conMaxLOD; ++iLOD )
			{
				int iStep = pow(2, iLOD);
				SChunkLOD& rLOD = pNewChunk->m_vLOD[iLOD];
				for (int m = 0; m < conChunkSize; m += iStep)
				{
					for (int n = 0; n < conChunkSize; n += iStep)
					{
						assert(iBase + m * iWidth + n < m_vGlobalVertex.size());
						assert(iBase + m * iWidth + n + iStep < m_vGlobalVertex.size());
						assert(iBase + (m + iStep) * iWidth + n < m_vGlobalVertex.size());

						assert(iBase + m * iWidth + n + iStep < m_vGlobalVertex.size());
						assert(iBase + (m + iStep) * iWidth + n + iStep < m_vGlobalVertex.size());
						assert(iBase + (m + iStep) * iWidth + n < m_vGlobalVertex.size());

						rLOD.m_vIndex.push_back(iBase + m * iWidth + n);
						rLOD.m_vIndex.push_back(iBase + m * iWidth + n + iStep);
						rLOD.m_vIndex.push_back(iBase + (m + iStep) * iWidth + n);

						rLOD.m_vIndex.push_back(iBase + m * iWidth + n + iStep);
						rLOD.m_vIndex.push_back(iBase + (m + iStep) * iWidth + n + iStep);
						rLOD.m_vIndex.push_back(iBase + (m + iStep) * iWidth + n);
					}
				}
			}

			Color4F randomColor = Color4F( RANDOM_0_1(), RANDOM_0_1(), RANDOM_0_1(), 1.0f );
			for ( int i = 0; i < pNewChunk->m_vLOD[0].m_vIndex.size(); ++i )
			{
				unsigned int iIndex = pNewChunk->m_vLOD[0].m_vIndex[i];
				m_vGlobalVertex[iIndex].m_color = randomColor;
			}
		}
	}

	glGenBuffers(1, &m_vertexDataObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, m_vGlobalVertex.size() * sizeof(SCommonVertex), &m_vGlobalVertex.front(), GL_STATIC_DRAW);
}

void CGLTerrain::SetGLProgram( GLint theProgram )
{
	m_theProgram = theProgram;
}

void CGLTerrain::Update( float deltaTime )
{

}

void CGLTerrain::Render()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_theProgram);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, m_vGlobalVertex.size() * sizeof(SCommonVertex), &m_vGlobalVertex.front(), GL_DYNAMIC_DRAW);

	glBindVertexArray(m_vertexAttributeObj);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_UV));

	GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
	if ( modelViewMatrixUnif >= 0 )
	{
		Mat4 viewMatrix = CDirector::GetInstance()->GetPerspectiveCamera()->GetViewMat();
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, (viewMatrix * m_transform.GetTransformMat()).m);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetProjMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	for (int i = 0; i < m_iChunkCountY; ++i)
	{
		for (int j = 0; j < m_iChunkCountX; ++j)
		{
			SChunk* pChunk = m_vChunk[i][j];

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pChunk->m_vertexIndexObj);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, pChunk->m_vLOD[pChunk->m_iCurLOD].m_vIndex.size() * sizeof(pChunk->m_vLOD[pChunk->m_iCurLOD].m_vIndex[0]), &pChunk->m_vLOD[pChunk->m_iCurLOD].m_vIndex.front(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pChunk->m_vertexIndexObj);
			glDrawElements(GL_TRIANGLES, pChunk->m_vLOD[pChunk->m_iCurLOD].m_vIndex.size(), GL_UNSIGNED_INT, 0);
		}
	}

	glUseProgram(0);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

