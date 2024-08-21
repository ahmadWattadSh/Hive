#include <memory>//shared_ptr
#include <sys/ioctl.h>//ioctl
#include <sys/socket.h>//socketpair
#include <assert.h>//assert
#include <fcntl.h>//open
#include <cstring>//c_str
#include <iostream>//cout
#include <linux/nbd.h>// NBD_SET_SIZE
#include <thread>//thread
#include <csignal>//signalmask
#include <arpa/inet.h>//htonl

#include "driver_data.hpp"
#include "i_driver_comm.hpp"
#include "nbd_driver_comm.hpp"

#include "utils.hpp"


namespace ilrd
{

#ifdef WORDS_BIGENDIAN
u_int64_t ntohll(u_int64_t a) {
  return a;
}
#else
u_int64_t ntohll(u_int64_t a) {
  u_int32_t lo = a & 0xffffffff;
  u_int32_t hi = a >> 32U;
  lo = ntohl(lo);
  hi = ntohl(hi);
  return ((u_int64_t) lo) << 32U | hi;
}
#endif
#define htonll ntohll


template<typename T>
static int ReadAll(int fd, T buf, size_t count)
{
    int bytes_read = 0;

    while (count > 0) 
    {
        bytes_read = read(fd, buf, count);
        assert(bytes_read > 0);
        buf += bytes_read;
        count -= bytes_read;
    }

    assert(count == 0);
    return 0;
}

template<typename T>
static int write_all(int fd, T buf, size_t count)
{
    int bytes_written;

    while (count > 0) 
    {
        bytes_written = write(fd, buf, count);
        assert(bytes_written > 0);
        buf += bytes_written;
        count -= bytes_written;
    }
    
    assert(count == 0);
    return 0;
}

 

    static ActionType ConvertTypeToActionType(__be32 type)
    { 
        switch(ntohl(type))
        {
            case NBD_CMD_READ:
            return ActionType::READ;
            
            case NBD_CMD_WRITE:
            return ActionType::WRITE;
            
            case NBD_CMD_DISC:
            return ActionType::DISCONNECT;

            case NBD_CMD_FLUSH:
            return ActionType::FLUSH;

            case NBD_CMD_TRIM:
            return ActionType::TRIM;
        }

        return ActionType(-1);
    }


    NBDDriverComm::NBDDriverComm(const std::string& path_dev_file_, size_t size_)
    {    
        int status = 0;
        int sp[2] = {0};

        //create socket
        status = socketpair(AF_UNIX, SOCK_STREAM, 0, sp);
        m_kernel_sk = sp[0];
        m_user_sk = sp[1];
        assert(0 == status); 
        //open dev file
        m_nbd = open(path_dev_file_.c_str(),  O_RDWR);

        //handle err
        assert("open failed\n" && (m_nbd != -1));
  
        //set size
        status = ioctl(m_nbd, NBD_SET_SIZE, size_);
        assert("ioctl failed\n" && (status != -1));

        //clear socket
        status = ioctl(m_nbd, NBD_CLEAR_SOCK);
        assert("ioctl failed\n" && (status != -1));

        //run thread
        m_thread = std::thread(&NBDDriverComm::NBDListner, this);
    }


    NBDDriverComm::NBDDriverComm(const std::string& path_dev_file_, size_t size_of_block_, size_t num_of_blocks_):NBDDriverComm(path_dev_file_, size_of_block_ * num_of_blocks_)
    {
    }
       
    void NBDDriverComm::NBDListner()
    { 
        int flags = 0;
        int status = 0;

        sigset_t sigset;

        //all signals masking
        status = sigfillset(&sigset);
        assert("sigfillset failed \n" && 0 == status);

        status = pthread_sigmask(SIG_SETMASK, &sigset, NULL);
        assert("pthread_sigmask failed \n" && 0 == status);

        status = ioctl(m_nbd, NBD_SET_SOCK, m_kernel_sk);
        assert("ioctl(nbd, NBD_SET_SOCK, sk) failed\n" && -1 != status);

        //configuration
        #if defined NBD_SET_FLAGS
        flags = 0;
        #if defined NBD_FLAG_SEND_TRIM
            flags |= NBD_FLAG_SEND_TRIM;
        #endif
        #if defined NBD_FLAG_SEND_FLUSH
            flags |= NBD_FLAG_SEND_FLUSH;
        #endif

        if(0 != flags)
        {
            status = ioctl(m_nbd, NBD_SET_FLAGS, flags);
            assert("ioctl(nbd, NBD_SET_SOCK, sk) failed\n" && (-1 != status));
        }
        #endif
    
        //start listening on nbd
        status = ioctl(m_nbd, NBD_DO_IT);
        DEBUG_ONLY(fprintf(stderr, "nbd device terminated with code %d\n", status);)
        assert("NBD_DO_IT terminated with error\n" && (-1 != status));
    
        exit(0);
    }

    
    NBDDriverComm::~NBDDriverComm()
    {
        int status = 0;

        Disconnect();
        close(m_user_sk);
        close(m_kernel_sk);
        m_thread.join();
        
        status = ioctl(m_nbd, NBD_CLEAR_QUE);
        assert("cleanup failed\n" && (-1 != status));
    
        status = ioctl(m_nbd, NBD_CLEAR_SOCK);
        assert("cleanup failed\n" && (-1 != status));

        close(m_nbd);
    }

    std::shared_ptr<DriverData> NBDDriverComm::ReceiveRequest() 
    {
        struct nbd_request request;
        ssize_t bytes_read = 0;

        //read
        bytes_read = read(m_user_sk, &request, sizeof(request));

        //check if read works
        //assert("failed to read\n" && bytes_read == sizeof(request));
        //assert("failed to read\n" && bytes_read != -1);

        //check it is the same magic number
        //assert("not the same\n" && request.magic == htonl(NBD_REQUEST_MAGIC));
        
        //add it to vector
        std::shared_ptr<DriverData> driver_data_ptr = std::make_shared<DriverData>
        (ConvertTypeToActionType(request.type), request.handle, ntohll(request.from), ntohl(request.len));

        /*if(ActionType::WRITE == driver_data_ptr->m_type)
        {            
            std::cout << "ssssssssssss IF WRITE sssssssss" << std::endl;
            driver_data_ptr->m_buff.clear();
            driver_data_ptr->m_buff.resize(driver_data_ptr->m_len);

            read(m_user_sk, driver_data_ptr->m_buff.data(), driver_data_ptr->m_len);
        }*/
        if(ntohl(request.type) == NBD_CMD_WRITE)
        {
            std::cout << "ssssssssssss IF Write sssssssss" << std::endl;

            //check if we can do this with copy to socket directly ??????
            char* temp_buffer = new char[driver_data_ptr->m_len];

            //read the data from user_sk to temp_buffer
            bytes_read = read(m_user_sk, temp_buffer, driver_data_ptr->m_len);
            assert("failed to read from socket\n" && bytes_read != -1);//it must check if bytes_read == sizeof(request)

            //clear the m_buffer
            driver_data_ptr->m_buff.clear();

            //copy the data from the temp buffer to the driver data buffer 
            driver_data_ptr->m_buff.assign(temp_buffer, temp_buffer + driver_data_ptr->m_len + 1);

            delete[] temp_buffer;
        }
        
        return driver_data_ptr;
    }

    void NBDDriverComm::SendReply(const std::shared_ptr<DriverData> data_)
    {
        struct nbd_reply reply;
        //copy handle
        std::copy(data_->m_handle, data_->m_handle + sizeof(reply.handle), reply.handle);
        //NBD_REPLY_HANDLE
        reply.magic = htonl(NBD_REPLY_MAGIC);
        //error  = reply from doing the request;
        reply.error = static_cast<int>(data_->m_status);


        ssize_t bytes_written = write(m_user_sk, &reply, sizeof(struct nbd_reply));
        assert("failed to write reply\n" && bytes_written != -1);

        //if the action type is read - must write to socket

        if(ActionType::READ == data_->m_type)
        {
            data_->m_buff.clear();
            
            std::cout << "ssssssssssss IF READ sssssssss" << std::endl;
            bytes_written = write(m_user_sk, data_->m_buff.data(), data_->m_len);
            assert(bytes_written != -1);
        }
        /*if(data_->m_type == ActionType::READ)
        {
            char* temp_buffer = new char[data_->m_len];

            std::copy(data_->m_buff.begin(), data_->m_buff.end(), temp_buffer);

            data_->m_buff.clear();

            bytes_written = write(m_user_sk, temp_buffer, data_->m_len);
            assert("failed to write to socket\n" && bytes_written != -1);//it must check if bytes_written == sizeof(struct nbd_reply)

            delete [] temp_buffer;

        }*/
        
    }

    void NBDDriverComm::Disconnect()
    {
        int status = 0;
        static int is_connected = 1;

        if(is_connected)
        {
            status = ioctl(m_nbd, NBD_DISCONNECT);
            assert(-1 != status);
        }
        
    }

    int NBDDriverComm::GetFd() const
    {
        return m_user_sk; 
    }


} // namespace ilrd;





