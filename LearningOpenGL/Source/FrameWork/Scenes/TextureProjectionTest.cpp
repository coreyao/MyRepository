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
	planeMesh->SetLightEnable(false);
	planeMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));
	planeMesh->m_renderState.m_bEnableGammaCorrection = false;
	planeMesh->m_renderState.m_eVertexOrder = EVertexOrder_Counter_ClockWise;
	planeMesh->m_bReceiveShadow = true;
	planeMesh->m_bReceiveProjector = true;
	m_vObject.push_back(planeMesh);

	CMesh* charactorMesh = new CMesh;
	charactorMesh->InitFromFile("hama.CSTM");
	charactorMesh->m_transform.SetPosition(Vec3(0, -50, -100) + Vec3(0, 10, 0));
	charactorMesh->m_transform.SetScale(Vec3(1, 1, -1));
	for (int i = 0; i < charactorMesh->m_vSubMesh.size(); ++i)
	{
		CMaterial newMaterial;
		newMaterial.SetBaseColorTexture("Hama.png");
		newMaterial.SetNormalMapTexture("Hama_NRM.png");
		newMaterial.SetShininess(64.0f);
		charactorMesh->SetMaterial(newMaterial, i);
	}
	charactorMesh->SetLightEnable(false);
	charactorMesh->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("SkinMesh"));
	charactorMesh->m_renderState.m_bEnableGammaCorrection = false;
	charactorMesh->m_renderState.m_eVertexOrder = EVertexOrder_Counter_ClockWise;
	charactorMesh->m_bReceiveShadow = false;
	charactorMesh->m_bReceiveProjector = true;
	charactorMesh->PlayAnim(0, 235, true, nullptr);
	m_vObject.push_back(charactorMesh);

	m_pProjector = new CProjector("bang.png");
	m_pProjector->worldTransform.SetPosition(Vec3(0, 50, -100));
	m_pProjector->worldTransform.SetRotation(Vec3(-90, 0, 0));
	m_pProjector->ViewMat = Mat4::CreateLookAt(m_pProjector->worldTransform.GetPosition(), m_pProjector->worldTransform.GetRotationMat() * Vec4(0, 0, -1, 0) * 1000.0f, Vec3(0, 0, -1));
}

void CTextureProjectionTest::OnExit()
{
	delete m_pProjector;
	m_pProjector = nullptr;

	CBaseScene::OnExit();
}

void CTextureProjectionTest::Update(float dt)
{
	CBaseScene::Update(dt);
}

void CTextureProjectionTest::Draw()
{
	for (auto& pObj : m_vObject)
	{
		glUseProgram(pObj->m_theProgram);

		GLint unif = glGetUniformLocation(pObj->m_theProgram, "ProjectorSpaceMatrix");
		if (unif >= 0)
		{
			glUniformMatrix4fv(unif, 1, GL_FALSE, (m_pProjector->ProjectionMat * m_pProjector->ViewMat).m);
		}

		unif = glGetUniformLocation(pObj->m_theProgram, "u_projectorImage");
		if (unif >= 0)
		{
			glUniform1i(unif, 2);

			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, m_pProjector->m_colorTex);
			glBindSampler(2, ((CMesh*)pObj)->m_Sampler);
		}

		GLint receiveProjectorUnif = glGetUniformLocation(pObj->m_theProgram, "u_enableProjector");
		if (receiveProjectorUnif >= 0)
		{
			glUniform1i(receiveProjectorUnif, true);
		}

		pObj->Render();
	}
}


