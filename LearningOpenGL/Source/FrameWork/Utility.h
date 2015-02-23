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

#ifndef MESH_FILE_DIR
#define MESH_FILE_DIR std::string("Mesh/")
#endif

namespace Utility
{
	float DegToRad(float fAngDeg);
	float RadToDeg(float fRad);
}