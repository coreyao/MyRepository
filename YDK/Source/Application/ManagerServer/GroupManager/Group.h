#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_GROUP_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_GROUP_H__INCLUDED

#include "CommonDataStruct/GroupData_Struct.h"

class CGroup : public SGroupData
{
public:
	CGroup();
	~CGroup();

	void UpdateGroupMember(SGroupMemeber& groupMember, uint8 oper = SGroupData::EOPER_JoinGroup);
	void UpdateGroup(uint32 memberID = 0, uint8 oper = SGroups::EOPER_AddGroup);
	void SaveGroup();

	void SendMsgToAllMembers(SMsg* pMsg);
};

#endif