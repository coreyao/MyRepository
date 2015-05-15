#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

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

	CGLTerrain(const std::string& sHeightMapFile);

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
	void InitTerrain(const unsigned char* pHeightMapData, int iWidth, int iHeight);
	void UpdateUniform();
	void UpdateChunkLOD(const Vec3& cameraPos);
	void UpdateCrackFix();

	struct SChunkLOD
	{
		std::vector<unsigned int> m_vIndex;
	};

	struct SChunk
	{
		SChunk()
			: m_iCurLOD(0)
		{
			memset( m_vNeighbor, 0, sizeof(m_vNeighbor) );
		}

		int m_iCurLOD;
		GLuint m_vertexIndexObj;
		SChunkLOD m_vLOD[conMaxLOD];
		SChunkLOD m_vFixCrack;

		SChunk* m_vNeighbor[4];
	};

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
};
