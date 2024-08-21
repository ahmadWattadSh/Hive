/*******************************************************************************
  Name: Ahmad
  Project: 
  Reviewer: ---
  Date: 
  Version: 1.0
*******************************************************************************/

#ifndef __HRD_FACTORY_HPP__
#define __HRD_FACTORY_HPP__

/*********************************LIBRARIES************************************/

#include <memory>  //shared pointer
#include <mutex>   //mutex
#include <atomic>  //atomic, memory_order_...()
#include <cstdlib> //atexit()
#include "singleton.hpp" 
#include <functional>
#include <map>
#include <iostream>
/********************************DEFINITIONS***********************************/

namespace ilrd
{

template<typename BASE, typename KEY, typename... ARGS>
class Factory
{
public:
    void Register(KEY key, std::function<std::shared_ptr<BASE>(ARGS...)> creator); 
    std::shared_ptr<BASE> Create(KEY key, ARGS... args);

    Factory(const Factory&)=delete;
    Factory& operator=(const Factory&)=delete;
private:
    std::map<KEY,std::function<std::shared_ptr<BASE>(ARGS...)>> m_map_creators;

    ~Factory()=default;
    Factory()=default;
    
    friend class Singleton<Factory<BASE, KEY, ARGS...>>;
};


template<typename BASE, typename KEY, typename... ARGS>
void Factory<BASE, KEY, ARGS...>::Register(KEY key, std::function<std::shared_ptr<BASE>(ARGS...)> creator)
{
   m_map_creators[key] = creator;
}

template<typename BASE, typename KEY, typename... ARGS>
std::shared_ptr<BASE> Factory<BASE, KEY, ARGS...>::Create(KEY key, ARGS... args)
{
  return m_map_creators[key](args...);

}



}//namespace ilrd


#endif /*(__HRD_FACTORY_HPP__)*/
