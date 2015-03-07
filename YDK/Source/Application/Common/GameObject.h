#ifndef COMMON_GAMEOBJECT_H__INCLUDED
#define COMMON_GAMEOBJECT_H__INCLUDED

#include "Timer/TimerObject.h"

using Core::Utility::CTimerObject;

const uint32 BASE_TIMER = 0;
const uint32 GROUP_LOAD_TIMER = BASE_TIMER + 1;

const uint32 GROUP_LOAD_TIMER_INTERVAL = 1;

class CGameObject : public CTimerObject
{
public:
	CGameObject(){}
	virtual ~CGameObject(){}
};

#endif