/*******************************************************************************
* Author: Ahmad Wattad
* Date:
* Version: 0
* Description:Logger API
*******************************************************************************/
#ifndef __ILRD_LOGGER_HPP__
#define __ILRD_LOGGER_HPP__

#include <fstream>              //  std::ofstream
#include <thread>               //  std::thread
#include <atomic>               //  atomic

#include "singleton.hpp"        //  Singleton
#include "waitable_queue.hpp"   //  WaitableQueue
/******************************************************************************/
namespace ilrd
{

class Logger
{
public:
    enum class LevelType
    {
        INFO = 0,
        WARNING = 1,
        ERROR = 2
    };

    Logger(const Logger& o_) = delete;
    Logger& operator=(const Logger& o_) = delete;
    Logger(Logger&& o_) = delete;
    Logger& operator=(Logger&& o_) = delete;   

    void Log(LevelType level_, const std::string& message_, size_t line_, const std::string& file_);
    void SetLogFile(const std::string& file_);  // Support thread safe.
    void SetLogLevel(LevelType level_); // Support thread safe.
    
private:
    Logger(const std::string& file_path_ = "./log.txt", LevelType level_ = LevelType::ERROR);
    ~Logger();//private - to prevent inheriting

    //change the ostringstream with string = this function
    static std::string CreateMsg(const std::string& message_, size_t line_, const std::string& file_);
    //thread function
    void ThreadFunc();

    std::mutex m_mutex;
    std::ofstream m_file;
    LevelType m_level;  
    std::thread m_thread;
    ilrd::WaitableQueue<std::string> m_wqueue;
    int m_is_running;

    friend class Singleton<Logger>;
};

}// namespace ilrd
/******************************************************************************/
#endif// __ILRD_LOGGER_HPP__