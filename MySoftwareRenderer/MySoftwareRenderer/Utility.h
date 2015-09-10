#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <memory>

using namespace std;

#define PI 3.141592f
#define DEG_TO_RAD(x) ((PI) * ((x) / (180.0f))) 
#define RAD_TO_DEG(x) ((180) * ((x) / (PI)) )
#define EPSILON 0.00001f
#define NEARLY_EQUAL(x,y) (fabs((x) - (y)) <= EPSILON)
#define ARGB(a, r, g, b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#ifndef MESH_FILE_DIR
#define MESH_FILE_DIR std::string("./Resource/Mesh/")
#endif

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

enum EVertexOrder
{
	EVertexOrder_ClockWise,
	EVertexOrder_Counter_ClockWise,
};

class CRenderObject
{
public:
	CRenderObject();

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

	bool m_bEnableCullFace;
	bool m_bDrawWireFrame;
	EVertexOrder m_eVertexOrder;
};

typedef shared_ptr<CRenderObject> RenderObjPtr;

