#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CGLTerrain
{
public:
	CGLTerrain(const std::string& sHeightMapFile);

	void Update(float deltaTime);
	void Render();
	void SetGLProgram(GLint theProgram);

	STransform m_transform;

private:
	void InitTerrain(const unsigned char* pHeightMapData, int iWidth, int iHeight);

	struct SChunkLOD
	{
		std::vector<unsigned int> m_vIndex;
	};

	struct SChunk
	{
		GLuint m_vertexIndexObj;
		SChunkLOD m_vLOD[4];
	};

	int m_vLODThreshold[4];
	SChunk*** m_vChunk;

	int m_iChunkCountX;
	int m_iChunkCountY;

	bool m_bDrawWireFrame;

	GLuint m_theProgram;
	GLuint m_vertexAttributeObj;
	GLuint m_vertexDataObj;
};