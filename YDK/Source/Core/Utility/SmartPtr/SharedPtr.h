#ifndef CORE_UTILITY_SHAREDPTR_H__INCLUDED
#define CORE_UTILITY_SHAREDPTR_H__INCLUDED

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

template <typename T>
class SharedPtr;

template <typename T>
class CSharedData
{
private:
	friend class SharedPtr<T>;

	explicit CSharedData(T* pT) 
		: m_ptr(pT)
		, m_uiCount(1)
	{
	}

	~CSharedData()
	{
		delete m_ptr;
	}

	void operator++()
	{
		++m_uiCount;
	}

	void operator--()
	{
		--m_uiCount;
		if (m_uiCount == 0)
		{
			delete this;
		}
	}

	T& operator*() const
	{
		return *m_ptr;
	}

	T* operator->() const
	{
		return m_ptr;
	}

	bool operator==(T* pT) const
	{
		return m_ptr == pT;
	}

	T* Get() const
	{
		return m_ptr;
	}

	uint32 GetCount() const
	{
		return m_uiCount;
	}

private:
	T* m_ptr;
	uint32 m_uiCount;
};

template <typename T>
class SharedPtr
{
	typedef CSharedData<T> element;

public:
	explicit SharedPtr(T* pT) : m_pData(NULL)
	{
		m_pData = new element(pT);
	}

	explicit SharedPtr() : m_pData(NULL)
	{
	}

	SharedPtr(const SharedPtr<T>& rT)
	{
		m_pData = rT.GetElement();

		if (m_pData != NULL)
		{
			++(*m_pData);
		}
	}

	SharedPtr<T>& operator=(const SharedPtr<T>& rT)
	{
		m_pData = rT.GetElement();
		if (m_pData != NULL)
		{
			++(*m_pData);
		}

		return *this;
	}

	T& operator*() const
	{
		return m_pData->operator*();
	}

	T& operator->() const
	{
		return m_pData->operator ->();
	}

	bool operator==(const SharedPtr<T>& rT) const
	{
		return rT.GetElement() == m_pData;
	}

	bool operator==(T* pT) const
	{
		if (m_pData == NULL)
		{
			return pT == NULL;
		}

		return *m_pData == pT;
	}

	~SharedPtr()
	{
		if (m_pData != NULL)
		{
			--(*m_pData);
		}
	}

	T* Get() const
	{
		if (m_pData == NULL)
		{
			return NULL;
		}
		else
		{
			return m_pData->Get();
		}
	}

	void Release()
	{
		if (m_pData != NULL)
		{
			--(*m_pData);

			m_pData = NULL;
		}
	}

	void Reset(T* pT)
	{
		if (m_pData != NULL)
		{
			--(*m_pData);

			m_pData = NULL;
		}

		m_pData = new element(pT);
	}

	bool IsNull()
	{
		return m_pData == NULL;
	}

private:
	element* GetElement() const
	{
		return m_pData;
	}

	element* m_pData;
};


END_NAMESPACE(Utility)
END_NAMESPACE(Core)


#endif