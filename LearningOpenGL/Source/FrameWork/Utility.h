#pragma once

#include <string>

#ifndef LOCAL_FILE_DIR
#define LOCAL_FILE_DIR std::string("../Resource/")
#endif

#ifndef SHADER_FILE_DIR
#define SHADER_FILE_DIR std::string("Shaders/")
#endif

#ifndef IMAGE_FILE_DIR
#define IMAGE_FILE_DIR std::string("Images/")
#endif

namespace Utility
{
	float DegToRad(float fAngDeg);
	float RadToDeg(float fRad);
}