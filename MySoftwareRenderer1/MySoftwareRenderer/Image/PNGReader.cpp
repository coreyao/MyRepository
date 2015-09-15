#include "PNGReader.h"
#include "png.h"
#include "../Utility.h"
#include <cstdio>

CPNGReader::CPNGReader( const std::string& sFileName )
	: CImageReader()
{
	ReadData(sFileName);
}

CPNGReader::~CPNGReader()
{

}

void CPNGReader::ReadData(const std::string& sFileName)
{
	FILE* file = fopen((IMAGE_FILE_DIR + sFileName).c_str(), "rb");
	if ( !file )
		return;

	ResetData();

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	png_infop info_ptr = png_create_info_struct(png_ptr);

	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, file);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

	m_fWidth = png_get_image_width(png_ptr, info_ptr);
	m_fHeight = png_get_image_height(png_ptr, info_ptr);
	int color_type = png_get_color_type(png_ptr, info_ptr);
	int size = m_fHeight * m_fWidth * 4;
	int pos = 0;
	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

	if ( color_type != 6 )
		m_eType = CImageReader::EImageType_RGB888;

	int block_size = color_type == 6 ? 4 : 3;
	m_pData = new unsigned char[size];
	for(int i = 0; i < m_fHeight; i++)
	{
		for(int j = 0; j < (block_size * m_fWidth); j += block_size)
		{
			m_pData[pos++] = row_pointers[i][j];   // red
			m_pData[pos++] = row_pointers[i][j + 1]; // green
			m_pData[pos++] = row_pointers[i][j + 2]; // blue
			if ( block_size == 3 )
			{
				m_pData[pos++] = 255; // alpha
			}
			else if ( block_size == 4 )
			{
				m_pData[pos++] = row_pointers[i][j + 3]; // alpha
			}
		}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	fclose(file);
}


