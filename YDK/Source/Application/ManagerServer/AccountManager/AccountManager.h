#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_ACCOUNTMANAGER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_ACCOUNTMANAGER_H__INCLUDED

#include <map>
#include "XString/XString.h"
#include "MsgDef.h"

using Core::Utility::XString;
using Core::Utility::uint32;
using Core::Utility::uint8;

struct SAccount
{
	SAccount()
		: m_unAccountID(0)
		, m_isOnline(false)
	{
		memset(m_account, 0, MAX_ACCOUNT_LEN);
	}

	uint32 m_unAccountID;
	char m_account[MAX_ACCOUNT_LEN];
	uint32 m_unGateID;
	uint32 m_unChallengeID;
	uint8 m_isOnline;
};

class CAccountManager
{
	enum
	{
		EAccountState_Online,
		EAccountState_Offline,
	};
	
public:
	CAccountManager();
	~CAccountManager();

	void OnJoin(uint32 accountID, char* account, uint32 gateID, uint32 challengeID);
	void OnLeave(uint32 accountID);
	SAccount* GetAccount(uint32 accountID);
	SAccount* GetAccount(const XString& strAccount);

protected:
	std::map<uint32, SAccount*> m_mapAccounts;
};


#endif