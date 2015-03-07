#include "ChatHistoryManager.h"
#include "LogSystem.h"
#include "ClientPlayer.h"

#define DB_HISTORY_FILE_NAME	TEXT(".\\historydb.db")
#define DB_TABLE_HISTORY		TEXT("TBHistory")

CChatHistoryManager::CChatHistoryManager()
{
	if (m_sqliteWrapper.Open(DB_HISTORY_FILE_NAME))
	{
		Core::LOG(TEXT("History DB File Loaded..."));
	}
	else
	{
		Core::LOG(TEXT("Failed to load History DB File..."));
	}
}

CChatHistoryManager::~CChatHistoryManager()
{
	if (m_sqliteWrapper.Close())
	{
		Core::LOG(TEXT("History DB File Closed..."));
	}
	else
	{
		Core::LOG(TEXT("Failed to close History DB File..."));
	}
}

void CChatHistoryManager::LoadHistory()
{
	LoadFriendsChatHistory();
	LoadGroupsChatHistory();
}

void CChatHistoryManager::Save(SChatData& chatData, bool bIsLocalSent)
{
	SChatHistoryData historyData;
	historyData.m_unType = chatData.m_unChannel;
	historyData.m_unTime = chatData.m_unTime;
	historyData.m_unSenderID = chatData.m_unSenderID;
	historyData.m_strSenderName = chatData.m_strSenderName;
	historyData.m_unTargetID = chatData.m_unTargetID;
	historyData.m_bIslocalSent = bIsLocalSent;
	historyData.m_strContent = chatData.m_strContent;

	uint8 type = chatData.m_unChannel;
	if (type == EChannel_Single)
	{
		uint32 playerID = bIsLocalSent ? historyData.m_unTargetID : historyData.m_unSenderID;
		MAPHISTORYDATA::iterator historyMapIter = m_mapFriendsHistoryData.find(playerID);
		if (historyMapIter != m_mapFriendsHistoryData.end())
		{
			historyMapIter->second.push_back(historyData);
		}
		else
		{
			LSTHISTORYLIST lstHistoryList;
			lstHistoryList.push_back(historyData);
			m_mapFriendsHistoryData.insert(std::make_pair(playerID, lstHistoryList));
		}

	}
	else if (type == EChannel_Group)
	{
		MAPHISTORYDATA::iterator historyMapIter = m_mapGroupsHistoryData.find(historyData.m_unTargetID);
		if (historyMapIter != m_mapGroupsHistoryData.end())
		{
			historyMapIter->second.push_back(historyData);
		}
		else
		{
			LSTHISTORYLIST lstHistoryList;
			lstHistoryList.push_back(historyData);
			m_mapGroupsHistoryData.insert(std::make_pair(historyData.m_unTargetID, lstHistoryList));
		}
	}

	SaveHistory(historyData);
}

void CChatHistoryManager::SaveHistory( SChatHistoryData& historyData )
{
	CDynamicPacket cPacket(1, CDynamicPacket::OP_WRITE);
	cPacket << historyData;

	XString sqlStatement;
	sqlStatement.Format(TEXT("insert into %s values(null,%d,%d,'%s',%d,%d,%d,%d,?)"), DB_TABLE_HISTORY
						, historyData.m_unTime
						, historyData.m_unSenderID
						, historyData.m_strSenderName.c_str()
						, historyData.m_unTargetID
						, static_cast<int>(historyData.m_bIslocalSent)
						, historyData.m_unType
						, cPacket.GetDataSize());

	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		if (stmt->BindBlob(0, cPacket.GetData(), cPacket.GetDataSize()))
		{
			Core::LOG(TEXT("bind history success"));
		}
		else
		{
			Core::LOG(TEXT("bind history failed"));
		}

		if (stmt->Execute()) 
		{
			Core::LOG(TEXT("update history success"));
		}
		else
		{
			Core::LOG(TEXT("update history failed"));
		}
	}
}

void CChatHistoryManager::LoadFriendsChatHistory()
{
	//load friends chat history
	XString sqlStatement;
	sqlStatement.Format(TEXT("select * from %s where type=%d"), DB_TABLE_HISTORY, EChannel_Single);
	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		while (stmt->NextRow())
		{
			SChatHistoryData historyData;
			historyData.m_unTime = stmt->ValueInt(1);
			historyData.m_unSenderID = stmt->ValueInt(2);
			historyData.m_strSenderName = stmt->ValueString(3);
			historyData.m_unTargetID = stmt->ValueInt(4);
			historyData.m_bIslocalSent = stmt->ValueInt(5) == 1 ? true : false;
			historyData.m_unType = stmt->ValueInt(6);
			CDynamicPacket tempPacket(const_cast<char*>(stmt->ValueBlob(8)), stmt->ValueInt(7));
			tempPacket << historyData;

			uint32 playerID = historyData.m_bIslocalSent ? historyData.m_unTargetID : historyData.m_unSenderID;
			MAPHISTORYDATA::iterator historyMapIter = m_mapFriendsHistoryData.find(playerID);
			if (historyMapIter != m_mapFriendsHistoryData.end())
			{
				historyMapIter->second.push_back(historyData);
			}
			else
			{
				LSTHISTORYLIST lstHistoryList;
				lstHistoryList.push_back(historyData);
				m_mapFriendsHistoryData.insert(std::make_pair(playerID, lstHistoryList));
			}
		}
	}
}

void CChatHistoryManager::LoadGroupsChatHistory()
{
	//load group chat history
	XString sqlStatement;
	sqlStatement.Format(TEXT("select * from %s where type=%d"), DB_TABLE_HISTORY, EChannel_Group);

	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		while (stmt->NextRow())
		{
			SChatHistoryData historyData;
			historyData.m_unTime = stmt->ValueInt(1);
			historyData.m_unSenderID = stmt->ValueInt(2);
			historyData.m_strSenderName = stmt->ValueString(3);
			historyData.m_unTargetID = stmt->ValueInt(4);
			historyData.m_bIslocalSent = stmt->ValueInt(5) == 1 ? true : false;
			historyData.m_unType = stmt->ValueInt(6);
			CDynamicPacket tempPacket(const_cast<char*>(stmt->ValueBlob(8)), stmt->ValueInt(7));
			tempPacket << historyData;

			MAPHISTORYDATA::iterator historyMapIter = m_mapGroupsHistoryData.find(historyData.m_unTargetID);
			if (historyMapIter != m_mapGroupsHistoryData.end())
			{
				historyMapIter->second.push_back(historyData);
			}
			else
			{
				LSTHISTORYLIST lstHistoryList;
				lstHistoryList.push_back(historyData);
				m_mapGroupsHistoryData.insert(std::make_pair(historyData.m_unTargetID, lstHistoryList));
			}
		}
	}
}

CChatHistoryManager::LSTHISTORYLIST* CChatHistoryManager::GetPlayerHistory(uint32 unPlayerID)
{
	MAPHISTORYDATA::iterator iter = m_mapFriendsHistoryData.find(unPlayerID);
	if (iter != m_mapFriendsHistoryData.end())
	{
		return &iter->second;
	}
	else
	{
		return NULL;
	}
}

CChatHistoryManager::LSTHISTORYLIST* CChatHistoryManager::GetGroupHistory(uint32 unGroupID)
{
	MAPHISTORYDATA::iterator iter = m_mapGroupsHistoryData.find(unGroupID);
	if (iter != m_mapGroupsHistoryData.end())
	{
		return &iter->second;
	}
	else
	{
		return NULL;
	}
}