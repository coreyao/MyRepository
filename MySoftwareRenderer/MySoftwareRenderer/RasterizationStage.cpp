#include "RasterizationStage.h"

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

float RasterizationStage::ConverToPixelPos(float value)
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
				DrawPixel(iCurX, ConverToPixelPos(iCurY), color);
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
				DrawPixel(ConverToPixelPos(iCurX), iCurY, color);
				iCurX += kInverse;
			}
		}
	}
}

void RasterizationStage::DrawTriangle(SVertex& v1, SVertex& v2, SVertex& v3, bool bWireFrame /*= true*/)
{
	Vec3& p1 = v1.m_pos;
	Vec3& p2 = v2.m_pos;
	Vec3& p3 = v3.m_pos;

	p1.x = ConverToPixelPos(p1.x);
	p1.y = ConverToPixelPos(p1.y);
	p2.x = ConverToPixelPos(p2.x);
	p2.y = ConverToPixelPos(p2.y);
	p3.x = ConverToPixelPos(p3.x);
	p3.y = ConverToPixelPos(p3.y);

	if ( bWireFrame )
	{
		DrawLine(p1.x, p1.y, p2.x, p2.y, 0xffffffff);
		DrawLine(p2.x, p2.y, p3.x, p3.y, 0xffffffff);
		DrawLine(p1.x, p1.y, p3.x, p3.y, 0xffffffff);
	}
	else
	{
		if (p1.y > p2.y)
			Helper::Swap(p1, p2);

		if (p1.y > p3.y)
			Helper::Swap(p1, p3);

		if (p2.y > p3.y)
			Helper::Swap(p2, p3);

		if (p1.y == p2.y && p2.y == p3.y
			|| p1.x == p2.x && p2.x == p3.x)
			return;

		if ( p2.y == p3.y )
		{
			float kInverseLeft = (p1.x - p2.x) / (p1.y - p2.y);
			float kInverseRight = (p1.x - p3.x) / (p1.y - p3.y);

			float fLeftX = p1.x;
			float fRightX = p1.x;
			for (float y = p1.y; y <= p2.y; ++y)
			{
				DrawLine(ConverToPixelPos(fLeftX), y, ConverToPixelPos(fRightX), y, 0xffffffff);

				fLeftX += kInverseLeft;
				fRightX += kInverseRight;
			}
		}
		else if (p1.y == p2.y)
		{
			float kInverseLeft = (p3.x - p1.x) / (p3.y - p1.y);
			float kInverseRight = (p3.x - p2.x) / (p3.y - p2.y);

			float fLeftX = p1.x;
			float fRightX = p2.x;
			for (float y = p1.y; y <= p3.y; ++y)
			{
				DrawLine(ConverToPixelPos(fLeftX), y, ConverToPixelPos(fRightX), y, 0xffffffff);

				fLeftX += kInverseLeft;
				fRightX += kInverseRight;
			}
		}
		else
		{
			if (p2.x < p3.x)
			{
				float kInverseRight = (p3.x - p1.x) / (p3.y - p1.y);

				SVertex newVertex;
				newVertex.m_pos.set(p1.x + (p2.y - p1.y) * kInverseRight, p2.y, 0);
				DrawTriangle(v1, v2, newVertex, bWireFrame);
				DrawTriangle(v2, newVertex, v3, bWireFrame);
			}
			else if ( p2.x >= p3.x )
			{
				float kInverseLeft = (p3.x - p1.x) / (p3.y - p1.y);

				SVertex newVertex;
				newVertex.m_pos.set(p1.x + (p2.y - p1.y) * kInverseLeft, p2.y, 0);
				DrawTriangle(v1, newVertex, v2, bWireFrame);
				DrawTriangle(newVertex, v2, v3, bWireFrame);
			}
		}
	}
}
