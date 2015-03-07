#ifndef COMMON_PLAYERINFO_H__INCLUDED
#define COMMON_PLAYERINFO_H__INCLUDED

#include "XString/XString.h"
#include "CommonDataStruct/FriendList_Struct.h"
#include "CommonDataStruct/GroupData_Struct.h"

using Core::Utility::XString;
using Core::Utility::uint32;

#define MAX_NIKCNAME_LENGTH 15
#define MAX_GROUPNAME_LEN 15
#define MAX_RELATIONDATA_LEN 1024
#define MAX_GROUPDATA_LEN 1024
#define MAX_CHATHISTORYDATA_LEN 1024

struct SPlayerBaseInfo
{
	SPlayerBaseInfo()
	{
		memset(m_nickName, 0, MAX_NIKCNAME_LENGTH);
	}

	//void SerializeClient(CDynamicPacket& ar)
	//{
	//	if (ar.IsReading())
	//	{
	//		char* temp;
	//		ar.ReadString(&temp);
	//		strcpy_s(m_nickName, temp);
	//	}
	//	else
	//	{
	//		ar.WriteString(m_nickName);
	//	}
	//}

	char m_nickName[MAX_NIKCNAME_LENGTH];
};

//struct SPlayerInfo
//{
//	//void SerializeClient(CDynamicPacket& ar)
//	//{
//	//	m_baseInfo.SerializeClient(ar);
//	//	m_relation.SerializeClient(ar);
//	//	m_groups.SerializeClient(ar);
//	//}
//
//	SPlayerBaseInfo m_baseInfo;
//	SRelation m_relation;
//	SGroups m_groups;
//};

#endif