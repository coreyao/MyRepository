#include <vector>
#include <cassert>
#include <ctime>

#include "Logger/Logger.h"
#include "Params/LogParam.h"
#include "Appenders/DebugWindowAppender.h"
#include "Appenders/ConsoleAppender.h"
#include "Appenders/FileAppender.h"
#include "Lockers/Guard.h"
#include "Lockers/Locker.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

using Core::Utility::CLocker;
using Core::Utility::CGuard;
using Core::Utility::XString;

CLocker* CLogger::sLocker = new CLocker;

typedef std::vector<CLogAppender*> LogAppenders;

class CLogger::LoggerManager
{
public:
    LoggerManager();
    ~LoggerManager();

    void AddAppender(CLogAppender* appender);
    void LogMessage(const XString& message);

    size_t GetMode() const { return mMode; }
    void SetMode(size_t val) { mMode = val; }

    const LogAppenders* GetLogAppenders() const { return mLogAppenders; }

    E_MessageLevel GetMessageLevel() const { return mMessageLevel; }
    void SetMessageLevel(E_MessageLevel val) { mMessageLevel = val; }

    //Locker* GetLocker() { return mLocker; }

private:
    size_t mMode;
    LogAppenders* mLogAppenders;
    //Locker* mLocker;
    E_MessageLevel mMessageLevel;
    
    DISALLOW_COPY_AND_ASSIGN(CLogger::LoggerManager);
};

CLogger::LoggerManager::LoggerManager()
: mMode(0)
//, mLocker(new Locker)
, mMessageLevel(MESSAGE_LEVEL_NO_MESSAGE)
, mLogAppenders(new LogAppenders)
{
    //mMode and mMessageLevel will be read from a configuration file later
    mMode = LOGMODE_DEBUGWINDOW | LOGMODE_CONSOLE /*| LOGMODE_FILE | */;
    mMessageLevel = MESSAGE_LEVEL_ALL_MESSAGE;
}

CLogger::LoggerManager::~LoggerManager()
{
    LogAppenders::iterator iter = mLogAppenders->begin();
    LogAppenders::iterator endIter = mLogAppenders->end();

    while (iter != endIter)
    {
        SAFE_DELETE(*iter);
        ++iter;
    }

    SAFE_DELETE(mLogAppenders);
//    SAFE_DELETE(mLocker);
}

CLogger::CLogger()
: mLoggerManager(new LoggerManager)
{
	Init();
}

CLogger::~CLogger()
{
    SAFE_DELETE(mLoggerManager);
}

void CLogger::Init()
{
    if (mLoggerManager->GetMode() & LOGMODE_DEBUGWINDOW)
    {
        mLoggerManager->AddAppender(new CDebugWindowAppender);
    }
    
    if (mLoggerManager->GetMode() & LOGMODE_CONSOLE)
    {
        mLoggerManager->AddAppender(new CConsoleAppender);
    }

    if (mLoggerManager->GetMode() & LOGMODE_FILE)
    {
        mLoggerManager->AddAppender(new CFileAppender);
    }
}

void CLogger::Log(const XChar* message)
{
    E_MessageLevel messageLevel = mLoggerManager->GetMessageLevel();
    if (messageLevel > MESSAGE_LEVEL_ERROR_MESSAGE)
    {
        mLoggerManager->LogMessage(message);
    }
}

void CLogger::LogError(const XChar* message, const XChar* fileName, size_t lineNumber, const XChar* functionName)
{
    E_MessageLevel messageLevel = mLoggerManager->GetMessageLevel();
    if (messageLevel > MESSAGE_LEVEL_NO_MESSAGE)
    {
        XString errMessage;
        errMessage.Format(TEXT("Error: (%s) Function: (%s) File: (%s) Line: %d"), message, functionName, fileName, lineNumber);
        mLoggerManager->LogMessage(errMessage);
    }
}

void CLogger::LoggerManager::AddAppender(CLogAppender* appender)
{
    assert(appender != NULL);
    mLogAppenders->push_back(appender);
}

void CLogger::LoggerManager::LogMessage(const XString& message)
{
    LogAppenders::const_iterator iter = mLogAppenders->begin();
    LogAppenders::const_iterator iterEnd = mLogAppenders->end();

    while (iter != iterEnd)
    {
        CLogParam param;
        param.SetLogTime(time(NULL));
        param.SetMessage(message);

        CGuard guard(sLocker);
        (*iter)->Log(param);
        ++iter;
    }
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
