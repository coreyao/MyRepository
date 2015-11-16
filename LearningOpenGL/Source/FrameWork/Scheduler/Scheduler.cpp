#include "FrameWork/Scheduler/Scheduler.h"

void CScheduler::Update(float dt)
{
	for (auto& cb : m_updateNormalList)
	{
		cb(dt);
	}
}

void CScheduler::AddUpdateCallBack(UpdateCallBack cb)
{
	m_updateNormalList.push_back(cb);
}

void CScheduler::ClearUpdateCallBack()
{
	m_updateNormalList.clear();
}

CScheduler* CScheduler::GetInstance()
{
	if ( !s_instance )
	{
		s_instance = new CScheduler;
	}

	return s_instance;
}

CScheduler* CScheduler::s_instance = nullptr;

