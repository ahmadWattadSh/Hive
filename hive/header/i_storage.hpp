#ifndef __ILRD_I_STORAGE__
#define __ILRD_I_STORAGE__

#include <memory>//shared_ptr
#include "driver_data.hpp"


namespace ilrd
{
    class IStorage
    {
    public:
        IStorage() = default;
        virtual ~IStorage() = default;

        IStorage(const IStorage &other) = delete;
        IStorage &operator=(const IStorage &other) = delete;

        virtual void Read(std::shared_ptr<DriverData> data_) const = 0;
        virtual void Write(std::shared_ptr<DriverData> data_) = 0;
    };

} // namespace ilrd;




#endif /*__ILRD_I_STORAGE__*/