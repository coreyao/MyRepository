#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_FILETRANSMANAGER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_FILETRANSMANAGER_H__INCLUDED

#include "CharactorManager/GamePlayer.h"
#include "MsgDef.h"

using Core::Utility::SMsgDynamic;

class CFileTransManager
{
public:
	CFileTransManager();
	~CFileTransManager();

	BEGIN_MSG_PROCESS
		REGISTER_FUNC(EFileMsg_ReqSendFile, OnReqSendFile, MsgReqSendFile)
		REGISTER_FUNC(EFileMsg_AckSendFile, OnAckSendFile, MsgAckSendFile)
		REGISTER_FUNC(EFileMsg_FileContent, OnRecvFileContent, SMsgDynamic)
	END_MSG_PROCESS

private:
	void  OnReqSendFile(MsgReqSendFile* pMsg);
	void OnAckSendFile(MsgAckSendFile* pMsg);
	void  OnRecvFileContent(SMsgDynamic* pMsg);
};

#endif