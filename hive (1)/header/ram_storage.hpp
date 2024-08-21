#ifndef __ILRD_RAM_STORAGE__
#define __ILRD_RAM_STORAGE__

#include <memory>//shared_ptr
#include <cstddef>//size_t
#include <unistd.h>//read

#include "driver_data.hpp"
#include "i_storage.hpp"


namespace ilrd
{
    class RAMStorage : public IStorage
    {
    public:
        explicit RAMStorage(size_t size_);
        explicit RAMStorage(size_t blockSize_, size_t numBlocks_);
        ~RAMStorage() = default;
        // disable copy and assignment operator
        void Read(std::shared_ptr<DriverData> data_) const override;
        void Write(std::shared_ptr<DriverData> data_) override;
      
    private:
        std::vector<unsigned char> m_pool;
        size_t m_len;
    };

} // namespace ilrd;





#endif /*__ILRD_RAM_STORAGE__*/