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
Socket::Socket(){
    fd_ = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    is_block_ = true;
}

Socket::Socket(int fd){
    fd_ = fd;
    is_block_ = true;
}

int Socket::getFd(){
    return fd_;
}

int Socket::getPort(){
    return port_;
}

int Socket::bindAddress(int port){
    struct sockaddr_in server_address;
    
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port=htons(port);

    return bind(fd_,(sockaddr*)&server_address,sizeof(server_address));
}

int Socket::connectServer(char* host,int port){
    struct sockaddr_in server_address;

    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(host);
    server_address.sin_port = htons(port);
    
    return connect(fd_,(struct sockaddr*)&server_address,sizeof(server_address));
}

void Socket::setNoBlock(){
    int socket_model_val;

    socket_model_val = fcntl(fd_,F_GETFL,0);
    fcntl(fd_,F_SETFL,socket_model_val | O_NONBLOCK);

    is_block_ = false;
}

int Socket::readData(){
    int n;
    if(is_block_ == true){/* block */
        while(1){
            n = read(fd_,buffer_.end,&(buffer_.buf[BUFFER_SIZE])-buffer_.end);
            if(n < 0){
                if(errno == EINTR)/* interput by system */
                    continue;
                return -1;
            }
            return n;
        }
    }else{/* noblock */

    }
}

int Socket::writeData(){
    int n;
    if(is_block_ == true){/* block */
        while(1){
            n = write(fd_,buffer_.start,buffer_.end-buffer_.start);
            if(n < 0){
                if (errno == EINTR)/* interput by system */
                    continue;
                return -1;
            }
            return n;
        }
    }else{/* no block */
       
    }
}


void Socket::shutdownWrite(){
    shutdown(fd_,SHUT_WR);
}

void Socket::shutdownRead(){
    shutdown(fd_,SHUT_RD);
}

void Socket::closeSocket(){
    close(fd_);
}

/* 
    class ServerScoket 
*/


