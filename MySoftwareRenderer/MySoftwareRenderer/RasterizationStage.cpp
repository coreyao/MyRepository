#include "RasterizationStage.h"
#include "Image/ImageManager.h"
#include "Shaders/Shader.h"

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

void RasterizationStage::CRasterizer::DrawAnyTriangle(CVertexRuntime& v1, CVertexRuntime& v2, CVertexRuntime& v3, float fAlpha, CRenderState* pRenderState)
{
	if (pRenderState->m_bDrawWireFrame)
	{
		auto& outPos1 = v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
		auto& outPos2 = v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;
		auto& outPos3 = v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4;

		DrawLine(outPos1.x, outPos1.y, outPos2.x, outPos2.y, Color4F::WHITE);
		DrawLine(outPos2.x, outPos2.y, outPos3.x, outPos3.y, Color4F::WHITE);
		DrawLine(outPos1.x, outPos1.y, outPos3.x, outPos3.y, Color4F::WHITE);
	}
	else
	{
		if (v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y > v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y)
			Helper::Swap(v1, v2);
		if (v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y > v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y)
			Helper::Swap(v1, v3);
		if (v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y > v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y)
			Helper::Swap(v2, v3);

		if (v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y == v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y
			|| (v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x == v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x 
			&& v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x == v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x))
			return;

		if (v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y == v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y)
		{
			if (v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x > v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x)
				Helper::Swap(v2, v3);

			HighPrecision fpScreenX1(v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY1(v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);
			HighPrecision fpScreenX2(v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY2(v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);
			HighPrecision fpScreenX3(v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY3(v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);

			auto fDY1 = 1.0f / (fpScreenY2 - fpScreenY1);
			auto fDY2 = 1.0f / (fpScreenY3 - fpScreenY1);

			auto kInverseSlopeLeftX = (fpScreenX2 - fpScreenX1) * fDY1;
			auto kInverseSlopeRightX = (fpScreenX3 - fpScreenX1) * fDY2;

			CVariable kInverseSlopeVertexAttributeLeft[EVertexAttributeVar_Max];
			CVariable kInverseSlopeVertexAttributeRight[EVertexAttributeVar_Max];
			for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
			{
				kInverseSlopeVertexAttributeLeft[eVar] = (v2.m_vVertexAttributeVar[eVar] - v1.m_vVertexAttributeVar[eVar]) * (float)fDY1;
				kInverseSlopeVertexAttributeRight[eVar] = (v3.m_vVertexAttributeVar[eVar] - v1.m_vVertexAttributeVar[eVar]) * (float)fDY2;
			}

			CVariable kInverseSlopeCustomVarLeft[conMaxCustomVar];
			CVariable kInverseSlopeCustomVarRight[conMaxCustomVar];
			for (int i = 0; i < conMaxCustomVar; ++i)
			{
				kInverseSlopeCustomVarLeft[i] = (v2.m_vCustomVariable[i] - v1.m_vCustomVariable[i]) * (float)fDY1;
				kInverseSlopeCustomVarRight[i] = (v3.m_vCustomVariable[i] - v1.m_vCustomVariable[i]) * (float)fDY2;
			}

			Helper::Clamp(fpScreenY1, HighPrecision(-0.5f), HighPrecision(SCREEN_HEIGHT - 0.5f));
			Helper::Clamp(fpScreenY2, HighPrecision(-0.5f), HighPrecision(SCREEN_HEIGHT - 0.5f));
			fpScreenY3 = fpScreenY2;

			int iStartY = ceil(fpScreenY1);
			int iEndY = ceil(fpScreenY2);
			if ( iStartY >= iEndY )
				return;
			
			auto fOffsetY = (HighPrecision(iStartY) - HighPrecision(v1.m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.y));
			auto fLeftX = fpScreenX1 + kInverseSlopeLeftX * fOffsetY;
			auto fRightX = fpScreenX1 + kInverseSlopeRightX * fOffsetY;

			CVariable kVertexAttributeLeft[EVertexAttributeVar_Max];
			CVariable kVertexAttributeRight[EVertexAttributeVar_Max];
			for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
			{
				kVertexAttributeLeft[eVar] = v1.m_vVertexAttributeVar[eVar] + kInverseSlopeVertexAttributeLeft[eVar] * (float)fOffsetY;
				kVertexAttributeRight[eVar] = v1.m_vVertexAttributeVar[eVar] + kInverseSlopeVertexAttributeRight[eVar] * (float)fOffsetY;
			}

			// - Interpolate custom variables
			CVariable kVertexCustomAttributeLeft[conMaxCustomVar];
			CVariable kVertexCustomAttributeRight[conMaxCustomVar];
			for (int i = 0; i < conMaxCustomVar; ++i)
			{
				kVertexCustomAttributeLeft[i] = v1.m_vCustomVariable[i] + kInverseSlopeCustomVarLeft[i] * (float)fOffsetY;
				kVertexCustomAttributeRight[i] = v1.m_vCustomVariable[i] + kInverseSlopeCustomVarRight[i] * (float)fOffsetY;
			}

			for (int y = iStartY; y < iEndY; ++y)
			{
				DrawScanline(fLeftX, fRightX, kVertexAttributeLeft, kVertexAttributeRight, kVertexCustomAttributeLeft, kVertexCustomAttributeRight, y, fAlpha, pRenderState);

				fLeftX += kInverseSlopeLeftX;
				fRightX += kInverseSlopeRightX;

				for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
				{
					kVertexAttributeLeft[eVar] += kInverseSlopeVertexAttributeLeft[eVar];
					kVertexAttributeRight[eVar] += kInverseSlopeVertexAttributeRight[eVar];
				}

				for (int i = 0; i < conMaxCustomVar; ++i)
				{
					kVertexCustomAttributeLeft[i] += kInverseSlopeCustomVarLeft[i];
					kVertexCustomAttributeRight[i] += kInverseSlopeCustomVarRight[i];
				}
			}
		}
		else if (v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y == v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y)
		{
			if (v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x > v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x)
				Helper::Swap(v1, v2);

			HighPrecision fpScreenX1(v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY1(v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);
			HighPrecision fpScreenX2(v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY2(v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);
			HighPrecision fpScreenX3(v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY3(v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);

			auto fDY1 = 1.0f / (fpScreenY3 - fpScreenY1);

			auto kInverseSlopeLeftX = (fpScreenX3 - fpScreenX1) * fDY1;
			auto kInverseSlopeRightX = (fpScreenX3 - fpScreenX2) * fDY1;

			CVariable kInverseSlopeVertexAttributeLeft[EVertexAttributeVar_Max];
			CVariable kInverseSlopeVertexAttributeRight[EVertexAttributeVar_Max];
			for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
			{
				kInverseSlopeVertexAttributeLeft[eVar] = (v3.m_vVertexAttributeVar[eVar] - v1.m_vVertexAttributeVar[eVar]) * (float)fDY1;
				kInverseSlopeVertexAttributeRight[eVar] = (v3.m_vVertexAttributeVar[eVar] - v2.m_vVertexAttributeVar[eVar]) * (float)fDY1;
			}

			CVariable kInverseSlopeCustomVarLeft[conMaxCustomVar];
			CVariable kInverseSlopeCustomVarRight[conMaxCustomVar];
			for (int i = 0; i < conMaxCustomVar; ++i)
			{
				kInverseSlopeCustomVarLeft[i] = (v3.m_vCustomVariable[i] - v1.m_vCustomVariable[i]) * (float)fDY1;
				kInverseSlopeCustomVarRight[i] = (v3.m_vCustomVariable[i] - v2.m_vCustomVariable[i]) * (float)fDY1;
			}

			Helper::Clamp(fpScreenY1, HighPrecision(-0.5f), HighPrecision(SCREEN_HEIGHT - 0.5f));
			Helper::Clamp(fpScreenY3, HighPrecision(-0.5f), HighPrecision(SCREEN_HEIGHT - 0.5f));
			fpScreenY2 = fpScreenY1;

			int iStartY = ceil(fpScreenY1);
			int iEndY = ceil(fpScreenY3);
			if (iStartY >= iEndY)
				return;

			auto fOffsetY = (HighPrecision(iStartY) - HighPrecision(v1.m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.y));
			auto fLeftX = fpScreenX1 + kInverseSlopeLeftX * fOffsetY;
			auto fRightX = fpScreenX2 + kInverseSlopeRightX * fOffsetY;

			CVariable kVertexAttributeLeft[EVertexAttributeVar_Max];
			CVariable kVertexAttributeRight[EVertexAttributeVar_Max];
			for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
			{
				kVertexAttributeLeft[eVar] = v1.m_vVertexAttributeVar[eVar] + kInverseSlopeVertexAttributeLeft[eVar] * (float)fOffsetY;
				kVertexAttributeRight[eVar] = v2.m_vVertexAttributeVar[eVar] + kInverseSlopeVertexAttributeRight[eVar] * (float)fOffsetY;
			}

			// - Interpolate custom variables
			CVariable kVertexCustomAttributeLeft[conMaxCustomVar];
			CVariable kVertexCustomAttributeRight[conMaxCustomVar];
			for (int i = 0; i < conMaxCustomVar; ++i)
			{
				kVertexCustomAttributeLeft[i] = v1.m_vCustomVariable[i] + kInverseSlopeCustomVarLeft[i] * (float)fOffsetY;
				kVertexCustomAttributeRight[i] = v2.m_vCustomVariable[i] + kInverseSlopeCustomVarRight[i] * (float)fOffsetY;
			}

			for (int y = iStartY; y < iEndY; ++y)
			{
				DrawScanline(fLeftX, fRightX, kVertexAttributeLeft, kVertexAttributeRight, kVertexCustomAttributeLeft, kVertexCustomAttributeRight, y, fAlpha, pRenderState);

				fLeftX += kInverseSlopeLeftX;
				fRightX += kInverseSlopeRightX;

				for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
				{
					kVertexAttributeLeft[eVar] += kInverseSlopeVertexAttributeLeft[eVar];
					kVertexAttributeRight[eVar] += kInverseSlopeVertexAttributeRight[eVar];
				}

				for (int i = 0; i < conMaxCustomVar; ++i)
				{
					kVertexCustomAttributeLeft[i] += kInverseSlopeCustomVarLeft[i];
					kVertexCustomAttributeRight[i] += kInverseSlopeCustomVarRight[i];
				}
			}
		}
		else
		{
			HighPrecision fpScreenX1(v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY1(v1.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);
			HighPrecision fpScreenX2(v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY2(v2.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);
			HighPrecision fpScreenX3(v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.x);
			HighPrecision fpScreenY3(v3.m_vVertexAttributeVar[EVertexAttributeVar::EVertexAttributeVar_Position].v4.y);

			float t = (fpScreenY2 - fpScreenY1) / (fpScreenY3 - fpScreenY1);
			CVertexRuntime newVertex;
			Helper::LerpVertex(&v1, &v3, t, &newVertex);
			newVertex.m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.x = fpScreenX1 + (fpScreenX3 - fpScreenX1) * t;
			newVertex.m_vVertexAttributeVar[EVertexAttributeVar_Position].v4.y = fpScreenY2;

			DrawAnyTriangle(v1, v2, newVertex, fAlpha, pRenderState);
			DrawAnyTriangle(v2, newVertex, v3, fAlpha, pRenderState);
		}
	}
}

void RasterizationStage::CRasterizer::DrawScanline(HighPrecision fLeftX, HighPrecision fRightX, CVariable* leftVal, 
	CVariable* rightVal, CVariable* leftCustomVal, CVariable* rightCustomVal, int y, float fAlpha, CRenderState* pRenderState)
{
	int iStartX = ceil(fLeftX);
	int iEndX = ceil(fRightX);
	Helper::Clamp(iStartX, 0, SCREEN_WIDTH - 1);
	Helper::Clamp(iEndX, 0, SCREEN_WIDTH - 1);
	if (iEndX > iStartX)
	{
		auto fDeltaX = 1.0f / ( fRightX - fLeftX );

		CVariable kInverseSlopeVertexAttribute[EVertexAttributeVar_Max];
		for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
		{
			kInverseSlopeVertexAttribute[eVar] = (rightVal[eVar] - leftVal[eVar]) * (float)fDeltaX;
		}

		// - Interpolate custom variables
		CVariable kInverseSlopeCustomVertexAttribute[conMaxCustomVar];
		for (int i = 0; i < conMaxCustomVar; ++i)
		{
			kInverseSlopeCustomVertexAttribute[i] = (rightCustomVal[i] - leftCustomVal[i]) * (float)fDeltaX;
		}

		auto fOffsetX = iStartX - fLeftX;
		CVariable kVertexAttribute[EVertexAttributeVar_Max];
		for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
			kVertexAttribute[eVar] = leftVal[eVar] + kInverseSlopeVertexAttribute[eVar] * (float)fOffsetX;

		CVariable kVertexCustomAttribute[conMaxCustomVar];
		for (int i = 0; i < conMaxCustomVar; ++i)
			kVertexCustomAttribute[i] = leftCustomVal[i] + kInverseSlopeCustomVertexAttribute[i] * (float)fOffsetX;

		for (int i = iStartX; i < iEndX; ++i)
		{
			int iPixelX = i;
			int iPixelY = y;
			if (CanDrawPixel(iPixelX, iPixelY, kVertexAttribute[EVertexAttributeVar_Position].v4.z))
			{
				SFragment frag;
				frag.fAlpha = fAlpha;

				float w = 1.0f / kVertexAttribute[EVertexAttributeVar_Position].v4.w;
				for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
					frag.m_vVertexAttributeVar[eVar] = kVertexAttribute[eVar] * w;

				for (int i = 0; i < conMaxCustomVar; ++i)
					frag.m_vCustomVariable[i] = kVertexCustomAttribute[i] * w;

				Color4F finalColor = pRenderState->m_pFragmentShader->ProcessFragment(&frag);
				if (AlphaTest(finalColor.a))
					DrawPixel(iPixelX, iPixelY, finalColor);
			}

			for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
				kVertexAttribute[eVar] += kInverseSlopeVertexAttribute[eVar];

			for (int i = 0; i < conMaxCustomVar; ++i)
				kVertexCustomAttribute[i] += kInverseSlopeCustomVertexAttribute[i];
		}
	}
}

Color4F RasterizationStage::CRasterizer::SampleTexture(int iTextureID, Vec2 uv)
{
	//return Color4F(1.0, 1.0, 1.0, 1.f);

	const CTexture* pTexture = CImageManager::GetInstance()->FindTexture(iTextureID);
	if (!pTexture)
	{
		return Color4F::WHITE;
	}
	else
	{
		if (pTexture->m_sampler.TEXTURE_MAG_FILTER == CSampler::ETextureFilter_Nearest)
		{
			return SampleNearset(pTexture, uv);
		}
		else if (pTexture->m_sampler.TEXTURE_MIN_FILTER == CSampler::ETextureFilter_Liner)
		{
			return SampleLinear(pTexture, uv);
		}
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
	bool bClampU = pTexture->m_sampler.UV_WRAP_S == CSampler::EUVWrapMode_Clamp;
	bool bClampV = pTexture->m_sampler.UV_WRAP_T == CSampler::EUVWrapMode_Clamp;
	if (bClampU)
		Helper::Clamp(uv.x, 0.0f, 1.0f);
	else
		uv.x = fmod(uv.x, 1.0f);

	if (bClampV)
		Helper::Clamp(uv.y, 0.0f, 1.0f);
	else
		uv.y = fmod(uv.y, 1.0f);

	int fWidth = pTexture->m_iWidth - 1;
	int fHeight = pTexture->m_iHeight - 1;

	int iU = int(uv.x * fWidth);
	int iV = int(uv.y * fHeight);

	unsigned char* pData = pTexture->m_pData;
	unsigned char* pColorData = &pData[iV * pTexture->m_iWidth * 4 + iU * 4];
	return Color4F(pColorData[0] / 255.0f, pColorData[1] / 255.0f, pColorData[2] / 255.0f, pColorData[3] / 255.0f);
}

Color4F RasterizationStage::CRasterizer::SampleLinear(const CTexture* pTexture, Vec2 &uv)
{
	bool bClampU = pTexture->m_sampler.UV_WRAP_S == CSampler::EUVWrapMode_Clamp;
	bool bClampV = pTexture->m_sampler.UV_WRAP_T == CSampler::EUVWrapMode_Clamp;
	if (bClampU)
		Helper::Clamp(uv.x, 0.0f, 1.0f);
	else
		uv.x = fmod(uv.x, 1.0f);

	if (bClampV)
		Helper::Clamp(uv.y, 0.0f, 1.0f);
	else
		uv.y = fmod(uv.y, 1.0f);

	int fWidth = pTexture->m_iWidth - 1;
	int fHeight = pTexture->m_iHeight - 1;
	unsigned char* pData = pTexture->m_pData;
	uv.x = uv.x * fWidth;
	uv.y = uv.y * fHeight;

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
