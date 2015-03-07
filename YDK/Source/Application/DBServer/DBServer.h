#ifndef CORE_UTILITY_APPLICATION_DBSERVER_DBSERVER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_DBSERVER_DBSERVER_H__INCLUDED

#include "Singleton.h"
#include "Network/ThreadServer.h"
#include "SqliteWrapper/SQLiteWrapper.h"
#include "CommonDataStruct/PlayerInfo_Struct.h"

#define INVALID_ACCOUNT_ID 0xffffffff

using Core::Utility::CThreadServer;
using Core::Utility::TSingleton;
using Core::Utility::uint32;
using Core::Utility::XString;
using Core::Utility::CSQLiteWrapper;

class CDBServer : public CThreadServer
{
public:
	CDBServer();
	virtual ~CDBServer();

	void Init();
	virtual void Process();

	bool OnRegisterAccount(const XString& strAccount, const XString& strPassword, const XString& strNickname);
	uint32 OnPlayerLoginCheck(const XString& strAccount, const XString& strPassword);
	bool OnPlayerEnterWorld(uint32 playerID, SPlayerBaseInfo& baseInfo
		, uint32& friendsInfoLen, char friendsInfo[MAX_RELATIONDATA_LEN]
	    , uint32& groupsInfoLen, char groupsInfo[MAX_GROUPDATA_LEN]);
	bool OnSavePlayerRelation(uint32 playerID, uint32 friendsInfoLen, const char* friendsInfo);
	bool OnLoadGroupsDataInfo(std::vector<SGroupData>& vecGroups);
	bool OnReqCreateGroup(const XString& groupName, SGroupData& groupData);
	bool OnSavePlayerGroups(uint32 playerID, uint32 groupsInfoLen, const char* groupsInfo);
	bool OnSaveGroupsInfo(uint32 groupID, uint32 groupsInfoLen, const char* groupsInfo);

protected:
	uint32 m_lastAccountID;
	XString m_dbName;
	CSQLiteWrapper m_sqliteWrapper;
};

#endif