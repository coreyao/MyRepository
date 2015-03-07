#ifndef CORE_UTILITY_APPLICATION_CLIENT_FILETRANSMANAGER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENT_FILETRANSMANAGER_H__INCLUDED

#include <map>
#include <fstream>
#include "XString/XString.h"
#include "Lockers/Locker.h"
#include "MsgDef.h"

using Core::Utility::XString;
using Core::Utility::uint32;
using Core::Utility::SMsgDynamic;
using Core::Utility::CLocker;

struct SFile
{
	XString			m_strFilePath;
	uint32			m_untotalLen;
	uint32			m_unTotalSent;
	uint32			m_unTotalReceived;
	bool			m_bIsSender;
	bool			m_bEnableSend;
	bool			m_bHasReqSent;
	uint32			m_unLocalFileNO;
	uint32			m_unOtherFileNO;
	std::fstream	m_fileOP;
};

class CFileTransManager
{
public:
	typedef std::map<uint32, SFile*> MAPFILELIST;
	typedef std::map<uint32, MAPFILELIST> MAPPLAYERFILES;
public:
	CFileTransManager();
	~CFileTransManager();

	bool SendFileReq(uint32 unTargetID, const XString& strFilePath);
	void SendFileAck(uint32 unSenderID, uint32 unSenderFileNO, const XString& strFilePath, uint32 unTotalLen);

	MAPFILELIST* GetFileList(uint32 unPlayerID);
	SFile* GetSFile(uint32 unPlayerID, uint32 fileNO);
	uint32 GetFileSize(std::fstream& fileStream);
	XString ConvertPathToFileName(const XString& strFilePath);
	uint32 GenerateFileNO();

	void CheckFiles();

	BEGIN_MSG_PROCESS
		REGISTER_FUNC(EFileMsg_ReqSendFile, OnReqSendFile, MsgReqSendFile)
		REGISTER_FUNC(EFileMsg_AckSendFile, OnAckSendFile, MsgAckSendFile)
		REGISTER_FUNC(EFileMsg_FileContent, OnRecvFileContent, SMsgDynamic)
	END_MSG_PROCESS

private:
	void  OnReqSendFile(MsgReqSendFile* pMsg);
	uint8 OnAckSendFile(MsgAckSendFile* pMsg);
	void  OnRecvFileContent(SMsgDynamic* pMsg);

private:
	MAPPLAYERFILES	m_mapPlayerFiles;
	uint32			m_unLastFileNO;
	CLocker			m_lock;
};

#endif
