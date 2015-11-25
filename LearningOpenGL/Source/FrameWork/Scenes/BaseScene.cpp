#include "FrameWork/Scenes/BaseScene.h"
#include "FrameWork/Scheduler/Scheduler.h"
#include "FrameWork/Renderer/Renderer.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Light.h"

void CBaseScene::OnEnter()
{
	
}

void CBaseScene::OnExit()
{
	for (auto& pObj : m_vObject)
		delete pObj;
	m_vObject.clear();

	CLightManager::Purge();
}

void CBaseScene::Update(float dt)
{
	for (auto& pObj : m_vObject)
	{
		pObj->Update(dt);
	}
}

void CBaseScene::Draw()
{
	for (auto& pObj : m_vObject)
	{
		pObj->Render();
	}
}

