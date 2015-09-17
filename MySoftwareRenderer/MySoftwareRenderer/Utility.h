#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <list>

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

#ifndef IMAGE_FILE_DIR
#define IMAGE_FILE_DIR std::string("./Resource/Images/")
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

class CTexture
{
public:
	CTexture()
		: m_iWidth(0)
		, m_iHeight(0)
		, m_pData(nullptr)
	{
	}

	int m_iWidth;
	int m_iHeight;
	unsigned char* m_pData;
};

class CSampler
{
public:
	enum EUVWrapMode
	{
		EUVWrapMode_Repeat,
		EUVWrapMode_Clamp,
	};

	enum ETextureFilter
	{
		ETextureFilter_Nearest,
		ETextureFilter_Liner,
	};

	CSampler()
		: UV_WRAP_S(EUVWrapMode_Clamp)
		, UV_WRAP_T(EUVWrapMode_Clamp)
		, TEXTURE_MAG_FILTER(ETextureFilter_Nearest)
		, TEXTURE_MIN_FILTER(ETextureFilter_Nearest)
	{
	}

	EUVWrapMode UV_WRAP_S;
	EUVWrapMode UV_WRAP_T;

	ETextureFilter TEXTURE_MAG_FILTER;
	ETextureFilter TEXTURE_MIN_FILTER;
};

class CMaterial
{
public:
	CMaterial();

	int GetBaseColorTex();
	void SetBaseColorTexture(const std::string& sFileName);

private:
	int m_baseColorTex;
};

