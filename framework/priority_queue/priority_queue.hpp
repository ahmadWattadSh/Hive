/*******************************************************************************
* Author: Agbaria Ahmad
* Date: 13.03.24
* Version: 0
* Description: Priority Queue API
*******************************************************************************/
#ifndef __ILRD_PRIORITY_QUEUE_HPP__
#define __ILRD_PRIORITY_QUEUE_HPP__

#include <queue> //priority_queue
#include <iostream> //priority_queue

/******************************************************************************/

namespace ilrd
{
/*
wrapper that adds front() to std::priority_queue
Concepts:
T should be copyable and assignable.
*/
template <typename T, typename CONTAINER = std::vector<T>, typename COMPARE = std::less<typename CONTAINER::value_type>>
class PriorityQueue: private std::priority_queue<T,CONTAINER,COMPARE>
{
public:
    using priority_queue = std::priority_queue<T, CONTAINER, COMPARE>;

    PriorityQueue() {std::cout<< "ctor called" <<std::endl;}
    ~PriorityQueue() = default;
    PriorityQueue(const PriorityQueue&) = default;
    PriorityQueue& operator=(const PriorityQueue&) = default;
    PriorityQueue(const PriorityQueue&&) = delete;
    PriorityQueue& operator=(const PriorityQueue&&) = delete;

    const T& front() const;
    using priority_queue::push;
    using priority_queue::pop;
    using priority_queue::empty;

private:
}; 

template <typename T, typename CONTAINER, typename COMPARE>
const T& PriorityQueue<T, CONTAINER, COMPARE>::front() const
{
    return priority_queue::top();
}

}//namespace ilrd
/******************************************************************************/
#endif /*__ILRD_PRIORITY_QUEUE_HPP__*/