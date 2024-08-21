#include <iostream>
#include <string>
#include <cstring>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <sstream>

#include "driver_data.hpp"
#include "ram_storage.hpp"
#include "nbd_driver_comm.hpp"
#include <cassert>

#define MAX_EVENTS 10
/******************************************************************************/
using namespace ilrd;

volatile sig_atomic_t stop_flag = 0;

void SignalHandler(int signum) 
{
    if (signum == SIGINT || signum == SIGTERM) 
    {
        stop_flag = 1;
    }
}

void ReadInput(std::string& input) 
{
    std::getline(std::cin, input);
}
/*
void HandleNbd(IDriverComm* nbd_driver, IStorage* ram_storage)
{
    std::shared_ptr<DriverData> driver_data_ptr;

    driver_data_ptr = nbd_driver->ReceiveRequest();
    
    while (finished)
    {
        
        switch(driver_data_ptr->m_type)
        {
            case ActionType::READ:
                ram_storage->Read(driver_data_ptr);

            case ActionType::WRITE:
                ram_storage->Write(driver_data_ptr);

            case ActionType::DISCONNECT:
                nbd_driver->Disconnect();
        }

        nbd_driver->SendReply(driver_data_ptr);

        driver_data_ptr = nbd_driver->ReceiveRequest();
    }

}*/


void HandleEvents(int epoll_fd, IDriverComm* nbd_driver, IStorage* ram_storage)
{
    struct epoll_event event;
    struct epoll_event events[1000];  
    std::shared_ptr<DriverData> driver_data_ptr;

    //prepare stdin for epoll
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event);

    //prepare fd for epoll
    event.data.fd = nbd_driver->GetFd();
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, nbd_driver->GetFd(), &event);

    while (!stop_flag) 
    {  
         std::cout << "while" << std::endl; 
        //blocking waiting for something...
        int num_events = epoll_wait(epoll_fd, events, 1000, -1);

        //handle error
        if (num_events == -1) 
        {
            perror("epoll_wait");
            break;
        }

        //check events which is which
        for (int i = 0; i < num_events; ++i) 
        {
            if (events[i].data.fd == STDIN_FILENO) 
            {
                std::cout << "stdin" << std::endl; 
                //handle input
                std::string input;
                ReadInput(input);
                if (input == "q") 
                {
                    stop_flag = 1;
                }
            } else if (events[i].data.fd == nbd_driver->GetFd())
            {
                std::cout << "recieve request" << std::endl; 

                driver_data_ptr = nbd_driver->ReceiveRequest();

                std::cout << "recieve request2" << std::endl; 

                switch(driver_data_ptr->m_type)
                {
                    case ActionType::READ:
                    std::cout << "recieved read request" << std::endl; 
                    ram_storage->Read(driver_data_ptr);
                    break;

                    case ActionType::WRITE:
                    std::cout << "recieved write request" << std::endl; 
                    ram_storage->Write(driver_data_ptr);
                    break;

                    case ActionType::DISCONNECT:
                    //nbd_driver->Disconnect();
                    std::cout << "recieved disconnect request" << std::endl; 
                    break;
                    
                    case ActionType::FLUSH:
                    std::cout << "recieved FLUSH request" << std::endl; 
                    break;

                    case ActionType::TRIM:
                    std::cout << "recieved TRIM request" << std::endl; 
                    break;
                }

                nbd_driver->SendReply(driver_data_ptr);
                //HandleNbd(nbd_driver, ram_storage);
            }
        }
    }

    // Clean up epoll
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, STDIN_FILENO, NULL);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, nbd_driver->GetFd(), NULL);
    delete nbd_driver;
    delete ram_storage;
}


int main(int argc, char *argv[]) 
{
    //check there are 3 args
    if (argc < 3) 
    {
        std::cerr << "Usage: " << argv[0] << " <size> <nbd_device>" << std::endl;
        return 1;
    }

    //size and path
    size_t nbd_size = std::stoi(argv[1]);
    std::string nbd_device(argv[2]);

    nbd_size *= 1024 *1024;

    //new nbd driver
    IDriverComm* nbd_driver = new NBDDriverComm(nbd_device, nbd_size);
    
    //storage
    IStorage* ram_storage = new RAMStorage(nbd_size);

    //prepare signal handler
    struct sigaction sig_int_handler;
    sig_int_handler.sa_handler = SignalHandler;
    sigemptyset(&sig_int_handler.sa_mask);
    sig_int_handler.sa_flags = 0;

    sigaction(SIGINT, &sig_int_handler, NULL);
    sigaction(SIGTERM, &sig_int_handler, NULL);

    //create epoll
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        perror("epoll_create1");
        return 1;
    }

    std::cout << "Waiting for commands. Press 'q' to quit." << std::endl;

    //handle events
    HandleEvents(epoll_fd, nbd_driver, ram_storage);

    std::cout << "Shutting down..." << std::endl;

    //cleanup everything
    nbd_driver->Disconnect();
    close(epoll_fd);

    return 0;

}
/*
#include "nbd_driver_comm.hpp"
#include <iostream>
#include <vector>

int main() {
    // Path to your NBD device file
    std::string dev_file = "/dev/nbd4";

    // Size of the device in bytes
    size_t size = 1024 * 1024; // 1MB

    // Create an instance of NBDDriverComm
    IDriverComm* nbd_driver = new NBDDriverComm(dev_file, size);
    char arr[] = "SomeHandle";
    // Prepare data for a write request
    ilrd::DriverData* writeRequest = new DriverData(ilrd::ActionType::WRITE, arr, 0, 10); // Writing 10 bytes

    // Populate data to write
    std::vector<unsigned char> writeData = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
    writeRequest->m_buff = writeData;

    // Send the write request
    nbd_driver->SendReply(std::make_shared<ilrd::DriverData>(writeRequest));

    // Receive a request (for illustration, you can remove this in actual use)
    auto requestData = nbd_driver->ReceiveRequest();

    // Process the request (Here, just print some info)
    std::cout << "Received Request:" << std::endl;
    std::cout << "Type: " << static_cast<int>(requestData->m_type) << std::endl;
    std::cout << "Offset: " << requestData->m_offset << std::endl;
    std::cout << "Length: " << requestData->m_len << std::endl;

    // Simulate some processing
    requestData->m_status = ilrd::Status::SUCCESS;

    // Send a reply (for illustration, you can remove this in actual use)
    nbd_driver->SendReply(requestData);

    // Disconnect
    nbd_driver->Disconnect();

    return 0;
}

*/