#include "FrameWork/OpenGL/GLFrameWork.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Utility.h"
#include "FrameWork/DataTypes.h"
#include "FrameWork/Mesh.h"
#include "FrameWork/Director.h"
#include "FrameWork/ParticleSystem.h"
#include "FrameWork/Label.h"
#include "FrameWork/Primitive.h"
#include "FrameWork/Terrain.h"
#include "FrameWork/SkyBox.h"
#include "FrameWork/ThirdPersonController.h"
#include "FrameWork/Light.h"
#include "FrameWork/ShadowMap.h"
#include "FrameWork/Scenes/SceneManager.h"

timeval g_fLastTime = {0, 0};
float g_fDeltaTime = 0.0f;
float g_fElapsedTime = 0.0f;
int g_iStepLength = 20;
int g_iFrame = 0;
float g_fAccumulatedTime = 0;
Vec2 g_lastMousePos;
bool g_bMouseRightButtonClicked = false;

void init()
{
	srand(time(nullptr));

	CSceneManager::GetInstance()->Next();

	//g_pDeltaTimeLabel = new CLabel(FONT_FILE_DIR + "simyou.ttf", 20);
	//g_pDeltaTimeLabel->m_transform.SetPosition(Vec3(-SCREEN_WIDTH / 2 + 10, SCREEN_HEIGHT / 2 - 20, 0));
	//g_pDeltaTimeLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);

	//g_pFPSLabel = new CLabel(FONT_FILE_DIR + "simyou.ttf", 20);
	//g_pFPSLabel->SetString("FPS:0");
	//g_pFPSLabel->m_transform.SetPosition(Vec3(g_pDeltaTimeLabel->m_transform.GetPosition().x, g_pDeltaTimeLabel->m_transform.GetPosition().y - 20, 0));
	//g_pFPSLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);

	//CMesh* planeMesh = new CMesh;
	//planeMesh->InitFromFile("plane.CSTM");
	//planeMesh->m_transform.SetScale(Vec3(10, 10, -10));
	//planeMesh->m_transform.SetPosition(Vec3(0, -30, -100));
	//for ( int i = 0; i < planeMesh->m_vSubMesh.size(); ++i )
	//{
	//	CMaterial newMaterial;
	//	newMaterial.SetBaseColorTexture("brickwall.png");
	//	newMaterial.SetNormalMapTexture("brickwall_normal.png");
	//	newMaterial.SetShininess(64.0f);
	//	planeMesh->SetMaterial(newMaterial, i);
	//}
	//planeMesh->m_renderState.m_bEnableCullFace = false;
	//planeMesh->SetLightEnable(true);
	//planeMesh->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh") );
}

void UpdateScene();
void DrawScene();

void display()
{
	struct timeval now;
	if (gettimeofday(&now, nullptr) != 0)
		return;

	if ( g_fLastTime.tv_sec == 0 && g_fLastTime.tv_usec == 0 )
	{
		g_fDeltaTime = 0.0f;
	}
	else
	{
		g_fDeltaTime = (now.tv_sec - g_fLastTime.tv_sec) + (now.tv_usec - g_fLastTime.tv_usec) / 1000000.0f;
		g_fDeltaTime = std::max(0.0f, g_fDeltaTime);
	}

	g_fDeltaTime = std::min(g_fDeltaTime, 0.02f);
	g_fElapsedTime += g_fDeltaTime;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDepthRange(0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UpdateScene();
	DrawScene();

	glutSwapBuffers();
	glutPostRedisplay();

	g_fLastTime = now;
}

void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard(unsigned char key, int x, int y)
{
	int iMoveLeftRight = 0;
	int iMoveForwardBack = 0;
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	case 'a': iMoveLeftRight = -g_iStepLength; break;
	case 'd': iMoveLeftRight = g_iStepLength; break;
	case 'w': iMoveForwardBack = g_iStepLength; break;
	case 's': iMoveForwardBack = -g_iStepLength; break;
	//case 'b': bDrawWireFrame = !bDrawWireFrame; break;
	}

	CDirector::GetInstance()->GetPerspectiveCamera()->Move(iMoveLeftRight, 0, iMoveForwardBack);
}

void mouse_down( int button, int state, int x, int y )
{
	if ( button == GLUT_LEFT_BUTTON )
	{
		if (state == GLUT_DOWN)
			g_bMouseRightButtonClicked = true;
		else
			g_bMouseRightButtonClicked = false;

		g_lastMousePos = Vec2(x, y);
	}
	else if ( button == GLUT_RIGHT_BUTTON)
	{
		g_lastMousePos = Vec2(x, y);
	}
	else if ( button == 3 )   
	{
		CDirector::GetInstance()->GetPerspectiveCamera()->Zoom(-1.0f);
	}
	else if ( button == 4 )
	{
		CDirector::GetInstance()->GetPerspectiveCamera()->Zoom(1.0f);
	}
}

void mouse_move(int x,int y)
{
	if ( g_bMouseRightButtonClicked )
	{
		float fPitchDelta = (g_lastMousePos.y - y) * 0.1f;
		float fYawDelta = (g_lastMousePos.x - x) * 0.1f;

		CDirector::GetInstance()->GetPerspectiveCamera()->Rotate(fPitchDelta, fYawDelta);
		//CDirector::GetInstance()->GetOrthographicCamera()->Rotate(fPitchDelta, fYawDelta);

		//g_pController->Rotate(fPitchDelta, fYawDelta);
		g_lastMousePos = Vec2(x, y);
	}
}

void DrawMesh()
{
}

//void DrawLabel()
//{
//	char buf[50] = {0};
//	sprintf(buf, "DeltaTime:%f", g_fDeltaTime);
//	g_pDeltaTimeLabel->SetString(buf);
//	if ( g_fDeltaTime >= 0.5f )
//		g_pDeltaTimeLabel->m_color = Color4F(1.0f, 0.0f, 0.0f, 1.0f);
//	else
//		g_pDeltaTimeLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);
//
//	g_fAccumulatedTime += g_fDeltaTime;
//	++g_iFrame;
//	if ( g_fAccumulatedTime >= 1.0f )
//	{
//		char buf[50] = {0};
//		sprintf(buf, "FPS:%d", g_iFrame);
//		g_pFPSLabel->SetString(buf);
//
//		if ( g_iFrame <= 30 )
//			g_pFPSLabel->m_color = Color4F(1.0f, 0.0f, 0.0f, 1.0f);
//		else
//			g_pFPSLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);
//
//		g_iFrame = 0;
//		g_fAccumulatedTime = 0;
//	}
//}

void DrawScene()
{
	CSceneManager::GetInstance()->Draw();
}

void UpdateScene()
{
	CSceneManager::GetInstance()->Update(g_fDeltaTime);
}

