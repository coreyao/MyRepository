#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_CHATHISTORYMANAGER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_CHATHISTORYMANAGER_H__INCLUDED

#include "SqliteWrapper/SQLiteWrapper.h"
#include "Network/Packet/DynamicPacket.h"
#include "XString/XString.h"
#include "CommonDataStruct/ChatData_Struct.h"
#include "Singleton.h"

using Core::Utility::CDynamicPacket;
using Core::Utility::XString;
using Core::Utility::CSQLiteWrapper;
using Core::Utility::TSingleton;
using Core::Utility::CSQLiteStatement;

struct SChatHistoryData
{
	void Serialize(CDynamicPacket& ar)
	{
		ar << m_strContent;
	}

	uint32 m_unTime;
	uint32 m_unSenderID;
	XString m_strSenderName;
	uint32 m_unTargetID;
	bool m_bIslocalSent;
	uint8 m_unType;
	XString m_strContent;
};

class CChatHistoryManager
{
public:
	typedef std::list<SChatHistoryData> LSTHISTORYLIST;
	typedef std::map<uint32, LSTHISTORYLIST> MAPHISTORYDATA;

public:
	CChatHistoryManager();
	~CChatHistoryManager();

	void LoadHistory();
	void Save(SChatData& chatData, bool bIsLocalSent);
	LSTHISTORYLIST* GetPlayerHistory(uint32 unPlayerID); 
	LSTHISTORYLIST* GetGroupHistory(uint32 unGroupID);

protected:
	void SaveHistory(SChatHistoryData& historyData);
	void LoadFriendsChatHistory();
	void LoadGroupsChatHistory();

protected:
	MAPHISTORYDATA m_mapFriendsHistoryData;
	MAPHISTORYDATA m_mapGroupsHistoryData;
	CSQLiteWrapper m_sqliteWrapper;
};


#endif