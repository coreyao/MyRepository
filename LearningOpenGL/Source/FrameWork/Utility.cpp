#include "Utility.h"

float Utility::DegToRad(float fAngDeg)
{
	const float fDegToRad = 3.14159f * 2.0f / 360.0f;
	return fAngDeg * fDegToRad;
}

float Utility::RadToDeg(float fRad)
{
	return 0;
}

