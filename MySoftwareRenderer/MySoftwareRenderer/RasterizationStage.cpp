#include "RasterizationStage.h"
#include "Image/ImageManager.h"

bool RasterizationStage::InitDX(HINSTANCE hInstance, HWND hWnd, int width, int height)
{
	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);
	d3d9->Release();

	pDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSurface, 0);

	return true;
}

void RasterizationStage::CRasterizer::DrawPixel(int x, int y, Color4F src)
{
	Blending(src, COLOR_BUFFER[y][x]);
}

void RasterizationStage::FlipDXSurface()
{
	// 获取后台缓存
	IDirect3DSurface9* backBuffer = 0;
	pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	// 使用自定义表面填充后台缓存
	pDevice->StretchRect(pSurface, 0, backBuffer, 0, D3DTEXF_NONE);

	// GetBackBuffer所得的缓存需要被释放，否则会内存泄露
	backBuffer->Release();

	// 将交换链中的后台缓存显示
	pDevice->Present(0, 0, 0, 0);
}

void RasterizationStage::ReleaseDX()
{
	pSurface->Release();
	pDevice->Release();
}

void RasterizationStage::FillDXSurface()
{
	memset(&lockedRect, 0, sizeof(lockedRect));
	pSurface->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD);

	Color4F** pColorBuffer = CRasterizer::GetInstance()->GetColorBuffer();
	DWORD* pBits = (DWORD*)RasterizationStage::lockedRect.pBits;
	for (int i = 0; i < SCREEN_WIDTH; ++i)
	{
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			pBits[i + j * (RasterizationStage::lockedRect.Pitch >> 2)] = pColorBuffer[j][i].ToARGB();
		}
	}

	pSurface->UnlockRect();
}

bool RasterizationStage::CRasterizer::OwnershipTest(int x, int y)
{
	if ( x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT )
		return true;

	return false;
}

int RasterizationStage::CRasterizer::ConvertToPixelPos(float value)
{
	return ceil(value);
}

void RasterizationStage::CRasterizer::DrawLine(int x1, int y1, int x2, int y2, Color4F color)
{
	// - DDA
	if ( x2 - x1 == 0 )
	{
		if (y1 > y2)
		{
			Helper::Swap(x1, x2);
			Helper::Swap(y1, y2);
		}

		for (int iCurY = y1; iCurY <= y2; ++iCurY)
		{
			if (CanDrawPixel(x1, iCurY, 0))
				DrawPixel(x1, iCurY, color);
		}
	}
	else
	{
		float k = (float)(y2 - y1) / (x2 - x1);
		float kInverse = (float)(x2 - x1) / (y2 - y1);
		if (fabs(k) <= 1)
		{
			if (x1 > x2)
			{
				Helper::Swap(x1, x2);
				Helper::Swap(y1, y2);
			}

			float iCurY = y1;
			for (int iCurX = x1; iCurX <= x2; ++iCurX)
			{
				int iPixelY = ConvertToPixelPos(iCurY);
				if (CanDrawPixel(iCurX, iPixelY, 0))
					DrawPixel(iCurX, iPixelY, color);
				iCurY += k;
			}
		}
		else
		{
			if (y1 > y2)
			{
				Helper::Swap(x1, x2);
				Helper::Swap(y1, y2);
			}

			float iCurX = x1;
			for (int iCurY = y1; iCurY <= y2; ++iCurY)
			{
				int iPixelX = ConvertToPixelPos(iCurX);
				if (CanDrawPixel(iPixelX, iCurY, 0))
					DrawPixel(iPixelX, iCurY, color);
				iCurX += kInverse;
			}
		}
	}
}

void RasterizationStage::CRasterizer::DrawAnyTriangle(SVertexRuntime& v1, SVertexRuntime& v2, SVertexRuntime& v3, float fAlpha, SRenderState* pRenderState)
{
	if (pRenderState->m_bDrawWireFrame)
	{
		DrawLine(v1.m_pos.x, v1.m_pos.y, v2.m_pos.x, v2.m_pos.y, Color4F::WHITE);
		DrawLine(v2.m_pos.x, v2.m_pos.y, v3.m_pos.x, v3.m_pos.y, Color4F::WHITE);
		DrawLine(v1.m_pos.x, v1.m_pos.y, v3.m_pos.x, v3.m_pos.y, Color4F::WHITE);
	}
	else
	{
		if (v1.m_pos.y > v2.m_pos.y)
			Helper::Swap(v1, v2);

		if (v1.m_pos.y > v3.m_pos.y)
			Helper::Swap(v1, v3);

		if (v2.m_pos.y > v3.m_pos.y)
			Helper::Swap(v2, v3);

		if (v1.m_pos.y == v2.m_pos.y && v2.m_pos.y == v3.m_pos.y
			|| v1.m_pos.x == v2.m_pos.x && v2.m_pos.x == v3.m_pos.x)
			return;

		if (v2.m_pos.y == v3.m_pos.y)
		{
			DrawBottomTriangle(v1, v2, v3, fAlpha, pRenderState);
		}
		else if (v1.m_pos.y == v2.m_pos.y)
		{
			DrawTopTriangle(v1, v2, v3, fAlpha, pRenderState);
		}
		else
		{
			if (v2.m_pos.x < v3.m_pos.x)
			{
				float kInverseSlopeRightX = (v3.m_pos.x - v1.m_pos.x) / (v3.m_pos.y - v1.m_pos.y);
				float kInverseSlopeRightZ = (v3.m_pos.z - v1.m_pos.z) / (v3.m_pos.y - v1.m_pos.y);

				Color4F kInverseSlopeRightColor = Color4F((v3.m_color.r - v1.m_color.r) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.g - v1.m_color.g) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.b - v1.m_color.b) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.a - v1.m_color.a) / (v3.m_pos.y - v1.m_pos.y));

				Vec2 kInverseSlopeRightUV = Vec2((v3.m_UV.x - v1.m_UV.x) / (v3.m_pos.y - v1.m_pos.y), (v3.m_UV.y - v1.m_UV.y) / (v3.m_pos.y - v1.m_pos.y));

				float kInverseSlopeRightInverseZ = (v3.m_inverseZ - v1.m_inverseZ) / (v3.m_pos.y - v1.m_pos.y);

				SVertexRuntime newVertex;
				newVertex.m_pos.set(v1.m_pos.x + (v2.m_pos.y - v1.m_pos.y) * kInverseSlopeRightX, v2.m_pos.y, v1.m_pos.z + (v2.m_pos.y - v1.m_pos.y) * kInverseSlopeRightZ);
				newVertex.m_color = (v1.m_color + kInverseSlopeRightColor * (v2.m_pos.y - v1.m_pos.y));
				newVertex.m_UV = v1.m_UV + kInverseSlopeRightUV * (v2.m_pos.y - v1.m_pos.y);
				newVertex.m_inverseZ = v1.m_inverseZ + kInverseSlopeRightInverseZ * (v2.m_pos.y - v1.m_pos.y);
				DrawAnyTriangle(v1, v2, newVertex, fAlpha, pRenderState);
				DrawAnyTriangle(v2, newVertex, v3, fAlpha, pRenderState);
			}
			else if ( v2.m_pos.x >= v3.m_pos.x )
			{
				float kInverseSlopeLeftX = (v3.m_pos.x - v1.m_pos.x) / (v3.m_pos.y - v1.m_pos.y);
				float kInverseSlopeLeftZ = (v3.m_pos.z - v1.m_pos.z) / (v3.m_pos.y - v1.m_pos.y);

				Color4F kInverseSlopeLeftColor = Color4F((v3.m_color.r - v1.m_color.r) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.g - v1.m_color.g) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.b - v1.m_color.b) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.a - v1.m_color.a) / (v3.m_pos.y - v1.m_pos.y));

				Vec2 kInverseSlopeLeftUV = Vec2((v3.m_UV.x - v1.m_UV.x) / (v3.m_pos.y - v1.m_pos.y), (v3.m_UV.y - v1.m_UV.y) / (v3.m_pos.y - v1.m_pos.y));

				float kInverseSlopeLeftInverseZ = (v3.m_inverseZ - v1.m_inverseZ) / (v3.m_pos.y - v1.m_pos.y);

				SVertexRuntime newVertex;
				newVertex.m_pos.set(v1.m_pos.x + (v2.m_pos.y - v1.m_pos.y) * kInverseSlopeLeftX, v2.m_pos.y, v1.m_pos.z + (v2.m_pos.y - v1.m_pos.y) * kInverseSlopeLeftZ);
				newVertex.m_color = (v1.m_color + kInverseSlopeLeftColor * (v2.m_pos.y - v1.m_pos.y));
				newVertex.m_UV = (v1.m_UV + kInverseSlopeLeftUV * (v2.m_pos.y - v1.m_pos.y));
				newVertex.m_inverseZ = (v1.m_inverseZ + kInverseSlopeLeftInverseZ * (v2.m_pos.y - v1.m_pos.y));
				DrawAnyTriangle(v1, newVertex, v2, fAlpha, pRenderState);
				DrawAnyTriangle(newVertex, v2, v3, fAlpha, pRenderState);
			}
		}
	}
}

void RasterizationStage::CRasterizer::DrawBottomTriangle(SVertexRuntime &v1, SVertexRuntime &v2, SVertexRuntime &v3, float fAlpha, SRenderState* pRenderState)
{
	float kInverseSlopeLeftX = (v1.m_pos.x - v2.m_pos.x) / (v1.m_pos.y - v2.m_pos.y);
	float kInverseSlopeRightX = (v1.m_pos.x - v3.m_pos.x) / (v1.m_pos.y - v3.m_pos.y);

	Color4F kInverseSlopeLeftColor = Color4F((v1.m_color.r - v2.m_color.r) / (v1.m_pos.y - v2.m_pos.y),
		(v1.m_color.g - v2.m_color.g) / (v1.m_pos.y - v2.m_pos.y),
		(v1.m_color.b - v2.m_color.b) / (v1.m_pos.y - v2.m_pos.y),
		(v1.m_color.a - v2.m_color.a) / (v1.m_pos.y - v2.m_pos.y));

	Color4F kInverseSlopeRightColor = Color4F((v1.m_color.r - v3.m_color.r) / (v1.m_pos.y - v3.m_pos.y),
		(v1.m_color.g - v3.m_color.g) / (v1.m_pos.y - v3.m_pos.y),
		(v1.m_color.b - v3.m_color.b) / (v1.m_pos.y - v3.m_pos.y),
		(v1.m_color.a - v3.m_color.a) / (v1.m_pos.y - v3.m_pos.y));

	Vec2 kInverseSlopeLeftUV = Vec2((v1.m_UV.x - v2.m_UV.x) / (v1.m_pos.y - v2.m_pos.y), (v1.m_UV.y - v2.m_UV.y) / (v1.m_pos.y - v2.m_pos.y));
	Vec2 kInverseSlopeRightUV = Vec2((v1.m_UV.x - v3.m_UV.x) / (v1.m_pos.y - v3.m_pos.y), (v1.m_UV.y - v3.m_UV.y) / (v1.m_pos.y - v3.m_pos.y));

	float kInverseSlopeLeftInverseZ = (v1.m_inverseZ - v2.m_inverseZ) / (v1.m_pos.y - v2.m_pos.y);
	float kInverseSlopeRightInverseZ = (v1.m_inverseZ - v3.m_inverseZ) / (v1.m_pos.y - v3.m_pos.y);

	float kInverseSlopeLeftZ = (v1.m_pos.z - v2.m_pos.z) / (v1.m_pos.y - v2.m_pos.y);
	float kInverseSlopeRightZ = (v1.m_pos.z - v3.m_pos.z) / (v1.m_pos.y - v3.m_pos.y);

	int iStartY = ConvertToPixelPos(v1.m_pos.y);
	int iEndY = ConvertToPixelPos(v2.m_pos.y);
	float fOffsetY = (iStartY - v1.m_pos.y);

	float fLeftX = v1.m_pos.x + kInverseSlopeLeftX * fOffsetY;
	float fRightX = v1.m_pos.x + kInverseSlopeRightX * fOffsetY;

	Color4F leftColor = v1.m_color + kInverseSlopeLeftColor * fOffsetY;
	Color4F rightColor = v1.m_color + kInverseSlopeRightColor * fOffsetY;

	Vec2 leftUV = v1.m_UV + kInverseSlopeLeftUV * fOffsetY;
	Vec2 rightUV = v1.m_UV + kInverseSlopeRightUV * fOffsetY;

	float fLeftInverseZ = v1.m_inverseZ + kInverseSlopeLeftInverseZ * fOffsetY;
	float fRightInverseZ = v1.m_inverseZ + kInverseSlopeRightInverseZ * fOffsetY;

	float fLeftZ = v1.m_pos.z + kInverseSlopeLeftZ * fOffsetY;
	float fRightZ = v1.m_pos.z + kInverseSlopeRightZ * fOffsetY;

	for (int y = iStartY; y < iEndY; ++y)
	{
		int iStartX = ConvertToPixelPos(fLeftX);
		int iEndX = ConvertToPixelPos(fRightX);

		int iDeltaX = iEndX - iStartX;
		if (iDeltaX != 0)
		{
			int iStep = 1;

			float fDeltaX = fRightX - fLeftX;
			Color4F kInverseSlopeColor = (rightColor - leftColor) / fDeltaX;
			Vec2 kInverseSlopeUV = (rightUV - leftUV) / fDeltaX;
			float kInverseSlopeInverseZ = (fRightInverseZ - fLeftInverseZ) / fDeltaX;
			float kInverseSlopeZ = (fRightZ - fLeftZ) / fDeltaX;
			if (iDeltaX < 0)
			{
				kInverseSlopeColor = kInverseSlopeColor * (-1);
				kInverseSlopeUV = kInverseSlopeUV * (-1);
				kInverseSlopeInverseZ = kInverseSlopeInverseZ * (-1);
				kInverseSlopeZ = kInverseSlopeZ * (-1);

				iStep = -1;
				iStartX -= 1;
			}
			float fOffsetX = iStartX - fLeftX;

			Color4F curColor = leftColor + kInverseSlopeColor * fOffsetX;
			Vec2 curUV = leftUV + kInverseSlopeUV * fOffsetX;
			float curInverseZ = fLeftInverseZ + kInverseSlopeInverseZ * fOffsetX;
			float curZ = fLeftZ + kInverseSlopeZ * fOffsetX;

			iDeltaX = abs(iDeltaX);
			for (int i = 0; i < iDeltaX; ++i)
			{
				int iPixelX = iStartX + i * iStep;
				int iPixelY = y;
				if (CanDrawPixel(iPixelX, iPixelY, curZ))
				{
					Color4F finalColor = curColor / curInverseZ * fAlpha;
					if (pRenderState->m_pMaterial)
						finalColor = SampleTexture(pRenderState->m_pMaterial->GetBaseColorTex(), curUV / curInverseZ) * finalColor;
					if (AlphaTest(finalColor.a))
						DrawPixel(iPixelX, iPixelY, finalColor);
				}

				curColor += kInverseSlopeColor;
				curUV += kInverseSlopeUV;
				curInverseZ += kInverseSlopeInverseZ;
				curZ += kInverseSlopeZ;
			}
		}

		fLeftX += kInverseSlopeLeftX;
		fRightX += kInverseSlopeRightX;

		leftColor += kInverseSlopeLeftColor;
		rightColor += kInverseSlopeRightColor;

		leftUV += kInverseSlopeLeftUV;
		rightUV += kInverseSlopeRightUV;

		fLeftInverseZ += kInverseSlopeLeftInverseZ;
		fRightInverseZ += kInverseSlopeRightInverseZ;

		fLeftZ += kInverseSlopeLeftZ;
		fRightZ += kInverseSlopeRightZ;
	}
}

void RasterizationStage::CRasterizer::DrawTopTriangle(SVertexRuntime &v1, SVertexRuntime &v2, SVertexRuntime &v3, float fAlpha, SRenderState* pRenderState)
{
	float kInverseSlopeLeftX = (v3.m_pos.x - v1.m_pos.x) / (v3.m_pos.y - v1.m_pos.y);
	float kInverseSlopeRightX = (v3.m_pos.x - v2.m_pos.x) / (v3.m_pos.y - v2.m_pos.y);

	Color4F kInverseSlopeLeftColor = Color4F((v3.m_color.r - v1.m_color.r) / (v3.m_pos.y - v1.m_pos.y),
		(v3.m_color.g - v1.m_color.g) / (v3.m_pos.y - v1.m_pos.y),
		(v3.m_color.b - v1.m_color.b) / (v3.m_pos.y - v1.m_pos.y),
		(v3.m_color.a - v1.m_color.a) / (v3.m_pos.y - v1.m_pos.y));

	Color4F kInverseSlopeRightColor = Color4F((v3.m_color.r - v2.m_color.r) / (v3.m_pos.y - v2.m_pos.y),
		(v3.m_color.g - v2.m_color.g) / (v3.m_pos.y - v2.m_pos.y),
		(v3.m_color.b - v2.m_color.b) / (v3.m_pos.y - v2.m_pos.y),
		(v3.m_color.a - v2.m_color.a) / (v3.m_pos.y - v2.m_pos.y));

	Vec2 kInverseSlopeLeftUV = Vec2((v3.m_UV.x - v1.m_UV.x) / (v3.m_pos.y - v1.m_pos.y), (v3.m_UV.y - v1.m_UV.y) / (v3.m_pos.y - v1.m_pos.y));
	Vec2 kInverseSlopeRightUV = Vec2((v3.m_UV.x - v2.m_UV.x) / (v3.m_pos.y - v2.m_pos.y), (v3.m_UV.y - v2.m_UV.y) / (v3.m_pos.y - v2.m_pos.y));

	float kInverseSlopeLeftInverseZ = (v3.m_inverseZ - v1.m_inverseZ) / (v3.m_pos.y - v1.m_pos.y);
	float kInverseSlopeRightInverseZ = (v3.m_inverseZ - v2.m_inverseZ) / (v3.m_pos.y - v2.m_pos.y);

	float kInverseSlopeLeftZ = (v3.m_pos.z - v1.m_pos.z) / (v3.m_pos.y - v1.m_pos.y);
	float kInverseSlopeRightZ = (v3.m_pos.z - v2.m_pos.z) / (v3.m_pos.y - v2.m_pos.y);

	int iStartY = ConvertToPixelPos(v1.m_pos.y);
	int iEndY = ConvertToPixelPos(v3.m_pos.y);
	float fOffsetY = (iStartY - v1.m_pos.y);

	float fLeftX = v1.m_pos.x + kInverseSlopeLeftX * fOffsetY;
	float fRightX = v2.m_pos.x + kInverseSlopeRightX * fOffsetY;

	Color4F leftColor = v1.m_color + kInverseSlopeLeftColor * fOffsetY;
	Color4F rightColor = v2.m_color + kInverseSlopeRightColor * fOffsetY;

	Vec2 leftUV = v1.m_UV + kInverseSlopeLeftUV * fOffsetY;
	Vec2 rightUV = v2.m_UV + kInverseSlopeRightUV * fOffsetY;

	float fLeftInverseZ = v1.m_inverseZ + kInverseSlopeLeftInverseZ * fOffsetY;
	float fRightInverseZ = v2.m_inverseZ + kInverseSlopeRightInverseZ * fOffsetY;

	float fLeftZ = v1.m_pos.z + kInverseSlopeLeftZ * fOffsetY;
	float fRightZ = v2.m_pos.z + kInverseSlopeRightZ * fOffsetY;
	
	for (int y = iStartY; y < iEndY; ++y)
	{
		int iStartX = ConvertToPixelPos(fLeftX);
		int iEndX = ConvertToPixelPos(fRightX);

		int iDeltaX = iEndX - iStartX;
		if (iDeltaX != 0)
		{
			int iStep = 1;

			float fDeltaX = fRightX - fLeftX;
			Color4F kInverseSlopeColor = (rightColor - leftColor) / fDeltaX;
			Vec2 kInverseSlopeUV = (rightUV - leftUV) / fDeltaX;
			float kInverseSlopeInverseZ = (fRightInverseZ - fLeftInverseZ) / fDeltaX;
			float kInverseSlopeZ = (fRightZ - fLeftZ) / fDeltaX;
			if (iDeltaX < 0)
			{
				kInverseSlopeColor = kInverseSlopeColor * (-1);
				kInverseSlopeUV = kInverseSlopeUV * (-1);
				kInverseSlopeInverseZ = kInverseSlopeInverseZ * (-1);
				kInverseSlopeZ = kInverseSlopeZ * (-1);

				iStep = -1;
				iStartX -= 1;
			}
			float fOffsetX = iStartX - fLeftX;

			Color4F curColor = leftColor + kInverseSlopeColor * fOffsetX;
			Vec2 curUV = leftUV + kInverseSlopeUV * fOffsetX;
			float curInverseZ = fLeftInverseZ + kInverseSlopeInverseZ * fOffsetX;
			float curZ = fLeftZ + kInverseSlopeZ * fOffsetX;

			iDeltaX = abs(iDeltaX);
			for (int i = 0; i < iDeltaX; ++i)
			{
				int iPixelX = iStartX + i * iStep;
				int iPixelY = y;
				if (CanDrawPixel(iPixelX, iPixelY, curZ))
				{
					Color4F finalColor = curColor / curInverseZ * fAlpha;
					if (pRenderState->m_pMaterial)
						finalColor = SampleTexture(pRenderState->m_pMaterial->GetBaseColorTex(), curUV / curInverseZ) * finalColor;
					if (AlphaTest(finalColor.a))
						DrawPixel(iPixelX, iPixelY, finalColor);
				}

				curColor += kInverseSlopeColor;
				curUV += kInverseSlopeUV;
				curInverseZ += kInverseSlopeInverseZ;
				curZ += kInverseSlopeZ;
			}
		}

		fLeftX += kInverseSlopeLeftX;
		fRightX += kInverseSlopeRightX;

		leftColor += kInverseSlopeLeftColor;
		rightColor += kInverseSlopeRightColor;

		leftUV += kInverseSlopeLeftUV;
		rightUV += kInverseSlopeRightUV;

		fLeftInverseZ += kInverseSlopeLeftInverseZ;
		fRightInverseZ += kInverseSlopeRightInverseZ;

		fLeftZ += kInverseSlopeLeftZ;
		fRightZ += kInverseSlopeRightZ;
	}
}

Color4F RasterizationStage::CRasterizer::SampleTexture(int iTextureID, Vec2 uv)
{
	//return Color4F(1.0, 1.0, 1.0, 1.f);

	const CTexture* pTexture = CImageManager::GetInstance()->FindTexture(iTextureID);
	if ( !pTexture )
	{
		return Color4F::WHITE;
	}
	else
	{
		return SampleNearset(pTexture, uv);
	}
}

RasterizationStage::CRasterizer* RasterizationStage::CRasterizer::GetInstance()
{
	if ( !s_pInstance )
	{
		s_pInstance = new CRasterizer;
	}

	return s_pInstance;
}

void RasterizationStage::CRasterizer::Init()
{
	COLOR_BUFFER = new Color4F*[SCREEN_HEIGHT];
	memset(COLOR_BUFFER, 0, SCREEN_HEIGHT);
	for (int i = 0; i < SCREEN_HEIGHT; ++i)
	{
		COLOR_BUFFER[i] = new Color4F[SCREEN_WIDTH];
		memset(COLOR_BUFFER[i], 0, SCREEN_WIDTH);
	}

	ZBUFFER = new float*[SCREEN_HEIGHT];
	memset(ZBUFFER, 0, SCREEN_HEIGHT);
	for (int i = 0; i < SCREEN_HEIGHT; ++i)
	{
		ZBUFFER[i] = new float[SCREEN_WIDTH];
		memset(ZBUFFER[i], 0, SCREEN_WIDTH);
	}
}

bool RasterizationStage::CRasterizer::CanDrawPixel(int x, int y, float z)
{
	if (!OwnershipTest(x, y))
		return false;

	if (!DepthTest(x, y, z))
		return false;

	return true;
}

void RasterizationStage::CRasterizer::ClearDepthBuffer(float val)
{
	for (int i = 0; i < SCREEN_HEIGHT; ++i)
	{
		for (int j = 0; j < SCREEN_WIDTH; ++j)
		{
			ZBUFFER[i][j] = val;
		}
	}
}

void RasterizationStage::CRasterizer::ClearColorBuffer(Color4F val)
{
	for (int i = 0; i < SCREEN_HEIGHT; ++i)
	{
		for (int j = 0; j < SCREEN_WIDTH; ++j)
		{
			COLOR_BUFFER[i][j] = val;
		}
	}
}

bool RasterizationStage::CRasterizer::DepthTest(int x, int y, float z)
{
	if (ZBUFFER[y][x] >= z)
	{
		ZBUFFER[y][x] = z;
		return true;
	}

	return false;
}

bool RasterizationStage::CRasterizer::AlphaTest(float fAlpha)
{
	if ( fAlpha <= 0 )
		return false;

	return true;
}

void RasterizationStage::CRasterizer::Blending(Color4F& src, Color4F& dst)
{
	dst = src * src.a + dst * (1.0f - src.a);
}

Color4F RasterizationStage::CRasterizer::SampleNearset(const CTexture* pTexture, Vec2 &uv)
{
	int fWidth = pTexture->m_iWidth - 1;
	int fHeight = pTexture->m_iHeight - 1;

	unsigned char* pData = pTexture->m_pData;
	int iU = int(uv.x * fWidth + 0.5f);
	int iV = int(uv.y * fHeight + 0.5f);

	//Helper::Clamp(iU, 0, fWidth);
	//Helper::Clamp(iV, 0, fHeight);

	unsigned char* pColorData = &pData[iV * pTexture->m_iWidth * 4 + iU * 4];
	return Color4F(pColorData[0] / 255.0f, pColorData[1] / 255.0f, pColorData[2] / 255.0f, pColorData[3] / 255.0f);
}

Color4F RasterizationStage::CRasterizer::SampleLinear(const CTexture* pTexture, Vec2 &uv)
{
	int fWidth = pTexture->m_iWidth - 1;
	int fHeight = pTexture->m_iHeight - 1;
	unsigned char* pData = pTexture->m_pData;
	uv.x *= fWidth;
	uv.y *= fHeight;

	float fPercentU = uv.x - int(uv.x);
	float fPercentV = uv.y - int(uv.y);

	int RU = int(uv.x);
	int LU = RU - 1;

	int BV = int(uv.y);
	int TV = BV - 1;

	Helper::Clamp(LU, 0, fWidth);
	Helper::Clamp(RU, 0, fWidth);
	Helper::Clamp(TV, 0, fHeight);
	Helper::Clamp(BV, 0, fHeight);

	unsigned char* pLeftTopColorData = &pData[TV * pTexture->m_iWidth * 4 + LU * 4];
	unsigned char* pLeftBottomColorData = &pData[BV * pTexture->m_iWidth * 4 + LU * 4];
	unsigned char* pRightTopColorData = &pData[TV * pTexture->m_iWidth * 4 + RU * 4];
	unsigned char* pRightBottomColorData = &pData[BV * pTexture->m_iWidth * 4 + RU * 4];

	Color4F c1 = Color4F(pLeftTopColorData[0] / 255.0f, pLeftTopColorData[1] / 255.0f, pLeftTopColorData[2] / 255.0f, pLeftTopColorData[3] / 255.0f) * (1.0f - fPercentU)
		+ Color4F(pRightTopColorData[0] / 255.0f, pRightTopColorData[1] / 255.0f, pRightTopColorData[2] / 255.0f, pRightTopColorData[3] / 255.0f) * fPercentU;

	Color4F c2 = Color4F(pLeftBottomColorData[0] / 255.0f, pLeftBottomColorData[1] / 255.0f, pLeftBottomColorData[2] / 255.0f, pLeftBottomColorData[3] / 255.0f) * (1.0f - fPercentU)
		+ Color4F(pRightBottomColorData[0] / 255.0f, pRightBottomColorData[1] / 255.0f, pRightBottomColorData[2] / 255.0f, pRightBottomColorData[3] / 255.0f) * fPercentU;

	Color4F c3 = c2 * fPercentV + c1 * (1.0f - fPercentV);
	return c3;
}

RasterizationStage::CRasterizer* RasterizationStage::CRasterizer::s_pInstance;
