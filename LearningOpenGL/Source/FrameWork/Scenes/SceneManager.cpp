#include "SceneManager.h"
#include "StaticMeshTest.h"
#include "HDRTest.h"
#include "BloomTest.h"
#include "DeferredShadingTest.h"
#include "TextureProjectionTest.h"

CSceneManager* CSceneManager::GetInstance()
{
	if (!s_instance)
	{
		s_instance = new CSceneManager;

		//{
		//	CStaticMeshTest* scene = new CStaticMeshTest;
		//	s_instance->m_vScene.push_back(scene);
		//}

		//{
		//	CHDRTest* scene = new CHDRTest;
		//	s_instance->m_vScene.push_back(scene);
		//}

		//{
		//	CBloomTest* scene = new CBloomTest;
		//	s_instance->m_vScene.push_back(scene);
		//}


		//{
		//	CDeferredShadingTest* scene = new CDeferredShadingTest;
		//	s_instance->m_vScene.push_back(scene);
		//}

		{
			CTextureProjectionTest* scene = new CTextureProjectionTest;
			s_instance->m_vScene.push_back(scene);
		}
	}

	return s_instance;
}

void CSceneManager::Next()
{
	int iCurIndex = m_curSceneIndex;
	int iNextIndex = m_curSceneIndex + 1;

	if ( iCurIndex > 0 )
		m_vScene[iCurIndex]->OnExit();

	if (iNextIndex >= m_vScene.size())
		iNextIndex = m_vScene.size() - 1;
	else if (iNextIndex < 0)
		iNextIndex = 0;

	m_vScene[iNextIndex]->OnEnter();
	m_curSceneIndex = iNextIndex;
}

void CSceneManager::Previous()
{
	int iCurIndex = m_curSceneIndex;
	int iNextIndex = m_curSceneIndex - 1;

	if (iCurIndex > 0)
		m_vScene[iCurIndex]->OnExit();

	if (iNextIndex >= m_vScene.size())
		iNextIndex = m_vScene.size() - 1;
	else if (iNextIndex < 0)
		iNextIndex = 0;

	m_vScene[iNextIndex]->OnEnter();
	m_curSceneIndex = iNextIndex;
}

CBaseScene* CSceneManager::GetCurScene()
{
	return m_vScene[m_curSceneIndex];
}

void CSceneManager::Update(float dt)
{
	m_vScene[m_curSceneIndex]->Update(dt);
}

void CSceneManager::Draw()
{
	m_vScene[m_curSceneIndex]->Draw();
}

CSceneManager::CSceneManager()
: m_curSceneIndex(-1)
{

}

CSceneManager* CSceneManager::s_instance = nullptr;

