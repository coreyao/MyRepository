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
		unsigned short  _width;                     ///<tga���  
		unsigned short  _height;                    ///<tga�߶�  
		unsigned char   _bitsPerPixel;              ///<tga����λ��  
		unsigned char   _describe;                  ///<tga�����ֽ�                
	};  

	TGA_HEADER                  m_header;                   ///< ͷ��Ϣ
	unsigned int                m_dataSize;                 ///< ���ݿ��С  
	unsigned int                m_bytesPerPixel;            ///< �����ֽڴ�С  
};