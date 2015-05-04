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




private:
	FT_Face           m_fontFace;
	static FT_Library s_library;

	std::string m_sContent;

	GLuint m_iTexture;
	int m_colorTexUnit;
	GLuint m_Sampler;
};