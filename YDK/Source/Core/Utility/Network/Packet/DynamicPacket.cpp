#include "Network/Packet/DynamicPacket.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CDynamicPacket::CDynamicPacket()
{
	m_ubOperation = OP_NONE;
	m_bError = false;
	m_pMsg = NULL;
	m_pData = NULL;
	m_uiCurByte = 0;
	m_uiMaxByte = 0;
	m_bNew = false;
}

CDynamicPacket::CDynamicPacket(SMsgDynamic* pMsg, uint8 ubOP /* = OP_READ */)
{
	m_ubOperation = ubOP;
	m_bError = false;
	m_pMsg = pMsg;
	m_pData = pMsg->szBuffer;

	if (m_ubOperation == OP_READ)
	{
		m_uiMaxByte = pMsg->GetLength() - sizeof(m_pMsg->m_header);
	}
	else
	{
		m_uiMaxByte = sizeof(pMsg->szBuffer);
	}

	m_uiCurByte = 0;
	m_bNew = false;
}

CDynamicPacket::CDynamicPacket(char *szBuffer, uint32 uiSize, uint8 ubOP)
{
	m_ubOperation = ubOP;
	m_bError = false;

	m_pMsg = NULL;
	m_pData = szBuffer;
	m_uiCurByte = 0;
	m_uiMaxByte = uiSize;
	m_bNew = false;
}

CDynamicPacket::CDynamicPacket( uint16 usType, uint8 ubOP /*= OP_WRITE*/ )
{
	m_ubOperation = ubOP;
	m_bError = false;

	m_pMsg = new SMsgDynamic(usType);
	m_pData = m_pMsg->szBuffer;
	m_uiCurByte = 0;
	m_uiMaxByte = sizeof(m_pMsg->szBuffer);
	m_bNew = true;
}

CDynamicPacket::~CDynamicPacket()
{
	m_pData = NULL;
	if (m_pMsg != NULL)
	{
		if (m_bNew)
		{
			SAFE_DELETE(m_pMsg);
		}

		m_pMsg = NULL;
	}
}

void CDynamicPacket::Serialize(void *data, uint32 len)
{
	if (m_ubOperation == OP_READ)
	{
		ReadData(data, len);
	}
	else
	{
		WriteData(data, len);
	}
}

bool CDynamicPacket::ReadData( void* data, uint32 len )
{
	if (m_uiCurByte + len > m_uiMaxByte)
	{
		m_bError = true;
		return false;
	}

	if (len != 0)
	{
		memcpy(data, m_pData + m_uiCurByte, len);
		m_uiCurByte += len;
	}

	return true;
}

bool CDynamicPacket::ReadInt8( int8* c )
{
	return ReadData(c, sizeof(int8));
}

bool CDynamicPacket::ReadUInt8( uint8* uc )
{
	return ReadData(uc, sizeof(uint8));
}

bool CDynamicPacket::ReadInt16( int16* s )
{
	return ReadData(s, sizeof(int16));
}

bool CDynamicPacket::ReadUInt16( uint16* us )
{
	return ReadData(us, sizeof(uint16));
}

bool CDynamicPacket::ReadInt32( int32* l )
{
	return ReadData(l, sizeof(int32));
}

bool CDynamicPacket::ReadUInt32( uint32* ul )
{
	return ReadData(ul, sizeof(uint32));
}

bool CDynamicPacket::ReadInt64( int64* q )
{
	return ReadData(q, sizeof(int64));
}

bool CDynamicPacket::ReadUInt64( uint64* uq )
{
	return ReadData(uq, sizeof(uint64));
}

bool CDynamicPacket::ReadBool( bool* b )
{
	return ReadData(b, sizeof(bool));
}

bool CDynamicPacket::ReadFloat( float* f )
{
	return ReadData(f, sizeof(float));
}

bool CDynamicPacket::ReadDouble( double* d )
{
	return ReadData(d, sizeof(double));
}

bool CDynamicPacket::ReadString( char** str )
{
	uint16 len;
	if (!ReadUInt16(&len) || len <= 0 || m_uiCurByte + len > m_uiMaxByte)
	{
		m_bError = true;
		return false;
	}

	*(m_pData + m_uiCurByte + len - 1) = '\0';
	*str = m_pData + m_uiCurByte;
	m_uiCurByte += len;

	return true;
}

bool CDynamicPacket::WriteData( void* data, uint32 len )
{
	if (m_uiCurByte + len > m_uiMaxByte)
	{
		m_bError = true;
		return false;
	}

	if (len != 0)
	{
		memcpy(m_pData + m_uiCurByte, data, len);
		m_uiCurByte += len;
		if (m_pMsg!= NULL)
		{
			m_pMsg->m_header.m_uiLength = m_uiCurByte + sizeof(m_pMsg->m_header);
		}
	}

	return true;
}

bool CDynamicPacket::WriteInt8( int8 c )
{
	return WriteData(&c, sizeof(int8));
}

bool CDynamicPacket::WriteUInt8( uint8 uc )
{
	return WriteData(&uc, sizeof(int8));
}

bool CDynamicPacket::WriteInt16( int16 s )
{
	return WriteData(&s, sizeof(int16));
}

bool CDynamicPacket::WriteUInt16( uint16 us )
{
	return WriteData(&us, sizeof(uint16));
}

bool CDynamicPacket::WriteInt32( int32 l )
{
	return WriteData(&l, sizeof(int32));
}

bool CDynamicPacket::WriteUInt32( uint32 ul )
{
	return WriteData(&ul, sizeof(uint32));
}

bool CDynamicPacket::WriteInt64( int64 q )
{
	return WriteData(&q, sizeof(int64));
}

bool CDynamicPacket::WriteUInt64( uint64 uq )
{
	return WriteData(&uq, sizeof(uint64));
}

bool CDynamicPacket::WriteBool( bool b )
{
	return WriteData(&b, sizeof(bool));
}

bool CDynamicPacket::WriteFloat( float f )
{
	return WriteData(&f, sizeof(float));
}

bool CDynamicPacket::WriteDouble( double d )
{
	return WriteData(&d, sizeof(double));
}

bool CDynamicPacket::WriteString( char* str )
{
	uint16 len = static_cast<uint16>(strlen(str)) + 1;
	if (!WriteUInt16(len))
	{
		return false;
	}

	return WriteData(str, len);
}

bool CDynamicPacket::WritePacket( CDynamicPacket* pPacket )
{
	uint32 newByteSize = GetDataSize() + pPacket->GetDataSize();
	if (newByteSize > m_uiMaxByte)
	{
		m_bError = true;
		return false;
	}

	return WriteData(pPacket->GetData(), pPacket->GetDataSize());
}

bool CDynamicPacket::Clone( CDynamicPacket* pPacket )
{
	m_uiMaxByte = pPacket->GetDataSize();
	memcpy(m_pData, pPacket->GetData(), pPacket->GetDataSize());
	m_uiCurByte = pPacket->GetDataSize();
	if (m_pMsg != NULL)
	{
		m_pMsg->m_header.m_uiLength = pPacket->GetDataSize();
	}

	return true;
}

template <>
CDynamicPacket& operator<<<int8>(CDynamicPacket& ar, int8& val)
{
	ar.Serialize(&val, sizeof(int8));

	return ar;
}

template <>
CDynamicPacket& operator<<<uint8>(CDynamicPacket& ar, uint8& val)
{
	ar.Serialize(&val, sizeof(uint8));

	return ar;
}

template <>
CDynamicPacket& operator<<<int16>(CDynamicPacket& ar, int16& val)
{
	ar.Serialize(&val, sizeof(int16));

	return ar;
}

template <>
CDynamicPacket& operator<<<uint16>(CDynamicPacket& ar, uint16& val)
{
	ar.Serialize(&val, sizeof(uint16));

	return ar;
}

template <>
CDynamicPacket& operator<<<int32>(CDynamicPacket& ar, int32& val)
{
	ar.Serialize(&val, sizeof(int32));

	return ar;
}

template <>
CDynamicPacket& operator<<<uint32>(CDynamicPacket& ar, uint32& val)
{
	ar.Serialize(&val, sizeof(uint32));

	return ar;
}

template <>
CDynamicPacket& operator<<<int64>(CDynamicPacket& ar, int64& val)
{
	ar.Serialize(&val, sizeof(int64));

	return ar;
}

template <>
CDynamicPacket& operator<<<uint64>(CDynamicPacket& ar, uint64& val)
{
	ar.Serialize(&val, sizeof(uint64));

	return ar;
}

template <>
CDynamicPacket& operator<<<bool>(CDynamicPacket& ar, bool& val)
{
	ar.Serialize(&val, sizeof(bool));

	return ar;
}

template <>
CDynamicPacket& operator<<<float>(CDynamicPacket& ar, float& val)
{
	ar.Serialize(&val, sizeof(float));

	return ar;
}

template <>
CDynamicPacket& operator<<<double>(CDynamicPacket& ar, double& val)
{
	ar.Serialize(&val, sizeof(double));

	return ar;
}

template <>
CDynamicPacket& operator<<<std::string>(CDynamicPacket& ar, std::string& val)
{
	if (ar.IsReading())
	{
		char* temp;
		if (!ar.ReadString(&temp))
		{
			return ar;
		}
		val = temp;
		return ar;
	}
	else
	{
		ar.WriteString((char*)val.c_str());
		return ar;
	}
}

template <>
CDynamicPacket& operator<<<XString>(CDynamicPacket& ar, XString& val)
{
	if (ar.IsReading())
	{
		char* temp;
		if (!ar.ReadString(&temp))
		{
			return ar;
		}
		val = temp;
		return ar;
	}
	else
	{
		ar.WriteString((char*)val.c_str());
		return ar;
	}
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)