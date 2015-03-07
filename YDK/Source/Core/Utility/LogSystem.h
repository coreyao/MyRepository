#ifndef CORE_LOGSYSTEM_LOGSYSTEM_H__INCLUDED
#define CORE_LOGSYSTEM_LOGSYSTEM_H__INCLUDED

#include "Logger/Logger.h"

namespace Core
{

#define LOG(message) Utility::TSingleton<Core::Utility::CLogger>::GetInstance().Log(message)

#ifdef UNICODE
#define LOG_ERROR(message) Utility::TSingleton<Core::Utility::CLogger>::GetInstance().LogError(message, __FILEW__, __LINE__, __FUNCTIONW__)
#else
#define LOG_ERROR(message) Utility::TSingleton<Core::Utility::CLogger>::GetInstance().LogError(message, __FILE__, __LINE__, __FUNCTION__)
#endif

}

#endif