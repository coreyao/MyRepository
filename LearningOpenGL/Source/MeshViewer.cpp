#include "FrameWork/OpenGL/GLFrameWork.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"
#include "FrameWork/DataTypes.h"
#include "FrameWork/GLMesh.h"
#include "FrameWork/Director.h"
#include "FrameWork/GLParticleSystem.h"

COGLMesh* g_planeMesh = nullptr;
std::vector<COGLMesh*> g_vMesh;
std::vector<GLuint> g_vGLProgram;

GLParticleSystem* g_particleSystem = nullptr;

timeval g_fLastTime = {0, 0};
float g_fDeltaTime = 0.0f;

Vec2 g_lastMousePos;
bool g_bMouseRightButtonClicked = false;

bool bDrawMesh = true;

void init()
{
	CGLProgramManager::GetInstance()->Add("SkinMesh", SHADER_FILE_DIR + "SkinMesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("NormalMesh", SHADER_FILE_DIR + "Mesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("Particle", SHADER_FILE_DIR + "Particle_Vertex_Shader.vert", SHADER_FILE_DIR + "Particle_Fragment_Shader.frag");

	g_particleSystem = new GLParticleSystem;
	CEmitter* pEmitter = new CEmitter;
	pEmitter->SetTexture("T_FX_guangyun01.png");
	pEmitter->m_transform.m_scale.set(10, 10, 1);
	pEmitter->m_transform.m_pos.set(50, 0, 0);
	g_particleSystem->AddEmitter(pEmitter);

	g_planeMesh = new COGLMesh;
	g_planeMesh->InitFromFile("plane.CSTM");
	auto& pSubMesh = g_planeMesh->m_data.m_vSubMesh[0];
	
	g_planeMesh->InitVBOAndVAO();
	for ( int i = 0; i < g_planeMesh->GetMeshData().m_vSubMesh.size(); ++i )
		g_planeMesh->SetTexture("default.png", i);
	//g_planeMesh->m_worldPos.set(0, -60, -100);
	//g_planeMesh->m_scale.set(1000, 1000, -1000);
	g_planeMesh->m_color = Color4F(0.5f, 0.5f, 0.5f, 1.0f);
	g_planeMesh->m_bEnableCullFace = false;
	g_planeMesh->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("NormalMesh") );

	COGLMesh* pSkinMesh = new COGLMesh;
	pSkinMesh->InitFromFile("bat.CSTM");
	for ( int i = 0; i < pSkinMesh->GetMeshData().m_vSubMesh.size(); ++i )
	{
		pSkinMesh->SetTexture("BatArmor.png", i);
	}
	pSkinMesh->m_worldPos.set(g_planeMesh->m_worldPos.x, g_planeMesh->m_worldPos.y + 30, g_planeMesh->m_worldPos.z);
	pSkinMesh->m_scale.set(1, 1, -1);
	pSkinMesh->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("SkinMesh") );
	pSkinMesh->SetVisible(false, "Box01");
	g_vMesh.push_back(pSkinMesh);

	//bDrawMesh = false;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

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

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( bDrawMesh )
	{
		g_planeMesh->m_rotation.x = 90;
		g_planeMesh->m_rotation.z += g_fDeltaTime * 5;
		g_planeMesh->Render();
		/*for (int i = 0; i < g_vMesh.size(); ++i)
		{
		g_vMesh[i]->Update(g_fDeltaTime);
		g_vMesh[i]->Render();
		}*/
	}

	//g_particleSystem->Update(g_fDeltaTime);
	//g_particleSystem->Render();
	
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
	case 'a': iMoveLeftRight = -10; break;
	case 'd': iMoveLeftRight = 10; break;
	case 'w': iMoveForwardBack = 10; break;
	case 's': iMoveForwardBack = -10; break;
	}

	CDirector::GetInstance()->GetCurCamera()->Move(iMoveLeftRight, 0, iMoveForwardBack);
}

void mouse_down( int button, int state, int x, int y )
{
	if ( button == GLUT_RIGHT_BUTTON )
	{
		if ( state == GLUT_DOWN )
			g_bMouseRightButtonClicked = true;
		else
			g_bMouseRightButtonClicked = false;

		g_lastMousePos = Vec2(x, y);
	}
}

void mouse_move(int x,int y)
{
	if ( g_bMouseRightButtonClicked )
	{
		float fPitchDelta = (g_lastMousePos.y - y) * 0.1f;
		float fYawDelta = (g_lastMousePos.x - x) * 0.1f;

		CDirector::GetInstance()->GetCurCamera()->Rotate(fPitchDelta, fYawDelta);
		g_lastMousePos = Vec2(x, y);
	}
}
