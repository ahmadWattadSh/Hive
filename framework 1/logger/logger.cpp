#include "logger.hpp"
#include <iostream>


namespace ilrd
{
    Logger::Logger(const std::string& file_path_ = "./log.txt", LevelType level_ = LevelType::ERROR)
    : m_file(std::ofstream(file_path_, std::ios::app)), m_level(level_), m_is_running(true)
    {
        
        m_thread = std::thread(&Logger::ThreadFunc, this);
    }

    Logger::~Logger()
    {
        m_is_running = false;
        
        try
        {
            m_thread.join();
        }
        catch(const std::system_error& e)
        {
            std::cerr << e.what() << '\n';
        }

    }

    void Logger::Log(LevelType level_, const std::string& message_, size_t line_, const std::string& file_)
    {
        //check if level is the same as m_level
        if(level_ < m_level.load(std::memory_order_relaxed))
        {
            return;
        }

        //create msg
        std::string ret_msg = CreateMsg(message_, line_, file_);

        //push
        m_wqueue.Push(ret_msg);

    }
    
    void Logger::SetLogFile(const std::string& file_)
    {
        //close the current
        m_file.close();
        //create another
        std::ofstream(file_, std::ios::app);

    }
    
    void Logger::SetLogLevel(LevelType level_)
    {
        //atomic so store
        m_level.store(level_, std::memory_order_relaxed);
        
    }


    std::string Logger::CreateMsg(const std::string& message_, size_t line_, const std::string& file_)
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);

        int year = 1900 + ltm->tm_year;
        int month = 1 + ltm->tm_mon;
        int day = ltm->tm_mday;
        int hour = ltm->tm_hour;
        int min = ltm->tm_min;
        int sec = ltm->tm_sec;


        std::string FullMessage =  
            "\033[4;36m" + file_  +
            " at line " + std::to_string(line_) + ":  with time_stamp: "
             + std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year) + " " +
            std::to_string(hour) + ":" + std::to_string(min) + ":" + std::to_string(sec)
             + "\n\n message: >" + message_ + "<\n\n";

        return FullMessage;

    }

    void Logger::ThreadFunc()
    {
        //need a msg
        std::string msg; 

        //
        while(m_is_running)
        {
            //pop from the queue msg
            bool status =m_wqueue.Pop(std::chrono::milliseconds(100), msg);
            
            if(!status)
            {
                //throw exception
                return;
            }

            //write
            m_file.write(msg.c_str(), msg.size());
        }
    }

}  
