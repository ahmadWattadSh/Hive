#include "priority_queue.hpp"
#include <iostream>

int main()
{
    using namespace ilrd;

    // Create a PriorityQueue of integers
    PriorityQueue<int> pq;

    // Push some elements into the priority queue
    pq.push(10);
    pq.push(20);
    pq.push(5);
    pq.push(30);

    // Check if the priority queue is empty
    if (!pq.empty()) {
        std::cout << "Priority Queue is not empty." << std::endl;
    }

    // Get the front (top) element of the priority queue
    std::cout << "Front element of the priority queue: " << pq.front() << std::endl;

    // Pop elements from the priority queue and print them
    while (!pq.empty()) {
        std::cout << "Popped element: " << pq.front() << std::endl;
        pq.pop();
    }

    // Check if the priority queue is empty after popping all elements
    if (pq.empty()) {
        std::cout << "Priority Queue is empty." << std::endl;
    }

    return 0;
}
