#ifndef CORE_UTILITY_MSG_H__INCLUDED
#define CORE_UTILITY_MSG_H__INCLUDED

#include "BasicDefines.h"
#include "UtilityTypes.h"

using Core::Utility::uint32;
using Core::Utility::uint16;
using Core::Utility::uint8;

const unsigned int ConMaxPacketSize = 1024*1024;
#define MAX_ACCOUNT_LEN 50
#define MAX_PASSWORD_LEN 8
#define MAX_NICKNAME_LEN 15
#define MAX_FILENAME_LEN 100
#define MAX_CONTENT_LEN SO_MAX_MSG_SIZE

enum EMsgLevel
{
	EMsgLevel_High = 0,
	EMsgLevel_Normal,
	EMsgLevel_Low
};

struct SMsgHeader
{
	uint32 m_uiID;
	uint32 m_uiLength;
	uint16 m_usType;
	uint8 m_ubMsgLevel;
	uint8 m_ubLine;

	SMsgHeader()
	{
		m_ubLine = 0;
		m_uiID = 0;
		m_ubMsgLevel = EMsgLevel_High;
		m_uiLength = sizeof(*this);
	}
};

struct SMsg
{
	SMsgHeader m_header;
	uint16 GetType(){return m_header.m_usType;}
	uint32 GetLength(){return m_header.m_uiLength;}
	uint8 GetLevel(){return m_header.m_ubMsgLevel;}
	void SetLevel(EMsgLevel lv){m_header.m_ubMsgLevel = static_cast<uint8>(lv);}
	uint32 GetPlayerID() {return m_header.m_uiID;}
	void SetPlayerID(uint32 ID) { m_header.m_uiID = ID;}
};

#endif