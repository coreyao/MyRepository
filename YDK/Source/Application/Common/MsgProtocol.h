#ifndef COMMON_MSGPROTOCOL_H__INCLUDED
#define COMMON_MSGPROTOCOL_H__INCLUDED

enum EProtocolRange
{
	EProtocolRange_WorldMsg_Begin = 0,
	EProtocolRange_WorldMsg_End = 500,

	EProtocolRange_Test_Begin = 501,
	EProtocolRange_Test_End = 550,

	EProtocolRange_PlayerMsg_Begin = 551,
	EProtocolRange_PlayerMsg_End = 1000,

	EProtocolRange_GroupMsg_Begin = 1001,
	EProtocolRange_GroupMsg_End = 1500,

	EProtocolRange_RelationMsg_Begin = 1501,
	EProtocolRange_RelationMsg_End = 2000,

	EProtocolRange_FileMsg_Begin = 2001,
	EProtocolRange_FileMsg_End = 2100,
};

enum EWorldMsg
{
	EWorldMsg_Begin = EProtocolRange_WorldMsg_Begin,

	EWorldMsg_G2M_ReqGateLine,
	EWorldMsg_M2G_AckGateLine,

	EWorldMsg_C2S_ReqLoginCheck,
	EWorldMsg_S2C_AckLoginCheck,

	EWorldMsg_C2S_ReqEnterWorld,
	EWorldMsg_D2M_AckEnterWorld,
	EWorldMsg_M2C_AckEnterWorld,

	EWorldMsg_Chat,

	EWorldMsg_ReqExitWorld,

	EWorldMsg_ReqRegisterAccount,
	EWorldMsg_AckRegisterAccount,

	EWorldMsg_End = EProtocolRange_WorldMsg_End,
};

enum EPlayerMsg
{
	EPlayerMsg_Begin = EProtocolRange_PlayerMsg_Begin,

	EPlayerMsg_M2C_UpdateRelationData,
	EPlayerMsg_M2C_UpdateAllRelationData,
	EPlayerMsg_M2D_SaveRelationData,
	EPlayerMsg_M2C_UpdateFriendGroupData,

	EPlayerMsg_S2C_UpdateGroupMember,
	EPlayerMsg_S2C_UpdateGroup,
	EPlayerMsg_S2C_UpdateAllGroup,
	EPlayerMsg_M2D_SaveGroupData,

	EPlayerMsg_End = EProtocolRange_PlayerMsg_End,
};

enum ERelationMsg
{
	ERelationMsg_Begin = EProtocolRange_RelationMsg_Begin,
	ERelationMsg_ReqAddRelation,
	ERelationMsg_ReqDelRelation,
	ERelationMsg_ReqCreateCustomGroup,
	ERelationMsg_AckCreateCustomGroup,
	ERelationMsg_ReqChangeCustomGroup,
	ERelationMsg_ReqDelCustomGroup,
	ERelationMsg_End = EProtocolRange_RelationMsg_End,
};

enum EGroupMsg
{
	EGroupMsg_Begin = EProtocolRange_GroupMsg_Begin,
	EGroupMsg_M2D_ReqLoadGroup,
	EGroupMsg_D2M_AckLoadGroup,
	EGroupMsg_C2S_ReqCreate,
	EGroupMsg_D2M_AckCreate,
	EGroupMsg_C2S_ReqJoin,
	EGroupMsg_C2S_ReqLeave,
	EGroupMsg_M2D_SaveGroupsInfo,
	EGroupMsg_End = EProtocolRange_GroupMsg_End,
};

enum EFileMsg
{
	EFileMsg_Begin = EProtocolRange_FileMsg_Begin,
	EFileMsg_ReqSendFile,
	EFileMsg_AckSendFile,
	EFileMsg_FileContent,
	EFileMsg_End = EProtocolRange_FileMsg_End,
};

enum ETestMsg
{
	ETestMsg_Begin = EProtocolRange_Test_Begin,
	ETestMSg_TestBigPacket,
	ETestMsg_End = EProtocolRange_Test_End,
};

#endif