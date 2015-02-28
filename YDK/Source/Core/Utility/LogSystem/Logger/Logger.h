#ifndef CORE_LOGSYSTEM_LOGGER_H__INCLUDED
#define CORE_LOGSYSTEM_LOGGER_H__INCLUDED

#include "UtilityTypes.h"
#include "Singleton.h"

namespace Core{namespace Utility{class CLocker;}}

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CLogger
{
public:
    void Log(const XChar* message);
    void LogError(const XChar* message, const XChar* fileName, size_t lineNumber, const XChar* functionName);

	CLogger();
	~CLogger();

private:
    void Init();

    class LoggerManager;
    LoggerManager* mLoggerManager;
    
    static Core::Utility::CLocker* sLocker;

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif