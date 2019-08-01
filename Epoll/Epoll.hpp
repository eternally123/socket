#ifndef _EPOLL_HPP_
#define _EPOLL_HPP_

class Epoll{
private:
    int epollfd_;
    int maxevents_;
    struct epoll_event* events_;

public:
    epoll_event* get_epoll_events();

public:
    void create(int size,int events_num);
    int wait(int timeout);
    void removeFd(int fd);
    void modifyFd(int fd, int ev);
    void addFd(int fd, int ev);
    void destroy();
};



#endif