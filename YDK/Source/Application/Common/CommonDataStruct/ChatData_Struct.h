#ifndef COMMON_CHATDATA_H__INCLUDED
#define COMMON_CHATDATA_H__INCLUDED

#include "XString/XString.h"
#include "Network/Packet/DynamicPacket.h"

using Core::Utility::uint32;
using Core::Utility::uint8;
using Core::Utility::XString;
using Core::Utility::CDynamicPacket;

enum EChannel
{
	EChannel_Single,
	EChannel_Group,
};

struct SChatData
{
public:
	SChatData()
		: m_unTime(0)
		, m_unTargetID(0)
		, m_unSenderID(0)
		, m_unChannel(EChannel_Single)
	{
	}

	void Serialize(CDynamicPacket& ar)
	{
		ar << m_unTime;
		ar << m_unTargetID;
		ar << m_unSenderID;
		ar << m_strSenderName;
		ar << m_unChannel;
		ar << m_strContent;
	}

	uint32 m_unTime;
	uint32 m_unTargetID;
	uint32 m_unSenderID;
	XString m_strSenderName;
	uint8 m_unChannel;
	XString m_strContent;
};


#endif