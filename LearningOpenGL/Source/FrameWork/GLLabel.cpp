#include "GLLabel.h"

FT_Library CGLLabel::s_library = nullptr;

CGLLabel::CGLLabel(const char* fontName, int iFontSize)
{
	if ( !s_library )
	{
		FT_Error error = FT_Init_FreeType( &s_library );
		if ( error != FT_Err_Ok )
		{
			s_library = nullptr;
		}
	}
	
	FT_Error error = FT_New_Face( s_library, fontName, 0, &m_fontFace );
	if ( error != FT_Err_Ok )
		return;

	if (FT_Select_Charmap(m_fontFace, FT_ENCODING_UNICODE))
		return;

	int dpi = 72;
	int fontSizePoints = (int)(64.f * iFontSize);
	if (FT_Set_Char_Size(m_fontFace, fontSizePoints, fontSizePoints, dpi, dpi))
		return;
}

void CGLLabel::SetString( const std::string& sContent )
{
	m_sContent = sContent;
}

void CGLLabel::Draw()
{
	for (int i = 0; i < m_sContent.size(); ++i)
	{
		int iLetter = m_sContent[i];

	}
}

void CGLLabel::InitVBOAndVAO()
{
	glGenBuffers(1, &m_vertexDataObj);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_vertexIndexObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_vertexAttributeObj);
	glBindVertexArray(0);

	glGenTextures(1, &m_iTexture);
}

