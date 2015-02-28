#include "FileTransManager/FileTransManager.h"
#include "Network/GateServerListener.h"
#include "Singleton.h"

using Core::Utility::TSingleton;

CFileTransManager::CFileTransManager()
{

}

CFileTransManager::~CFileTransManager()
{

}

void CFileTransManager::OnReqSendFile( MsgReqSendFile* pMsg )
{
	pMsg->SetPlayerID(pMsg->m_unTargetID);
	TSingleton<CGateServerListener>::GetInstance().SendMsg(pMsg);
}

void CFileTransManager::OnAckSendFile( MsgAckSendFile* pMsg )
{
	pMsg->SetPlayerID(pMsg->m_unSenderID);
	TSingleton<CGateServerListener>::GetInstance().SendMsg(pMsg);
}

void CFileTransManager::OnRecvFileContent( SMsgDynamic* pMsg )
{
	uint32 receiverID = 0;
	CDynamicPacket cPacket(pMsg);
	cPacket << receiverID;

	pMsg->SetPlayerID(receiverID);
	TSingleton<CGateServerListener>::GetInstance().SendMsg(pMsg);
}