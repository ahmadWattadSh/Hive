#ifndef __ILRD_NBD_DRIVER_COMM__
#define __ILRD_NBD_DRIVER_COMM__

#include<cstddef>
#include <memory>
#include <thread>

#include "driver_data.hpp"
#include "i_driver_comm.hpp"
#include "utils.hpp"


namespace ilrd
{
    class NBDDriverComm : public IDriverComm
    {
    public:
        explicit NBDDriverComm(const std::string& pathDevFile_, size_t size_);
        explicit NBDDriverComm(const std::string& pathDevFile_, size_t sizeOfBLock_, size_t numOfBLock_);// Ctor may throw opening socket exception
        ~NBDDriverComm() noexcept;

        NBDDriverComm(const NBDDriverComm& other_) = delete;
        NBDDriverComm& operator=(const NBDDriverComm& other_) = delete;

        std::shared_ptr<DriverData> ReceiveRequest() override;             // can throw read exception
        void SendReply(const std::shared_ptr<DriverData> data_) override; // can throw write exception
        void Disconnect() override;                                        // can throw close exception
        int GetFd()const override;

    private:
        // You will have more private members and functions - implementation...
        int m_user_sk;
        int m_kernel_sk;
        int m_nbd;
        std::thread m_thread;
        
        void NBDListner();

    };

} // namespace ilrd;





#endif /*__ILRD_NBD_DRIVER_COMM__*/
