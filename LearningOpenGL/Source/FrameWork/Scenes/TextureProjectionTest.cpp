#include "TextureProjectionTest.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Mesh.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Director.h"

void CTextureProjectionTest::OnEnter()
{
	CBaseScene::OnEnter();

	CMesh* planeMesh = new CMesh;
	planeMesh->InitFromFile("plane.CSTM");
	planeMesh->m_transform.SetPosition(Vec3(0, -50, -100));
	planeMesh->m_transform.SetScale(Vec3(10, 10, -10));
	for (int i = 0; i < planeMesh->m_vSubMesh.size(); ++i)
	{
		CMaterial newMaterial;
		newMaterial.SetBaseColorTexture("brickwall.png");
		newMaterial.SetNormalMapTexture("brickwall_normal.png");
		newMaterial.SetShininess(64.0f);
		planeMesh->SetMaterial(newMaterial, i);
	}
	planeMesh->SetLightEnable(true);
	planeMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
	planeMesh->m_renderState.m_bEnableGammaCorrection = false;
	planeMesh->m_renderState.m_eVertexOrder = EVertexOrder_Counter_ClockWise;
	planeMesh->m_bReceiveShadow = true;
	m_vObject.push_back(planeMesh);
}

void CTextureProjectionTest::OnExit()
{
	CBaseScene::OnExit();
}

void CTextureProjectionTest::Update(float dt)
{
	CBaseScene::Update(dt);
}

void CTextureProjectionTest::Draw()
{
}

