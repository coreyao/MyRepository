#include "HDRTest.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Mesh.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Director.h"
#include "FrameWork/HDR.h"

void CHDRTest::OnEnter()
{
	CBaseScene::OnEnter();

	CMesh* cubeMesh = new CMesh;
	cubeMesh->InitFromFile("cube.CSTM");
	cubeMesh->m_transform.SetPosition(Vec3(0, -50, -100));
	cubeMesh->m_transform.SetScale(Vec3(10, 10, 60));
	for (int i = 0; i < cubeMesh->m_vSubMesh.size(); ++i)
	{
		CMaterial newMaterial;
		newMaterial.SetBaseColorTexture("brickwall.png");
		//newMaterial.SetNormalMapTexture("brickwall_normal.png");
		newMaterial.SetShininess(64.0f);
		cubeMesh->SetMaterial(newMaterial, i);
	}
	cubeMesh->SetLightEnable(true);
	cubeMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
	cubeMesh->m_renderState.m_bEnableGammaCorrection = false;
	cubeMesh->m_renderState.m_bCullBackFace = false;
	cubeMesh->m_bFlipNormal = true;
	m_vObject.push_back(cubeMesh);

	CPointLight* pPointLight = new CPointLight;
	pPointLight->m_lightPos = cubeMesh->m_transform.GetPosition() + Vec3(0, 200, -1000);
	pPointLight->m_ambientColor = Vec3(0, 0, 0);
	pPointLight->m_diffuseColor = Vec3(500, 500, 500);
	pPointLight->m_specularColor = Vec3(0, 0, 0);
	pPointLight->m_attenuation_quadratic = 0.001f;
	CLightManager::GetInstance()->AddLight(pPointLight);

	m_hdr = new CHDR;
	m_hdr->Init();
}

void CHDRTest::OnExit()
{
	CBaseScene::OnExit();
	delete m_hdr;
}

void CHDRTest::Update(float dt)
{
	CBaseScene::Update(dt);
}

void CHDRTest::Draw()
{
	m_hdr->PreRender();
	CBaseScene::Draw();
	m_hdr->PostRender();
}

