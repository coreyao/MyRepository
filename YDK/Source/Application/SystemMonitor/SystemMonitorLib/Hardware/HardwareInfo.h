#ifndef APPLICATION_SYSTEMMONITORLIB_HARDWAREINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_HARDWAREINFO_H__INCLUDED

#include "XString/XString.h"

using Core::Utility::XString;
using Core::XChar;

namespace Application
{
namespace SystemMonitor
{
    class HardwareInfo
    {
    public:
        HardwareInfo();
        ~HardwareInfo();

        XString GetCompanyName() const { return mCompanyName; }
        XString GetProductName() const { return mProductName; }

    protected:
        XString mCompanyName;
        XString mProductName;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int /* file_version */)
        {
            ar & mCompanyName & mProductName;
        }
    };
}
}

#endif