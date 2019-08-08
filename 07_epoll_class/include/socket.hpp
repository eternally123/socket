#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_


#include "config.hpp"


struct Buffer{
    char buf[BUFFER_SIZE];
    char* start;
    char* end;
};
/* 
    not used for listen 
*/
class Socket{
private:
    int fd_;
    int port_;
    bool is_block_;
    int exception_code_;
    Buffer buffer_;
public:
    Socket();
    Socket(int fd);
public:
    int getFd();
    int getPort();
public:
    int bindAddress(int port);
    int connectServer(char* host,int port);
    void setNoBlock();
    int readData();//todo
    int writeData();//todo
    void shutdownWrite();
    void shutdownRead();
    void closeSocket();
};
/* 
    used for listen
*/
class ServerSocket{
private:
    int fd_;
    int port_;
    bool is_block_;
    int exception_code_;
public:
    ServerSocket();
public:
    int getFd();
    int getPort();
public:
    int bindAddress(int port);
    int listenEvent(int listen_queue_num);
    Socket* acceptClient();
    void setNoBlock();
    void shutdownWrite();
    void shutdownRead();
    void closeSocket();
};


#endif