
#include <string>           //  string
#include <bits/types.h>     //  std::time_t / std::tm
#include <system_error>     //  std::system_error

#include "logger.hpp"        //  Logger
/******************************************************************************/
namespace ilrd
{

std::string Logger::CreateMsg(const std::string& message_, size_t line_, const std::string& file_)
{
    std::string msg = message_ + " at line:" + std::to_string(line_) + " at file:" + file_ + "\0";
    return msg;
}

void Logger::ThreadFunc()
{
    std::string msg; 

    while(!m_wqueue.IsEmpty() || m_is_running)
    {
        if(m_wqueue.Pop(std::chrono::milliseconds(100), msg))
        {
            auto now = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            std::tm *timeinfo = std::localtime(&now_time);
            char timeString[20];
            std::strftime(timeString, sizeof(timeString), "%d/%m/%Y - %H:%M", timeinfo);
            std::string logMessage = "[" + std::string(timeString) + "] " + msg + "\n";

            m_file.write(logMessage.c_str(), logMessage.size());
        }
    }
}

Logger::Logger(const std::string& file_path_, LevelType level_)
            :m_file(std::ofstream(file_path_, std::ios::app)), m_level(level_),
            m_is_running(true)
{
    //m_thread is initialized in the the body after all the MIL has been initialized
    m_thread = std::thread(&Logger::ThreadFunc, this);
}

Logger::~Logger()
{
    m_is_running = false;
    try
    {
        m_thread.join();
        //m_file.close();
    }
    catch(const std::system_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Logger::Log(LevelType level_, const std::string& message_, size_t line_, const std::string& file_)
{
    if(level_ < m_level.load(std::memory_order_relaxed))
    {
        return;
    }
    
    std::string ret_msg = CreateMsg(message_, line_, file_);
    
    m_wqueue.Push(ret_msg);

}

void Logger::SetLogFile(const std::string& file_)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_file.close();
    m_file.open(file_, std::ios::app);
}

void Logger::SetLogLevel(LevelType level_)
{
    m_level.store(level_, std::memory_order_relaxed);
}


}   //  namesapce ilrd.