#include "DBServer.h"
#include "Network/ManagerServerListener.h"
#include "LogSystem.h"
#include "Network/Packet/DynamicPacket.h"
#include "msini/CMSINIReader.h"

using Core::Utility::TSingleton;
using Core::Utility::CSQLiteStatement;
using Core::Utility::CDynamicPacket;
using Core::Utility::CMSINIReader;

#define DBSERVER_IP "127.0.0.1"
#define DBSERVER_PORT 23000
#define DBSERVER_MAX_CONNECTIONS 10

#define DB_FILE_NAME		TEXT(".\\chatdb.db")
#define DB_TABLE_ACCOUNT	TEXT("TBAccount")
#define DB_TABLE_PLAYER		TEXT("TBPlayer")
#define DB_TABLE_GROUP		TEXT("TBGroup")

CDBServer::CDBServer()
{
	if (m_sqliteWrapper.Open(DB_FILE_NAME))
	{
		Core::LOG(TEXT("DB File Loaded..."));
	}
	else
	{
		Core::LOG(TEXT("Failed to load DB File..."));
	}
}

CDBServer::~CDBServer()
{
	if (m_sqliteWrapper.Close())
	{
		Core::LOG(TEXT("DB File Closed..."));
	}
	else
	{
		Core::LOG(TEXT("Failed to close DB File..."));
	}
}

void CDBServer::Init()
{
	//Load Configuration File
	TSingleton<CMSINIReader>::GetInstance().SetFileName(TEXT(".\\ServerConfig.ini"));
	XString DBServerIP = TSingleton<CMSINIReader>::GetInstance().GetFieldStringValue(TEXT("DBServer"), TEXT("DBServerIP"));
	int32 DBServerPort = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("DBServer"), TEXT("DBServerPort"));
	uint32 DBServerMaxConnections = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("DBServer"), TEXT("DBServerMaxConnections"));

	TSingleton<CManagerServerListener>::GetInstance().Init(static_cast<uint16>(DBServerPort), DBServerMaxConnections, DBServerIP);
	TSingleton<CManagerServerListener>::GetInstance().Start();
}

void CDBServer::Process()
{
	TSingleton<CManagerServerListener>::GetInstance().ProcessMsg();
}

bool CDBServer::OnRegisterAccount( const XString& strAccount, const XString& strPassword, const XString& strNickname)
{
	XString sqlStatement;
	sqlStatement.Format(TEXT("select * from %s where account='%s'"), DB_TABLE_ACCOUNT, strAccount.c_str());

	//check if this account has already been registered
	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL && stmt->NextRow())
	{
		return false;
	}

	//insert data into account table
	sqlStatement.clear();
	sqlStatement.Format(TEXT("insert into %s values(null,'%s','%s')"), DB_TABLE_ACCOUNT
		, strAccount.c_str()
		, strPassword.c_str());
	stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt == NULL)
	{
		return false;
	}

	if (!stmt->Execute()) 
	{
		return false;
	}

	//insert data into player table
	sqlStatement.clear();
	sqlStatement.Format(TEXT("insert into %s values(%d,'%s',0,null,0,null)"), DB_TABLE_PLAYER
		, m_sqliteWrapper.GetLastInsertRowID()
		, strNickname.c_str());
	stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt == NULL)
	{
		return false;
	}

	if (!stmt->Execute()) 
	{
		return false;
	}

	return true;
}

uint32 CDBServer::OnPlayerLoginCheck( const XString& strAccount, const XString& strPassword )
{
	XString sqlStatement;
	sqlStatement.Format(TEXT("select * from %s where account='%s' and password='%s'"), DB_TABLE_ACCOUNT, 
		strAccount.c_str(), strPassword.c_str());

	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		if (stmt->NextRow())
		{
			return static_cast<uint32>(stmt->ValueInt(0));
		}
	}

	return INVALID_ACCOUNT_ID;
}

bool CDBServer::OnPlayerEnterWorld(uint32 playerID, SPlayerBaseInfo& baseInfo 
								   , uint32& friendsInfoLen, char friendsInfo[MAX_RELATIONDATA_LEN]
								   , uint32& groupsInfoLen, char groupsInfo[MAX_GROUPDATA_LEN])
{
	XString sqlStatement;
	sqlStatement.Format(TEXT("select * from %s where playerid=%d"), DB_TABLE_PLAYER, playerID);

	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL && stmt->NextRow())
	{
		memcpy(baseInfo.m_nickName, stmt->ValueString(1).c_str(), stmt->ValueString(1).size());

		friendsInfoLen = static_cast<uint32>(stmt->ValueInt(2));
		memcpy(friendsInfo, stmt->ValueBlob(3), friendsInfoLen < MAX_RELATIONDATA_LEN ? friendsInfoLen : MAX_RELATIONDATA_LEN);

		groupsInfoLen = static_cast<uint32>(stmt->ValueInt(4));
		memcpy(groupsInfo, stmt->ValueBlob(5), groupsInfoLen < MAX_GROUPDATA_LEN ? groupsInfoLen : MAX_GROUPDATA_LEN);

		return true;
	}

	return false;
}


bool CDBServer::OnSavePlayerRelation( uint32 playerID, uint32 friendsInfoLen, const char* friendsInfo )
{
	XString sqlStatement;
	sqlStatement.Format(TEXT("update %s set friendsInfoLen=?,friendsInfo=? where playerid=%d"), DB_TABLE_PLAYER, playerID);

	bool result = true;
	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		if (stmt->BindInt(0, friendsInfoLen))
		{
			Core::LOG(TEXT("pos 0 bind success"));
		}
		else
		{
			Core::LOG(TEXT("pos 0 bind failed"));
			result = false;
		}

		if (stmt->BindBlob(1, friendsInfo, friendsInfoLen))
		{
			Core::LOG(TEXT("pos 1 bind success"));
		}
		else
		{
			Core::LOG(TEXT("pos 1 bind failed"));
			result = false;
		}

		if (stmt->Execute()) 
		{
			Core::LOG(TEXT("update success"));
		}
		else
		{
			Core::LOG(TEXT("update failed"));
			result = false;
		}
	}

	return result;
}

bool CDBServer::OnLoadGroupsDataInfo(std::vector<SGroupData>& vecGroups)
{
	XString sqlStatement;
	sqlStatement.Format(TEXT("select * from %s"), DB_TABLE_GROUP);

	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		while (stmt->NextRow())
		{
			SGroupData groupData;
			CDynamicPacket cPacket(const_cast<char*>(stmt->ValueBlob(3)), stmt->ValueInt(2), CDynamicPacket::OP_READ);
			cPacket << groupData;
			groupData.m_unGroupID = stmt->ValueInt(0);

			vecGroups.push_back(groupData);
		}

		return true;
	}

	return false;
}

bool CDBServer::OnReqCreateGroup( const XString& groupName, SGroupData& groupData )
{
	groupData.m_strGroupName = groupName;
	CDynamicPacket cPacket(1, CDynamicPacket::OP_WRITE);
	cPacket << groupData;

	XString sqlStatement;
	sqlStatement.Format(TEXT("insert into %s values(null,'%s',?,?)"), DB_TABLE_GROUP, groupName.c_str());
	bool result = true;
	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		if (stmt->BindInt(0, cPacket.GetDataSize()))
		{
			Core::LOG(TEXT("pos 0 bind success"));
		}
		else
		{
			Core::LOG(TEXT("pos 0 bind failed"));
			result = false;
		}

		if (stmt->BindBlob(1, cPacket.GetData(), cPacket.GetDataSize()))
		{
			Core::LOG(TEXT("pos 1 bind success"));
		}
		else
		{
			Core::LOG(TEXT("pos 1 bind failed"));
			result = false;
		}

		if (stmt->Execute()) 
		{
			groupData.m_unGroupID = m_sqliteWrapper.GetLastInsertRowID();
			Core::LOG(TEXT("update success"));
		}
		else
		{
			Core::LOG(TEXT("update failed"));
			result = false;
		}
	}

	return result;
}

bool CDBServer::OnSavePlayerGroups( uint32 playerID, uint32 groupsInfoLen, const char* groupsInfo )
{
	XString sqlStatement;
	sqlStatement.Format(TEXT("update %s set groupsInfoLen=?,groupsInfo=? where playerid=%d"), DB_TABLE_PLAYER, playerID);

	bool result = true;
	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		if (stmt->BindInt(0, groupsInfoLen))
		{
			Core::LOG(TEXT("pos 0 bind success"));
		}
		else
		{
			Core::LOG(TEXT("pos 0 bind failed"));
			result = false;
		}

		if (stmt->BindBlob(1, groupsInfo, groupsInfoLen))
		{
			Core::LOG(TEXT("pos 1 bind success"));
		}
		else
		{
			Core::LOG(TEXT("pos 1 bind failed"));
			result = false;
		}

		if (stmt->Execute()) 
		{
			Core::LOG(TEXT("update success"));
		}
		else
		{
			Core::LOG(TEXT("update failed"));
			result = false;
		}
	}

	return result;
}

bool CDBServer::OnSaveGroupsInfo( uint32 groupID, uint32 groupsInfoLen, const char* groupsInfo )
{
	XString sqlStatement;
	sqlStatement.Format(TEXT("update %s set groupsInfoLen=?,groupsInfo=? where groupid=%d"), DB_TABLE_GROUP, groupID);

	bool result = true;
	CSQLiteStatement* stmt = m_sqliteWrapper.Statement(sqlStatement);
	if (stmt != NULL)
	{
		if (stmt->BindInt(0, groupsInfoLen))
		{
			Core::LOG(TEXT("pos 0 bind success"));
		}
		else
		{
			Core::LOG(TEXT("pos 0 bind failed"));
			result = false;
		}

		if (stmt->BindBlob(1, groupsInfo, groupsInfoLen))
		{
			Core::LOG(TEXT("pos 1 bind success"));
		}
		else
		{
			Core::LOG(TEXT("pos 1 bind failed"));
			result = false;
		}

		if (stmt->Execute()) 
		{
			Core::LOG(TEXT("update success"));
		}
		else
		{
			Core::LOG(TEXT("update failed"));
			result = false;
		}
	}

	return result;
}