/*******************************************************************************
* Author: Agbaria Ahmad
* Date: 21.03.24
* Version: 0
* Description: Task API
*******************************************************************************/
#ifndef __ILRD_I_TASK_HPP__
#define __ILRD_I_TASK_HPP__

#include <cstddef> //size_t
#include <memory> //shared_ptr
/******************************************************************************/
namespace ilrd
{

enum class TaskPriority
{
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2, 
    //NOTE: add prioritys before admin
    ADMIN = 3
};

class ITask
{
public:
    explicit ITask(TaskPriority p_ = TaskPriority::LOW);
    virtual ~ITask()= default;
    ITask(const ITask&) = default;
    ITask& operator= (const ITask&) = default;
    ITask(ITask&&) = delete; 
    ITask& operator= (ITask&&) = delete;

    virtual void Execute() = 0;
    bool operator<(const ITask& other_) const; 
    
private:
    TaskPriority m_priority;  
};

}//namespace ilrd
/******************************************************************************/
#endif// __ILRD_I_TASK_HPP__
