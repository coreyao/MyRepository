#pragma once

#include "ImageReader.h"

class CTGAReader : public CImageReader
{
public:
	CTGAReader(const std::string& sFileName);
	virtual ~CTGAReader();

	virtual void ReadData(const std::string& sFileName);

private:
	bool _loadUncompressedTGA(FILE *);  
	bool _loadCompressedTGA(FILE *);  

	struct TGA_HEADER  
	{  
		unsigned short  _width;                     ///<tga宽度  
		unsigned short  _height;                    ///<tga高度  
		unsigned char   _bitsPerPixel;              ///<tga像素位数  
		unsigned char   _describe;                  ///<tga描述字节                
	};  

	TGA_HEADER                  m_header;                   ///< 头信息
	unsigned int                m_dataSize;                 ///< 数据块大小  
	unsigned int                m_bytesPerPixel;            ///< 像素字节大小  
};