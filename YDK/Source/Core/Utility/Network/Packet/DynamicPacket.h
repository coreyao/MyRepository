#ifndef CORE_UTILITY_DYNAMICPACKET_H__INCLUDED
#define CORE_UTILITY_DYNAMICPACKET_H__INCLUDED

#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>

#include "Network/Packet/Msg.h"
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

struct SMsgDynamic : public SMsg
{
	friend class CDynamicPacket;

	SMsgDynamic()
	{
		m_header.m_usType = 0;
		m_header.m_uiLength = sizeof(m_header);
		m_header.m_uiID = 0;
		m_header.m_ubLine = 0;
	}

	SMsgDynamic(unsigned short usType)
	{
		m_header.m_usType = usType;
		m_header.m_uiLength = sizeof(m_header);
		m_header.m_uiID = 0;
		m_header.m_ubLine = 0;
	}

	char szBuffer[ConMaxPacketSize];
};

class CDynamicPacket
{
public:
	enum
	{
		OP_READ = 1,
		OP_WRITE = 2,
		OP_NONE = 3
	};
public:
	CDynamicPacket(SMsgDynamic* pMsg, uint8 ubOP =  OP_READ);
	CDynamicPacket(uint16 usType, uint8 ubOP = OP_WRITE);
	CDynamicPacket(char* szBuffer, uint32 uiSize, uint8 ubOP = OP_READ);
	virtual ~CDynamicPacket();

	SMsgDynamic* GetMsg(){return m_pMsg;}
	void Serialize(void* data, uint32 len);

	bool ReadData(void* data, uint32 len);
	bool ReadInt8(int8* c);
	bool ReadUInt8(uint8* uc);
	bool ReadInt16(int16* s);
	bool ReadUInt16(uint16* us);
	bool ReadInt32(int32* l);
	bool ReadUInt32(uint32* ul);
	bool ReadInt64(int64* q);
	bool ReadUInt64(uint64* uq);
	bool ReadBool(bool* b);
	bool ReadFloat(float* f);
	bool ReadDouble(double* d);
	bool ReadString(char** str);

	bool WriteData(void* data, uint32 len);
	bool WriteInt8(int8 c);
	bool WriteUInt8(uint8 uc);
	bool WriteInt16(int16 s);
	bool WriteUInt16(uint16 us);
	bool WriteInt32(int32 l);
	bool WriteUInt32(uint32 ul);
	bool WriteInt64(int64 q);
	bool WriteUInt64(uint64 uq);
	bool WriteBool(bool b);
	bool WriteFloat(float f);
	bool WriteDouble(double d);
	bool WriteString(char* str);

	bool WritePacket(CDynamicPacket* packet);

	char* GetData() { return m_pData; }
	uint32 GetDataSize()
	{
		if (m_ubOperation == OP_READ)
		{
			return m_uiMaxByte;
		}
		else
		{
			return m_uiCurByte;
		}
	}

	bool Clone(CDynamicPacket* packet);
	void SetOperation(uint8 op) { m_ubOperation = op; }
	bool IsReading() { return m_ubOperation == OP_READ; }
	bool IsWriting() { return m_ubOperation == OP_WRITE; }
	bool IsError() { return m_bError; }

	uint32 GetCurrentPostion() { return m_uiCurByte; }
	void SetCurrentPosition(uint32 uiCurPos) { m_uiCurByte =  uiCurPos; }

private:
	CDynamicPacket();

	uint8 m_ubOperation;
	bool m_bError;

	SMsgDynamic* m_pMsg;
	char* m_pData;
	uint32 m_uiCurByte;
	uint32 m_uiMaxByte;

	bool m_bNew;
};

template <typename T>
CDynamicPacket& operator<< (CDynamicPacket& ar, T* obj)
{
	if (ar.IsReading())
	{
		if (obj == NULL)
		{
			obj = new T();
		}

		obj->Serialize(ar);
	}
	else
	{
		obj->Serialize(ar);
	}

	return ar;
}

template <typename T>
CDynamicPacket& operator<< (CDynamicPacket& ar, T& obj)
{
	obj.Serialize(ar);

	return ar;
}

template <>
CDynamicPacket& operator<<<int8>(CDynamicPacket& ar, int8& val);

template <>
CDynamicPacket& operator<<<uint8>(CDynamicPacket& ar, uint8& val);

template <>
CDynamicPacket& operator<<<int16>(CDynamicPacket& ar, int16& val);

template <>
CDynamicPacket& operator<<<uint16>(CDynamicPacket& ar, uint16& val);

template <>
CDynamicPacket& operator<<<int32>(CDynamicPacket& ar, int32& val);

template <>
CDynamicPacket& operator<<<uint32>(CDynamicPacket& ar, uint32& val);

template <>
CDynamicPacket& operator<<<int64>(CDynamicPacket& ar, int64& val);

template <>
CDynamicPacket& operator<<<uint64>(CDynamicPacket& ar, uint64& val);

template <>
CDynamicPacket& operator<<<bool>(CDynamicPacket& ar, bool& val);

template <>
CDynamicPacket& operator<<<float>(CDynamicPacket& ar, float& val);

template <>
CDynamicPacket& operator<<<double>(CDynamicPacket& ar, double& val);

template <>
CDynamicPacket& operator<<<std::string>(CDynamicPacket& ar, std::string& val);

template <>
CDynamicPacket& operator<<<XString>(CDynamicPacket& ar, XString& val);

template <typename T>
CDynamicPacket& operator<<(CDynamicPacket& ar, std::vector<T>& container)
{
	if (ar.IsReading())
	{
		container.clear();
		uint16 len = 0;
		ar << len;

		for (uint16 i = 0; i < len; ++i)
		{
			T temp;
			ar << temp;
			container.push_back(temp);
		}
	}
	else
	{
		uint16 len = static_cast<uint16>(container.size());
		ar << len;

		for (uint16 i = 0; i < len; ++i)
		{
			ar << container[i];
		}
	}

	return ar;
}

template <typename T>
CDynamicPacket& operator<<(CDynamicPacket& ar, std::list<T>& container)
{
	if (ar.IsReading())
	{
		container.clear();
		uint16 len = 0;
		ar << len;

		for (uint16 i = 0; i < len; ++i)
		{
			T temp;
			ar << temp;
			container.push_back(temp);
		}
	}
	else
	{
		uint16 len = static_cast<uint16>(container.size());
		ar << len;

		std::list<T>::iterator iter = container.begin();
		for (; iter != container.end(); ++iter)
		{
			ar << *iter;
		}
	}

	return ar;
}

template <typename T1, typename T2>
CDynamicPacket& operator<<(CDynamicPacket& ar, std::pair<T1, T2>& container)
{
	if (ar.IsReading())
	{
		ar << static_cast<T1>(container.first) << container.second;
	}
	else
	{
		ar << static_cast<T1>(container.first) << container.second;
	}
	return ar;
}

template <typename T1, typename T2>
CDynamicPacket& operator<<(CDynamicPacket& ar, std::map<T1, T2>& container)
{
	if (ar.IsReading())
	{
		container.clear();
		uint16 len = 0;
		ar << len;

		for (uint16 i = 0; i < len; ++i)
		{
			T1 key;
			T2 value;

			ar << key << value;
			container.insert(std::make_pair(key, value));
		}
	}
	else
	{
		uint16 len = static_cast<uint16>(container.size());
		ar << len;

		std::map<T1, T2>::iterator iter = container.begin();
		for (; iter != container.end(); ++iter)
		{
			ar << *iter;
		}
	}
	return ar;
}

template <typename T1, typename T2>
CDynamicPacket& operator<<(CDynamicPacket& ar, std::multimap<T1, T2>& container)
{
	if (ar.IsReading())
	{
		container.clear();
		uint16 len = 0;
		ar << len;

		for (uint16 i = 0; i < len; ++i)
		{
			T1 key;
			T2 value;

			ar << key << value;
			container.push_back(std::pair<T1, T2>(key, value));
		}
	}
	else
	{
		uint16 len = static_cast<uint16>(container.size());
		ar << len;

		std::multimap<T1, T2>::iterator iter = container.begin();
		for (; iter != container.end(); ++iter)
		{
			ar << *iter;
		}
	}
	return ar;
}

template <typename T>
CDynamicPacket& operator<<(CDynamicPacket& ar, std::set<T>& container)
{
	if (ar.IsReading())
	{
		container.clear();
		uint16 len = 0;
		ar << len;

		for (uint16 i = 0; i < len; ++i)
		{
			T temp;
			ar << temp;
			container.insert(temp);
		}
	}
	else
	{
		uint16 len = static_cast<uint16>(container.size());
		ar << len;

		std::set<T>::iterator iter = container.begin();
		for (; iter != container.end(); ++iter)
		{
			ar << static_cast<T>(*iter);
		}
	}
	return ar;
}

template <typename T>
CDynamicPacket& operator<<(CDynamicPacket& ar, std::queue<T>& container)
{
	if (ar.IsReading())
	{
		container.clear();
		uint16 len = 0;
		ar << len;

		for (uint16 i = 0; i < len; ++i)
		{
			T temp;
			ar << temp;
			container.push_back(temp);
		}
	}
	else
	{
		uint16 len = static_cast<uint16>(container.size());
		ar << len;

		std::list<T>::iterator iter = container.begin();
		for (; iter != container.end(); ++iter)
		{
			ar << *iter;
		}
	}
	return ar;
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif