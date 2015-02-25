#include "FrameWork/OpenGL/OpenGLFrameWork.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"
#include "FrameWork/DataTypes.h"
#include "FrameWork/OpenGL/RenderingDataStructure.h"

COGLMesh g_mesh;
float g_YAngle = 0.0f;

void init()
{
	g_mesh.InitFromFile("Cube.CSTM");
	g_mesh.SetTexture("HelloWorld.png");
	g_mesh.m_worldPos[1] = -30.0f;
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	}
}
