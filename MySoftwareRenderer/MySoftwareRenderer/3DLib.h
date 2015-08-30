#ifndef CPPYIN_3DLib
#define CPPYIN_3DLib

#include <d3d9.h>
#include <d3dx9.h>

// �궨��

// ��ű�
#define ARGB(a, r, g, b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

namespace _CPPYIN_3DLib
{
	// ȫ�ֱ���
	static IDirect3DDevice9* pDevice = 0;
	static IDirect3DSurface9* pSurface = 0;
	static D3DLOCKED_RECT lockedRect;

	// ����
	bool Init3DLib(HINSTANCE hInstance, HWND hWnd, int width, int height);
	int LockSurface();
	int UnlockSurface();
	int DrawPixel(int x, int y, DWORD color);
	void FlipSurface();
	void Release3DLib();
}

#endif