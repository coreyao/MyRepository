// 头文件
#include <Windows.h>
#include <WindowsX.h>

#include "RasterizationStage.h"
#include "Image/ImageManager.h"
#include "Mesh.h"
#include "Director.h"
#include "Pipeline.h"

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
float g_iStepLength = 5;
Vec2 g_lastMouseClockPos;
bool g_bLeftMouseClicked = false;

std::vector<CMesh*> g_vMesh;

void InitMesh()
{
	CMaterial material1;
	material1.SetBaseColorTexture("hama.png");
	CMesh* pFileCube = new CMesh;
	pFileCube->InitFromFile("hama.CSTM");
	pFileCube->SetMaterial(material1, 0);
	pFileCube->m_transform.SetPosition(Vec3(0, -25, -100));
	pFileCube->m_transform.SetScale(Vec3(1, 1, -1));
	//pFileCube->m_transform.SetUseQuaternion(true);
	//pFileCube->m_transform.SetRotation(Quaternion(60, Vec3(0, 1, 0)));
	//pFileCube->m_transform.SetRotation(Vec3(0, 60, 0));
	pFileCube->m_renderState.m_eVertexOrder = EVertexOrder_Counter_ClockWise;
	g_vMesh.push_back(pFileCube);

	/*Vec3 v(1.0f, 0.0f, 0.0f);
	Vec4 v1(1.0f, 0.0f, 0.0f, 0.0f);
	Quaternion q(90, Vec3(0, 0, 1));
	Mat4 m = Mat4::CreateRotationMat(q);

	v = q * v;
	v1 = m * v1;*/

	pFileCube->m_animator.PlayAnim(0, 369, true, nullptr);

	{
		SMeshData meshData;
		SSubMeshData subMeshData1;
		{
			SVertex vertex;
			vertex.m_pos.set(-10, 10, 1);
			vertex.m_UV.set(0, 0);
			subMeshData1.m_vVertex.push_back(vertex);
		}

		{
			SVertex vertex;
			vertex.m_pos.set(-10, -10, 1);
			vertex.m_UV.set(0, 1);
			subMeshData1.m_vVertex.push_back(vertex);
		}

		{
			SVertex vertex;
			vertex.m_pos.set(10, -10, 1);
			vertex.m_UV.set(1, 1);
			subMeshData1.m_vVertex.push_back(vertex);
		}


		{
			SVertex vertex;
			vertex.m_pos.set(10, 10, 1);
			vertex.m_UV.set(1, 0);
			subMeshData1.m_vVertex.push_back(vertex);
		}

		{
			SFace face;
			face.m_VertexIndex1 = 0;
			face.m_VertexIndex2 = 1;
			face.m_VertexIndex3 = 2;
			subMeshData1.m_vFace.push_back(face);
		}

		{
			SFace face;
			face.m_VertexIndex1 = 3;
			face.m_VertexIndex2 = 0;
			face.m_VertexIndex3 = 2;
			subMeshData1.m_vFace.push_back(face);
		}

		subMeshData1.m_MeshMatrix = Mat4::IDENTITY;
		meshData.m_vSubMesh.push_back(subMeshData1);
		CMesh* pTriangle = new CMesh;
		CMaterial material1;
		material1.SetBaseColorTexture("HelloWorld.png");
		//pTriangle->m_renderState.m_bEnableCullFace = false;
		pTriangle->InitFromData(&meshData);
		pTriangle->SetMaterial(material1, 0);
		pTriangle->m_transform.SetPosition(Vec3(0, 0, 0));
		pTriangle->m_transform.SetScale(Vec3(1, 1, 1));
		pTriangle->m_transform.SetRotation(Vec3(0, -30, 0));
		pTriangle->m_renderState.m_eVertexOrder = EVertexOrder_Counter_ClockWise;

		//g_vMesh.push_back(pTriangle);
	}
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
	RasterizationStage::InitDX(g_HInstance, g_WindowHandle, SCREEN_WIDTH, SCREEN_HEIGHT);
	RasterizationStage::CRasterizer::GetInstance()->Init();

	InitMesh();
	return 1;
}
 
int Game_Update(float dt)
{
	for (auto& pMesh : g_vMesh)
	{
		//pMesh->m_transform.SetRotation(Vec3(0, pMesh->m_transform.GetRotation().y + dt *30, 0));
		pMesh->Update(dt);
		pMesh->Render();
	}
	CPipeline::GetInstance()->Draw();

	RasterizationStage::FillDXSurface();
	RasterizationStage::FlipDXSurface();

	return 1;
}

int Game_Shutdown()
{
	RasterizationStage::ReleaseDX();
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
				pMesh->m_renderState.m_bDrawWireFrame = !pMesh->m_renderState.m_bDrawWireFrame;
			}
		}
		else if (KEY_DOWN('F'))
		{
			for (auto& pMesh : g_vMesh)
			{
				pMesh->m_renderState.m_bEnableCullFace = !pMesh->m_renderState.m_bEnableCullFace;
			}
		}
		else if (KEY_DOWN('X'))
		{
			for (auto& pMesh : g_vMesh)
			{
				if (pMesh->m_renderState.m_eVertexOrder == EVertexOrder_Counter_ClockWise)
					pMesh->m_renderState.m_eVertexOrder = EVertexOrder_ClockWise;
				else
					pMesh->m_renderState.m_eVertexOrder = EVertexOrder_Counter_ClockWise;
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
	case WM_LBUTTONDOWN:
	{
		int mouse_x = (int)LOWORD(lParam);
		int mouse_y = (int)HIWORD(lParam);
		g_lastMouseClockPos = Vec2(mouse_x, mouse_y);
		g_bLeftMouseClicked = true;

		return 0;
	}
	case WM_LBUTTONUP:
	{
		g_bLeftMouseClicked = false;
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (g_bLeftMouseClicked)
		{
		    int mouse_x = (int)LOWORD(lParam);
			int mouse_y = (int)HIWORD(lParam);

			Vec2 delta = g_lastMouseClockPos - Vec2(mouse_x, mouse_y);
			delta = delta * 0.1f;
			CDirector::GetInstance()->GetPerspectiveCamera()->Rotate(delta.y, delta.x);

			g_lastMouseClockPos = Vec2(mouse_x, mouse_y);
		}
		
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

			Game_Update(g_deltaTime / 1000.0f);
		}

		g_deltaTime = GetClock() - g_Clock;
	}

	Game_Shutdown();

	return msg.wParam;
}