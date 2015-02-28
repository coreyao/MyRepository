#include "Timer/TimerObject.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CTimerObject::MAPTIMEROBJECTS CTimerObject::s_mapTimerObjects;
CTimerObject::MAPTIMEROBJECTS CTimerObject::s_mapWaitTimerObjects;

Core::Utility::CTimerObject::CTimerObject()
: m_bIsAdded(false)
{
}

CTimerObject::~CTimerObject()
{
	ClearTimer();

	MAPTIMEROBJECTS::iterator timerIter = s_mapTimerObjects.find(this);
	if (timerIter != s_mapTimerObjects.end())
	{
		timerIter->second = false;
	}

	MAPTIMEROBJECTS::iterator waitTimerIter = s_mapWaitTimerObjects.find(this);
	if (waitTimerIter != s_mapWaitTimerObjects.end())
	{
		waitTimerIter->second = false;
	}
	
}

//Should be derived
void CTimerObject::OnTimer( uint32 unTimerID )
{
}

void CTimerObject::SetTimer( uint32 unTimerID, uint32 unInterval )
{
	if (unInterval == 0)
	{
		StopTimer(unTimerID);
		return;
	}

	STimer* pTimer = FindTimer(unTimerID);
	if (pTimer == NULL)
	{
		pTimer = new STimer;
		pTimer->m_unTimerID = unTimerID;
		pTimer->m_bActive = true;
		m_timerList.push_back(pTimer);
	}

	pTimer->m_unInterval = unInterval;
	pTimer->m_bValid = true;
	pTimer->m_unLastCallTime = static_cast<uint32>(::time(NULL));

	if (!m_bIsAdded)
	{
		AddTimerObject(this);
	}
}

bool CTimerObject::ActiveTimer( uint32 unTimerID, bool bActive )
{
	STimer* pTimer = FindTimer(unTimerID);
	if (pTimer == NULL)
	{
		return false;
	}

	pTimer->m_bActive = bActive;

	return true;
}

bool CTimerObject::StopTimer( uint32 unTimerID )
{
	STimer* pTimer = FindTimer(unTimerID);
	if (pTimer == NULL)
	{
		return false;
	}

	pTimer->m_bValid = false;

	return true;
}

void CTimerObject::StopAllTimer()
{
	std::list<STimer*>::iterator iter = m_timerList.begin();
	for (;iter != m_timerList.end(); ++iter)
	{
		(*iter)->m_bValid = false;
	}
}

void CTimerObject::ClearTimer()
{
	std::list<STimer*>::iterator iter = m_timerList.begin();
	for (;iter != m_timerList.end(); ++iter)
	{
		delete *iter;
	}

	m_timerList.clear();
}

uint32 CTimerObject::UpdateTimer( uint32 unNow )
{
	if (m_timerList.size() == 0)
	{
		return 0;
	}

	uint32 unValid = 0;
	STimer* pTimer = NULL;
	std::list<STimer*>::iterator iter = m_timerList.begin();
	for (; iter != m_timerList.end();)
	{
		pTimer = *iter;
		if (!pTimer->m_bValid)
		{
			iter = m_timerList.erase(iter);
			delete pTimer;
			continue;
		}

		if (!pTimer->m_bActive)
		{
			++iter;
			continue;
		}

		++unValid;

		if (unNow > pTimer->m_unLastCallTime && unNow - pTimer->m_unLastCallTime >= pTimer->m_unInterval)
		{
			OnTimer(pTimer->m_unTimerID);
			pTimer->m_unLastCallTime = unNow;
		}

		++iter;
		continue;
	}

	return unValid;
}

STimer* CTimerObject::FindTimer( uint32 unTimerID )
{
	std::list<STimer*>::iterator iter = m_timerList.begin();
	for (; iter != m_timerList.end(); ++iter)
	{
		if ((*iter)->m_unTimerID == unTimerID)
		{
			return *iter;
		}
	}

	return NULL;
}

bool CTimerObject::CheckTimerValid( uint32 unTimerID )
{
	STimer* pTimer = FindTimer(unTimerID);
	if (pTimer == NULL)
	{
		return false;
	}

	return pTimer->m_bValid;
}

bool CTimerObject::CheckTimerActive( uint32 unTimerID )
{
	STimer* pTimer = FindTimer(unTimerID);
	if (pTimer == NULL)
	{
		return false;
	}

	return pTimer->m_bActive;
}

void CTimerObject::AddTimerObject( CTimerObject* pTimerObject )
{
	s_mapWaitTimerObjects[pTimerObject] = true;

	pTimerObject->m_bIsAdded = true;
}

void CTimerObject::UpdateTimerObject()
{
	MAPTIMEROBJECTS::iterator waitIter = s_mapWaitTimerObjects.begin();
	for (; waitIter != s_mapWaitTimerObjects.end(); ++waitIter)
	{
		if (waitIter->second)
		{
			s_mapTimerObjects[waitIter->first] = true;
		}
	}

	s_mapWaitTimerObjects.clear();

	uint32 unNow = static_cast<uint32>(::time(NULL));
	MAPTIMEROBJECTS::iterator timerIter = s_mapTimerObjects.begin();
	for (; timerIter != s_mapTimerObjects.end();)
	{
		if (!timerIter->second)
		{
			timerIter = s_mapTimerObjects.erase(timerIter);
			continue;
		}

		CTimerObject* pObject = static_cast<CTimerObject*>(timerIter->first);
		if (pObject->UpdateTimer(unNow) <= 0)
		{
			timerIter = s_mapTimerObjects.erase(timerIter);
			pObject->m_bIsAdded = false;
			continue;
		}

		++timerIter;
		continue;
	}
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)