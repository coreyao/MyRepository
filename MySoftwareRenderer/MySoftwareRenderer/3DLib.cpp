#include "3DLib.h"

bool Lib3D::Init3DLib(HINSTANCE hInstance, HWND hWnd, int width, int height)
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

int Lib3D::LockSurface()
{
	memset(&lockedRect, 0, sizeof(lockedRect));
	pSurface->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD);
	return 1;
}

int Lib3D::UnlockSurface()
{
	pSurface->UnlockRect();
	return 1;
}

int Lib3D::DrawPixel(int x, int y, DWORD color)
{
	DWORD* pBits = (DWORD*)lockedRect.pBits;
	pBits[x + y * (lockedRect.Pitch >> 2)] = color;

	return 1;
}

void Lib3D::FlipSurface()
{
	// ��ȡ��̨����
	IDirect3DSurface9* backBuffer = 0;
	pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	// ʹ���Զ����������̨����
	pDevice->StretchRect(pSurface, 0, backBuffer, 0, D3DTEXF_NONE);

	// GetBackBuffer���õĻ�����Ҫ���ͷţ�������ڴ�й¶
	backBuffer->Release();

	// ���������еĺ�̨������ʾ
	pDevice->Present(0, 0, 0, 0);
}

void Lib3D::Release3DLib()
{
	pSurface->Release();
	pDevice->Release();
}

void Lib3D::DrawLine(int x1, int y1, int x2, int y2, DWORD color)
{
	// - DDA
	if ( x2 - x1 == 0 )
	{
		if (y1 > y2)
		{
			Swap(x1, x2);
			Swap(y1, y2);
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
				Swap(x1, x2);
				Swap(y1, y2);
			}

			float iCurY = y1;
			for (int iCurX = x1; iCurX <= x2; ++iCurX)
			{
				DrawPixel(iCurX, (int)(iCurY + 0.5f), color);
				iCurY += k;
			}
		}
		else
		{
			if (y1 > y2)
			{
				Swap(x1, x2);
				Swap(y1, y2);
			}

			float iCurX = x1;
			for (int iCurY = y1; iCurY <= y2; ++iCurY)
			{
				DrawPixel((int)(iCurX + 0.5f), iCurY, color);
				iCurX += kInverse;
			}
		}
	}
}
