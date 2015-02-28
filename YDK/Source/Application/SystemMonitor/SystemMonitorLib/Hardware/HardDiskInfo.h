#ifndef APPLICATION_SYSTEMMONITORLIB_HARDDISKINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_HARDDISKINFO_H__INCLUDED

#include "Hardware/HardwareInfo.h"
#include <vector>

namespace Application
{
namespace SystemMonitor
{
    class HardDiskInfo : public HardwareInfo
    {
    public:
        struct PartitionInfo
        {
            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive& ar, const unsigned int /* file_version */)
            {
                ar & mPartitionName & mPartitionVolumeName & mPartitionSize & mPartitionFreespace;
            }

            XString mPartitionName;
            XString mPartitionVolumeName;
            float mPartitionSize;
            float mPartitionFreespace;
        };

        HardDiskInfo();
        ~HardDiskInfo();

        void Update();

        float GetTotalSize() const { return mTotalSize; }
        float GetTotalFreespace() const { return mTotalFreespace; }
        size_t GetPartitionCount() const { return mPartitionCount; }
        std::vector<PartitionInfo> GetPartitionInfos() const { return mPartitionInfos; }

    private:
        float   mTotalSize;
        float   mTotalFreespace;
        size_t  mPartitionCount;
        std::vector<PartitionInfo> mPartitionInfos;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int /* file_version */)
        {
            ar & boost::serialization::base_object<HardwareInfo>(*this);
            ar & mTotalSize & mTotalFreespace & mPartitionCount;
            ar & mPartitionInfos;
        }
 
    };
}
}


#endif