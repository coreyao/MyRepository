#ifndef COMMON_MSGDEF_H__INCLUDED
#define COMMON_MSGDEF_H__INCLUDED

#include "Network/Packet/Msg.h"
#include "CommonDataStruct/PlayerInfo_Struct.h"
#include "MsgProtocol.h"

#define BEGIN_MSG_PROCESS	\
	void ProcessMsg(SMsg* pMsg)				\
	{										\
	switch (pMsg->GetType())			\
		{									\


#define REGISTER_FUNC(protocol, func, msgDef)	\
	case protocol:												\
	{															\
	func(static_cast<msgDef*>(pMsg));						\
	break;													\
	}															\


#define END_MSG_PROCESS   }}


struct MsgReqGateLine : public SMsg
{
	MsgReqGateLine()
	{
		m_header.m_usType = EWorldMsg_G2M_ReqGateLine;
		m_header.m_uiLength = sizeof(*this);
	}
};

struct MsgAckGateLine : public SMsg
{
	MsgAckGateLine()
	{
		m_header.m_usType = EWorldMsg_M2G_AckGateLine;
		m_header.m_uiLength = sizeof(*this);
		m_unGateLine = 0;
	}

	uint8 m_unGateLine;
};

struct MsgReqRegisterAccount : public SMsg
{
	MsgReqRegisterAccount()
	{
		m_header.m_usType = EWorldMsg_ReqRegisterAccount;
		m_header.m_uiLength = sizeof(*this);
		memset(m_account, 0, MAX_ACCOUNT_LEN);
		memset(m_password, 0, MAX_PASSWORD_LEN);
		memset(m_nickname, 0, MAX_NICKNAME_LEN);
		m_challengeID = 0;
		m_gateID = 0;
	}

	uint32 m_challengeID;
	uint32 m_gateID;
	char m_account[MAX_ACCOUNT_LEN];
	char m_password[MAX_PASSWORD_LEN];
	char m_nickname[MAX_NICKNAME_LEN];
};

struct MsgAckRegisterAccount : public SMsg
{
	enum
	{
		EResult_Success,
		EResult_Failed,
	};

	MsgAckRegisterAccount()
	{
		m_header.m_usType = EWorldMsg_AckRegisterAccount;
		m_header.m_uiLength = sizeof(*this);
		m_result = 0;
		m_challengeID = 0;
		m_gateID = 0;
	}

	uint8 m_result;
	uint32 m_challengeID;
	uint32 m_gateID;
};

struct MsgReqLoginCheck : public SMsg
{
	MsgReqLoginCheck()
	{
		m_header.m_usType = EWorldMsg_C2S_ReqLoginCheck;
		m_header.m_uiLength = sizeof(*this);

		memset(account, 0, MAX_ACCOUNT_LEN);
		memset(password, 0, MAX_PASSWORD_LEN);
		m_challengeID = 0;
		m_gateID = 0;
	}

	uint32 m_challengeID;
	uint32 m_gateID;
	char account[MAX_ACCOUNT_LEN];
	char password[MAX_PASSWORD_LEN];
};

struct MsgAckLoginCheck : public SMsg
{
	enum
	{
		EResult_Success,
		EResult_AlreadyLogined,
		EResult_IncorrectAccOrPwd,
	};

	MsgAckLoginCheck()
	{
		m_header.m_usType = EWorldMsg_S2C_AckLoginCheck;
		m_header.m_uiLength = sizeof(*this);

		result = EResult_Success;
		challengeID = 0;
		accountID = 0;
		gateID = 0;
		memset(account, 0, MAX_ACCOUNT_LEN);
	}

	uint8 result;
	uint32 challengeID;
	uint32 accountID;
	uint32 gateID;
	char account[MAX_ACCOUNT_LEN];
};

struct MsgReqEnterWorld : public SMsg
{
	MsgReqEnterWorld()
	{
		m_header.m_usType = EWorldMsg_C2S_ReqEnterWorld;
		m_header.m_uiLength = sizeof(*this);
	}
};

struct DBMsgAckEnterWorld : public SMsg
{
	enum
	{
		EResult_Success,
		EResult_PlayerNotFound,
		EResult_PlayerAlreadyInWorld,
	};

	DBMsgAckEnterWorld()
	{
		m_header.m_usType = EWorldMsg_D2M_AckEnterWorld;
		m_header.m_uiLength = sizeof(*this);

		m_result = EResult_Success;

		m_friendsInfoLen = 0;
		memset(m_friendsInfo, 0, MAX_RELATIONDATA_LEN);
		m_groupsInfoLen = 0;
		memset(m_groupsInfo, 0, MAX_GROUPDATA_LEN);
	}

	uint8 m_result;
	SPlayerBaseInfo m_baseInfo;
	uint32 m_friendsInfoLen;
	char m_friendsInfo[MAX_RELATIONDATA_LEN];
	uint32 m_groupsInfoLen;
	char m_groupsInfo[MAX_GROUPDATA_LEN];
};

struct MsgAckEnterWorld : public SMsg
{
	enum
	{
		EResult_Success,
		EResult_PlayerNotFound,
		EResult_PlayerAlreadyInWorld,
	};

	MsgAckEnterWorld()
	{
		m_header.m_usType = EWorldMsg_M2C_AckEnterWorld;
		m_header.m_uiLength = sizeof(*this);

		m_result = EResult_Success;
		m_unChallengeID = 0;

	}

	uint8 m_result;
	uint32 m_unChallengeID;
	SPlayerBaseInfo m_playerBaseInfo;
};

struct MsgReqAddRelation : public SMsg
{
	MsgReqAddRelation()
	{
		m_header.m_usType = ERelationMsg_ReqAddRelation;
		m_header.m_uiLength = sizeof(*this);
		memset(m_targetAccount, 0, MAX_ACCOUNT_LEN);
	}

	char m_targetAccount[MAX_ACCOUNT_LEN];
};

struct MsgReqDelRelation : public SMsg
{
	MsgReqDelRelation()
	{
		m_header.m_usType = ERelationMsg_ReqDelRelation;
		m_header.m_uiLength = sizeof(*this);
		m_unPlayerID = 0;
	}

	uint32 m_unPlayerID;
};

struct MsgReqExitWorld : public SMsg
{
	MsgReqExitWorld()
	{
		m_header.m_usType = EWorldMsg_ReqExitWorld;
		m_header.m_uiLength = sizeof(*this);
	}
};

struct MsgReqCreateGroup : public SMsg
{
	MsgReqCreateGroup()
	{
		m_header.m_usType = EGroupMsg_C2S_ReqCreate;
		m_header.m_uiLength = sizeof(*this);
		
		memset(m_groupName, 0, MAX_GROUPNAME_LEN);
	}

	char m_groupName[MAX_GROUPNAME_LEN];
};

struct MsgReqCreateFriendGroup : public SMsg
{
	MsgReqCreateFriendGroup()
	{
		m_header.m_usType = ERelationMsg_ReqCreateCustomGroup;
		m_header.m_uiLength = sizeof(*this);

		memset(m_groupName, 0, MAX_GROUPNAME_LEN);
	}

	char m_groupName[MAX_GROUPNAME_LEN];
};

struct MsgReqJoinGroup : public SMsg
{
	MsgReqJoinGroup()
		: m_unGroupID(0)
	{
		m_header.m_usType = EGroupMsg_C2S_ReqJoin;
		m_header.m_uiLength = sizeof(*this);
	}

	uint32 m_unGroupID;
};

struct MsgReqLeaveGroup : public SMsg
{
	MsgReqLeaveGroup()
		: m_unGroupID(0)
	{
		m_header.m_usType = EGroupMsg_C2S_ReqLeave;
		m_header.m_uiLength = sizeof(*this);
	}

	uint32 m_unGroupID;
};

struct MsgReqSendFile : public SMsg 
{
	MsgReqSendFile()
	{
		m_header.m_usType = EFileMsg_ReqSendFile;
		m_header.m_uiLength = sizeof(*this);

		memset(m_strSenderName, 0, MAX_NICKNAME_LEN);
		memset(m_strFileName, 0, MAX_FILENAME_LEN);
		m_unFileTotalLength = 0;
		m_unSenderID = 0;
		m_unTargetID = 0;
		m_unLocalFileNO = 0;
	}

	char m_strSenderName[MAX_NICKNAME_LEN];
	uint32 m_unFileTotalLength;
	char m_strFileName[MAX_FILENAME_LEN];
	uint32 m_unSenderID;
	uint32 m_unTargetID;
	uint32 m_unLocalFileNO;
};

struct MsgAckSendFile : public SMsg
{
	enum
	{
		EResult_Agreed,
		EResult_Refused,
	};

	MsgAckSendFile()
	{
		m_header.m_usType = EFileMsg_AckSendFile;
		m_header.m_uiLength = sizeof(*this);

		m_unResult = EResult_Agreed;
		m_unSenderID = 0;
		m_unTargetID = 0;
		m_unSenderFileNO = 0;
		m_unLocalFileNO = 0;
	}

	uint8 m_unResult;
	uint32 m_unSenderID;
	uint32 m_unTargetID;
	uint32 m_unSenderFileNO;
	uint32 m_unLocalFileNO;
};

struct MsgReqChangeCustomGroup : public SMsg
{
	MsgReqChangeCustomGroup()
		: m_unFriendID(0)
		, m_unNewCustomGroupID(0)
	{
		m_header.m_usType = ERelationMsg_ReqChangeCustomGroup;
		m_header.m_uiLength = sizeof(*this);
	}

	uint32 m_unFriendID;
	uint32 m_unNewCustomGroupID;
};

struct MsgReqDelCustomGroup : public SMsg
{
	MsgReqDelCustomGroup()
		: m_unCustomGroupID(0)
	{
		m_header.m_usType = ERelationMsg_ReqDelCustomGroup;
		m_header.m_uiLength = sizeof(*this);
	}

	uint32 m_unCustomGroupID;
};


struct MsgTestBigPacket : public SMsg
{
	MsgTestBigPacket()
	{
		m_header.m_usType = ETestMSg_TestBigPacket;
		m_header.m_uiLength = sizeof(*this);
		memset(content, 0, 2* 8195 - 10);
	}

	char content[2 * 8195 - 10];
};

#endif