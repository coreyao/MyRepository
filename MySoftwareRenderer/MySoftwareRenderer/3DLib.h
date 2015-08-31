#ifndef CPPYIN_3DLib
#define CPPYIN_3DLib

#include <d3d9.h>
#include <d3dx9.h>

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
	int DrawPixel(int x, int y, DWORD color);
	void FlipSurface();
	void Release3DLib();

	template<typename T>
	void Swap( T& a, T& b );
	void DrawLine(int x1, int y1, int x2, int y2, DWORD color);
}

template<typename T>
void Lib3D::Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

#endif