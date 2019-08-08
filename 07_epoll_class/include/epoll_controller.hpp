#ifndef _EPOLL_HPP_
#define _EPOLL_HPP_

#include "socket.hpp"
#include "config.hpp"

#include <sys/epoll.h>


#include <map>

class EpollController{
private:
    int epollfd_;
    ServerSocket* listen_socket_;
    struct epoll_event events_[EPOLL_MAX_HANDLE];
public:
    void init(int size,ServerSocket* listen_socket);
    void run();
private:
    void removeFd(int fd);
    void modifyFd(int fd, int ev);
    void addFd(int fd, int ev, void* event_ptr);

};



#endif