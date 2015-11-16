#include "StaticMeshTest.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Mesh.h"
#include "FrameWork/OpenGL/GLProgramManager.h"

void CStaticMeshTest::OnEnter()
{
	CMaterial material1;
	material1.SetBaseColorTexture("HelloWorld.png");
	CMesh* pFileCube = new CMesh;
	pFileCube->InitFromFile("cube.CSTM");
	pFileCube->SetMaterial(material1, 0);
	pFileCube->m_transform.SetPosition(Vec3(0, -25, 0));
	pFileCube->m_transform.SetScale(Vec3(1, 1, -1));
	pFileCube->m_transform.SetRotation(Vec3(0, 60, 0));
	pFileCube->SetGLProgram(CGLProgramManager::GetInstance()->CreateProgramByName("StaticMesh"));

	m_vObject.push_back(pFileCube);
}

void CStaticMeshTest::OnExit()
{
	for (auto& pObj : m_vObject)
	{
		delete pObj;
	}
	m_vObject.clear();
}

