#pragma once

#ifdef _DEBUG
#define USE_SIMD 0;
#else
#define USE_SIMD 0;
#endif
#define SIMD_ALIGN __declspec(align(16))
#include <emmintrin.h>

#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <list>
#include <cassert>

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/fixedpoint.h"

using namespace std;

#define PI 3.141592f
#define DEG_TO_RAD(x) ((PI) * ((x) / (180.0f))) 
#define RAD_TO_DEG(x) ((180) * ((x) / (PI)) )
#define EPSILON 0.00001f
#define NEARLY_EQUAL(x,y) (fabs((x) - (y)) <= EPSILON)
#define ARGB(a, r, g, b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

#ifndef MESH_FILE_DIR
#define MESH_FILE_DIR std::string("./Resource/Mesh/")
#endif

#ifndef IMAGE_FILE_DIR
#define IMAGE_FILE_DIR std::string("./Resource/Images/")
#endif


typedef double HighPrecision;

namespace Helper
{
	template<typename T>
	void Swap(T& a, T& b);

	template<typename T>
	void Clamp(T& tval, const T& tmin, const T& tmax);
}

template<typename T>
void Helper::Clamp(T& tval, const T& tmin, const T& tmax)
{
	if ( tval < tmin )
		tval = tmin;
	else if ( tval > tmax )
		tval = tmax;
}

template<typename T>
void Helper::Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}
