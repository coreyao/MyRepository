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

CMesh* g_pCharactor = nullptr;
CMesh* g_pLightMesh = nullptr;
std::vector<CMesh*> g_vMesh;
std::vector<GLuint> g_vGLProgram;

CParticleSystem* g_particleSystem = nullptr;
CLabel* g_pDeltaTimeLabel = nullptr;
CLabel* g_pFPSLabel = nullptr;

CPrimitive* g_pLineDrawer = nullptr;
CPrimitive* g_pPointDrawer = nullptr;

CTerrain* g_pTerrain = nullptr;
CSkyBox* g_pSkyBox = nullptr;

CThirdPersonController* g_pController = nullptr;

timeval g_fLastTime = {0, 0};
float g_fDeltaTime = 0.0f;
float g_fElapsedTime = 0.0f;
int g_iStepLength = 20;

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
	CGLProgramManager::GetInstance()->Add("ShadowMap", SHADER_FILE_DIR + "shadow_map.vert", SHADER_FILE_DIR + "shadow_map.frag");

	g_pDeltaTimeLabel = new CLabel(FONT_FILE_DIR + "simyou.ttf", 20);
	g_pDeltaTimeLabel->m_transform.m_pos.x = -RESOLUTION_WIDTH / 2 + 10;
	g_pDeltaTimeLabel->m_transform.m_pos.y = RESOLUTION_HEIGHT / 2 - 20;
	g_pDeltaTimeLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);

	g_pFPSLabel = new CLabel(FONT_FILE_DIR + "simyou.ttf", 20);
	g_pFPSLabel->SetString("FPS:0");
	g_pFPSLabel->m_transform.m_pos.x = g_pDeltaTimeLabel->m_transform.m_pos.x;
	g_pFPSLabel->m_transform.m_pos.y = g_pDeltaTimeLabel->m_transform.m_pos.y - 20;
	g_pFPSLabel->m_color = Color4F(0.0f, 1.0f, 0.0f, 1.0f);

	g_pLineDrawer = new CPrimitive(CPrimitive::EPrimitiveType_Line);

	g_pPointDrawer = new CPrimitive(CPrimitive::EPrimitiveType_Point);
	g_pPointDrawer->DrawPoint(Vec3(-100, -100, 0), 5);

	g_particleSystem = new CParticleSystem;
	g_particleSystem->GetTransformData().m_pos.x = 300;
	g_particleSystem->GetTransformData().m_rotation.x = -90;
	g_particleSystem->GetTransformData().m_scale.x = g_particleSystem->GetTransformData().m_scale.y = g_particleSystem->GetTransformData().m_scale.z = 10.0f;
	CEmitter* pEmitter = new CEmitter;
	pEmitter->SetTotalDuration(1.0f);
	pEmitter->SetBlendMode(CEmitter::EBlendMode_ALPHA_BLEND);
	pEmitter->SetTexture("T_FX_guangyun01.png");
	pEmitter->SetEmitMode(CEmitter::EEmitMode_Relative);
	pEmitter->SetEmissionRate(50.0f);
	pEmitter->GetEmitterShapeRef().SetShape(CEmitterShape::EShape_Cone);
	pEmitter->GetEmitterShapeRef().SetRadius(30.0f);
	pEmitter->GetEmitterShapeRef().SetAngle(30.0f);
	pEmitter->GetParticleStartLifeTimeRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, 4.f, 5.f);
	pEmitter->GetParticleStartSizeRef().Init<double>(EPropertyType_Constant, 30.0f);
	pEmitter->GetParticleStartZRotationRef().Init<double>(EPropertyType_RandomBetweenConstant, 2, -12.0f, 14.0f);
	pEmitter->GetParticleStartSpeedRef().Init<double>(EPropertyType_Constant, 50.0f);
	g_particleSystem->AddEmitter(pEmitter);

	CMesh* planeMesh = new CMesh;
	planeMesh->InitFromFile("plane.CSTM");
	planeMesh->m_transform.m_scale.set(10, 10, -10);
	planeMesh->m_transform.m_pos.set(0, -30, -100);
	for ( int i = 0; i < planeMesh->GetMeshData().m_vSubMesh.size(); ++i )
	{
		CMaterial newMaterial;
		newMaterial.SetBaseColorTexture("brickwall.png");
		newMaterial.SetNormalMapTexture("brickwall_normal.png");
		newMaterial.SetShininess(64.0f);
		planeMesh->SetMaterial(newMaterial, i);
	}
	planeMesh->m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
	planeMesh->m_bEnableCullFace = false;
	planeMesh->SetLightEnable(true);
	planeMesh->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh") );
	g_vMesh.push_back(planeMesh);

	g_pLightMesh = new CMesh;
	g_pLightMesh->InitFromFile("ball.CSTM");
	g_pLightMesh->m_transform.m_pos.set(0, 100, -100);
	for ( int i = 0; i < g_pLightMesh->GetMeshData().m_vSubMesh.size(); ++i )
	{
		CMaterial newMaterial;
		newMaterial.SetBaseColorTexture("default.png");
		g_pLightMesh->SetMaterial(newMaterial, i);
	}
	g_pLightMesh->m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
	g_pLightMesh->m_bEnableCullFace = false;
	g_pLightMesh->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh") );
	//g_vMesh.push_back(g_pLightMesh);

	g_pCharactor = new CMesh;
	g_pCharactor->InitFromFile("hama.CSTM");
	g_pCharactor->m_transform.m_scale.set(1, 1, -1);
	g_pCharactor->m_transform.m_rotation.set(0, 0, 0);
	g_pCharactor->m_transform.m_pos.set(0, 350, 100);
	g_pCharactor->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("SkinMesh") );
	//g_pCharactor->PlayAnim(0, 25, true, nullptr);
	g_pCharactor->SetLightEnable(false);
	//g_vMesh.push_back(g_pCharactor);

	CMaterial material1;
	material1.SetBaseColorTexture("HelloWorld.png");
	CMesh* pFileCube = new CMesh;
	pFileCube->InitFromFile("cube.CSTM");
	pFileCube->SetMaterial(material1, 0);
	pFileCube->m_transform.m_pos = (Vec3(0, -25, 0));
	pFileCube->m_transform.m_scale = (Vec3(1, 1, -1));
	pFileCube->m_transform.m_rotation = (Vec3(0, 60, 0));
	pFileCube->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
	//g_vMesh.push_back(pFileCube);

	g_pTerrain = new CTerrain();
	g_pTerrain->SetDetailTexture("dirt.png", "Grass2.png", "road.png", "GreenSkin.png");
	g_pTerrain->SetDetailTextureSize(20, 20, 20, 20);
	g_pTerrain->SetAlphaTexture("alphamap.png");
	g_pTerrain->SetDrawWireFrame(false);
	g_pTerrain->m_transform.m_scale.x = 100;
	g_pTerrain->m_transform.m_scale.y = 10;
	g_pTerrain->m_transform.m_scale.z = 100;
	g_pTerrain->Init("heightmap16.png");

	g_pSkyBox = new CSkyBox;
	g_pSkyBox->Init("skybox/right.png", "skybox/left.png",
		"skybox/top.png", "skybox/bottom.png",
		"skybox/back.png", "skybox/front.png"
		);

	g_pController = new CThirdPersonController;
	g_pController->m_transform.m_pos.y = g_pTerrain->GetHeight( Vec2(g_pController->m_transform.m_pos.x, g_pController->m_transform.m_pos.z) );
	//g_pController->SetCharactor(g_pCharactor);

	CDirectionalLight* pDirectionalLight = new CDirectionalLight;
	pDirectionalLight->m_ambientColor = Vec3(0.1f, 0.1f, 0.1f);
	pDirectionalLight->m_diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	pDirectionalLight->m_specularColor = Vec3(1.0f, 1.0f, 1.0f);
	pDirectionalLight->m_lightDir = Vec3(1, 1, 1);
	pDirectionalLight->m_lightDir.normalize();
	pDirectionalLight->m_pDebugMesh = g_pLightMesh;
	CLightManager::GetInstance()->AddLight(pDirectionalLight);

	CPointLight* pPointLight = new CPointLight;
	pPointLight->m_ambientColor = Vec3(0.1f, 0.1f, 0.1f);
	pPointLight->m_diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	pPointLight->m_specularColor = Vec3(1.0f, 1.0f, 1.0f);
	pPointLight->m_pDebugMesh = g_pLightMesh;
	//CLightManager::GetInstance()->AddLight(pPointLight);

	CSpotLight* pSpotLight = new CSpotLight;
	pSpotLight->m_ambientColor = Vec3(0.1f, 0.1f, 0.1f);
	pSpotLight->m_diffuseColor = Vec3(0.8f, 0.8f, 0.8f);
	pSpotLight->m_specularColor = Vec3(1.0f, 1.0f, 1.0f);
	pSpotLight->m_lightDir = Vec3(0, -1, 0);
	pSpotLight->m_lightDir.normalize();
	pSpotLight->fInnerAngle = 30;
	pSpotLight->fOuterAngle = 60;
	//pSpotLight->m_attenuation_linear = 0.0009f;
	//pSpotLight->m_attenuation_quadratic = 0.032f;
	pSpotLight->m_pDebugMesh = g_pLightMesh;
	//CLightManager::GetInstance()->AddLight(pSpotLight);

	g_pLightMesh->m_transform.m_pos = Vec3( 0, 500, 0 );
	pDirectionalLight->m_lightDir = -pDirectionalLight->m_pDebugMesh->m_transform.m_pos;
	CDirector::GetInstance()->m_pShadowMap = new CShadowmap;
	CDirector::GetInstance()->m_pShadowMap->Init(pDirectionalLight);
}

void UpdateScene();
void DrawScene();

void DrawMesh();
void DrawLabel();
void DrawTerrain();
void DrawPrimitive();
void DrawSkyBox();
void DrawParticleSystem();

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
	CDirector::GetInstance()->m_pShadowMap->PreRender();
	DrawScene();
	CDirector::GetInstance()->m_pShadowMap->PostRender();
	//CDirector::GetInstance()->m_pShadowMap->DebugRenderShadowMap();
	DrawScene();
	//DrawSkyBox();
	DrawLabel();

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

	//g_pTerrain->SetDrawWireFrame(bDrawWireFrame);

	CDirector::GetInstance()->GetPerspectiveCamera()->Move(iMoveLeftRight, 0, iMoveForwardBack);
	//CDirector::GetInstance()->GetOrthographicCamera()->Move(iMoveLeftRight, 0, iMoveForwardBack);
	/*g_pController->m_transform.m_pos.y = g_pTerrain->GetHeight( Vec2(g_pController->m_transform.m_pos.x, g_pController->m_transform.m_pos.z) );
	g_pController->Move(iMoveLeftRight, 0, iMoveForwardBack);*/
}

void mouse_down( int button, int state, int x, int y )
{
	if ( button == GLUT_LEFT_BUTTON )
	{
		//if ( !g_bMouseRightButtonClicked )
		//{
		//	Vec3 pos = CDirector::GetInstance()->Unproject(Vec2(x, y));
		//	CRay ray(Vec3(0, 0, 0), pos);
		//	ray.Transform( CDirector::GetInstance()->GetCurViewMat().Inverse() );
		//	if ( ray.m_direction.Dot(Vec3(0, 1, 0)) < 0 )
		//	{
		//		Vec3 intersectionPos = g_pTerrain->GetIntersectionPoint(ray);
		//		g_pCharactor->m_transform.m_pos = intersectionPos;
		//	}
		//	//g_pLineDrawer->DrawLine(ray.m_origin, ray.m_origin + ray.m_direction * 10000);
		//}

		/*	if ( state == GLUT_DOWN )
		g_pCharactor->PlayAnim(215, 245, false, [](){ g_pCharactor->PlayAnim(10, 25, true, nullptr); });*/

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
	if ( bDrawMesh )
	{
		//g_pLightMesh->m_transform.m_pos = Vec3( 500 * cos(g_fElapsedTime), 500, 500 * sin(g_fElapsedTime) );
		//g_pLightMesh->m_transform.m_pos = Vec3( -300, 1000, 300 );

		for (int i = 0; i < g_vMesh.size(); ++i)
		{
			g_vMesh[i]->Render();
		}
	}
}

void DrawLabel()
{
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

	g_pDeltaTimeLabel->Render();
	g_pFPSLabel->Render();
}

void DrawTerrain()
{
	g_pTerrain->Render();
}

void DrawPrimitive()
{
	g_pPointDrawer->Render();
	g_pLineDrawer->Render();
}

void DrawSkyBox()
{
	g_pSkyBox->Render();
}

void DrawParticleSystem()
{
	g_particleSystem->Render();
}

void DrawScene()
{
	DrawMesh();
	//DrawTerrain();
	//DrawPrimitive();
	//DrawParticleSystem();
}

void UpdateScene()
{
	if ( bDrawMesh )
	{
		for (int i = 0; i < g_vMesh.size(); ++i)
		{
			//g_vMesh[i]->m_transform.m_rotation.set(0, g_vMesh[i]->m_transform.m_rotation.y + 30 * g_fDeltaTime, 0);
			g_vMesh[i]->Update(g_fDeltaTime);
		}
	}
	//g_pTerrain->Update(g_fDeltaTime);
	//g_pSkyBox->Update(g_fDeltaTime);
	//g_particleSystem->Update(g_fDeltaTime);
	//g_particleSystem->GetTransformData().SetMat(g_pCharactor->m_vSocket[0].GetWorldMat());
}

