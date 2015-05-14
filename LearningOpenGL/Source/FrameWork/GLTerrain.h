#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CGLTerrain
{
public:
	static const int conMaxLOD = 4;

	CGLTerrain(const std::string& sHeightMapFile);

	void Update(float deltaTime);
	void Render();
	void SetGLProgram(GLint theProgram);
	void SetLODThreshold( float LOD_1, float LOD_2, float LOD_3 );
	void SetDetailTexture( const std::string& sTex1, const std::string& sTex2 = "");

	STransform m_transform;

private:
	void InitTerrain(const unsigned char* pHeightMapData, int iWidth, int iHeight);
	void InitUniform();
	void UpdateChunkLOD(const Vec3& cameraPos);

	struct SChunkLOD
	{
		std::vector<unsigned int> m_vIndex;
	};

	struct SChunk
	{
		SChunk()
			: m_iCurLOD(0)
		{
		}

		int m_iCurLOD;
		GLuint m_vertexIndexObj;
		SChunkLOD m_vLOD[conMaxLOD];
	};

	int m_vLODThreshold[conMaxLOD - 1];
	SChunk*** m_vChunk;

	int m_iChunkCountX;
	int m_iChunkCountY;

	std::vector<SCommonVertex> m_vGlobalVertex;
	bool m_bDrawWireFrame;

	GLuint m_theProgram;
	GLuint m_vertexAttributeObj;
	GLuint m_vertexDataObj;

	std::vector<std::string> m_vDetailTex;
	std::vector<GLuint> m_vTexture;
	std::vector<GLuint> m_vSampler;
	int m_colorTexUnit;
};