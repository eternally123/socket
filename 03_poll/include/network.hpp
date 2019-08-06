#ifndef _NETWORK_HPP_
#define _NETWORK_HPP_

#include <unistd.h>

int open_listenfd(int port);
int accept_client(int listenfd);

int open_connectfd(char *ip,int port);



ssize_t readn(int fd,void *buff,size_t n);
ssize_t writen(int fd,const void *buff,size_t n);
ssize_t readline(int fd,void *buff,size_t maxLength);

#endif