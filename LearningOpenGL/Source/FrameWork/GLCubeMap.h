#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CGLCubeMap
{
public:
	void Init(const std::string& sPositiveX, const std::string& sNegativeX, const std::string& sPositiveY, const std::string& sNegativeY
		, const std::string& sPositiveZ, const std::string& sNegativeZ);

	GLuint m_cubeTex;
	GLuint m_cubeSampler;
};