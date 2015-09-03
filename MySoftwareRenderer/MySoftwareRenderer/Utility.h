#pragma once

#include <cmath>
#include <string>
#include <vector>

using namespace std;

#define PI 3.141592f
#define DEG_TO_RAD(x) ((PI) * ((180.0f) / (x))) 
#define RAD_TO_DEG(x) ((180) * ((PI) / (x)) )
#define EPSILON 0.00001f
#define NEARLY_EQUAL(x,y) (fabs((x) - (y)) <= EPSILON)
#define ARGB(a, r, g, b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

namespace Helper
{
	template<typename T>
	void Swap(T& a, T& b);
}

template<typename T>
void Helper::Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

