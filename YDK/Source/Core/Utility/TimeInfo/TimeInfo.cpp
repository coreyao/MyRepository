#define  WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "TimeInfo/TimeInfo.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

namespace
{
	const size_t TIME_OFFSET	= 60;
	const size_t DAY_OFFSET		= 24;
	const size_t MONTH_OFFSET   = 30;
	const size_t YEAR_OFFSET	= 12;

	const size_t MINUTE_IN_SECONDS	= TIME_OFFSET;
	const size_t HOUR_IN_SECONDS	= TIME_OFFSET * MINUTE_IN_SECONDS;
	const size_t DAY_IN_SECONDS		= DAY_OFFSET * HOUR_IN_SECONDS;
	const size_t MONTH_IN_SECONDS   = MONTH_OFFSET * DAY_IN_SECONDS;
	const size_t YEAR_IN_SECONDS	= YEAR_OFFSET * MONTH_IN_SECONDS;
}

CTimeInfo::CTimeInfo()
: mSeconds(0)
, mMinutes(0)
, mHours(0)
, mDays(0)
, mMonths(0)
, mYears(0)
{
}

CTimeInfo::CTimeInfo( time_t seconds )
: mSeconds(0)
, mMinutes(0)
, mHours(0)
, mDays(0)
, mMonths(0)
, mYears(0)
{
	ConvertSecondsToTimeInfo(seconds);
}

CTimeInfo::CTimeInfo( const CTimeInfo& other )
{
	*this = other;
}

CTimeInfo& CTimeInfo::operator=( const CTimeInfo& other )
{
	if (this != &other)
	{
		this->mSeconds = other.mSeconds;
		this->mMinutes = other.mMinutes;
		this->mHours = other.mHours;
		this->mDays = other.mDays;
		this->mMonths = other.mMonths;
		this->mYears = other.mYears;
	}

	return *this;
}

XString CTimeInfo::ToString()
{
	StreamWriter timestring;

	/*if ( mYears )
	{
		timestring << mYears << TEXT(" Year(s) ");
	} 

	if (mMonths)
	{
		timestring << mMonths << TEXT(" Month(s) ");
	}

	if ( mDays )
	{
		if ( !timestring.str().empty() )
			timestring << TEXT(" ");

		timestring << mDays << TEXT(" Day(s) ");
	}

	if ( mHours )
	{
		if ( !timestring.str().empty() )
			timestring << TEXT(" ");

		timestring << mHours << TEXT(" Hour(s) ");
	}

	if ( mMinutes )
	{
		if ( !timestring.str().empty() )
			timestring << TEXT(" ");

		timestring << mMinutes << TEXT(" Minute(s) ");
	}

	if ( !timestring.str().empty() )
		timestring << TEXT(" ");

	timestring << mSeconds << TEXT(" Second(s) ");*/

	timestring << mHours << TEXT(":") << mMinutes << TEXT(":") << mSeconds;

	return timestring.str();
}

void CTimeInfo::ConvertSecondsToTimeInfo( time_t seconds )
{
	/*mYears = static_cast<size_t>(seconds ? seconds / YEAR_IN_SECONDS : 0);
	seconds = seconds - mYears * YEAR_IN_SECONDS ;

	mMonths = static_cast<size_t>(seconds ? seconds / MONTH_IN_SECONDS : 0);
	seconds = seconds - mMonths * MONTH_IN_SECONDS;

	mDays = static_cast<size_t>(seconds ? seconds / DAY_IN_SECONDS : 0);
	seconds = seconds - mDays * DAY_IN_SECONDS ;

	mHours = static_cast<size_t>(seconds ? seconds / HOUR_IN_SECONDS : 0);
	seconds = seconds - mHours * HOUR_IN_SECONDS ;

	mMinutes = static_cast<size_t>(seconds ? seconds / MINUTE_IN_SECONDS : 0);

	mSeconds = static_cast<size_t>(seconds - mMinutes * MINUTE_IN_SECONDS);*/
	localtime_s(&m_tm, &seconds);

	/*mYears = m_tm.tm_year;
	mMonths = m_tm.tm_mon;
	mDays = m_tm.tm_mday;*/
	mHours = m_tm.tm_hour;
	mMinutes = m_tm.tm_min;
	mSeconds = m_tm.tm_sec;
}

uint32 CTimeInfo::GetNowTime()
{
	time_t curTime = ::time(NULL);
	return static_cast<uint32>(curTime);
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
