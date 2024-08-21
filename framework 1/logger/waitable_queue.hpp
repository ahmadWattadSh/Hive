/*******************************************************************************
* Author: Ahmad Wattad
* Date:
* Version: 0
* Description: Waitable Queue API
*******************************************************************************/
#ifndef __ILRD_WAITABLE_QUEUE_HPP__
#define __ILRD_WAITABLE_QUEUE_HPP__

#include <queue>              // std::queue
#include <mutex>              // std::timed_mutex, std::lock_guard, std::unique_lock
#include <condition_variable> // std:condition_variable
#include <chrono>             // std::chrono

/******************************************************************************/

namespace ilrd
{
    /*
    Concepts:
    - T should be copyable and assignable.
    - CONTAINER should support an API identical to std::queue's API: push(), pop(), front(), empty().
    */
    template <typename T, typename CONTAINER = std::queue<T>>
    class WaitableQueue
    {
    public:
        WaitableQueue() = default;
        WaitableQueue(const WaitableQueue &other_) = delete;
        WaitableQueue &operator=(const WaitableQueue &other_) = delete;
        WaitableQueue(WaitableQueue &&other_) = delete;
        WaitableQueue &operator=(WaitableQueue &&other_) = delete;

        void Push(const T &item_);
        void Pop(T &out_item_);
        bool Pop(std::chrono::milliseconds timeout_ms_, T &out_item_);
        bool IsEmpty() const;

    private:
        CONTAINER m_container;
        mutable std::timed_mutex m_mutex;
        std::condition_variable_any m_cond;
        // It would be better factor this out method of the class, to avoid code bloat
        // (this method is not related to the template). Put it in some utility library.
        // Howver for calrity of this solution, just put it here:
        inline std::chrono::milliseconds CurrnetTimeMiliseconds()
        {
            
            std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch());

            return ms;
        }
    };

    template <typename T, typename CONTAINER>
    bool WaitableQueue<T, CONTAINER>::IsEmpty() const
    {
        std::unique_lock<std::timed_mutex> u_lock(m_mutex);
        return m_container.empty();

    }

    template <typename T, typename CONTAINER>
    void WaitableQueue<T, CONTAINER>::Push(const T &item_)
    {
        std::unique_lock<std::timed_mutex> u_lock(m_mutex);
        m_container.push(item_);
        m_cond.notify_one();

    }

    template <typename T, typename CONTAINER>
    void WaitableQueue<T, CONTAINER>::Pop(T &out_item_)
    {
        std::unique_lock<std::timed_mutex> u_lock(m_mutex);

        m_cond.wait(u_lock, [this]{return (m_container.empty() == false);});
        out_item_ = m_container.front();
        m_container.pop();
    }
    
    template <typename T, typename CONTAINER>
    bool WaitableQueue<T, CONTAINER>::Pop(std::chrono::milliseconds timeout_ms_, T &out_item_)
    {
       
        std::chrono::milliseconds start_time = CurrnetTimeMiliseconds();

        std::unique_lock<std::timed_mutex> u_lock(m_mutex, timeout_ms_);
    
        bool lockAcquired = u_lock.owns_lock();

        if(!lockAcquired)
        {
            return false;
        }
    
        timeout_ms_ -= ( CurrnetTimeMiliseconds() - start_time);

        bool status = m_cond.wait_for(u_lock, timeout_ms_, [this]{return (m_container.empty() == false);});
        if(!status)
        {
            return false;
        }

        out_item_ = m_container.front();
        m_container.pop();

        return true;
    }

}//namespace ilrd
#endif//__ILRD_WAITABLE_QUEUE_HPP__