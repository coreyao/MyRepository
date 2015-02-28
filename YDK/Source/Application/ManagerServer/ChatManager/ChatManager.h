#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_CHATMANAGER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_CHATMANAGER_H__INCLUDED

#include "CharactorManager/GamePlayer.h"
#include "Network/Packet/DynamicPacket.h"
#include "MsgDef.h"
#include "CommonDataStruct/ChatData_Struct.h"

using Core::Utility::SMsgDynamic;

#define BEGIN_CHAT_MSG_PROCESS										\
	void DispatchMsg(SChatData chatData, uint32 senderID)			\
	{																\
			switch(chatData.m_unChannel)							\
			{														\


#define REGISTER_CHAT_FUNC(channel, func)						\
	case channel:												\
	{															\
		func(chatData, senderID);								\
		break;													\
	}															\


#define END_CHAT_MSG_PROCESS   }}


class CChatManager
{
public:
	CChatManager();
	~CChatManager();

	void ProcessMsg(SMsg* pMsg);

	BEGIN_CHAT_MSG_PROCESS
		REGISTER_CHAT_FUNC(EChannel_Single, OnDispatchSingleMessage)
		REGISTER_CHAT_FUNC(EChannel_Group, OnDispatchGroupMessage)
	END_CHAT_MSG_PROCESS

	void OnDispatchSingleMessage(SChatData& chatData, uint32 unSenderID);
	void OnDispatchGroupMessage(SChatData& chatData, uint32 unSenderID);
	
};


#endif