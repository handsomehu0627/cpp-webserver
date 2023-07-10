/*
 * @Author       : mark
 * @Date         : 2020-06-15
 * @copyleft Apache 2.0
 */ 
#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h> //epoll_ctl()
#include <fcntl.h>  // fcntl()
#include <unistd.h> // close()
#include <assert.h> // close()
#include <vector>
#include <errno.h>

class Epoller {
public:
    explicit Epoller(int maxEvent = 1024);

    ~Epoller();

    bool AddFd(int fd, uint32_t events);

    bool ModFd(int fd, uint32_t events);

    bool DelFd(int fd);

    int Wait(int timeoutMs = -1);

    int GetEventFd(size_t i) const;

    uint32_t GetEvents(size_t i) const;
        
private:
    int epollFd_;

//    struct epoll_event {
//        uint32_t      events;  /* Epoll events */
//        epoll_data_t  data;    /* User data variable */
//    };
//
//    union epoll_data {
//        void     *ptr;
//        int       fd;
//        uint32_t  u32;
//        uint64_t  u64;
//    };
    std::vector<struct epoll_event> events_;    
};

#endif //EPOLLER_H