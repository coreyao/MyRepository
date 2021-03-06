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
	bool InitDX(HINSTANCE hInstance, HWND hWnd, int width, int height);
	void ReleaseDX();
	void FillDXSurface();
	void FlipDXSurface();

	class CRasterizer
	{
	public:
		void Init();

		bool CanDrawPixel(int x, int y, float z);
		bool OwnershipTest(int x, int y);
		bool DepthTest(int x, int y, float z);
		bool AlphaTest(float fAlpha);
		void Blending(Color4F& src, Color4F& dst);

		void ClearDepthBuffer(float val);
		void ClearColorBuffer(Color4F val);

		void DrawPixel(int x, int y, Color4F src);
		void DrawLine(int x1, int y1, int x2, int y2, Color4F color);
		void DrawAnyTriangle(CVertexRuntime& v1, CVertexRuntime& v2, CVertexRuntime& v3, float fAlpha, CRenderState* pRenderState);

		void DrawScanline(HighPrecision fLeftX, HighPrecision fRightX, CVariable* leftVal, CVariable* rightVal, CVariable* leftCustomVal, CVariable* rightCustomVal, int y, float fAlpha, CRenderState* pRenderState);

		void DrawTopTriangle(CVertexRuntime &v1, CVertexRuntime &v2, CVertexRuntime &v3, float fAlpha, CRenderState* pRenderState);
		void DrawBottomTriangle(CVertexRuntime &v1, CVertexRuntime &v2, CVertexRuntime &v3, float fAlpha, CRenderState* pRenderState);

		Color4F SampleTexture(int iTextureID, Vec2 uv);
		Color4F SampleNearset(const CTexture* pTexture, Vec2 &uv);
		Color4F SampleLinear(const CTexture* pTexture, Vec2 &uv);

		Color4F** GetColorBuffer() { return COLOR_BUFFER; }
		int ConvertToPixelPos(float value);

		static CRasterizer* GetInstance();

	private:
		static CRasterizer* s_pInstance;

		float** ZBUFFER;
		Color4F** COLOR_BUFFER;
	};
}
