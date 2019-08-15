#ifndef _NDSL_SERVER_EPOLL_UTILS_H_
#define _NDSL_SERVER_EPOLL_UTILS_H_

#include "utils.hh"
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENT_NUMBER 100


int setnonblocking(int fd);
void addfd(int epfd, int fd, unsigned mode = EPOLLIN | EPOLLOUT);
void deletefd(int epfd, int fd);
int Epoll_create();
int Epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);

#endif // _NDSL_SERVER_EPOLL_UTILS_H_
