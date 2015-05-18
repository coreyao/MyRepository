#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"
#include "GLCollision.h"

class CGLTerrain
{
public:
	static const int conMaxLOD = 4;

	enum EChunkNeighbor
	{
		EChunkNeighbor_Left,
		EChunkNeighbor_Right,
		EChunkNeighbor_Up,
		EChunkNeighbor_Bottom,
	};

	CGLTerrain();

	void Init(const std::string& sHeightMapFile);
	void Update(float deltaTime);
	void Render();
	void SetGLProgram(GLint theProgram);
	void SetLODThreshold( float LOD_1, float LOD_2, float LOD_3 );
	void SetAlphaTexture( const std::string& sAlphaMap);
	void SetDetailTexture( const std::string& sTex1, const std::string& sTex2 = "", const std::string& sTex3 = "", const std::string& sTex4 = "");
	void SetDetailTextureSize( int iTex1Size, int iTex2Size = 20, int iTex3Size = 20, int iTex4Size = 20);
	void SetDrawWireFrame(bool bDraw);

	STransform m_transform;

private:
	struct SChunkLOD
	{
		std::vector<unsigned int> m_vIndex;
	};

	struct SQuadNode;
	struct SChunk
	{
		SChunk()
			: m_iCurLOD(0)
			, m_x(0)
			, m_y(0)
			, m_bDraw(true)
			, m_pParent(nullptr)
		{
			memset( m_vNeighbor, 0, sizeof(m_vNeighbor) );
		}

		int m_iCurLOD;
		int m_x;
		int m_y;
		GLuint m_vertexIndexObj;
		SChunkLOD m_vLOD[conMaxLOD];
		SChunkLOD m_vFixCrack;
		SChunk* m_vNeighbor[4];
		bool m_bDraw;

		SQuadNode* m_pParent;
	};

	struct SQuadNode
	{
		SQuadNode()
			: m_pRelatedChunk(nullptr)
			, m_pParent(nullptr)
		{
		}

		SQuadNode(int x, int y, int iWidth, int iHeight, SQuadNode* pParent, CGLTerrain* pTerrain);

		SChunk* m_pRelatedChunk;
		
		SQuadNode* m_pParent;
		std::vector<SQuadNode*> m_vChildren;
		CAABB m_boundingBoxLocal;
	};

	SQuadNode* m_pRoot;

	int m_vLODThreshold[conMaxLOD - 1];
	SChunk*** m_vChunk;

	int m_iChunkCountX;
	int m_iChunkCountY;
	int m_iHeightMapWidth;
	int m_iHeightMapHeight;

	std::vector<SCommonVertex> m_vGlobalVertex;
	bool m_bDrawWireFrame;

	GLuint m_theProgram;
	GLuint m_vertexAttributeObj;
	GLuint m_vertexDataObj;

	GLuint m_iAlphaTexture;
	GLuint m_iAlphaSampler;
	std::vector<GLuint> m_vDetailTexture;
	std::vector<int> m_vDetailTexSize;
	GLuint m_detailTexSampler;

private:
	void InitTerrain(const unsigned char* pHeightMapData, int iWidth, int iHeight);
	void LoadVertex( const unsigned char* pHeightMapData, int iWidth, int iHeight );
	void InitNeighbor();
	void GenerateChunk( SChunk* pNewChunk, int j, int i );
	void UpdateUniform();
	void UpdateChunkLOD();
	void UpdateChunkLODInternal( SChunk* pChunk );
	void UpdateCrackFix();
	void UpdateCrackFixInternal( SChunk* pCurChunk);
	void VisitQuadTree(SQuadNode* pNode, const std::function< void(SChunk*) >& pCallBack);
	void VisitQuadTree(SQuadNode* pNode, const std::function< bool(SQuadNode*) >& pCondition, const std::function< void(SChunk*) >& pCallBack);
	void RenderChunk(const SChunk* pChunk);
	bool IsInFrustrum(const SQuadNode* pNode);
};
