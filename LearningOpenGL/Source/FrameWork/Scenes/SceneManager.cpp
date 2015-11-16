#include "SceneManager.h"

CSceneManager* CSceneManager::GetInstance()
{
	if (!s_instance)
	{
		s_instance = new CSceneManager;
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

