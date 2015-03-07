#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_CLIENTCMD_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_CLIENTCMD_H__INCLUDED

#include "Singleton.h"
#include "Network/ThreadServer.h"
#include "MsgDef.h"

using Core::Utility::CThreadServer;
using Core::Utility::TSingleton;
using Core::Utility::XString;
using Core::Utility::SMsgDynamic;

struct SChatData;
class CClientGUI : public CThreadServer
{
public:
	CClientGUI();
	virtual ~CClientGUI();

	void Init();
	virtual void Process();

	void OnReqLogin(const XString& strAccount, const XString& strPassword);
	bool OnAckLogin(MsgAckLoginCheck* pMsg);

	void OnReqEnterWorld();
	void OnAckEnterWorld(MsgAckEnterWorld* pMsg);
	void OnReqExitWorld();

	void OnReqRegister(const XString& strAccount, const XString& strPassword, const XString& strNickName);
	void OnAckRegister(MsgAckRegisterAccount* pMsg);

	void OnAckInitRelation(SMsgDynamic* pMsg);
	void OnAckUpdateRelation(SMsgDynamic* pMsg);
	void OnAckUpdateFriendGroup(SMsgDynamic* pMsg);
	void OnSendSingleChat(uint32 targetID, const XString& strContent, time_t sendTime);
	void OnRecvSingleChat(SChatData* pChatData);
	void OnAddFriend(const XString& strAccount);
	void OnDelFriend(uint32 unPlayerID);

	void OnAckInitGroups(SMsgDynamic* pMsg);
	void OnAckUpdateGroup(SMsgDynamic* pMsg);
	void OnAckUpdateGroupMember(SMsgDynamic* pMsg);
	void OnSendGroupChat(uint32 targetID, const XString& strContent, time_t sendTime);
	void OnRecvGroupChat(SChatData* pChatData);
	void OnAddGroup(const XString& strAccount);
	void OnDelGroup(uint32 unGroupID);
	void OnCreateGroup(const XString& strName);
	void OnCreateFriendGroup(const XString& strName);
	void OnReqChangeFriendGroup(uint32 unPlayerID, uint32 unFriendGroupID);
	void OnReqDeleteFriendGroup(uint32 unFriendGroupID);

};


#endif