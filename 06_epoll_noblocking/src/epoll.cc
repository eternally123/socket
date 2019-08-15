#include "epoll.hpp"
#include "config.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>
#include <map>

epoll_event* Epoll::get_epoll_events()
{
    return events_;
}

void Epoll::create(int size, int events_num)
{
    /* 创建epoll失败退出程序 */
    if ((epollfd_ = epoll_create(size)) < 0) {
        std::cerr << "epoll_crate failed" << std::endl;
        exit(1);
    }
    /* 创建epoll_event数组 */
    maxevents_ = events_num;
    events_ = new epoll_event[events_num];
    return;
}

int Epoll::wait(int timeout)
{
    int num_ready;
    while (1) {
        num_ready = epoll_wait(epollfd_, events_, maxevents_, timeout);
        if (num_ready < 0) { /* epoll_wait返回值异常 */
            if (errno == EINTR)
                continue;
            else {
                std::cerr << "Epoll_wait error" << std::endl;
                exit(1);
            }
        } else {
            return num_ready;
        }
    }
}

void Epoll::addFd(int fd, int ev)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev;
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event);
}

void Epoll::removeFd(int fd)
{
    epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void Epoll::modifyFd(int fd, int ev)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev;
    epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event);
}

void Epoll::destroy()
{
    if (events_ != NULL)
        delete[] events_;
    return;
}
