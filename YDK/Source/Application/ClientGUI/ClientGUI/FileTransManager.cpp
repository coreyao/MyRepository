#include "FileTransManager.h"
#include "ClientPlayer.h"
#include "Lockers/Guard.h"
#include "Singleton.h"
#include "Network/ServerConnector.h"
#include "UI/MainFrame.h"

using Core::Utility::CGuard;
using Core::Utility::TSingleton;

CFileTransManager::CFileTransManager()
: m_unLastFileNO(0)
{
}

CFileTransManager::~CFileTransManager()
{
}

bool CFileTransManager::SendFileReq(uint32 unTargetID, const XString& strFilePath)
{
	std::fstream fileStream(strFilePath.c_str());
	if (!fileStream.is_open())
	{
		return false;
	}
	fileStream.close();

	SFile* pFile = new SFile;
	pFile->m_strFilePath = strFilePath;
	pFile->m_fileOP.open(strFilePath.c_str(), std::ios::in | std::ios::binary);
	pFile->m_untotalLen = GetFileSize(pFile->m_fileOP);
	pFile->m_unTotalSent = 0;
	pFile->m_unTotalReceived = 0;
	pFile->m_bIsSender = true;
	pFile->m_bEnableSend = false;
	pFile->m_bHasReqSent = false;
	pFile->m_unLocalFileNO = GenerateFileNO();
	pFile->m_unOtherFileNO = 0;

	{
		CGuard guard(&m_lock);
		CFileTransManager::MAPFILELIST* mapFileLists = GetFileList(unTargetID);
		mapFileLists->insert(std::make_pair(pFile->m_unLocalFileNO, pFile));
	}

	return true;
}

void CFileTransManager::SendFileAck( uint32 unSenderID, uint32 unSenderFileNO, const XString& strFilePath, uint32 unTotalLen )
{
	MsgAckSendFile ackMsg;
	ackMsg.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	ackMsg.m_unSenderFileNO = unSenderFileNO;
	ackMsg.m_unSenderID = unSenderID;
	ackMsg.m_unTargetID = ackMsg.GetPlayerID();

	if (strFilePath.empty())
	{
		ackMsg.m_unResult = MsgAckSendFile::EResult_Refused;
	}
	else
	{
		//create file
		std::fstream fileStream(strFilePath.c_str(), std::ios::out);
		if (!fileStream.is_open())
		{
			ackMsg.m_unResult = MsgAckSendFile::EResult_Refused;
		}
		else
		{
			fileStream.close();

			SFile* pFile = new SFile;
			pFile->m_strFilePath = strFilePath;
			pFile->m_fileOP.open(strFilePath.c_str(), std::ios::out | std::ios::binary);
			pFile->m_untotalLen = unTotalLen;
			pFile->m_unTotalSent = 0;
			pFile->m_unTotalReceived = 0;
			pFile->m_bIsSender = false;
			pFile->m_bEnableSend = false;
			pFile->m_bHasReqSent = false;
			pFile->m_unLocalFileNO = GenerateFileNO();
			pFile->m_unOtherFileNO = unSenderFileNO;

			{
				CGuard guard(&m_lock);
				CFileTransManager::MAPFILELIST* mapFileLists = GetFileList(unSenderID);
				mapFileLists->insert(std::make_pair(pFile->m_unLocalFileNO, pFile));
			}

			ackMsg.m_unResult = MsgAckSendFile::EResult_Agreed;
			ackMsg.m_unLocalFileNO = pFile->m_unLocalFileNO;
		}
	}

	TSingleton<CServerConnector>::GetInstance().SendMsg(&ackMsg);
}

void CFileTransManager::CheckFiles()
{
	CGuard guard(&m_lock);
	MAPPLAYERFILES::iterator playerIter = m_mapPlayerFiles.begin();
	for (; playerIter != m_mapPlayerFiles.end();)
	{
		MAPFILELIST& mapFiles = playerIter->second;
		if (mapFiles.empty())
		{
			playerIter = m_mapPlayerFiles.erase(playerIter);
			continue;
		}
		else
		{
			MAPFILELIST::iterator fileIter = mapFiles.begin();
			for (; fileIter != mapFiles.end();)
			{
				SFile* pFile = fileIter->second;
				if (pFile == NULL)
				{
					++fileIter;
					continue;
				}

				if (pFile->m_unTotalSent == pFile->m_untotalLen || pFile->m_unTotalReceived == pFile->m_untotalLen)
				{
					pFile->m_fileOP.close();
					delete pFile;
					fileIter = mapFiles.erase(fileIter);
					continue;
				}
				else
				{
					if (!pFile->m_bIsSender)
					{
						++fileIter;
						continue;
					}

					if (!pFile->m_bHasReqSent)
					{
						MsgReqSendFile reqMsg;
						reqMsg.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
						strcpy_s(reqMsg.m_strFileName, ConvertPathToFileName(pFile->m_strFilePath).c_str());
						reqMsg.m_unLocalFileNO = pFile->m_unLocalFileNO;
						reqMsg.m_unFileTotalLength = pFile->m_untotalLen;
						reqMsg.m_unTargetID = playerIter->first;
						reqMsg.m_unSenderID = reqMsg.GetPlayerID();
						strcpy_s(reqMsg.m_strSenderName, TSingleton<CClientPlayer>::GetInstance().GetNickName().c_str());
						TSingleton<CServerConnector>::GetInstance().SendMsg(&reqMsg);

						pFile->m_bHasReqSent = true;
					}
					else
					{
						if (!pFile->m_bEnableSend)
						{
							++fileIter;
							continue;
						}

						//Read & Send file
						char content[MAX_CONTENT_LEN] = {0};
						pFile->m_fileOP.read(content, MAX_CONTENT_LEN);
						if (!pFile->m_fileOP.bad())
						{
							uint32 totalRead = static_cast<uint32>(pFile->m_fileOP.gcount());
							uint32 currentPlayerID = TSingleton<CClientPlayer>::GetInstance().GetAccountID();

							CDynamicPacket cPacket(EFileMsg_FileContent);
							cPacket.GetMsg()->SetPlayerID(currentPlayerID);
							cPacket << static_cast<uint32>(playerIter->first);			//receiver id
							cPacket << currentPlayerID;									//sender id
							cPacket << pFile->m_unOtherFileNO;							//target fileno
							cPacket << totalRead;									
							cPacket.WriteData(content, totalRead);
							TSingleton<CServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());

							pFile->m_unTotalSent += totalRead;

							if (pFile->m_fileOP.eof())
							{
								pFile->m_bEnableSend = false;
							}

							wxCommandEvent evt(EVT_COMMAND_SHOW_PERCENT, ID_MainFrame);
							evt.SetClientData(pFile);
							wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
						}
					}
				}	

				++fileIter;
			}

			++playerIter;
		}
	}
}

void CFileTransManager::OnReqSendFile( MsgReqSendFile* pMsg )
{
	//promote client
	wxCommandEvent evt(EVT_COMMAND_PROMOTE_FILE_SEND, ID_MainFrame);
	evt.SetClientData(pMsg);
	wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
}

uint8 CFileTransManager::OnAckSendFile( MsgAckSendFile* pMsg )
{
	MAPFILELIST* pMapFileList = GetFileList(pMsg->m_unTargetID);
	if (pMapFileList != NULL)
	{
		MAPFILELIST::iterator fileIter = pMapFileList->begin();
		for (; fileIter != pMapFileList->end(); ++fileIter)
		{
			if (fileIter->second->m_unLocalFileNO == pMsg->m_unSenderFileNO)
			{
				if (pMsg->m_unResult == MsgAckSendFile::EResult_Agreed)
				{
					fileIter->second->m_unOtherFileNO = pMsg->m_unLocalFileNO;
					fileIter->second->m_bEnableSend = true;
					break;
				}
				else if (pMsg->m_unResult == MsgAckSendFile::EResult_Refused)
				{
					fileIter->second->m_fileOP.close();
					pMapFileList->erase(fileIter);
					break;
				}
			}
		}
	}

	return pMsg->m_unResult;
}

void CFileTransManager::OnRecvFileContent( SMsgDynamic* pMsg )
{
	uint32 senderID = 0;
	uint32 receiverID = 0;
	uint32 fileNO = 0;
	uint32 contentLen = 0;
	char content[MAX_CONTENT_LEN] = {0};
	CDynamicPacket cPacket(pMsg);
	cPacket << receiverID;
	cPacket << senderID;
	cPacket << fileNO;
	cPacket << contentLen;
	cPacket.ReadData(content, contentLen);

	SFile* pFile = GetSFile(senderID, fileNO);
	if (pFile != NULL)
	{
		pFile->m_fileOP.write(content, contentLen);
		if (!pFile->m_fileOP.bad())
		{
			pFile->m_unTotalReceived += contentLen;

			wxCommandEvent evt(EVT_COMMAND_SHOW_PERCENT, ID_MainFrame);
			evt.SetClientData(pFile);
			wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
		}
	}
}

CFileTransManager::MAPFILELIST* CFileTransManager::GetFileList( uint32 unPlayerID )
{
	MAPPLAYERFILES::iterator playerIter = m_mapPlayerFiles.find(unPlayerID);
	if (playerIter != m_mapPlayerFiles.end())
	{
		return &playerIter->second;
	}
	else
	{
		MAPFILELIST mapFileList;
		m_mapPlayerFiles.insert(std::make_pair(unPlayerID, mapFileList));
		return &m_mapPlayerFiles[unPlayerID];
	}
}

uint32 CFileTransManager::GenerateFileNO()
{
	return ++m_unLastFileNO;
}

uint32 CFileTransManager::GetFileSize(std::fstream& fileStream)
{
	uint32 length = 0;
	fileStream.seekg(0, std::ios::end);
	length = static_cast<uint32>(fileStream.tellg());
	fileStream.seekg(0, std::ios::beg);

	return length;
}

XString CFileTransManager::ConvertPathToFileName( const XString& strFilePath )
{
	uint32 pos = strFilePath.find_last_of(TEXT("/\\"));
	return strFilePath.substr(pos + 1);
}

SFile* CFileTransManager::GetSFile( uint32 unPlayerID, uint32 fileNO )
{
	MAPFILELIST* mapFileList = GetFileList(unPlayerID);
	MAPFILELIST::iterator fileIter = mapFileList->begin();
	for (; fileIter != mapFileList->end(); ++fileIter)
	{
		if (fileIter->second->m_unLocalFileNO == fileNO)
		{
			return fileIter->second;
		}
	}

	return NULL;
}