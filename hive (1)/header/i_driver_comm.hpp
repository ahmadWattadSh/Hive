#ifndef __ILRD_I_DRIVER_COMM__
#define __ILRD_I_DRIVER_COMM__

#include <memory>//shared_ptr
#include "driver_data.hpp"


namespace ilrd
{
    class IDriverComm
    {
    public:
        explicit IDriverComm() = default;
        virtual ~IDriverComm() = default;

        IDriverComm(const IDriverComm &other) = delete;
        IDriverComm &operator=(const IDriverComm &other) = delete;

        virtual std::shared_ptr<DriverData> ReceiveRequest() = 0;
        virtual void SendReply(const std::shared_ptr<DriverData> data_) = 0;

        virtual int GetFd()const = 0; // we add it because I need it later for the reactor
        virtual void Disconnect() = 0;

    };

} // namespace ilrd;




#endif /*__ILRD_HIVE__*/