#ifndef APPLICATION_SYSTEMMONITORLIB_SYSTEMINFOOBSERVER_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_SYSTEMINFOOBSERVER_H__INCLUDED

#include <vector>
#include <comdef.h>
#include <Wbemidl.h>
#include "XString/XString.h"

namespace Application
{
namespace SystemMonitor
{
    using Core::Utility::XString;
    using Core::XChar;

    typedef std::vector<VARIANT> VARIANTS;
    typedef std::vector<VARIANTS> VARIANTS_ARRAY;
    typedef std::vector<XString> STRINGS;
    typedef std::vector<STRINGS> STRINGS_ARRAY;
    typedef std::vector<unsigned short> USHORTS;
    typedef std::vector<unsigned long> ULONGS;

    class SystemInfoObserver
    {
    public:
        static SystemInfoObserver*  GetInstance();

        VARIANTS                    GetInfo(const XString& className, const XString& attributeName);
        VARIANTS_ARRAY              GetInfoArray(const XString& className, const std::vector<XString>& attributeNames);

        STRINGS                     GetStringValueWithClassAndAttribute(const XString& className, const XString& attributeName);
        USHORTS                     GetUShortValueWithClassAndAttribute(const XString& className, const XString& attributeName);
        ULONGS                      GetULongValueWithClassAndAttribute(const XString& className, const XString& attributeName);
        STRINGS_ARRAY               GetStringValueArrayWithClassAndAttibuteArray(const XString& className, const std::vector<XString>& attributeNames);
        std::vector< std::vector<XChar*> > GetAddressValueArrayWithClassAndAttibuteArray(const XString& className, const std::vector<XString>& attributeNames);

    private:
        static SystemInfoObserver*  mInstance;
        IWbemServices*              mSrv;
        IWbemLocator*               mLoc;

        SystemInfoObserver();
        ~SystemInfoObserver();

        void Init();

        SystemInfoObserver(const SystemInfoObserver& other);
        SystemInfoObserver& operator=(const SystemInfoObserver& other);
    };
}
}

#endif