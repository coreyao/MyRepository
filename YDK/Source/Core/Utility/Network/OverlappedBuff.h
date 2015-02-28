#ifndef CORE_UTILITY_NETWORK_OVERLAPPED_H__INCLUDED
#define CORE_UTILITY_NETWORK_OVERLAPPED_H__INCLUDED

#include "Network/SocketPlatform.h"
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class COverLappedBuffer : public OVERLAPPED
{
public:
	COverLappedBuffer(uint32 unMaxBufferSize = SO_MAX_MSG_SIZE);
	virtual ~COverLappedBuffer();


	WSABUF* GetWSABuff();
	char* GetBuffer();

	bool IsEmpty() const;
	uint32 GetUsedLength() const;
	void SetUsedLength(uint32 uLength);

	uint32 GetMaxBuffSize() const;
	void SetMaxBuffSize(uint32 uiMaxBuffSize);

	void SetUsed(bool bUsed);
	bool IsUsed() const;

	void SetOper(int32 oper);
	int32 GetOper() const;

	bool PopData(uint32 uiLength);
	uint32 PushData(const char* pData, uint32 uiLength);
	void Clear();

	DWORD GetNumberOfBytes() const;
	void SetNumberOfBytes(DWORD dwNumberOfBytes);

	void SwapData(COverLappedBuffer& iobuff);

protected:
	bool m_bUsed;
	DWORD m_uiNumberOfBytes;
	WSABUF m_wsaBuff;
	int32 m_iOper;
	uint32 m_uiMaxBufferSize;
	char* m_pszBuff;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif