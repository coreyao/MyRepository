#include "FrameWork/OpenGL/OpenGLFrameWork.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"
#include "FrameWork/DataTypes.h"
#include "FrameWork/OpenGL/RenderingDataStructure.h"

COGLMesh g_mesh;
CSkeletonAnimator g_animator;

timeval g_fLastTime = {0, 0};
float g_fDeltaTime = 0.0f;

float g_XAngle = 0.0f;
float g_YAngle = 0;

void init()
{
	g_mesh.InitFromFile("police.CSTM");
	//g_mesh.SetTexture("HelloWorld.png");
	g_mesh.m_worldPos.set(0, -30, -100);
	g_mesh.m_scale.set(50.0f, 50.0f, 50.0f);
	g_animator.SetTarget(&g_mesh);

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

	g_animator.Update(g_fDeltaTime);

	g_mesh.m_rotation[0] = cml::rad(g_XAngle);
	g_mesh.m_rotation[1] = cml::rad(g_YAngle);
	g_mesh.Render();

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
