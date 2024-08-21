#include "waitable_queue.hpp"
#include <iostream>
#include <thread>
#include <vector>

void producer(ilrd::WaitableQueue<int>& queue) {
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
        queue.Push(i);
        std::cout << "Produced: " << i << std::endl;
    }
}

void consumer(ilrd::WaitableQueue<int>& queue) {
    for (int i = 0; i < 10; ++i) {
        int value;
        bool success = queue.Pop(std::chrono::milliseconds(200), value);
        if (success) {
            std::cout << "Consumed: " << value << std::endl;
        } else {
            std::cout << "Timeout reached. Exiting." << std::endl;
            break;
        }
    }
}

int main() {
    ilrd::WaitableQueue<int> queue;

    // Create threads for producer and consumer
    std::thread producerThread(producer, std::ref(queue));
    std::thread consumerThread(consumer, std::ref(queue));

    // Join threads to wait for their completion
    producerThread.join();
    consumerThread.join();

    return 0;
}
