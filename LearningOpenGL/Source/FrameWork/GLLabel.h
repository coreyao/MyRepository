#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

#include "ft2build.h"
#include FT_FREETYPE_H

class CGLLabel
{
public:
	CGLLabel(const std::string& fontName, int iFontSize);
	~CGLLabel();

	void SetString(const std::string& sContent);
	void Render();
	void SetGLProgram(GLuint theProgram);

	STransform m_transform;
	Color4F m_color;

private:
	void InitVBOAndVAO();
	void InitUniform();

	struct SVertex
	{
		Vec3 m_pos;
		Color4F m_color;
		Vec2 m_UV;
	};

	struct SLetter
	{
		SVertex m_vVertex[4];
		unsigned short m_vVertexIndex[6];
	};

private:
	static FT_Library s_library;

	FT_Face	m_fontFace;

	int m_colorTexUnit;
	GLuint m_Sampler;
	GLuint m_iTexture;

	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;

	GLuint m_theProgram;


	std::string m_sContent;
	char* m_pTexData;
};