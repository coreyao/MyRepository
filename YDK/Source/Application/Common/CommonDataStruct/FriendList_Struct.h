#ifndef COMMON_FRIENDLIST_H__INCLUDED
#define COMMON_FRIENDLIST_H__INCLUDED

#include <vector>
#include "XString/XString.h"
#include "Network/Packet/DynamicPacket.h"
#include "LogSystem.h"

using Core::Utility::XString;
using Core::Utility::uint32;
using Core::Utility::CDynamicPacket;

#define INVALID_FRIEND_GROUP_ID 0xffffffff

struct SFriendGroupData
{
	SFriendGroupData()
		: m_unFriendGroupID(1)
	{
	}

	void Serialize(CDynamicPacket& ar)
	{
		ar << m_unFriendGroupID;
		ar << m_strFriendGroupName;
	}

	uint8 m_unFriendGroupID;
	XString m_strFriendGroupName;
};

struct SRelationData
{
	SRelationData()
		: m_unCharactorID(0)
		, m_unFriendGroupID(0)
		, m_bIsOnline(false)
	{
	}

	virtual ~SRelationData()
	{

	}

	void Serialize(CDynamicPacket& ar)
	{
		ar << m_unCharactorID;
		ar << m_strNickname;
		ar << m_unFriendGroupID;
	}

	void SerializeClient(CDynamicPacket& ar)
	{
		ar << m_unCharactorID;
		ar << m_strNickname;
		ar << m_bIsOnline;
		ar << m_unFriendGroupID;
	}

	uint32 m_unCharactorID;
	XString m_strNickname;
	bool m_bIsOnline;
	uint8 m_unFriendGroupID;
};

struct SRelation
{
public:
	enum
	{
		EOPER_AddRelation,
		EOPER_DelRelation,
		EOPER_AddFriendGroup,
		EOPER_DelFriendGroup,
	};

public:
	SRelation()
	{
	}

	void AddRelation(const SRelationData& relationData)
	{
		SRelationData* pRelationData = FindRelation(relationData.m_unCharactorID);
		if (pRelationData != NULL)
		{
			*pRelationData = relationData;
			Core::LOG(XString().Format(TEXT("%s is %s"), relationData.m_strNickname.c_str()
				, relationData.m_bIsOnline ? TEXT("Online") : TEXT("Offline")).c_str());
		}
		else
		{
			m_mapRelations.insert(std::make_pair(relationData.m_unCharactorID, relationData));
		}
	}

	void DelRelation(const SRelationData& relationData)
	{
		SRelationData* pRelationData = FindRelation(relationData.m_unCharactorID);
		if (pRelationData != NULL)
		{
			m_mapRelations.erase(pRelationData->m_unCharactorID);
		}
	}

	SRelationData* FindRelation(uint32 unPlayerID)
	{
		std::map<uint32, SRelationData>::iterator iter = m_mapRelations.find(unPlayerID);
		if (iter != m_mapRelations.end())
		{
			return &iter->second;
		}

		return NULL;
	}

	void AddFriendGroup(const SFriendGroupData& friendGroupData)
	{
		m_mapFriendGroups.insert(std::make_pair(friendGroupData.m_unFriendGroupID, friendGroupData));
	}

	void DelFriendGroup(const SFriendGroupData& friendGroupData)
	{
		m_mapFriendGroups.erase(friendGroupData.m_unFriendGroupID);
	}

	SFriendGroupData* FindFriendGroup(uint32 unFriendGroupID)
	{
		std::map<uint32, SFriendGroupData>::iterator iter = m_mapFriendGroups.find(unFriendGroupID);
		if (iter != m_mapFriendGroups.end())
		{
			return &iter->second;
		}

		return NULL;
	}

	std::vector<uint32> GetPlayersInFriendGroup(uint32 unFriendGroupID)
	{
		std::vector<uint32> vecResult;
		std::map<uint32, SRelationData>::iterator iter = m_mapRelations.begin();
		for (; iter != m_mapRelations.end(); ++iter)
		{
			if (iter->second.m_unFriendGroupID == unFriendGroupID)
			{
				vecResult.push_back(iter->first);
			}
		}

		return vecResult;
	}

	void Serialize(CDynamicPacket& ar)
	{
		ar << m_mapRelations;
		ar << m_mapFriendGroups;
	}

	void SerializeClient(CDynamicPacket& ar)
	{
		if (ar.IsReading())
		{
			m_mapRelations.clear();
			uint16 len = 0;
			ar << len;

			for (uint16 i = 0; i < len; ++i)
			{
				SRelationData value;
				value.SerializeClient(ar);
				m_mapRelations.insert(std::make_pair(value.m_unCharactorID,value));
			}
			
			ar << m_mapFriendGroups;
		}
		else
		{
			uint16 len = static_cast<uint16>(m_mapRelations.size());
			ar << len;

			std::map<uint32, SRelationData>::iterator iter = m_mapRelations.begin();
			for (; iter != m_mapRelations.end(); ++iter)
			{
				iter->second.SerializeClient(ar);
			}

			ar << m_mapFriendGroups;
		}
	}

	std::map<uint32, SRelationData> m_mapRelations;
	std::map<uint32, SFriendGroupData> m_mapFriendGroups;
};

#endif