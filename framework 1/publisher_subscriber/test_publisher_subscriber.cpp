#include "msg_broker.hpp"
#include <iostream>

// Define a simple message struct for testing
struct SimpleMessage {
    int data;
};

// Define a simple subscriber class
class SimpleSubscriber {
public:
    void Update(const SimpleMessage& msg) {
        std::cout << "Received message: " << msg.data << std::endl;
    }

    void Disconnect() {
        std::cout << "Disconnected." << std::endl;
    }
};


int main() {
    // Create a dispatcher
    ilrd::Dispatcher<SimpleMessage> dispatcher;
    
    // Create a subscriber
    SimpleSubscriber subscriber;


    // Create a callback for the subscriber
    auto actionMethod = &SimpleSubscriber::Update;
    auto disconnectMethod = &SimpleSubscriber::Disconnect;
    
    ilrd::CallBack<SimpleMessage, SimpleSubscriber> callback(&dispatcher, subscriber, actionMethod, disconnectMethod);

    // Create another subscriber and callback
    SimpleSubscriber subscriber2;
    ilrd::CallBack<SimpleMessage, SimpleSubscriber> callback2(&dispatcher, subscriber2, actionMethod, disconnectMethod);

    // Notify all subscribers
    SimpleMessage msg1{100};
    dispatcher.NotifyAll(msg1);

    // Notify again
    SimpleMessage msg2{200};
    dispatcher.NotifyAll(msg2);

    // Notify once more
    SimpleMessage msg3{300};
    dispatcher.NotifyAll(msg3);

    // Clean up remaining subscribers
    //dispatcher.NotifyDiscAll();

    return 0;
}

