// 头文件
#include <Windows.h>
#include <WindowsX.h>

#include "RasterizationStage.h"
#include "Mesh.h"
#include "Director.h"
using namespace RasterizationStage;

// 宏定义
#define WINDOW_CLASS_NAME TEXT("MySoftwareRenderer")
#define WINDOW_TITLE TEXT("MySoftwareRenderer")
#define SCREEN_BPP 32
#define MAX_FRAME_RATE (1.0f / 60)

// 全局变量定义
HWND g_WindowHandle;
HINSTANCE g_HInstance;
DWORD g_Clock;
DWORD g_deltaTime = 0;
float g_iStepLength = 50;

std::vector<CMesh*> g_vMesh;

void InitMesh()
{
	SSubMeshData subMeshData;

	{
		SVertex vertex;
		vertex.m_pos.set(-10, 10, 10);
		subMeshData.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(10, 10, 10);
		subMeshData.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(10, -10, 10);
		subMeshData.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(-10, -10, 10);
		subMeshData.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(-10, 10, -10);
		subMeshData.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(10, 10, -10);
		subMeshData.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(10, -10, -10);
		subMeshData.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(-10, -10, -10);
		subMeshData.m_vVertex.push_back(vertex);
	}

	subMeshData.m_MeshMatrix = Mat4::IDENTITY;

	{
		SFace face;
		face.m_VertexIndex1 = 0;
		face.m_VertexIndex2 = 1;
		face.m_VertexIndex3 = 3;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 1;
		face.m_VertexIndex2 = 2;
		face.m_VertexIndex3 = 3;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 4;
		face.m_VertexIndex2 = 7;
		face.m_VertexIndex3 = 5;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 5;
		face.m_VertexIndex2 = 7;
		face.m_VertexIndex3 = 6;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 4;
		face.m_VertexIndex2 = 5;
		face.m_VertexIndex3 = 0;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 5;
		face.m_VertexIndex2 = 1;
		face.m_VertexIndex3 = 0;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 7;
		face.m_VertexIndex2 = 3;
		face.m_VertexIndex3 = 6;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 6;
		face.m_VertexIndex2 = 3;
		face.m_VertexIndex3 = 2;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 0;
		face.m_VertexIndex2 = 3;
		face.m_VertexIndex3 = 4;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 4;
		face.m_VertexIndex2 = 3;
		face.m_VertexIndex3 = 7;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 1;
		face.m_VertexIndex2 = 5;
		face.m_VertexIndex3 = 2;
		subMeshData.m_vFace.push_back(face);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 5;
		face.m_VertexIndex2 = 6;
		face.m_VertexIndex3 = 2;
		subMeshData.m_vFace.push_back(face);
	}
	
	CMesh* pCube = new CMesh;
	pCube->m_transform.m_pos.set(0, 0, 0);
	pCube->m_transform.m_rotation.set(0, 150, 0);
	pCube->m_meshData.m_vSubMesh.push_back(subMeshData);
	//g_vMesh.push_back(pCube);

	CMesh* g_pCharactor = new CMesh;
	g_pCharactor->InitFromFile("hama.CSTM");
	g_pCharactor->m_transform.m_scale.set(1, 1, -1);
	g_pCharactor->m_transform.m_rotation.set(0, 0, 0);
	g_pCharactor->m_transform.m_pos.set(0, -100, 0);
	g_pCharactor->m_eVertexOrder = EVertexOrder_Counter_ClockWise;
	g_vMesh.push_back(g_pCharactor);

	SSubMeshData subMeshData1;
	{
		SVertex vertex;
		vertex.m_pos.set(-10, 0, 0);
		subMeshData1.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(10, 0, 0);
		subMeshData1.m_vVertex.push_back(vertex);
	}

	{
		SVertex vertex;
		vertex.m_pos.set(10, 10, 0);
		subMeshData1.m_vVertex.push_back(vertex);
	}

	{
		SFace face;
		face.m_VertexIndex1 = 0;
		face.m_VertexIndex2 = 2;
		face.m_VertexIndex3 = 1;
		subMeshData1.m_vFace.push_back(face);
	}
	subMeshData1.m_MeshMatrix = Mat4::IDENTITY;
	CMesh* pTriangle = new CMesh;
	pTriangle->m_meshData.m_vSubMesh.push_back(subMeshData1);
	pTriangle->m_bEnableCullFace = false;
	//g_vMesh.push_back(pTriangle);
}

bool IsOutSideScreen(int x, int y)
{
	if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT)
		return false;

	return true;
}

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

DWORD GetClock()
{
	return GetTickCount();
}

int Game_Init()
{
	Init3DLib(g_HInstance, g_WindowHandle, SCREEN_WIDTH, SCREEN_HEIGHT);
	InitMesh();
	return 1;
}

int Game_Main(float dt)
{
	// 表面加锁
	LockSurface();
	
	for (auto& pMesh : g_vMesh)
	{
		pMesh->Update(dt);
		pMesh->Render();
	}

	// 表面解锁
	UnlockSurface();

	// 输出
	FlipSurface();

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

	float iMoveLeftRight = 0;
	float iMoveForwardBack = 0;
	float fPitch = 0;
	float fYaw = 0;

	switch (msg)
	{
	case WM_CREATE:
		return 0;
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
		{
			PostMessage(g_WindowHandle, WM_DESTROY, 0, 0);
		}
		else if (KEY_DOWN('W'))
		{
			iMoveForwardBack = g_iStepLength;
		}
		else if (KEY_DOWN('A'))
		{
			iMoveLeftRight = -g_iStepLength;
		}
		else if (KEY_DOWN('S'))
		{
			iMoveForwardBack = -g_iStepLength;
		}
		else if (KEY_DOWN('D'))
		{
			iMoveLeftRight = g_iStepLength;
		}
		else if (KEY_DOWN('J'))
		{
			fPitch = 5.0f;
		}
		else if (KEY_DOWN('K'))
		{
			fPitch = -5.0f;
		}
		else if (KEY_DOWN('N'))
		{
			fYaw = 5.0f;
		}
		else if (KEY_DOWN('M'))
		{
			fYaw = -5.0f;
		}
		else if (KEY_DOWN('B'))
		{
			for (auto& pMesh : g_vMesh)
			{
				pMesh->m_bDrawWireFrame = !pMesh->m_bDrawWireFrame;
			}
		}

		CDirector::GetInstance()->GetPerspectiveCamera()->Rotate(fPitch, fYaw);
		CDirector::GetInstance()->GetPerspectiveCamera()->Move(iMoveLeftRight, 0, iMoveForwardBack);

		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
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
		if (g_deltaTime == 0 || g_deltaTime >= MAX_FRAME_RATE)
		{
			g_Clock = GetClock();
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			Game_Main(g_deltaTime / 1000.0f);
		}

		g_deltaTime = GetClock() - g_Clock;
	}

	Game_Shutdown();

	return msg.wParam;
}