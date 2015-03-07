#ifndef CORE_UTILITY_SINGLETON_H__INCLUDED
#define CORE_UTILITY_SINGLETON_H__INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "BasicDefines.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

template <typename T>
class TSingleton
{
public:
	TSingleton(){}
	virtual ~TSingleton(){}

	static T& GetInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new T;
		}

		return *s_pInstance;
	}

protected:
	static T* s_pInstance;

private:
	TSingleton<T>* operator=(const TSingleton<T>&);
};

template <typename T>
T* TSingleton<T>::s_pInstance = NULL;

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif