#pragma once

#include "ImageReader.h"

class CPNGReader : public CImageReader
{
public:
	CPNGReader(const std::string& sFileName);
	virtual ~CPNGReader();

	virtual void ReadData(const std::string& sFileName);
};