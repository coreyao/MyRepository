#include "StaticMeshTest.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Mesh.h"
#include "FrameWork/OpenGL/GLProgramManager.h"

void CStaticMeshTest::OnEnter()
{
	CMesh* planeMesh = new CMesh;
	planeMesh->InitFromFile("plane.CSTM");
	planeMesh->m_transform.SetScale(Vec3(15, 15, -15));
	planeMesh->m_transform.SetPosition(Vec3(0, -50, -100));
	for (int i = 0; i < planeMesh->m_vSubMesh.size(); ++i)
	{
		CMaterial newMaterial;
		newMaterial.SetBaseColorTexture("brickwall.png");
		newMaterial.SetNormalMapTexture("brickwall_normal.png");
		newMaterial.SetShininess(64.0f);
		planeMesh->SetMaterial(newMaterial, i);
	}
	planeMesh->m_renderState.m_bEnableCullFace = false;
	planeMesh->SetLightEnable(true);
	planeMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
	m_vObject.push_back(planeMesh);

	CPointLight* pPointLight = new CPointLight;
	pPointLight->m_ambientColor = Vec3(0.3f, 0.3f, 0.3f);
	pPointLight->m_diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	pPointLight->m_specularColor = Vec3(1.0f, 1.0f, 1.0f);
	pPointLight->m_lightPos = Vec3(planeMesh->m_transform.GetPosition() + Vec3(0, 100, 0));
	CLightManager::GetInstance()->AddLight(pPointLight);
}

void CStaticMeshTest::OnExit()
{
	for (auto& pObj : m_vObject)
	{
		delete pObj;
	}
	m_vObject.clear();

	CLightManager::Purge();
}

