#pragma once

#include "CubeMap.h"

class CSkyBox
{
public:
	void Init(const std::string& sPositiveX, const std::string& sNegativeX, const std::string& sPositiveY, const std::string& sNegativeY
		, const std::string& sPositiveZ, const std::string& sNegativeZ);
	void SetGLProgram(GLint theProgram);

	void Update(float dt);
	void Render();

	STransform m_transform;

private:
	void InitVBOAndVAO();
	void InitUniform();

private:
	CCubeMap m_CubeMap;

	GLuint m_theProgram;
	GLuint m_vertexAttributeObj;
	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
};