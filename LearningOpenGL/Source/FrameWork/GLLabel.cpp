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
	int iX = 0;
	int iY = 0;

	for (int i = 0; i < m_sContent.size(); ++i)
	{
		int iLetter = m_sContent[i];
		auto glyphIndex = FT_Get_Char_Index(m_fontFace, iLetter);
		if(glyphIndex)
		{
			 if (FT_Load_Glyph(m_fontFace, glyphIndex, FT_LOAD_RENDER))
			 {
				 unsigned char* pBitmap = m_fontFace->glyph->bitmap.buffer;
				 long bitmapHeight = m_fontFace->glyph->bitmap.rows;
				 long bitmapWidth = m_fontFace->glyph->bitmap.width;
				 char* dest = new char[bitmapHeight * bitmapWidth]();
				 for (long y = 0; y < bitmapHeight; ++y)
				 {
					 long bitmap_y = y * bitmapWidth;

					 for (int x = 0; x < bitmapWidth; ++x)
					 {
						 unsigned char cTemp = pBitmap[bitmap_y + x];

						 // the final pixel
						 dest[(iX + ( iY * 512 ) )] = cTemp;

						 iX += 1;
					 }

					 iX = 0;
					 iY += 1;
				 }
			 }
		}
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

