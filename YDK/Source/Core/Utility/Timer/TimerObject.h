#ifndef CORE_UTILITY_TIMEROBJECT_H__INCLUDED
#define CORE_UTILITY_TIMEROBJECT_H__INCLUDED

#include <map>
#include <list>
#include <ctime>

#include "BasicDefines.h"
#include "UtilityTypes.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

struct STimer
{
	bool m_bValid;
	bool m_bActive;
	uint32 m_unTimerID;
	uint32 m_unInterval;
	uint32 m_unLastCallTime;
};

class CTimerObject
{
public:
	typedef std::map<CTimerObject*, bool> MAPTIMEROBJECTS;

public:
	CTimerObject();
	virtual ~CTimerObject();

	virtual void OnTimer(uint32 unTimerID);
	void SetTimer(uint32 unTimerID, uint32 unInterval);
	bool ActiveTimer(uint32 unTimerID, bool bActive);
	bool StopTimer(uint32 unTimerID);
	void StopAllTimer();
	void ClearTimer();
	uint32 UpdateTimer(uint32 unNow);
	STimer* FindTimer(uint32 unTimerID);
	bool CheckTimerValid(uint32 unTimerID);
	bool CheckTimerActive(uint32 unTimerID);

	static void UpdateTimerObject();
	static void AddTimerObject(CTimerObject* pTimerObject);

protected:
	std::list<STimer*> m_timerList;
	bool m_bIsAdded;
	static MAPTIMEROBJECTS s_mapTimerObjects;
	static MAPTIMEROBJECTS s_mapWaitTimerObjects;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif
