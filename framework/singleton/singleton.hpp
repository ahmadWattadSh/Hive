/*******************************************************************************
  Name: Ahmad
  Project: 
  Reviewer: ---
  Date: 
  Version: 1.0
*******************************************************************************/

#ifndef __HRD_SIMPLE_SINGLETON_HPP__
#define __HRD_SIMPLE_SINGLETON_HPP__

/*********************************LIBRARIES************************************/

#include <memory>  //shared pointer
#include <mutex>   //mutex
#include <atomic>  //atomic, memory_order_...()
#include <cstdlib> //atexit()

/********************************DEFINITIONS***********************************/

namespace ilrd
{

template <typename T>// T must be uncopyable, unassignable, must have private default ctor and it must have singltone friend   
class Singleton
{
public:
    static T* GetInstance();
    ~Singleton()= delete;
    
    Singleton()= delete;
    Singleton(const Singleton& o_) = delete;
    Singleton& operator=(const Singleton& o_) = delete;

private:
    static void CleanUp();

    static std::atomic<T*> m_instance_ptr;
    static std::mutex m_mutex;
};

template <typename T>
std::atomic<T*> Singleton<T>::m_instance_ptr(nullptr);

template <typename T>
std::mutex Singleton<T>::m_mutex;

template<typename T>
T* Singleton<T>::GetInstance()
{
    //regitster atexit function
    int status = atexit(Singleton<T>::CleanUp);
    if(0 != status)
    {
        return nullptr;
    }

    //Memory barrier, no reads or writes in the current thread can be reordered before this load
    std::atomic_thread_fence(std::memory_order_acquire); //no reads or writes in the current thread can be reordered before this load. All writes in other threads that release the same atomic variable are visible in the current thread

    //Instanciate temp in an atomic operation
    T* temp = m_instance_ptr.load(std::memory_order_relaxed); 

    if(nullptr == temp)
    {
        m_mutex.lock();

        //assign instance ptr to temp
        temp = m_instance_ptr.load(std::memory_order_relaxed);

        if(nullptr == m_instance_ptr)
        {
           // try
           // {
                temp = new T;
           // }
            //catch(const std::bad_alloc exception &e_)
           // {
           //     std::cout << "Bad alloc" << e_.what() << std::endl;
           //     //release mutex
           //     throw e_;
           // }

            //Memory barrier, no reads or writes in the current thread can be reordered after this store
            std::atomic_thread_fence(std::memory_order_release);
            
            //instance ptr = temp (atomic operation)
            m_instance_ptr.store(temp, std::memory_order_relaxed);

        }

        m_mutex.unlock();
    }

    return temp;
}

template<typename T>
void Singleton<T>::CleanUp()
{
    T* tmp = m_instance_ptr;
    
    m_instance_ptr.store(reinterpret_cast<T*>(0xDEADBEEF), std::memory_order_acquire);  
    delete tmp;

}

}//namespace ilrd
#endif /*(__HRD_SIMPLE_SINGLETON_HPP__)*/
