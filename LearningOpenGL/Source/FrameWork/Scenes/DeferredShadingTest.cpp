#include "DeferredShadingTest.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Mesh.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Director.h"
#include "FrameWork/DeferredShading.h"

void CDeferredShadingTest::OnEnter()
{
	CBaseScene::OnEnter();

	CMaterial material1;
	material1.SetBaseColorTexture("default.png");
	CMaterial material2;
	material2.SetBaseColorTexture("brickwall.png");
	CMaterial material3;
	material3.SetBaseColorTexture("brickwall.png");
	material3.SetNormalMapTexture("brickwall_normal.png");

	CMesh* planeMesh = new CMesh;
	planeMesh->InitFromFile("plane.CSTM");
	planeMesh->m_transform.SetPosition(Vec3(0, -50, -100));
	planeMesh->m_transform.SetScale(Vec3(10, 10, -10));
	for (int i = 0; i < planeMesh->m_vSubMesh.size(); ++i)
		planeMesh->SetMaterial(material3, i);
	planeMesh->SetLightEnable(true);
	planeMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
	planeMesh->m_renderState.m_eVertexOrder = EVertexOrder_Counter_ClockWise;
	m_vObject.push_back(planeMesh);

	CMesh* cubeMesh = new CMesh;
	cubeMesh->InitFromFile("cube.CSTM");
	cubeMesh->m_transform.SetPosition(Vec3(0, 200, -100));
	cubeMesh->m_transform.SetScale(Vec3(3, 3, 3));
	cubeMesh->m_color = Color4F(0, 0, 1, 1);
	cubeMesh->SetMaterial(material1, 0);
	cubeMesh->SetLightEnable(true);
	cubeMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
	m_vObject.push_back(cubeMesh);

	CPointLight* pPointLight = new CPointLight;
	pPointLight->m_lightPos = cubeMesh->m_transform.GetPosition();
	pPointLight->m_ambientColor = Vec3(0.2f, 0.2f, 0.2f);
	pPointLight->m_diffuseColor = Vec3(cubeMesh->m_color.r, cubeMesh->m_color.g, cubeMesh->m_color.b);
	pPointLight->m_specularColor = pPointLight->m_diffuseColor;
	CLightManager::GetInstance()->AddLight(pPointLight);

	m_DeferredShading = new CDeferredShading;
	m_DeferredShading->Init();
}

void CDeferredShadingTest::OnExit()
{
	CBaseScene::OnExit();
}

void CDeferredShadingTest::Update(float dt)
{
	CBaseScene::Update(dt);
}

void CDeferredShadingTest::Draw()
{
	m_DeferredShading->PreRender();
	std::vector<GLuint> vProgram;
	vProgram.reserve(m_vObject.size());
	for (auto& pObj : m_vObject)
	{
		vProgram.push_back(pObj->m_theProgram);
		pObj->m_theProgram = m_DeferredShading->m_GeometryPassProgram;
		pObj->Render();
	}
	m_DeferredShading->PostRender();

	int iIndex = 0;
	for (auto& pObj : m_vObject)
		pObj->m_theProgram = vProgram[iIndex++];

	//CBaseScene::Draw();
}

