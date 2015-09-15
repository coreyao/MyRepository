#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "Utility.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "DataTypes.h"

namespace RasterizationStage
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

	bool IsOutSideScreen(int x, int y);
	float ConvertToPixelPos(float value);
	
	int DrawPixel(int x, int y, DWORD color);
	void DrawLine(int x1, int y1, int x2, int y2, DWORD color);
	void DrawAnyTriangle(SVertex& v1, SVertex& v2, SVertex& v3, bool bWireFrame = true, int iTextureID = 0);
	void DrawTopTriangle(SVertex &v1, SVertex &v2, SVertex &v3);
	void DrawBottomTriangle(SVertex &v1, SVertex &v2, SVertex &v3);
	Color4F SampleTexture(int iTextureID, Vec2 uv);
}
