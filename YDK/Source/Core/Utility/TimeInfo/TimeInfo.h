#ifndef CORE_UTILITY_TIMEINFO_H__INCLUDED
#define CORE_UTILITY_TIMEINFO_H__INCLUDED

#include <ctime>
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CTimeInfo
{
public:
	CTimeInfo();
	CTimeInfo(time_t seconds);
	CTimeInfo(const CTimeInfo& other);
	CTimeInfo& operator=(const CTimeInfo& other);

	XString ToString();
	uint32 GetNowTime();

private:
	void ConvertSecondsToTimeInfo(time_t seconds);

	size_t mSeconds;
	size_t mMinutes;
	size_t mHours;
	size_t mDays;
	size_t mMonths;
	size_t mYears;

	struct tm m_tm;
	
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif