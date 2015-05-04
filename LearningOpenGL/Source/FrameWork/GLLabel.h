#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

#include "ft2build.h"
#include FT_FREETYPE_H

class CGLLabel
{
public:
	CGLLabel(const char* fontName, int iFontSize);
	void SetString(const std::string& sContent);

private:
	void Draw();
	void InitVBOAndVAO();

	struct SLetterDefinition
	{
		int U;
		int V;
		int UL;
		int VL;
	};

private:
	static FT_Library s_library;

	FT_Face	m_fontFace;
	std::string m_sContent;
	std::map<int, SLetterDefinition> m_mLetter;

	STransform m_transform;

	int m_colorTexUnit;
	GLuint m_Sampler;
	GLuint m_iTexture;

	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;

	GLuint m_theProgram;
};