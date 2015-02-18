#pragma once

#include <string>

#ifndef LOCAL_FILE_DIR
#define LOCAL_FILE_DIR std::string("../Resource/")
#endif

namespace Utility
{
	float DegToRad(float fAngDeg);
	float RadToDeg(float fRad);
}