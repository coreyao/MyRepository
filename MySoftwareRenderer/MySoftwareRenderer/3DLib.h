#ifndef CPPYIN_3DLib
#define CPPYIN_3DLib

#include <d3d9.h>
#include <d3dx9.h>
#include "Math.h"

// 宏定义

// 宏脚本
#define ARGB(a, r, g, b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

namespace Lib3D
{
	// 全局变量
	static IDirect3DDevice9* pDevice = 0;
	static IDirect3DSurface9* pSurface = 0;
	static D3DLOCKED_RECT lockedRect;

	// 函数
	bool Init3DLib(HINSTANCE hInstance, HWND hWnd, int width, int height);
	int LockSurface();
	int UnlockSurface();
	void FlipSurface();
	void Release3DLib();
	
	int DrawPixel(int x, int y, DWORD color);
	void DrawLine(int x1, int y1, int x2, int y2, DWORD color);
	void DrawTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3, bool bWireFrame = true);
}

#endif