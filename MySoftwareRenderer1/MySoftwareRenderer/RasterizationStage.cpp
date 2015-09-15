#include "RasterizationStage.h"
#include "Image/ImageManager.h"

bool RasterizationStage::Init3DLib(HINSTANCE hInstance, HWND hWnd, int width, int height)
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

int RasterizationStage::LockSurface()
{
	memset(&lockedRect, 0, sizeof(lockedRect));
	pSurface->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD);
	for (int i = 0; i < SCREEN_WIDTH; ++i)
	{
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
			DrawPixel(i, j, 0);
	}

	return 1;
}

int RasterizationStage::UnlockSurface()
{
	pSurface->UnlockRect();
	return 1;
}

int RasterizationStage::DrawPixel(int x, int y, DWORD color)
{
	if (IsOutSideScreen(x, y))
		return 0;

	DWORD* pBits = (DWORD*)lockedRect.pBits;
	pBits[x + y * (lockedRect.Pitch >> 2)] = color;

	return 1;
}

void RasterizationStage::FlipSurface()
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

void RasterizationStage::Release3DLib()
{
	pSurface->Release();
	pDevice->Release();
}

bool RasterizationStage::IsOutSideScreen(int x, int y)
{
	if ( x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT )
		return true;

	return false;
}

float RasterizationStage::ConvertToPixelPos(float value)
{
	return floor(value + 0.5f);
	//return int(value + 0.5f);
}

void RasterizationStage::DrawLine(int x1, int y1, int x2, int y2, DWORD color)
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
			DrawPixel(x1, iCurY, color);
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
				DrawPixel(iCurX, ConvertToPixelPos(iCurY), color);
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
				DrawPixel(ConvertToPixelPos(iCurX), iCurY, color);
				iCurX += kInverse;
			}
		}
	}
}

void RasterizationStage::DrawAnyTriangle(SVertex& v1, SVertex& v2, SVertex& v3, bool bWireFrame /*= true*/, int iTextureID /*= 0*/)
{
	v1.m_pos.x = ConvertToPixelPos(v1.m_pos.x);
	v1.m_pos.y = ConvertToPixelPos(v1.m_pos.y);
	v2.m_pos.x = ConvertToPixelPos(v2.m_pos.x);
	v2.m_pos.y = ConvertToPixelPos(v2.m_pos.y);
	v3.m_pos.x = ConvertToPixelPos(v3.m_pos.x);
	v3.m_pos.y = ConvertToPixelPos(v3.m_pos.y);

	if (bWireFrame)
	{
		DrawLine(v1.m_pos.x, v1.m_pos.y, v2.m_pos.x, v2.m_pos.y, 0xffffffff);
		DrawLine(v2.m_pos.x, v2.m_pos.y, v3.m_pos.x, v3.m_pos.y, 0xffffffff);
		DrawLine(v1.m_pos.x, v1.m_pos.y, v3.m_pos.x, v3.m_pos.y, 0xffffffff);
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
			DrawBottomTriangle(v1, v2, v3);
		}
		else if (v1.m_pos.y == v2.m_pos.y)
		{
			DrawTopTriangle(v1, v2, v3);
		}
		else
		{
			if (v2.m_pos.x < v3.m_pos.x)
			{
				float kInverseSlopeRightX = (v3.m_pos.x - v1.m_pos.x) / (v3.m_pos.y - v1.m_pos.y);

				Color4F kInverseSlopeRightColor = Color4F((v3.m_color.r - v1.m_color.r) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.g - v1.m_color.g) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.b - v1.m_color.b) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.a - v1.m_color.a) / (v3.m_pos.y - v1.m_pos.y));

				Vec2 kInverseSlopeRightUV = Vec2((v3.m_UV.x - v1.m_UV.x) / (v3.m_pos.y - v1.m_pos.y), (v3.m_UV.y - v1.m_UV.y) / (v3.m_pos.y - v1.m_pos.y));

				SVertex newVertex;
				newVertex.m_pos.set(v1.m_pos.x + (v2.m_pos.y - v1.m_pos.y) * kInverseSlopeRightX, v2.m_pos.y, 0);
				newVertex.m_color = (v1.m_color + kInverseSlopeRightColor * (v2.m_pos.y - v1.m_pos.y));
				newVertex.m_UV = v1.m_UV + kInverseSlopeRightUV * (v2.m_pos.y - v1.m_pos.y);
				DrawAnyTriangle(v1, v2, newVertex, bWireFrame);
				DrawAnyTriangle(v2, newVertex, v3, bWireFrame);
			}
			else if ( v2.m_pos.x >= v3.m_pos.x )
			{
				float kInverseSlopeLeftX = (v3.m_pos.x - v1.m_pos.x) / (v3.m_pos.y - v1.m_pos.y);

				Color4F kInverseSlopeLeftColor = Color4F((v3.m_color.r - v1.m_color.r) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.g - v1.m_color.g) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.b - v1.m_color.b) / (v3.m_pos.y - v1.m_pos.y),
					(v3.m_color.a - v1.m_color.a) / (v3.m_pos.y - v1.m_pos.y));

				Vec2 kInverseSlopeLeftUV = Vec2((v3.m_UV.x - v1.m_UV.x) / (v3.m_pos.y - v1.m_pos.y), (v3.m_UV.y - v1.m_UV.y) / (v3.m_pos.y - v1.m_pos.y));

				SVertex newVertex;
				newVertex.m_pos.set(v1.m_pos.x + (v2.m_pos.y - v1.m_pos.y) * kInverseSlopeLeftX, v2.m_pos.y, 0);
				newVertex.m_color = (v1.m_color + kInverseSlopeLeftColor * (v2.m_pos.y - v1.m_pos.y));
				newVertex.m_UV = (v1.m_UV + kInverseSlopeLeftUV * (v2.m_pos.y - v1.m_pos.y));
				DrawAnyTriangle(v1, newVertex, v2, bWireFrame);
				DrawAnyTriangle(newVertex, v2, v3, bWireFrame);
			}
		}
	}
}

void RasterizationStage::DrawBottomTriangle(SVertex &v1, SVertex &v2, SVertex &v3)
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

	float fLeftX = v1.m_pos.x;
	float fRightX = v1.m_pos.x;
	Color4F leftColor = v1.m_color;
	Color4F rightColor = v1.m_color;
	Vec2 leftUV = v1.m_UV;
	Vec2 rightUV = v1.m_UV;
	for (float y = v1.m_pos.y; y <= v2.m_pos.y; ++y)
	{
		int iStartX = ConvertToPixelPos(fLeftX);
		int iEndX = ConvertToPixelPos(fRightX);
		Color4F curColor = leftColor;
		Vec2 curUV = leftUV;
		if (iEndX != iStartX)
		{
			int iStep = 1;
			int iDeltaX = iEndX - iStartX;
			Color4F kInverseSlopeColor = (rightColor - leftColor) / iDeltaX;
			Vec2 kInverseSlopeUV = (rightUV - leftUV) / iDeltaX;
			if (iDeltaX < 0)
			{
				iStep = -1;
				kInverseSlopeColor = kInverseSlopeColor * (-1);
				kInverseSlopeUV = kInverseSlopeUV * (-1);
			}

			iDeltaX = abs(iDeltaX);
			for (int i = 0; i <= iDeltaX; ++i)
			{
				Color4F texColor = SampleTexture(1, curUV);
				DrawPixel(iStartX + i * iStep, y, (texColor * curColor).ToARGB());

				curColor += kInverseSlopeColor;
				curUV += kInverseSlopeUV;
			}
		}
		else
		{
			Color4F texColor = SampleTexture(1, curUV);
			DrawPixel(iStartX, y, (texColor * curColor).ToARGB());
		}

		fLeftX += kInverseSlopeLeftX;
		fRightX += kInverseSlopeRightX;

		leftColor += kInverseSlopeLeftColor;
		rightColor += kInverseSlopeRightColor;

		leftUV += kInverseSlopeLeftUV;
		rightUV += kInverseSlopeRightUV;
	}
}

void RasterizationStage::DrawTopTriangle(SVertex &v1, SVertex &v2, SVertex &v3)
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

	float fLeftX = v1.m_pos.x;
	float fRightX = v2.m_pos.x;
	Color4F leftColor = v1.m_color;
	Color4F rightColor = v2.m_color;
	Vec2 leftUV = v1.m_UV;
	Vec2 rightUV = v2.m_UV;
	for (float y = v1.m_pos.y; y <= v3.m_pos.y; ++y)
	{
		int iStartX = ConvertToPixelPos(fLeftX);
		int iEndX = ConvertToPixelPos(fRightX);
		Color4F curColor = leftColor;
		Vec2 curUV = leftUV;
		if (iEndX != iStartX)
		{
			int iStep = 1;
			int iDeltaX = iEndX - iStartX;
			Color4F kInverseSlopeColor = (rightColor - leftColor) / iDeltaX;
			Vec2 kInverseSlopeUV = (rightUV - leftUV) / iDeltaX;
			if (iDeltaX < 0)
			{
				iStep = -1;
				kInverseSlopeColor = kInverseSlopeColor * (-1);
				kInverseSlopeUV = kInverseSlopeUV * (-1);
			}

			iDeltaX = abs(iDeltaX);
			for (int i = 0; i <= iDeltaX; ++i)
			{
				Color4F texColor = SampleTexture(1, curUV);
				DrawPixel(iStartX + i * iStep, y, (texColor * curColor).ToARGB());

				curColor += kInverseSlopeColor;
				curUV += kInverseSlopeUV;
			}
		}
		else
		{
			Color4F texColor = SampleTexture(1, curUV);
			DrawPixel(iStartX, y, (texColor * curColor).ToARGB());
		}

		fLeftX += kInverseSlopeLeftX;
		fRightX += kInverseSlopeRightX;

		leftColor += kInverseSlopeLeftColor;
		rightColor += kInverseSlopeRightColor;
	}
}

Color4F RasterizationStage::SampleTexture(int iTextureID, Vec2 uv)
{
	const CTexture* pTexture = CImageManager::GetInstance()->FindTexture(iTextureID);
	if ( !pTexture )
	{
		return Color4F::WHITE;
	}
	else
	{
		unsigned char* pData = pTexture->m_pData;
		int iU = int(uv.x * (pTexture->m_iWidth - 1) + 0.5f);
		int iV = int(uv.y * (pTexture->m_iHeight - 1) + 0.5f);

		if (iU > pTexture->m_iWidth)
			iU = pTexture->m_iWidth;

		if ( iU < 0 )
			iU = 0;

		if (iV > pTexture->m_iHeight)
			iV = pTexture->m_iHeight;

		if (iV < 0)
			iV = 0;

		unsigned char* pColorData = &pData[iV * pTexture->m_iWidth * 4 + iU * 4];
		return Color4F(pColorData[0] / 255.0f, pColorData[1] / 255.0f, pColorData[2] / 255.0f, pColorData[3] / 255.0f);
	}
}
