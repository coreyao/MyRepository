#include "PNGReader.h"
#include <cstdio>
#include "png.h"

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
	FILE* file = fopen(sFileName.c_str(), "rb");
	if ( !file )
		return;

	ResetData();

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	png_infop info_ptr = png_create_info_struct(png_ptr);

	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, file);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

	int m_width = png_get_image_width(png_ptr, info_ptr);
	int m_height = png_get_image_height(png_ptr, info_ptr);
	int color_type = png_get_color_type(png_ptr, info_ptr);
	int size = m_height * m_width * 4;
	int pos = 0;
	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

	m_pData = new unsigned char[size];
	for(int i = 0; i < m_height; i++)
	{
		for(int j = 0; j < (4 * m_width); j += 4)
		{
			m_pData[pos++] = row_pointers[i][j];   // red
			m_pData[pos++] = row_pointers[i][j + 1]; // green
			m_pData[pos++] = row_pointers[i][j + 2]; // blue
			m_pData[pos++] = row_pointers[i][j + 3]; // alpha
		}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	fclose(file);

	return;
}


