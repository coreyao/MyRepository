#include "StaticMeshTest.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Mesh.h"
#include "FrameWork/OpenGL/GLProgramManager.h"
#include "FrameWork/Director.h"

void CStaticMeshTest::OnEnter()
{
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
	m_vObject.push_back(planeMesh);

	for (int i = 0; i < 3; ++i)
	{
		CMesh* charactorMesh = new CMesh;
		charactorMesh->InitFromFile("hama.CSTM");
		charactorMesh->m_transform.SetPosition(planeMesh->m_transform.GetPosition() + Vec3(i * 500 - 300, 10, 0));
		charactorMesh->m_transform.SetScale(Vec3(1, 1, -1));
		for (int i = 0; i < charactorMesh->m_vSubMesh.size(); ++i)
		{
			CMaterial newMaterial;
			newMaterial.SetBaseColorTexture("Hama.png");
			charactorMesh->SetMaterial(newMaterial, i);
		}
		charactorMesh->SetLightEnable(true);
		charactorMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
		charactorMesh->m_renderState.m_bEnableGammaCorrection = false;
		charactorMesh->m_renderState.m_eVertexOrder = EVertexOrder_Counter_ClockWise;
		m_vObject.push_back(charactorMesh);
	}

	CDirectionalLight* pDirLight = new CDirectionalLight;
	pDirLight->m_ambientColor = Vec3(0.3f, 0.3f, 0.3f);
	pDirLight->m_diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	pDirLight->m_specularColor = Vec3(1.0f, 1.0f, 1.0f);
	pDirLight->m_lightPos = Vec3(planeMesh->m_transform.GetPosition() + Vec3(1000, 1000, 0));
	pDirLight->m_lightDir = Vec3(1, 1, 0).GetNormalized();
	CLightManager::GetInstance()->AddLight(pDirLight);

	CDirector::GetInstance()->m_pShadowMap = new CShadowmap;
	CDirector::GetInstance()->m_pShadowMap->Init(pDirLight);

	//CPointLight* pPointLight = new CPointLight;
	//pPointLight->m_ambientColor = Vec3(0.3f, 0.3f, 0.3f);
	//pPointLight->m_diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	//pPointLight->m_specularColor = Vec3(1.0f, 1.0f, 1.0f);
	//pPointLight->m_lightPos = Vec3(planeMesh->m_transform.GetPosition() + Vec3(100, 100, 0));
	//CLightManager::GetInstance()->AddLight(pPointLight);
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

void CStaticMeshTest::Draw()
{
	CDirector::GetInstance()->m_pShadowMap->PreRender();
	std::vector<GLuint> vProgram;
	vProgram.reserve(m_vObject.size());
	for (auto& pObj : m_vObject)
	{
		vProgram.push_back(pObj->m_theProgram);
		pObj->m_theProgram = CDirector::GetInstance()->m_pShadowMap->m_theProgram;
		pObj->Render();
	}

	CDirector::GetInstance()->m_pShadowMap->PostRender();
	CDirector::GetInstance()->m_pShadowMap->DebugRenderShadowMap();

	int iIndex = 0;
	for (auto& pObj : m_vObject)
	{
		pObj->m_theProgram = vProgram[iIndex++];
		pObj->Render();
	}
}

