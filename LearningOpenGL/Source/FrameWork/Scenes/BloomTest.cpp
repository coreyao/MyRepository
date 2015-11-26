#include "BloomTest.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Mesh.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Director.h"
#include "FrameWork/Bloom.h"

void CBloomTest::OnEnter()
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
	cubeMesh->m_color = Color4F(0, 100, 0, 1);
	cubeMesh->SetMaterial(material1, 0);
	cubeMesh->SetLightEnable(true);
	cubeMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
	m_vLighters.push_back(cubeMesh);

	CPointLight* pPointLight = new CPointLight;
	pPointLight->m_lightPos = cubeMesh->m_transform.GetPosition();
	pPointLight->m_ambientColor = Vec3(0.2f, 0.2f, 0.2f);
	pPointLight->m_diffuseColor = Vec3(cubeMesh->m_color.r, cubeMesh->m_color.g, cubeMesh->m_color.b);
	pPointLight->m_specularColor = pPointLight->m_diffuseColor;
	pPointLight->m_attenuation_quadratic = 0.00001f;
	CLightManager::GetInstance()->AddLight(pPointLight);

	m_pBloom = new CBloom;
	m_pBloom->Init();
	m_pBloom->m_fExposure = 0.1f;
}

void CBloomTest::OnExit()
{
	CBaseScene::OnExit();
}

void CBloomTest::Update(float dt)
{
	CBaseScene::Update(dt);
}

void CBloomTest::Draw()
{
	m_pBloom->PreRender();

	std::vector<GLuint> vProgram;
	vProgram.reserve(m_vObject.size());
	for (auto& pObj : m_vObject)
	{
		vProgram.push_back(pObj->m_theProgram);
		pObj->m_theProgram = m_pBloom->m_firstPassProgram;
		pObj->Render();
	}

	for (auto& pObj : m_vLighters)
	{
		vProgram.push_back(pObj->m_theProgram);
		pObj->m_theProgram = m_pBloom->m_firstPassLighterProgram;
		pObj->Render();
	}

	m_pBloom->PostRender();

	int iIndex = 0;
	for (auto& pObj : m_vObject)
		pObj->m_theProgram = vProgram[iIndex++];

	for (auto& pObj : m_vLighters)
		pObj->m_theProgram = vProgram[iIndex++];
}

