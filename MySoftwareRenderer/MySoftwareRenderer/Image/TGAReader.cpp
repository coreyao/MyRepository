#include "TGAReader.h"
#include "../Utility.h"
#include <cstdio>

CTGAReader::CTGAReader( const std::string& sFileName )
	: CImageReader()
{
	ReadData(sFileName);
}

CTGAReader::~CTGAReader()
{
}

void CTGAReader::ReadData(const std::string& sFileName)
{
	ResetData();

	FILE * __file = fopen((IMAGE_FILE_DIR + sFileName).c_str(), "rb");  
	if(__file == NULL)  
		return;  

	unsigned char __header[12];  
	if(fread(&__header, sizeof(__header), 1, __file) == 0)  
	{  
		fclose(__file);  
		return;  
	}  

	//未压缩TGA文件头  
	unsigned char uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};  
	//压缩TGA文件头  
	unsigned char cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};  

	if(memcmp(uTGAcompare, &__header, sizeof(__header)) == 0)  
	{  
		_loadUncompressedTGA(__file);  
	}  
	else if(memcmp(cTGAcompare, &__header, sizeof(__header)) == 0)        
	{  
		_loadCompressedTGA(__file);  
	}  
	else  
	{  
		fclose(__file);  
		return;  
	}
}

bool CTGAReader::_loadUncompressedTGA( FILE * file )
{
	 if(fread(&m_header, sizeof(m_header), 1, file) == 0)  
    {                                         
        fclose(file);  
        return false;  
    }     
    if((m_header._bitsPerPixel != 24) && (m_header._bitsPerPixel !=32))  
    {  
        fclose(file);  
        return false;  
    }  
  
    m_bytesPerPixel = (m_header._bitsPerPixel / 8);      
	m_fWidth = m_header._width;
	m_fHeight = m_header._height;

	// Compute the number of BYTES per pixel  
    m_dataSize = (m_bytesPerPixel * m_header._width * m_header._height);                        // Compute the total amout ofmemory needed to store data  
    m_pData = new unsigned char[m_dataSize];                                                // Allocate that much memory  
  
    if(m_pData == NULL)  
    {  
        fclose(file);  
        return false;  
    }  
  
    if(fread(m_pData, 1, m_dataSize, file) != m_dataSize)                                        // Attempt to read image data  
    {  
        free(m_pData);  
        fclose(file);  
        return false;  
    }  

	if ( m_bytesPerPixel == 3 )
		m_eType = EImageType_RGB888;
	else
		m_eType = EImageType_RGB8888;
  
    fclose(file);  
    return true;  
}

bool CTGAReader::_loadCompressedTGA( FILE * file)
{
	if(fread(&m_header, sizeof(m_header), 1, file) == 0)  
	{                                         
		fclose(file);  
		return false;  
	}     

	if((m_header._bitsPerPixel != 24) && (m_header._bitsPerPixel !=32))  
	{  
		fclose(file);  
		return false;  
	}  

	m_bytesPerPixel = (m_header._bitsPerPixel / 8);     
	m_fWidth = m_header._width;
	m_fHeight = m_header._height;
	// Compute BYTES per pixel  
	m_dataSize = (m_bytesPerPixel * m_header._width * m_header._height);                // Compute amout of memory needed to store image  
	m_pData = new unsigned char[m_dataSize];                                       // Allocate that much memory  

	if(m_pData == NULL)  
	{  
		fclose(file);  
		return false;  
	}  

	unsigned int pixelcount = m_header._width * m_header._height;                       // Nuber of pixels in the image  
	unsigned int currentpixel = 0;                                                      // Current pixel being read  
	unsigned int currentbyte = 0;                                                       // Current byte   
	unsigned char * colorbuffer = (unsigned char *)malloc(m_bytesPerPixel);             // Storage for 1 pixel  

	do  
	{  
		unsigned char chunkheader = 0;    
		if(fread(&chunkheader, sizeof(unsigned char), 1, file) == 0)                    // Read in the 1 byte header  
		{  
			fclose(file);  
			delete [] m_pData;  
			return false;                                                               // Return failed  
		}  

		if(chunkheader < 128)                                                            // If the ehader is < 128, it means the that is the number of RAW color packets minus 1  
		{                                                                               // that follow the header  
			chunkheader++;                                                              // add 1 to get number of following color values  
			for(short counter = 0; counter < chunkheader; counter++)                 // Read RAW color values  
			{  
				if(fread(colorbuffer, 1, m_bytesPerPixel, file) != m_bytesPerPixel)     // Try to read 1 pixel  
				{  
					fclose(file);  
					free(colorbuffer);    
					delete [] m_pData;  
					return false;                                                       // Return failed  
				}  
				// write to memory  
				m_pData[currentbyte      ] = colorbuffer[2];                             // Flip R and B vcolor values around in the process   
				m_pData[currentbyte + 1  ] = colorbuffer[1];  
				m_pData[currentbyte + 2  ] = colorbuffer[0];  

				if(m_bytesPerPixel == 4)                                                // if its a 32 bpp image  
				{  
					m_pData[currentbyte + 3] = colorbuffer[3];                           // copy the 4th byte  
				}  

				currentbyte += m_bytesPerPixel;                                         // Increase thecurrent byte by the number of bytes per pixel  
				currentpixel++;                                                         // Increase current pixel by 1  

				if(currentpixel > pixelcount)                                            // Make sure we havent read too many pixels  
				{  
					fclose(file);  
					free(colorbuffer);    
					delete [] m_pData;  
					return false;                                                       // Return failed  
				}  
			}  
		}  
		else                                                                            // chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times  
		{  
			chunkheader -= 127;                                                         // Subteact 127 to get rid of the ID bit  
			if(fread(colorbuffer, 1, m_bytesPerPixel, file) != m_bytesPerPixel)         // Attempt to read following color values  
			{     
				fclose(file);  
				free(colorbuffer);    
				delete [] m_pData;  
				return false;  
			}  

			for(short counter = 0; counter < chunkheader; counter++)                 // copy the color into the image data as many times as dictated   
			{                                                                           // by the header  
				m_pData[currentbyte      ] = colorbuffer[2];                             // switch R and B bytes areound while copying  
				m_pData[currentbyte + 1  ] = colorbuffer[1];  
				m_pData[currentbyte + 2  ] = colorbuffer[0];  

				if(m_bytesPerPixel == 4)                                                // If TGA images is 32 bpp  
				{  
					m_pData[currentbyte + 3] = colorbuffer[3];                           // Copy 4th byte  
				}  

				currentbyte += m_bytesPerPixel;                                         // Increase current byte by the number of bytes per pixel  
				currentpixel++;                                                         // Increase pixel count by 1  

				if(currentpixel > pixelcount)                                            // Make sure we havent written too many pixels  
				{  
					fclose(file);  
					free(colorbuffer);    
					delete [] m_pData;  
					return false;                                                       // Return failed  
				}  
			}  
		}  
	}  
	while(currentpixel < pixelcount);                                                    // Loop while there are still pixels left  

	if ( m_bytesPerPixel == 3 )
		m_eType = EImageType_RGB888;
	else
		m_eType = EImageType_RGB8888;

	fclose(file);                                                                       // Close the file  
	return true;           
}
