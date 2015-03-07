#include "FrameWork/OpenGL/OpenGLFrameWork.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"
#include "FrameWork/DataTypes.h"
#include "FrameWork/OpenGL/RenderingDataStructure.h"

COGLMesh g_mesh;
CSkeletonAnimator g_animator;

float g_XAngle = 0.0f;
float g_YAngle = 0;

void init()
{
	g_mesh.InitFromFile("test.CSTM");
	g_mesh.SetTexture("HelloWorld.png");
	g_mesh.m_worldPos.set(0, -30, -100);
	//g_mesh.m_scale.set(1.0f, 1.0f, 1.0f);
	g_animator.SetTarget(&g_mesh);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_animator.Update(0.0001f);

	g_mesh.m_rotation[0] = cml::rad(g_XAngle);
	g_mesh.m_rotation[1] = cml::rad(g_YAngle);
	g_mesh.Render();

	glutSwapBuffers();
	glutPostRedisplay();
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
