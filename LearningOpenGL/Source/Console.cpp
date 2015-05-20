#include "FrameWork/OpenGL/GLFrameWork.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"
#include "FrameWork/DataTypes.h"
#include "FrameWork/GLMesh.h"
#include "FrameWork/Director.h"
#include "FrameWork/GLParticleSystem.h"
#include "FrameWork/GLLabel.h"
#include "FrameWork/GLPrimitive.h"
#include "FrameWork/GLTerrain.h"
#include "FrameWork/GLSkyBox.h"

COGLMesh* g_pCharactor = nullptr;
std::vector<COGLMesh*> g_vMesh;
std::vector<GLuint> g_vGLProgram;

GLParticleSystem* g_particleSystem = nullptr;
CGLLabel* g_pDeltaTimeLabel = nullptr;
CGLLabel* g_pFPSLabel = nullptr;

CGLPrimitive* g_pLineDrawer = nullptr;
CGLPrimitive* g_pPointDrawer = nullptr;

CGLTerrain* g_pTerrain = nullptr;
CGLSkyBox* g_pSkyBox = nullptr;

timeval g_fLastTime = {0, 0};
float g_fDeltaTime = 0.0f;
float g_fElapsedTime = 0.0f;
int g_iStepLength = 100;

int g_iFrame = 0;
float g_fAccumulatedTime = 0;

Vec2 g_lastMousePos;
bool g_bMouseRightButtonClicked = false;

bool bDrawMesh = true;

bool bDrawWireFrame = false;

void UpdatePos()
{
	Vec3 lookAt = CDirector::GetInstance()->GetPerspectiveCamera()->GetLookAtDir();
	Vec3 cameraPos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
	Vec3 charactorPos = cameraPos + lookAt * 250;

	charactorPos.y = g_pTerrain->GetHeight( Vec2(charactorPos.x, charactorPos.z) );
	g_pCharactor->m_transform.m_pos = charactorPos;

	cameraPos.y = g_pTerrain->GetHeight( Vec2(cameraPos.x, cameraPos.z) ) + 250;
	CDirector::GetInstance()->GetPerspectiveCamera()->SetCameraPos(cameraPos);
}

void init()
{
	srand(time(nullptr));

	CGLProgramManager::GetInstance()->Add("SkinMesh", SHADER_FILE_DIR + "SkinMesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("StaticMesh", SHADER_FILE_DIR + "Mesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("Particle", SHADER_FILE_DIR + "Particle_Vertex_Shader.vert", SHADER_FILE_DIR + "Particle_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("Label", SHADER_FILE_DIR + "Label_Vertex_Shader.vert", SHADER_FILE_DIR + "Label_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("Primitive", SHADER_FILE_DIR + "Primitive_Vertex_Shader.vert", SHADER_FILE_DIR + "Primitive_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("Terrain", SHADER_FILE_DIR + "Terrain_Vertex_Shader.vert", SHADER_FILE_DIR + "Terrain_Fragment_Shader.frag");
	CGLProgramManager::GetInstance()->Add("SkyBox", SHADER_FILE_DIR + "SkyBox_Vertex_Shader.vert", SHADER_FILE_DIR + "SkyBox_Fragment_Shader.frag");

	g_pDeltaTimeLabel = new CGLLabel(FONT_FILE_DIR + "simyou.ttf", 20);
	g_pDeltaTimeLabel->m_transform.m_pos.x = -RESOLUTION_WIDTH / 2 + 10;
	g_pDeltaTimeLabel->m_transform.m_pos.y = RESOLUTION_HEIGHT / 2 - 20;
	g_pDeltaTimeLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);

	g_pFPSLabel = new CGLLabel(FONT_FILE_DIR + "simyou.ttf", 20);
	g_pFPSLabel->SetString("FPS:0");
	g_pFPSLabel->m_transform.m_pos.x = g_pDeltaTimeLabel->m_transform.m_pos.x;
	g_pFPSLabel->m_transform.m_pos.y = g_pDeltaTimeLabel->m_transform.m_pos.y - 20;
	g_pFPSLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);

	g_pLineDrawer = new CGLPrimitive(CGLPrimitive::EPrimitiveType_Line);
	g_pLineDrawer->DrawLine(Vec3(0, 0, 0), Vec3(100, 100, 0));
	g_pLineDrawer->DrawLine(Vec3(100, 100, 0), Vec3(200, 100, 0));

	g_pPointDrawer = new CGLPrimitive(CGLPrimitive::EPrimitiveType_Point);
	g_pPointDrawer->DrawPoint(Vec3(-100, -100, 0), 5);

	g_particleSystem = new GLParticleSystem;
	//g_particleSystem->GetTransformData().m_rotation.x = -90;
	//g_particleSystem->GetTransformData().m_pos.z = 80;
	//CEmitter* pEmitter = new CEmitter;
	//pEmitter->SetTotalDuration(2.0f);
	//pEmitter->SetBlendMode(CEmitter::EBlendMode_ADD);
	//pEmitter->SetTexture("ParticleFlamesSheet.png");
	//pEmitter->SetTextureAnimationInfo(4, 8, 2);
	//pEmitter->SetEmitMode(CEmitter::EEmitMode_Free);
	//pEmitter->SetEmissionRate(3.0f);
	//pEmitter->GetParticleTexSheetFrameOverLifeTimeRef().Init<int>(EPropertyType_Liner, 2, SKeyNode<int>(0.0f, 0), SKeyNode<int>(1.0f, 32));
	//pEmitter->GetParticleStartLifeTimeRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, 0.7f, 1.0f);
	//pEmitter->GetParticleStartSizeRef().Init<double>(EPropertyType_Constant, 2.0f);
	//pEmitter->GetParticleStartZRotationRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, -12.0f, 14.0f);
	//pEmitter->GetParticleStartSpeedRef().Init<double>(EPropertyType_Constant, 0.0f);
	//pEmitter->GetParticleAlphaOverLifeTimeRef().Init<double>(EPropertyType_Liner, 4, SKeyNode<float>(0.0f, 0.0f), SKeyNode<float>(0.191f, 255.0f), SKeyNode<float>(0.818f, 69.0f), SKeyNode<float>(1.0f, 0.0f));
	//g_particleSystem->AddEmitter(pEmitter);

	CEmitter* pEmitter = new CEmitter;
	pEmitter->SetTotalDuration(5.0f);
	pEmitter->SetTexture("ParticleCloudWhite.png");
	pEmitter->SetEmitMode(CEmitter::EEmitMode_Relative);
	pEmitter->SetEmissionRate(100.0f);
	pEmitter->SetMaxParticles(1000);
	pEmitter->SetRenderMode(CEmitter::ERenderMode_VerticalBillboard);
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
	pEmitter->GetEmitterShapeRef().SetShape(CEmitterShape::EShape_Box);
	pEmitter->GetEmitterShapeRef().SetExtent(Vec3(100, 0, 100));
	g_particleSystem->AddEmitter(pEmitter);

	COGLMesh* planeMesh = new COGLMesh;
	planeMesh->InitFromFile("plane.CSTM");
	planeMesh->m_transform.m_scale.set(10, 10, -10);
	planeMesh->m_transform.m_pos.set(0, -30, -100);
	for ( int i = 0; i < planeMesh->GetMeshData().m_vSubMesh.size(); ++i )
		planeMesh->SetTexture("default.png", i);
	planeMesh->m_color = Color4F(0.5f, 0.5f, 0.5f, 1.0f);
	planeMesh->m_bEnableCullFace = false;
	planeMesh->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh") );
	//g_vMesh.push_back(planeMesh);

	g_pCharactor = new COGLMesh;
	g_pCharactor->InitFromFile("test.CSTM");
	for ( int i = 0; i < g_pCharactor->GetMeshData().m_vSubMesh.size(); ++i )
		g_pCharactor->SetTexture("cubemap_hills/hills_negative_x.png", i);
	g_pCharactor->m_transform.m_scale.set(1, 1, -1);
	g_pCharactor->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("SkinMesh") );
	g_vMesh.push_back(g_pCharactor);

	g_pTerrain = new CGLTerrain();
	g_pTerrain->SetDetailTexture("dirt.png", "Grass2.png", "road.png", "GreenSkin.png");
	g_pTerrain->SetDetailTextureSize(20, 20, 20, 20);
	g_pTerrain->SetAlphaTexture("alphamap.png");
	g_pTerrain->SetDrawWireFrame(false);
	g_pTerrain->m_transform.m_scale.x = 100;
	g_pTerrain->m_transform.m_scale.y = 10;
	g_pTerrain->m_transform.m_scale.z = 100;
	g_pTerrain->Init("heightmap16.png");

	g_pSkyBox = new CGLSkyBox;
	g_pSkyBox->Init("skybox/right.png", "skybox/left.png",
		"skybox/top.png", "skybox/bottom.png",
		"skybox/back.png", "skybox/front.png"
		);

	UpdatePos();
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
		for (int i = 0; i < g_vMesh.size(); ++i)
		{
			g_vMesh[i]->Update(g_fDeltaTime);
			g_vMesh[i]->Render();
		}
	}
	
	char buf[50] = {0};
	sprintf(buf, "DeltaTime:%f", g_fDeltaTime);
	g_pDeltaTimeLabel->SetString(buf);
	if ( g_fDeltaTime >= 0.5f )
		g_pDeltaTimeLabel->m_color = Color4F(1.0f, 0.0f, 0.0f, 1.0f);
	else
		g_pDeltaTimeLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);

	g_fAccumulatedTime += g_fDeltaTime;
	++g_iFrame;
	if ( g_fAccumulatedTime >= 1.0f )
	{
		char buf[50] = {0};
		sprintf(buf, "FPS:%d", g_iFrame);
		g_pFPSLabel->SetString(buf);

		if ( g_iFrame <= 30 )
			g_pFPSLabel->m_color = Color4F(1.0f, 0.0f, 0.0f, 1.0f);
		else
			g_pFPSLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);

		g_iFrame = 0;
		g_fAccumulatedTime = 0;
	}

	g_pTerrain->Update(g_fDeltaTime);
	g_pTerrain->Render();

	//g_pPointDrawer->Render();
	//g_pLineDrawer->Render();

	g_particleSystem->Update(g_fDeltaTime);
	g_particleSystem->Render();

	g_pSkyBox->Update(g_fDeltaTime);
	g_pSkyBox->Render();

	g_pDeltaTimeLabel->Render();
	g_pFPSLabel->Render();
	
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
	case 'b': bDrawWireFrame = !bDrawWireFrame; break;
	}

	g_pTerrain->SetDrawWireFrame(bDrawWireFrame);

	CDirector::GetInstance()->GetPerspectiveCamera()->Move(iMoveLeftRight, 0, iMoveForwardBack);

	UpdatePos();
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

		CDirector::GetInstance()->GetPerspectiveCamera()->Rotate(fPitchDelta, fYawDelta);
		UpdatePos();

		g_lastMousePos = Vec2(x, y);
	}
}

