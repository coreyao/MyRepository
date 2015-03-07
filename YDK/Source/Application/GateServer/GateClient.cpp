#include "GateClient.h"


CGateClient::CGateClient()
: m_unAccountID(0)
, m_clientSession(NULL)
, m_bIsInWorld(false)
{
}

CGateClient::~CGateClient()
{
}

SMsg* CGateClient::PopMsgPack()
{
	return m_clientSession->PopMsgPack();
}