#ifndef _NDSL_SERVER_UTILS_H_
#define _NDSL_SERVER_UTILS_H_

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAXLINE 4096
#define LISTENQ 50

// =======================================================================
void msg_msg(const char*);
void msg_quit(const char*);
void err_msg(const char*);
void err_quit(const char*);

char* Fgets(char* ptr, int n, FILE* stream);
void  Fputs(const char* ptr, FILE* stream);

ssize_t readn(int, void*, ssize_t);
ssize_t writen(int, void*, ssize_t);
void 	Readn(int, void*, ssize_t);
void	Writen(int, void*, ssize_t);
ssize_t readline(int fd, void* vptr, ssize_t maxlen);
ssize_t Readline(int fd, void* ptr, ssize_t maxlen);
ssize_t Read(int fd, void* ptr, ssize_t nbytes);
ssize_t Write(int fd, void* ptr, ssize_t nbytes);

// =======================================================================
int	Socket(int family, int type, int protocal);
void 	Connect(int sockfd, const struct sockaddr* servaddr, socklen_t addrlen);
void 	Bind(int sockfd, const struct sockaddr* myaddr, socklen_t addrlen); 
void 	Listen(int sockfd, int backlog);
int 	Accept(int sockfd, struct sockaddr* cliaddr, socklen_t* addrlen);

// =======================================================================
pid_t 	Fork(void);
void 	Close(int fd);
void    Shutdown(int sockfd, int howto);

typedef void Sigfunc(int);
Sigfunc* Signal(int signo, Sigfunc* func);

void Inet_pton(int family, const char* strptr, void* addrptr);
int Select(int nfds, fd_set* readset, fd_set* writeset, fd_set* exceptset, struct timeval* timeout);

#endif // _NDSL_SERVER_UTILS_H_
