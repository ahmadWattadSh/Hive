#include <memory>//shared_ptr
#include <cstddef>//size_t
#include <unistd.h>//read
#include <assert.h>//assert

#include "driver_data.hpp"
#include "ram_storage.hpp"
#include "i_storage.hpp"


#include <string.h>


namespace ilrd
{
   

RAMStorage::RAMStorage(size_t size_): m_len(size_)
{
    m_pool.resize(size_);
}

RAMStorage::RAMStorage(size_t block_size_, size_t num_blocks_) : RAMStorage(num_blocks_ * block_size_)
{
}

void RAMStorage::Read(std::shared_ptr<DriverData> data_)const 
{
    //size_t end_pos = data_->m_offset + data_->m_len;

    //ssert(end_pos < m_len);

    data_->m_buff.assign(m_pool.begin() + data_->m_offset, m_pool.begin() + data_->m_offset + data_->m_len + 1);

    data_->m_status = Status::SUCCESS;
}

void RAMStorage::Write(std::shared_ptr<DriverData> data_)
{
    //assert(end_pos < m_len);

    std::copy(data_->m_buff.begin(), data_->m_buff.begin() + data_->m_len, m_pool.begin() + data_->m_offset);

    data_->m_status = Status::SUCCESS;
}

} // namespace ilrd;

/*
#include <iostream>

int main()
{
    // Create a RAMStorage object with a size of 100 bytes
    ilrd::RAMStorage ram_storage(100);

    // Create a DriverData object to read from RAMStorage
    char read_handle[] = "READ_HANDLE";
    std::shared_ptr<ilrd::DriverData> read_data = std::make_shared<ilrd::DriverData>(
        ilrd::ActionType::READ, read_handle, 0, 10);

    // Read 10 bytes from offset 0
    ram_storage.Read(read_data);

    // Print the read buffer
    std::cout << "Read Data:" << std::endl;
    for (const auto& byte : read_data->m_buff) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // Create a DriverData object to write to RAMStorage
    char write_handle[] = "WRITE_HANDLE";
    std::shared_ptr<ilrd::DriverData> write_data = std::make_shared<ilrd::DriverData>(
        ilrd::ActionType::WRITE, write_handle, 0, 5);
    write_data->m_buff = {1, 2, 3, 4, 5};

    // Write 5 bytes to offset 0
    ram_storage.Write(write_data);

    // Reset read_data for the next read operation
    read_data = std::make_shared<ilrd::DriverData>(ilrd::ActionType::READ, read_handle, 0, 10);

    // Read the same 10 bytes again
    ram_storage.Read(read_data);

    // Print the updated read buffer
    std::cout << "After Write, Read Data:" << std::endl;
    for (const auto& byte : read_data->m_buff) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    return 0;
}

*/





