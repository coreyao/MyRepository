#include "GLLabel.h"
#include "Director.h"
#include "Image/PNGReader.h"

FT_Library CGLLabel::s_library = nullptr;

const float conTexWidth = 512.0f;
const float conTexHeight = 512.0f;

CGLLabel::CGLLabel(const std::string& fontName, int iFontSize)
	: m_colorTexUnit(0)
	, m_pTexData(nullptr)
{
	if ( !s_library )
	{
		FT_Error error = FT_Init_FreeType( &s_library );
		if ( error != FT_Err_Ok )
		{
			s_library = nullptr;
		}
	}
	
	FT_Error error = FT_New_Face( s_library, fontName.c_str(), 0, &m_fontFace );
	if ( error != FT_Err_Ok )
		return;

	if (FT_Select_Charmap(m_fontFace, FT_ENCODING_UNICODE))
		return;

	int dpi = 72;
	int fontSizePoints = (int)(64.f * iFontSize);
	if (FT_Set_Char_Size(m_fontFace, fontSizePoints, fontSizePoints, dpi, dpi))
		return;

	m_pTexData = new char[(int)conTexWidth * (int)conTexHeight];
	memset(m_pTexData, 255, conTexWidth * conTexHeight);

	InitVBOAndVAO();
}

CGLLabel::~CGLLabel()
{
	delete [] m_pTexData;
}

void CGLLabel::SetString( const std::string& sContent )
{
	m_sContent = sContent;
}

void CGLLabel::Render()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	int iX = 0;
	int iY = 0;
	std::vector<SLetter> vLetter;
	for ( int i = 0; i < m_sContent.size(); ++i )
	{
		int iLetter = m_sContent[i];
		auto glyphIndex = FT_Get_Char_Index(m_fontFace, iLetter);
		if(glyphIndex)
		{
			if (!FT_Load_Glyph(m_fontFace, glyphIndex, FT_LOAD_RENDER))
			{
				unsigned char* pBitmap = m_fontFace->glyph->bitmap.buffer;
				long bitmapHeight = m_fontFace->glyph->bitmap.rows;
				long bitmapWidth = m_fontFace->glyph->bitmap.width;
				for (long y = 0; y < bitmapHeight; ++y)
				{
					long bitmap_y = y * bitmapWidth;

					for (int x = 0; x < bitmapWidth; ++x)
					{
						unsigned char cTemp = pBitmap[bitmap_y + x];

						// the final pixel
						m_pTexData[iY * (int)conTexWidth + iX] = cTemp;
						++iX;
					}

					iX = 0;
					++iY;
				}

				glBindTexture(GL_TEXTURE_2D, m_iTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, conTexWidth, conTexHeight, 0,
					GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pTexData);

				SLetter letter;
				letter.m_vVertex[0].m_pos.x = -0.5f;
				letter.m_vVertex[0].m_pos.y = 0.5f;
				letter.m_vVertex[0].m_UV.x = 0;
				letter.m_vVertex[0].m_UV.y = 0;
				letter.m_vVertex[0].m_color = Color4F::WHITE;

				letter.m_vVertex[1].m_pos.x = 0.5f;
				letter.m_vVertex[1].m_pos.y = 0.5f;
				letter.m_vVertex[1].m_UV.x = bitmapWidth / conTexWidth;
				//letter.m_vVertex[1].m_UV.x = 1;
				letter.m_vVertex[1].m_UV.y = 0;
				letter.m_vVertex[1].m_color = Color4F::WHITE;

				letter.m_vVertex[2].m_pos.x = 0.5f;
				letter.m_vVertex[2].m_pos.y = -0.5f;
				letter.m_vVertex[2].m_UV.x = bitmapWidth / conTexWidth;
				letter.m_vVertex[2].m_UV.y = bitmapHeight / conTexHeight;
				//letter.m_vVertex[2].m_UV.x = 1;
				//letter.m_vVertex[2].m_UV.y = 1;
				letter.m_vVertex[2].m_color = Color4F::WHITE;

				letter.m_vVertex[3].m_pos.x = -0.5f;
				letter.m_vVertex[3].m_pos.y = -0.5f;
				letter.m_vVertex[3].m_UV.x = 0;
				letter.m_vVertex[3].m_UV.y = bitmapHeight / conTexHeight;
				//letter.m_vVertex[3].m_UV.y = 1;
				letter.m_vVertex[3].m_color = Color4F::WHITE;

				letter.m_vVertexIndex[0] = i * 6 + 0;
				letter.m_vVertexIndex[1] = i * 6 + 1;
				letter.m_vVertexIndex[2] = i * 6 + 3;

				letter.m_vVertexIndex[3] = i * 6 + 1;
				letter.m_vVertexIndex[4] = i * 6 + 2;
				letter.m_vVertexIndex[5] = i * 6 + 3;

				vLetter.push_back(letter);
			}
		}
	}

	std::vector< SVertex > vVertex;
	std::vector< unsigned short > vVertexIndex;
	for ( auto& letter : vLetter )
	{
		for ( auto& vertex : letter.m_vVertex )
			vVertex.push_back(vertex);

		for ( auto& vertexIndex : letter.m_vVertexIndex )
			vVertexIndex.push_back(vertexIndex);
	}

	glUseProgram(m_theProgram);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, vVertex.size() * sizeof(SVertex), &vVertex.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vVertexIndex.size() * sizeof(unsigned short), &vVertexIndex.front(), GL_STATIC_DRAW);

	glBindVertexArray(m_vertexAttributeObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(SVertex, m_pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(SVertex, m_color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(SVertex, m_UV));

	glActiveTexture(GL_TEXTURE0 + m_colorTexUnit);
	glBindTexture(GL_TEXTURE_2D, m_iTexture);
	glBindSampler(m_colorTexUnit, m_Sampler);

	GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
	if ( modelViewMatrixUnif >= 0 )
	{
		Mat4 viewMatrix = CDirector::GetInstance()->GetCurCamera()->GetViewMat();
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, (viewMatrix * m_transform.GetTransformMat()).m);
	}

	GLint colorUnif = glGetUniformLocation(m_theProgram, "u_color");
	if ( colorUnif >= 0 )
	{
		glUniform4f( colorUnif, 1.0, 1.0, 1.0, 1.0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, vVertexIndex.size(), GL_UNSIGNED_SHORT, 0);

	glUseProgram(0);
	glBindSampler(m_colorTexUnit, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
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

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void CGLLabel::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
	InitUniform();
}

void CGLLabel::InitUniform()
{
	glUseProgram(m_theProgram);

	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_colorTexture");
	if ( colorTextureUnif >= 0 )
	{
		glUniform1i(colorTextureUnif, m_colorTexUnit);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetCurCamera()->GetProjMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	glUseProgram(0);
}

