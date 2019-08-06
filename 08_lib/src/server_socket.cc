#include "socket.hpp"
#include "config.hpp"

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <fcntl.h>

/*
    class Socket
 */
ServerSocket::ServerSocket(){
    fd_ = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    is_block_ = true;
}



int ServerSocket::getFd(){
    return fd_;
}

int ServerSocket::getPort(){
    return port_;
}

int ServerSocket::bindAddress(int port){
    struct sockaddr_in server_address;
    
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port=htons(port);

    return bind(fd_,(sockaddr*)&server_address,sizeof(server_address));
}

int ServerSocket::listenEvent(int listen_queue_num){
    if(listen(fd_,listen_queue_num)<0)
        return -1;
    return fd_;
}

Socket* ServerSocket::acceptClient(){
    int new_connectfd;
    struct sockaddr_in client_address;
    socklen_t client_address_length;

    /* 与客户端建立连接 */
    if((new_connectfd = accept(fd_,(sockaddr *)&client_address,&client_address_length)) < 0)
        return NULL;
    Socket new_socket(new_connectfd);
    return &new_socket;
}

void ServerSocket::setNoBlock(){
    int socket_model_val;

    socket_model_val = fcntl(fd_,F_GETFL,0);
    fcntl(fd_,F_SETFL,socket_model_val | O_NONBLOCK);

    is_block_ = false;
}

void ServerSocket::shutdownWrite(){
    shutdown(fd_,SHUT_WR);
}

void ServerSocket::shutdownRead(){
    shutdown(fd_,SHUT_RD);
}

void ServerSocket::closeSocket(){
    close(fd_);
}

/* 
    class ServerScoket 
*/


