#include "ClientSessionExt.h"

CClientSessionExt::CClientSessionExt()
: m_recvFlag(0)
, m_bChecked(false)
, m_unChallengeID(0)
{
}

CClientSessionExt::~CClientSessionExt()
{

}

void CClientSessionExt::AddRecvFlag()
{
	++m_recvFlag;
}

void CClientSessionExt::ClearRecvFlag()
{
	m_recvFlag = 0;
}

uint16 CClientSessionExt::GetRecvFlag()
{
	return m_recvFlag;
}

uint32 CClientSessionExt::GetChallengeID()
{
	return m_unChallengeID;
}

void CClientSessionExt::SetChallengeID( uint32 val )
{
	m_unChallengeID = val;
}