#include "CubeMap.h"
#include "Image/PNGReader.h"

void CCubeMap::Init( const std::string& sPositiveX, const std::string& sNegativeX, const std::string& sPositiveY, const std::string& sNegativeY , 
					  const std::string& sPositiveZ, const std::string& sNegativeZ )
{
	glGenTextures(1, &m_cubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);

	std::string vFile[6];
	vFile[0] = sPositiveX;
	vFile[1] = sNegativeX;
	vFile[2] = sPositiveY;
	vFile[3] = sNegativeY;
	vFile[4] = sPositiveZ;
	vFile[5] = sNegativeZ;

	for (int i = 0; i < 6; ++i)
	{
		CPNGReader pngReader(vFile[i]);
		if ( pngReader.GetData() )
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, pngReader.GetWidth(), pngReader.GetHeight(), 0,
				GL_RGBA, GL_UNSIGNED_BYTE, pngReader.GetData());
		}
	}

	glGenSamplers(1, &m_cubeSampler);
	glSamplerParameteri(m_cubeSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_cubeSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_cubeSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glSamplerParameteri(m_cubeSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_cubeSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

