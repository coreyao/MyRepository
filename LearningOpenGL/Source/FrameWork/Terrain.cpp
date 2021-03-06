#include "Terrain.h"

#include "Image/PNGReader.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include <iterator>

const int conChunkSize = 32;

CTerrain::CTerrain()
	: m_bDrawWireFrame(false)
	, m_iHeightMapWidth(0)
	, m_iHeightMapHeight(0)
{
}

void CTerrain::InitTerrain(const unsigned char* pHeightMapData, int iWidth, int iHeight)
{
	m_iHeightMapWidth = iWidth;
	m_iHeightMapHeight = iHeight;

	LoadVertex(pHeightMapData, iWidth, iHeight);

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
			GenerateChunk(pNewChunk, j, i);
		}
	}
	InitNeighbor();
	m_pRoot = new SQuadNode(0, 0, m_iHeightMapWidth, m_iHeightMapHeight, nullptr, this);
}

void CTerrain::SetGLProgram( GLint theProgram )
{
	m_theProgram = theProgram;
}

void CTerrain::Update( float deltaTime )
{
	UpdateChunkLOD();
	UpdateCrackFix();
}

void CTerrain::Render()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	if ( m_bDrawWireFrame )
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

	UpdateUniform();

	for (int i = 0; i < m_vDetailTexture.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_vDetailTexture[i]);
		glBindSampler(i, m_detailTexSampler);
	}

	glActiveTexture(GL_TEXTURE0 + m_vDetailTexture.size());
	glBindTexture(GL_TEXTURE_2D, m_iAlphaTexture);
	glBindSampler(m_vDetailTexture.size(), m_iAlphaSampler);

	GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
	if ( modelViewMatrixUnif >= 0 )
	{
		Mat4 viewMatrix = CDirector::GetInstance()->GetCurViewMat();
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, (viewMatrix * m_transform.GetTransformMat()).m);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetCurProjectionMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	VisitQuadTree(m_pRoot, std::bind(&CTerrain::IsInFrustrum, this, std::placeholders::_1), std::bind(&CTerrain::RenderChunk, this, std::placeholders::_1));

	/*for (int i = 0; i < m_iChunkCountY; ++i)
	{
	for (int j = 0; j < m_iChunkCountX; ++j)
	{
	SChunk* pChunk = m_vChunk[i][j];

	std::vector<unsigned int> vIndex = pChunk->m_vLOD[pChunk->m_iCurLOD].m_vIndex;
	std::copy(pChunk->m_vFixCrack.m_vIndex.begin(), pChunk->m_vFixCrack.m_vIndex.end(), std::back_inserter(vIndex));

	if ( !vIndex.empty() )
	{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pChunk->m_vertexIndexObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vIndex.size() * sizeof(vIndex[0]), &vIndex.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pChunk->m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, vIndex.size(), GL_UNSIGNED_INT, 0);
	}
	}
	}*/

	glUseProgram(0);
	glBindVertexArray(0);

	if ( m_bDrawWireFrame )
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CTerrain::RenderChunk( const SChunk* pChunk )
{
	std::vector<unsigned int> vIndex = pChunk->m_vLOD[pChunk->m_iCurLOD].m_vIndex;
	std::copy(pChunk->m_vFixCrack.m_vIndex.begin(), pChunk->m_vFixCrack.m_vIndex.end(), std::back_inserter(vIndex));

	if ( !vIndex.empty() )
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pChunk->m_vertexIndexObj);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vIndex.size() * sizeof(vIndex[0]), &vIndex.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pChunk->m_vertexIndexObj);
		glDrawElements(GL_TRIANGLES, vIndex.size(), GL_UNSIGNED_INT, 0);
	}
}

void CTerrain::UpdateChunkLOD()
{
	VisitQuadTree(m_pRoot, std::bind(&CTerrain::UpdateChunkLODInternal, this, std::placeholders::_1));
}

void CTerrain::UpdateCrackFixInternal( SChunk* pCurChunk )
{
	int iCurStep = pow(2, pCurChunk->m_iCurLOD);

	SChunkLOD& rFixCrackLOD = pCurChunk->m_vFixCrack;
	rFixCrackLOD.m_vIndex.clear();

	for (int n = 0; n < 4; ++n)
	{
		SChunk* pNeighbor = pCurChunk->m_vNeighbor[n];
		if ( pNeighbor && pCurChunk->m_iCurLOD < pNeighbor->m_iCurLOD )
		{
			int iNeighborStep = pow(2, pNeighbor->m_iCurLOD);
			int iBase = pCurChunk->m_y * conChunkSize * m_iHeightMapWidth + pCurChunk->m_x * conChunkSize;
			EChunkNeighbor eNeighbor = (EChunkNeighbor)n;
			switch (eNeighbor)
			{
			case CTerrain::EChunkNeighbor_Left:
				{
					for (int m = 0; m < conChunkSize; m += iNeighborStep)
					{
						int iIndex_1 = iBase + m * m_iHeightMapWidth; 
						int iIndex_2 = iBase + (m + iCurStep) * m_iHeightMapWidth; 
						int iIndex_3 = iBase +(m + iCurStep + iCurStep) * m_iHeightMapWidth;
						int iIndex_4 = iIndex_3 - iNeighborStep;

						rFixCrackLOD.m_vIndex.push_back(iIndex_1);
						rFixCrackLOD.m_vIndex.push_back(iIndex_3);
						rFixCrackLOD.m_vIndex.push_back(iIndex_2);

						rFixCrackLOD.m_vIndex.push_back(iIndex_1);
						rFixCrackLOD.m_vIndex.push_back(iIndex_2);
						rFixCrackLOD.m_vIndex.push_back(iIndex_4);

						rFixCrackLOD.m_vIndex.push_back(iIndex_2);
						rFixCrackLOD.m_vIndex.push_back(iIndex_3);
						rFixCrackLOD.m_vIndex.push_back(iIndex_4);
					}
				}
				break;
			case CTerrain::EChunkNeighbor_Right:
				{
					for (int m = 0; m < conChunkSize; m += iNeighborStep)
					{
						int iIndex_1 = iBase + m * m_iHeightMapWidth + conChunkSize; 
						int iIndex_2 = iBase + (m + iCurStep) * m_iHeightMapWidth + conChunkSize; 
						int iIndex_3 = iBase +(m + iCurStep + iCurStep) * m_iHeightMapWidth + conChunkSize;
						int iIndex_4 = iIndex_1 + iNeighborStep;

						rFixCrackLOD.m_vIndex.push_back(iIndex_1);
						rFixCrackLOD.m_vIndex.push_back(iIndex_2);
						rFixCrackLOD.m_vIndex.push_back(iIndex_3);

						rFixCrackLOD.m_vIndex.push_back(iIndex_1);
						rFixCrackLOD.m_vIndex.push_back(iIndex_4);
						rFixCrackLOD.m_vIndex.push_back(iIndex_2);

						rFixCrackLOD.m_vIndex.push_back(iIndex_2);
						rFixCrackLOD.m_vIndex.push_back(iIndex_4);
						rFixCrackLOD.m_vIndex.push_back(iIndex_3);
					}
				}
				break;
			case CTerrain::EChunkNeighbor_Up:
				{
					for (int m = 0; m < conChunkSize; m += iNeighborStep)
					{
						int iIndex_1 = iBase + m; 
						int iIndex_2 = iBase + m + iCurStep; 
						int iIndex_3 = iBase + m + iCurStep + iCurStep; 
						int iIndex_4 = iIndex_3 - iNeighborStep * m_iHeightMapWidth;

						rFixCrackLOD.m_vIndex.push_back(iIndex_1);
						rFixCrackLOD.m_vIndex.push_back(iIndex_2);
						rFixCrackLOD.m_vIndex.push_back(iIndex_3);

						rFixCrackLOD.m_vIndex.push_back(iIndex_1);
						rFixCrackLOD.m_vIndex.push_back(iIndex_4);
						rFixCrackLOD.m_vIndex.push_back(iIndex_2);

						rFixCrackLOD.m_vIndex.push_back(iIndex_2);
						rFixCrackLOD.m_vIndex.push_back(iIndex_4);
						rFixCrackLOD.m_vIndex.push_back(iIndex_3);
					}
				}
				break;
			case CTerrain::EChunkNeighbor_Bottom:
				{
					for (int m = 0; m < conChunkSize; m += iNeighborStep)
					{
						int iIndex_1 = iBase + conChunkSize * m_iHeightMapWidth + m; 
						int iIndex_2 = iBase + conChunkSize * m_iHeightMapWidth + m + iCurStep; 
						int iIndex_3 = iBase + conChunkSize * m_iHeightMapWidth + m + iCurStep + iCurStep; 
						int iIndex_4 = iIndex_1 + iNeighborStep * m_iHeightMapWidth;

						rFixCrackLOD.m_vIndex.push_back(iIndex_1);
						rFixCrackLOD.m_vIndex.push_back(iIndex_3);
						rFixCrackLOD.m_vIndex.push_back(iIndex_2);

						rFixCrackLOD.m_vIndex.push_back(iIndex_1);
						rFixCrackLOD.m_vIndex.push_back(iIndex_2);
						rFixCrackLOD.m_vIndex.push_back(iIndex_4);

						rFixCrackLOD.m_vIndex.push_back(iIndex_2);
						rFixCrackLOD.m_vIndex.push_back(iIndex_3);
						rFixCrackLOD.m_vIndex.push_back(iIndex_4);
					}
				}
				break;
			default:
				break;
			}
		}
	}
}
void CTerrain::SetLODThreshold( float LOD_1, float LOD_2, float LOD_3 )
{
	m_vLODThreshold[0] = LOD_1;
	m_vLODThreshold[1] = LOD_2;
	m_vLODThreshold[2] = LOD_3;
}

void CTerrain::SetDetailTexture( const std::string& sTex1, const std::string& sTex2, const std::string& sTex3, const std::string& sTex4 )
{
	std::vector<std::string> vDetailTex;
	if ( !sTex1.empty() )
		vDetailTex.push_back(sTex1);
	if ( !sTex2.empty() )
		vDetailTex.push_back(sTex2);
	if ( !sTex3.empty() )
		vDetailTex.push_back(sTex3);
	if ( !sTex4.empty() )
		vDetailTex.push_back(sTex4);

	m_vDetailTexture.resize( vDetailTex.size() );

	for (int i = 0; i < vDetailTex.size(); ++i)
	{
		CPNGReader pngReader(vDetailTex[i]);
		if ( pngReader.GetData() )
		{
			glGenTextures(1, &m_vDetailTexture[i]);
			glBindTexture(GL_TEXTURE_2D, m_vDetailTexture[i]);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pngReader.GetWidth(), pngReader.GetHeight(), 0,
				GL_RGBA, GL_UNSIGNED_BYTE, pngReader.GetData());

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glGenSamplers(1, &m_detailTexSampler);
	glSamplerParameteri(m_detailTexSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_detailTexSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_detailTexSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_detailTexSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void CTerrain::SetDetailTextureSize( int iTex1Size, int iTex2Size /*= 20*/, int iTex3Size /*= 20*/, int iTex4Size /*= 20*/ )
{
	m_vDetailTexSize.push_back(iTex1Size);
	m_vDetailTexSize.push_back(iTex2Size);
	m_vDetailTexSize.push_back(iTex3Size);
	m_vDetailTexSize.push_back(iTex4Size);
}

void CTerrain::SetAlphaTexture( const std::string& sAlphaMap )
{
	if ( sAlphaMap.empty() )
		return;

	CPNGReader pngReader(sAlphaMap);
	if ( pngReader.GetData() )
	{
		glGenTextures(1, &m_iAlphaTexture);
		glBindTexture(GL_TEXTURE_2D, m_iAlphaTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pngReader.GetWidth(), pngReader.GetHeight(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pngReader.GetData());

		glBindTexture(GL_TEXTURE_2D, 0);

		glGenSamplers(1, &m_iAlphaSampler);
		glSamplerParameteri(m_iAlphaSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(m_iAlphaSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(m_iAlphaSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(m_iAlphaSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		delete [] pngReader.GetData();
	}
}

void CTerrain::UpdateUniform()
{
	for (int i = 0; i < m_vDetailTexture.size(); ++i)
	{
		std::ostringstream oss;
		oss << "u_DetailTex" << i;

		GLint colorTextureUnif = glGetUniformLocation(m_theProgram, oss.str().c_str());
		if ( colorTextureUnif >= 0 )
		{
			glUniform1i(colorTextureUnif, i);
		}

		oss.str("");
		oss << "u_detailSize[" << i <<"]";
		GLint colorTextureSizeUnif = glGetUniformLocation(m_theProgram, oss.str().c_str());
		if ( colorTextureSizeUnif >= 0 )
		{
			glUniform1f(colorTextureSizeUnif, m_vDetailTexSize[i]);
		}
	}

	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_AlphaTex");
	if ( colorTextureUnif >= 0 )
	{
		glUniform1i(colorTextureUnif, m_vDetailTexture.size());
	}
}

void CTerrain::SetDrawWireFrame( bool bDraw )
{
	m_bDrawWireFrame = bDraw;
}

void CTerrain::UpdateCrackFix()
{
	VisitQuadTree(m_pRoot, std::bind(&CTerrain::UpdateCrackFixInternal, this, std::placeholders::_1));
}

void CTerrain::VisitQuadTree( SQuadNode* pNode, const std::function< void(SChunk*) >& pCallBack )
{
	if ( !pNode )
		return;

	if ( pNode->m_vChildren.empty() )
	{
		if ( pCallBack )
			pCallBack(pNode->m_pRelatedChunk);

		return;
	}

	for (int i = 0; i < pNode->m_vChildren.size(); ++i)
	{
		VisitQuadTree(pNode->m_vChildren[i], pCallBack);
	}
}

void CTerrain::VisitQuadTree( SQuadNode* pNode, const std::function< bool(SQuadNode*) >& pCondition, const std::function< void(SChunk*) >& pCallBack )
{
	if ( !pNode || !pCondition(pNode) )
		return;

	if ( pNode->m_vChildren.empty() )
	{
		if ( pCallBack )
			pCallBack(pNode->m_pRelatedChunk);

		return;
	}

	for (int i = 0; i < pNode->m_vChildren.size(); ++i)
	{
		VisitQuadTree(pNode->m_vChildren[i], pCondition, pCallBack);
	}
}

void CTerrain::UpdateChunkLODInternal( SChunk* pChunk )
{
	Vec3 cameraPos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
	Vec3 centerPos = pChunk->m_pParent->m_boundingBoxLocal.GetCenter();
	centerPos = m_transform.GetTransformMat() * Vec4(centerPos, 1.0f);

	float dist = centerPos.Distance(cameraPos);
	pChunk->m_iCurLOD = conMaxLOD - 1;
	for (int i = 0; i < conMaxLOD - 1; ++i)
	{
		if ( dist <= m_vLODThreshold[i] )
		{
			pChunk->m_iCurLOD = i;
			break;
		}
	}
}

void CTerrain::GenerateChunk( SChunk* pNewChunk, int j, int i )
{
	pNewChunk->m_x = j;
	pNewChunk->m_y = i;
	glGenBuffers(1, &pNewChunk->m_vertexIndexObj);

	int iBase = i * conChunkSize * m_iHeightMapWidth + j * conChunkSize;
	for ( int iLOD = 0; iLOD < conMaxLOD; ++iLOD )
	{
		int iStep = pow(2, iLOD);
		SChunkLOD& rLOD = pNewChunk->m_vLOD[iLOD];
		rLOD.m_vIndex.clear();
		for (int m = 0; m < conChunkSize; m += iStep)
		{
			for (int n = 0; n < conChunkSize; n += iStep)
			{
				rLOD.m_vIndex.push_back(iBase + m * m_iHeightMapWidth + n);
				rLOD.m_vIndex.push_back(iBase + m * m_iHeightMapWidth + n + iStep);
				rLOD.m_vIndex.push_back(iBase + (m + iStep) * m_iHeightMapWidth + n);

				rLOD.m_vIndex.push_back(iBase + (m + iStep) * m_iHeightMapWidth + n);
				rLOD.m_vIndex.push_back(iBase + m * m_iHeightMapWidth + n + iStep);
				rLOD.m_vIndex.push_back(iBase + (m + iStep) * m_iHeightMapWidth + n + iStep);
			}
		}
	}
}

void CTerrain::InitNeighbor()
{
	for (int i = 0; i < m_iChunkCountY; ++i)
	{
		for (int j = 0; j < m_iChunkCountX; ++j)
		{
			SChunk* pChunk = m_vChunk[i][j];

			if ( j - 1 >= 0 )
				pChunk->m_vNeighbor[ EChunkNeighbor_Left ] = m_vChunk[i][j - 1];
			if ( j + 1 < m_iChunkCountX )
				pChunk->m_vNeighbor[ EChunkNeighbor_Right ] = m_vChunk[i][j + 1];
			if ( i - 1 >= 0 )
				pChunk->m_vNeighbor[ EChunkNeighbor_Up ] = m_vChunk[i - 1][j];
			if ( i + 1 < m_iChunkCountY )
				pChunk->m_vNeighbor[ EChunkNeighbor_Bottom ] = m_vChunk[i + 1][j];
		}
	}
}

void CTerrain::LoadVertex( const unsigned char* pHeightMapData, int iWidth, int iHeight )
{
	for (int i = 0; i < m_iHeightMapHeight; ++i)
	{
		for (int j = 0; j < m_iHeightMapWidth; ++j)
		{
			SCommonVertex newVertex;
			float fHeight = pHeightMapData[ i * iWidth + j ];
			newVertex.m_pos = Vec3( j - iWidth / 2, fHeight, i - iHeight / 2);
			newVertex.m_color = Color4F::WHITE;
			newVertex.m_UV = Vec2( j * 1.0f / iWidth, i * 1.0f / iHeight );

			m_vGlobalVertex.push_back(newVertex);
		}
	}

	glGenBuffers(1, &m_vertexDataObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, m_vGlobalVertex.size() * sizeof(SCommonVertex), &m_vGlobalVertex.front(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vertexAttributeObj);
}

bool CTerrain::IsInFrustrum( const SQuadNode* pNode )
{
	return CDirector::GetInstance()->GetPerspectiveCamera()->IsInFrustrum( pNode->m_boundingBoxLocal.Transform( m_transform.GetTransformMat() ) );
}

void CTerrain::Init(const std::string& sHeightMapFile)
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
		SetLODThreshold(500, 1000, 1500);

		delete [] pngReader.GetData();
	}
}

float CTerrain::GetHeight( const Vec2& worldPos )
{
	Vec3 p( worldPos.x, 0, worldPos.y );

	Mat4 dd = Mat4::CreateTranslationMat(m_iHeightMapWidth / 2, 0, m_iHeightMapHeight / 2);
	Vec3 localPos = dd * m_transform.GetTransformMat().GetInversed() * Vec4(p, 1.0f);

	int i = (int)localPos.x;
	int j = (int)localPos.z;
	float u = localPos.x - i;
	float v = localPos.z - j;
	if ( i < 0 || i >= m_iHeightMapWidth - 1 || j < 0 || j >= m_iHeightMapHeight - 1  )
		return 0;

	float lt = m_vGlobalVertex[ j * m_iHeightMapWidth + i ].m_pos.y;
	float rt = m_vGlobalVertex[ j * m_iHeightMapWidth + i + 1].m_pos.y;
	float lb = m_vGlobalVertex[ (j + 1) * m_iHeightMapWidth + i].m_pos.y;
	float rb = m_vGlobalVertex[ (j + 1) * m_iHeightMapWidth + i + 1].m_pos.y;

	float m = lt * (1.0f - u) + rt * u;
	float n = lb * (1.0f - u) + rb * u;
	float fLocalHeight = m * (1.0f - v) + n * v;

	localPos.y = fLocalHeight;
	p = (m_transform.GetTransformMat() * dd.GetInversed()) * Vec4(localPos, 1.0f);

	return p.y;
}

Vec3 CTerrain::GetIntersectionPoint( const CRay& worldRay )
{
	Vec3 startPos = worldRay.m_origin;
	Vec3 lastPos = startPos;
	float fHeight = GetHeight( Vec2( startPos.x, startPos.z ) );
	while (startPos.y > fHeight )
	{
		lastPos = startPos;
		startPos += worldRay.m_direction * 8;
		fHeight = GetHeight( Vec2( startPos.x, startPos.z ) );
	}

	for (int i = 0; i < 32; ++i)
	{
		Vec3 middlePos = (startPos + lastPos) / 2;
		if ( middlePos.y < fHeight )
			startPos = middlePos;
		else
			lastPos = middlePos;
	}

	return (startPos + lastPos) / 2;
	return startPos;
}

CTerrain::SQuadNode::SQuadNode( int x, int y, int iWidth, int iHeight, SQuadNode* pParent, CTerrain* pTerrain )
	: m_pRelatedChunk(nullptr)
	, m_pParent(nullptr)
{
	m_pParent = pParent;

	if ( (iWidth / 2) / conChunkSize > 0 && (iHeight / 2) / conChunkSize > 0  )
	{
		// - left top
		auto pLTChild = new SQuadNode(x, y, iWidth / 2, iHeight / 2, this, pTerrain);
		m_vChildren.push_back(pLTChild);
		m_boundingBoxLocal.Merge(pLTChild->m_boundingBoxLocal);

		// - right top
		auto pRTChild = new SQuadNode(x + iWidth / 2, y, iWidth / 2, iHeight / 2, this, pTerrain);
		m_vChildren.push_back(pRTChild);
		m_boundingBoxLocal.Merge(pRTChild->m_boundingBoxLocal);

		// - left bottom
		auto pLBChild = new SQuadNode(x, y + iHeight / 2, iWidth / 2, iHeight / 2, this, pTerrain);
		m_vChildren.push_back(pLBChild);
		m_boundingBoxLocal.Merge(pLBChild->m_boundingBoxLocal);

		// - right bottom
		auto pRBChild = new SQuadNode(x + iWidth / 2, y + iHeight / 2, iWidth / 2, iHeight / 2, this, pTerrain);
		m_vChildren.push_back(pRBChild);
		m_boundingBoxLocal.Merge(pRBChild->m_boundingBoxLocal);
	}
	else
	{
		m_pRelatedChunk = pTerrain->m_vChunk[ x / conChunkSize ][ y / conChunkSize ];
		m_pRelatedChunk->m_pParent = this;

		std::vector<Vec3> vPos;
		for (int i = 0; i < m_pRelatedChunk->m_vLOD[0].m_vIndex.size(); ++i)
		{
			int iVertexIndex = m_pRelatedChunk->m_vLOD[0].m_vIndex[i];
			vPos.push_back( pTerrain->m_vGlobalVertex[ iVertexIndex ].m_pos );
		}
		m_boundingBoxLocal.Init(vPos);
	}
}
