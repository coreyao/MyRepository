#ifndef COMMON_GROUPDATA_H__INCLUDED
#define COMMON_GROUPDATA_H__INCLUDED

#include "XString/XString.h"
#include "Network/Packet/DynamicPacket.h"

using Core::Utility::uint32;
using Core::Utility::XString;
using Core::Utility::CDynamicPacket;

struct SGroupMemeber
{
public:
	SGroupMemeber()
		: m_unMemeberID(0)
		, m_bIsOnline(false)
	{
	}

	void Serialize(CDynamicPacket& ar)
	{
		ar << m_unMemeberID;
		ar << m_strName;
	}

	void SerializeClient(CDynamicPacket& ar)
	{
		ar << m_unMemeberID;
		ar << m_strName;
		ar << m_bIsOnline;
	}

	uint32 m_unMemeberID;
	XString m_strName;
	bool m_bIsOnline;
};

struct SGroupData
{
public:
	typedef std::map<uint32, SGroupMemeber> MAPGROUPMEMBERS;

	enum
	{
		EOPER_JoinGroup,
		EOPER_LeaveGroup,
	};

public:
	SGroupData()
		: m_unGroupID(0)
	{
	}

	virtual ~SGroupData()
	{
	}

	void Serialize(CDynamicPacket& ar)
	{
		ar << m_unGroupID;
		ar << m_strGroupName;
		ar << m_mapMembers;
	}

	void SerializeClient(CDynamicPacket& ar)
	{
		ar << m_unGroupID;
		ar << m_strGroupName;
		
		if (ar.IsReading())
		{
			m_mapMembers.clear();
			uint16 len = 0;
			ar << len;

			for (uint16 i = 0; i < len; ++i)
			{
				SGroupMemeber value;
				value.SerializeClient(ar);
				m_mapMembers.insert(std::make_pair(value.m_unMemeberID, value));
			}
		}
		else
		{
			uint16 len = static_cast<uint16>(m_mapMembers.size());
			ar << len;

			std::map<uint32, SGroupMemeber>::iterator iter = m_mapMembers.begin();
			for (; iter != m_mapMembers.end(); ++iter)
			{
				iter->second.SerializeClient(ar);
			}
		}
	}

	void JoinGroup(const SGroupMemeber& groupMember)
	{
		SGroupMemeber* pMember = GetMember(groupMember.m_unMemeberID);
		if (pMember != NULL)
		{
			*pMember = groupMember;
		}
		else
		{
			m_mapMembers.insert(std::make_pair(groupMember.m_unMemeberID, groupMember));
		}
	}

	void LeaveGroup(const SGroupMemeber& groupMember)
	{
		SGroupMemeber* pMember = GetMember(groupMember.m_unMemeberID);
		if (pMember != NULL)
		{
			m_mapMembers.erase(pMember->m_unMemeberID);
		}
	}

	SGroupMemeber* GetMember(uint32 unPlayerID)
	{
		MAPGROUPMEMBERS::iterator iter = m_mapMembers.find(unPlayerID);
		if (iter != m_mapMembers.end())
		{
			return &iter->second;
		}
		else
		{
			return NULL;
		}
	}

	uint32 m_unGroupID;
	XString m_strGroupName;
	MAPGROUPMEMBERS m_mapMembers;
};

struct SGroups
{
public:
	typedef std::map<uint32, SGroupData> MAPGROUPS;

	enum
	{
		EOPER_AddGroup,
		EOPER_DelGroup,
	};

public:
	void Serialize(CDynamicPacket& ar)
	{
		ar << m_mapGroups;
	}

	void SerializeClient(CDynamicPacket& ar)
	{
		if (ar.IsReading())
		{
			m_mapGroups.clear();
			uint16 len = 0;
			ar << len;

			for (uint16 i = 0; i < len; ++i)
			{
				SGroupData temp;
				temp.SerializeClient(ar);
				m_mapGroups.insert(std::make_pair(temp.m_unGroupID, temp));
			}
		}
		else
		{
			uint16 len = static_cast<uint16>(m_mapGroups.size());
			ar << len;

			MAPGROUPS::iterator iter = m_mapGroups.begin();
			for (; iter != m_mapGroups.end(); ++iter)
			{
				iter->second.SerializeClient(ar);
			}
		}
	}

	SGroupData* GetGroup(uint32 groupID)
	{
		MAPGROUPS::iterator iter = m_mapGroups.find(groupID);
		if (iter != m_mapGroups.end())
		{
			return &iter->second;
		}

		return NULL;
	}

	void AddGroup(const SGroupData& groupData)
	{
		SGroupData* pGroup = GetGroup(groupData.m_unGroupID);
		if (pGroup != NULL)
		{
			*pGroup = groupData;
		}
		else
		{
			m_mapGroups.insert(std::make_pair(groupData.m_unGroupID, groupData));
		}
	}

	void DelGroup(const SGroupData& groupData)
	{
		SGroupData* pGroup = GetGroup(groupData.m_unGroupID);
		if (pGroup != NULL)
		{
			m_mapGroups.erase(pGroup->m_unGroupID);
		}
	}

	MAPGROUPS m_mapGroups;
};

#endif
