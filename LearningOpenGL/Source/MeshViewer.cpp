#include "FrameWork/OpenGL/OpenGLFrameWork.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"
#include "FrameWork/DataTypes.h"
#include "FrameWork/OpenGL/OGLRenderingDataStructure.h"

std::vector<COGLMesh*> g_vMesh;

timeval g_fLastTime = {0, 0};
float g_fDeltaTime = 0.0f;

float g_XAngle = 0.0f;
float g_YAngle = 0;

void init()
{
	COGLMesh* pPlane = new COGLMesh;
	pPlane->InitFromFile("plane.CSTM");
	for ( int i = 0; i < pPlane->GetMeshData().m_vSubMesh.size(); ++i )
	{
		pPlane->SetTexture("HelloWorld.png", i);
	}

	pPlane->m_worldPos.set(0, -60, -100);
	pPlane->m_scale.set(100, 100, -100);
	pPlane->m_bEnableCullFace = false;

	g_vMesh.push_back(pPlane);

	{
		COGLMesh* mesh = new COGLMesh;
		mesh->InitFromFile("bat.CSTM");
		for ( int i = 0; i < mesh->GetMeshData().m_vSubMesh.size(); ++i )
		{
			mesh->SetTexture("BatArmor.png", i);
		}

		mesh->m_worldPos.set(pPlane->m_worldPos.x, pPlane->m_worldPos.y + 30, pPlane->m_worldPos.z);
		mesh->m_scale.set(1, 1, -1);

		g_vMesh.push_back(mesh);
	}
	

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
