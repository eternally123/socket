/* 用户自定义头文件 */
#include "config.hpp"
#include "epoll_controller.hpp"
#include "socket.hpp"


#include <iostream>
// std::cout<<""<<std::endl;

int main(int argc,char* argv[]){

    if(argc != 2){
        std::cerr<< "argument num incorrect!"<< std::endl;
        exit(0); 
    }

    ServerSocket server_socket;
    server_socket.bindAddress(atoi(argv[1]));
    server_socket.listenEvent(LISTEN_QUEUE_NUM);
    EpollController epoll_controller;
    epoll_controller.init(EPOLL_MAX_HANDLE,&server_socket);
    std::cout<<"init finish"<<std::endl;
    epoll_controller.run();

    return 0;
}
