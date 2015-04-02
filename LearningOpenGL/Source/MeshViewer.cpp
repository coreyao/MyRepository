#include "FrameWork/OpenGL/GLFrameWork.h"
#include "FrameWork/OpenGL/GLMesh.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"
#include "FrameWork/DataTypes.h"

COGLMesh* g_planeMesh = nullptr;
std::vector<COGLMesh*> g_vMesh;
std::vector<GLuint> g_vGLProgram;

timeval g_fLastTime = {0, 0};
float g_fDeltaTime = 0.0f;

float g_XAngle = 0.0f;
float g_YAngle = 0;

void init()
{
	CGLProgramManager::GetInstance()->Add("SkinMesh", SHADER_FILE_DIR + "SkinMesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("NormalMesh", SHADER_FILE_DIR + "Mesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");

	g_planeMesh = new COGLMesh;
	g_planeMesh->InitFromFile("plane.CSTM");
	for ( int i = 0; i < g_planeMesh->GetMeshData().m_vSubMesh.size(); ++i )
		g_planeMesh->SetTexture("default.png", i);
	g_planeMesh->m_worldPos.set(0, -60, -100);
	g_planeMesh->m_scale.set(1000, 1000, -1000);
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

	g_planeMesh->Render();
	for (int i = 0; i < g_vMesh.size(); ++i)
	{
		g_vMesh[i]->m_rotation.x = g_XAngle;
		g_vMesh[i]->m_rotation.y = g_YAngle;
		g_vMesh[i]->Update(g_fDeltaTime);
		g_vMesh[i]->Render();
	}

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
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	case 'a': ++g_YAngle; break;
	case 'd': --g_YAngle; break;
	case 'w': ++g_XAngle; break;
	case 's': --g_XAngle; break;
	}
}
