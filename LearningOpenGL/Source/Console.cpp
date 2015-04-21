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
float g_fElapsedTime = 0.0f;
int g_iStepLength = 1;

Vec2 g_lastMousePos;
bool g_bMouseRightButtonClicked = false;

bool bDrawMesh = true;

void init()
{
	CGLProgramManager::GetInstance()->Add("SkinMesh", SHADER_FILE_DIR + "SkinMesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("NormalMesh", SHADER_FILE_DIR + "Mesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("Particle", SHADER_FILE_DIR + "Particle_Vertex_Shader.vert", SHADER_FILE_DIR + "Particle_Fragment_Shader.frag");

	g_particleSystem = new GLParticleSystem;
	g_particleSystem->GetTransformData().m_rotation.x = -90;
	g_particleSystem->GetTransformData().m_pos.z = 80;
	CEmitter* pEmitter = new CEmitter;
	pEmitter->SetTotalDuration(2.0f);
	pEmitter->SetBlendMode(CEmitter::EBlendMode_Add);
	pEmitter->SetTexture("ParticleFlamesSheet.png");
	pEmitter->SetTextureAnimationInfo(4, 8, 2);
	pEmitter->SetEmitMode(CEmitter::EEmitMode_Free);
	pEmitter->SetEmissionRate(3.0f);
	pEmitter->GetParticleTexSheetFrameOverLifeTimeRef().Init<int>(EPropertyType_Liner, 2, SKeyNode<int>(0.0f, 0), SKeyNode<int>(1.0f, 32));
	pEmitter->GetParticleStartLifeTimeRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, 0.7f, 1.0f);
	pEmitter->GetParticleStartSizeRef().Init<double>(EPropertyType_Constant, 1.2f);
	pEmitter->GetParticleStartZRotationRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, -12.0f, 14.0f);
	pEmitter->GetParticleStartSpeedRef().Init<double>(EPropertyType_Constant, 0.0f);
	pEmitter->GetParticleAlphaOverLifeTimeRef().Init<double>(EPropertyType_Liner, 4, SKeyNode<float>(0.0f, 0.0f), SKeyNode<float>(0.191f, 255.0f), SKeyNode<float>(0.818f, 69.0f), SKeyNode<float>(1.0f, 0.0f));
	pEmitter->GetEmitterShapeRef().SetShape(CEmitterShape::EShape_Cone);
	pEmitter->GetEmitterShapeRef().SetAngle(5.0f);
	pEmitter->GetEmitterShapeRef().SetRadius(0.1f);
	g_particleSystem->AddEmitter(pEmitter);

	/*CEmitter* pEmitter = new CEmitter;
	pEmitter->SetTotalDuration(5.0f);
	pEmitter->SetTexture("ParticleCloudWhite.png");
	pEmitter->SetEmitMode(CEmitter::EEmitMode_Relative);
	pEmitter->SetEmissionRate(100.0f);
	pEmitter->SetMaxParticles(1000);
	pEmitter->SetShaderColor(Color4F(204.0f / 255, 190.0f / 255, 174.0f / 255, 18.0f / 255));
	pEmitter->GetParticleStartLifeTimeRef().Init<double>(EPropertyType_Constant, 5.0f);
	pEmitter->GetParticleStartSpeedRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, 20.0f, 40.0f);
	pEmitter->GetParticleStartSizeRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, 15.0f, 25.0f);
	pEmitter->GetParticleStartZRotationRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, 0, 360);
	pEmitter->GetParticleStartColorRef().Init<Color3B>(EPropertyType_RandomBetweenConstant, 2, Color3B(79, 79, 79), Color3B(161, 161, 161));
	pEmitter->GetParticleStartAlphaRef().Init<double>(EPropertyType_Constant, 255.0f);
	pEmitter->GetParticleAlphaOverLifeTimeRef().Init<double>(EPropertyType_Liner, 4, SKeyNode<float>(0.0f, 0.0f), SKeyNode<float>(0.211f, 255.0f), SKeyNode<float>(0.67f, 255.0f), SKeyNode<float>(1.0f, 0.0f));
	pEmitter->GetParticleSizeOverLifeTimeRef().Init<double>(EPropertyType_Constant, 1.0f);
	pEmitter->GetParticleZRotationOverLifeTimeRef().Init<double>(EPropertyType_RandomBetweenCurve, 2, 4, 180.0f, 180.0f, 30.0f, 30.0f, 4, -180.0f, -180.0f, -30.0f, -30.0f);
	pEmitter->GetEmitterShapeRef().SetShape(CEmitterShape::EShape_Cone);
	pEmitter->GetEmitterShapeRef().SetAngle(5.0f);
	pEmitter->GetEmitterShapeRef().SetRadius(100.0f);
	g_particleSystem->AddEmitter(pEmitter);*/

	g_planeMesh = new COGLMesh;
	g_planeMesh->InitFromFile("plane.CSTM");
	g_planeMesh->m_transform.m_scale.set(10, 10, -10);
	g_planeMesh->m_transform.m_pos.set(0, -30, -100);
	for ( int i = 0; i < g_planeMesh->GetMeshData().m_vSubMesh.size(); ++i )
		g_planeMesh->SetTexture("default.png", i);
	g_planeMesh->m_color = Color4F(0.5f, 0.5f, 0.5f, 1.0f);
	g_planeMesh->m_bEnableCullFace = false;
	g_planeMesh->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("NormalMesh") );

	COGLMesh* pSkinMesh = new COGLMesh;
	pSkinMesh->InitFromFile("bat.CSTM");
	for ( int i = 0; i < pSkinMesh->GetMeshData().m_vSubMesh.size(); ++i )
	{
		pSkinMesh->SetTexture("BatArmor.png", i);
	}
	pSkinMesh->m_transform.m_pos.set(g_planeMesh->m_transform.m_pos.x, g_planeMesh->m_transform.m_pos.y + 30, g_planeMesh->m_transform.m_pos.z);
	pSkinMesh->m_transform.m_scale.set(1, 1, -1);
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

	g_fElapsedTime += g_fDeltaTime;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( bDrawMesh )
	{
		g_planeMesh->Update(g_fDeltaTime);
		g_planeMesh->Render();
		/*for (int i = 0; i < g_vMesh.size(); ++i)
		{
		g_vMesh[i]->Update(g_fDeltaTime);
		g_vMesh[i]->Render();
		}*/
	}

	//g_particleSystem->GetTransformData().m_pos.x = 50 * sin(10 * g_fElapsedTime);
	g_particleSystem->Update(g_fDeltaTime);
	g_particleSystem->Render();
	
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
	}

	CDirector::GetInstance()->GetCurCamera()->Move(iMoveLeftRight, 0, iMoveForwardBack);
}

void mouse_down( int button, int state, int x, int y )
{
	if ( button == GLUT_LEFT_BUTTON )
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
