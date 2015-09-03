// 头文件
#include <Windows.h>
#include <WindowsX.h>

#include "Rasterization.h"
#include "Mesh.h"
using namespace Rasterization;

// 宏定义
#define WINDOW_CLASS_NAME TEXT("MySoftwareRenderer")
#define WINDOW_TITLE TEXT("MySoftwareRenderer")
#define SCREEN_BPP 32
#define WAIT_TIME 30

// 全局变量定义
HWND g_WindowHandle;
HINSTANCE g_HInstance;
DWORD g_Clock;

CMesh g_mesh;

void InitMesh()
{
	SVertex vertex1;
	vertex1.m_pos.set(0, 100, 0);
	SVertex vertex2;
	vertex2.m_pos.set(100, 0, 0);
	SVertex vertex3;
	vertex3.m_pos.set(-100, 0, 0);

	SSubMeshData subMeshData;
	subMeshData.m_MeshMatrix = Mat4::IDENTITY;
	subMeshData.m_vVertex.push_back(vertex1);
	subMeshData.m_vVertex.push_back(vertex2);
	subMeshData.m_vVertex.push_back(vertex3);

	SFace face;
	face.m_VertexIndex1 = 0;
	face.m_VertexIndex2 = 1;
	face.m_VertexIndex3 = 2;
	subMeshData.m_vFace.push_back(face);

	g_mesh.m_meshData.m_vSubMesh.push_back(subMeshData);
}

// 宏脚本
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// 函数定义
DWORD GetClock()
{
	return GetTickCount();
}

void StartClock()
{
	g_Clock = GetClock();
}

void WaitClock()
{
	while ((GetClock() - g_Clock) < WAIT_TIME)
	{
		Sleep(5);
	}
}

int Game_Init()
{
	Init3DLib(g_HInstance, g_WindowHandle, SCREEN_WIDTH, SCREEN_HEIGHT);
	InitMesh();
	return 1;
}

int Game_Main()
{
	// 计时
	StartClock();

	// 表面加锁
	LockSurface();
	
	g_mesh.Update(0);
	g_mesh.Render();

	// 表面解锁
	UnlockSurface();

	// 输出
	FlipSurface();

	// 锁帧
	WaitClock();
	return 1;
}

int Game_Shutdown()
{
	Release3DLib();
	return 1;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_CREATE:
	{
					  return 0;
	} break;
	case WM_KEYDOWN:
	{
					   if (wParam == VK_ESCAPE)
					   {
						   PostMessage(g_WindowHandle, WM_DESTROY, 0, 0);
					   } break;
	}
	case WM_PAINT:
	{
					 hdc = BeginPaint(hwnd, &ps);
					 EndPaint(hwnd, &ps);
					 return 0;
	} break;
	case WM_DESTROY:
	{
					   PostQuitMessage(0);
					   return 0;
	} break;
	default:break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	WNDCLASS winclass;
	HWND hwnd;
	MSG msg;

	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClass(&winclass))
	{
		return 0;
	}

	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		return 0;
	}

	g_HInstance = hInstance;
	g_WindowHandle = hwnd;

	RECT window_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRectEx(&window_rect, GetWindowStyle(g_WindowHandle), GetMenu(g_WindowHandle) != NULL, GetWindowExStyle(g_WindowHandle));
	MoveWindow(g_WindowHandle, 0, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, FALSE);

	if (!Game_Init())
	{
		return 0;
	}

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Game_Main();
	}

	Game_Shutdown();

	return msg.wParam;
}