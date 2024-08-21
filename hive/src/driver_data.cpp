

#include <cstddef>//size_t
#include <vector>//vector
#include <algorithm>//vector

#include "driver_data.hpp"


namespace ilrd
{
 
    DriverData::DriverData(ActionType type_, char * handle_, size_t offset_, size_t len_): 
    m_offset(offset_), m_len(len_)
    {
        m_status = Status::SUCCESS;
        m_type =type_;
        std::copy(handle_, handle_+ sizeof(handle_), m_handle);
        m_buff.clear();
    }

} // namespace ilrd;



