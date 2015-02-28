#include "AccountManager/AccountManager.h"
#include "LogSystem.h"

CAccountManager::CAccountManager()
{
	
}

CAccountManager::~CAccountManager()
{

}

void CAccountManager::OnJoin( uint32 accountID, char* account, uint32 gateID, uint32 challengeID)
{
	std::map<uint32, SAccount*>::iterator iter = m_mapAccounts.find(accountID);
	if (iter == m_mapAccounts.end())
	{
		SAccount* pAccount = new SAccount;
		pAccount->m_unAccountID = accountID;
		pAccount->m_unGateID = gateID;
		pAccount->m_unChallengeID = challengeID;
		pAccount->m_isOnline = EAccountState_Online;
		memcpy(pAccount->m_account, account, MAX_ACCOUNT_LEN);
		m_mapAccounts[accountID] = pAccount;
		Core::LOG(XString().Format("Account %s is Logining...", pAccount->m_account).c_str());
	}
}

void CAccountManager::OnLeave( uint32 accountID )
{
	std::map<uint32, SAccount*>::iterator iter = m_mapAccounts.find(accountID);
	if (iter != m_mapAccounts.end())
	{
		XString account = iter->second->m_account;
		SAFE_DELETE(iter->second);
		m_mapAccounts.erase(iter);
		Core::LOG(XString().Format("Account %s is exiting...", account.c_str()).c_str());
	}
}

SAccount* CAccountManager::GetAccount( uint32 accountID )
{
	std::map<uint32, SAccount*>::iterator iter = m_mapAccounts.find(accountID);
	if (iter != m_mapAccounts.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

SAccount* CAccountManager::GetAccount( const XString& strAccount )
{
	std::map<uint32, SAccount*>::iterator iter = m_mapAccounts.begin();
	for (; iter != m_mapAccounts.end(); ++iter)
	{
		if (XString(iter->second->m_account) == strAccount)
		{
			return iter->second;
		}
	}

	return NULL;
}
