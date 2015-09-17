#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "Utility.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "DataTypes.h"

namespace RasterizationStage
{
	// ȫ�ֱ���
	static IDirect3DDevice9* pDevice = 0;
	static IDirect3DSurface9* pSurface = 0;
	static D3DLOCKED_RECT lockedRect;

	// ����
	bool InitDX(HINSTANCE hInstance, HWND hWnd, int width, int height);
	void ReleaseDX();
	int LockSurface();
	int UnlockSurface();
	void FlipSurface();

	class CRasterizer
	{
	public:
		void Init();
		bool IsOutSideScreen(int x, int y);
		float ConvertToPixelPos(float value);

		void ClearDepthBuffer(float val);
		bool CanDrawPixel(int x, int y, float z);
		int DrawPixel(int x, int y, DWORD color);
		void DrawLine(int x1, int y1, int x2, int y2, DWORD color);
		void DrawAnyTriangle(SVertexRuntime& v1, SVertexRuntime& v2, SVertexRuntime& v3, bool bWireFrame = true, int iTextureID = 0);
		void DrawTopTriangle(SVertexRuntime &v1, SVertexRuntime &v2, SVertexRuntime &v3);
		void DrawBottomTriangle(SVertexRuntime &v1, SVertexRuntime &v2, SVertexRuntime &v3);
		Color4F SampleTexture(int iTextureID, Vec2 uv);

		static CRasterizer* GetInstance();

	private:
		static CRasterizer* s_pInstance;

		float** ZBUFFER;
	};
}
