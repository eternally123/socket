#include "epoll_controller.hpp"
#include "config.hpp"

#include <unistd.h>
#include <sys/epoll.h>

#include <iostream>


void EpollController::init(int size,ServerSocket* listen_socket){
    if((epollfd_ = epoll_create(size))<0){
		std::cerr << "epoll_crate failed" << std::endl;
        exit(1);       
    }
    listen_socket_ = listen_socket;
    std::cout<<"init..."<<listen_socket_->getFd()<<std::endl;
    addFd(listen_socket_->getFd(),EPOLLIN,NULL);
    return;
}

void EpollController::run(){
    int num_ready;
    while(1){
        std::cout<<std::endl<<std::endl;
        std::cout<<"before"<<num_ready<<std::endl;

        num_ready = epoll_wait(epollfd_,events_,
                            EPOLL_MAX_HANDLE,EPOLL_TIMEOUT);
        std::cout<<"after"<<num_ready<<std::endl;
        
        if(num_ready<0){
            if(errno == EINTR)
                continue;
            else{
                std::cerr << "EpollController::run() error" << std::endl;
                exit(1);
            }
        }else{
            for(int i=0;i<num_ready;i++){
                    std::cout<<"fd: "<<events_[i].data.fd<<" "<<listen_socket_->getFd()<<std::endl;                
                if(events_[i].data.fd == listen_socket_->getFd()){
                    std::cout<<"new connection"<<std::endl;

                    Socket* new_socket = listen_socket_->acceptClient();
                    addFd(new_socket->getFd(),EPOLLIN,(void*)new_socket);


                    /* new connection */
                }else if(events_[i].events & EPOLLIN){
                    std::cout<<"read"<<std::endl;

                    Socket* connected_socket =(Socket*)events_[i].data.ptr;
                    connected_socket->readData();
                    modifyFd(events_[i].data.fd,EPOLLOUT);
                    /* read */
                }else if(events_[i].events &EPOLLOUT){
                    std::cout<<"write"<<std::endl;

                    Socket* connected_socket =(Socket*)events_[i].data.ptr;
                    connected_socket->writeData();
                    modifyFd(events_[i].data.fd,EPOLLIN);
                    /* write */
                }
            }
        }        
    }
}


void EpollController::addFd(int fd, int ev, void* event_ptr){
        std::cout<<"add ptr"<<std::endl;
	
    epoll_event event;
    if(event_ptr == NULL)
        event.data.fd = fd;
	else{
                    
        std::cout<<"add ptr"<<std::endl;

        event.data.ptr = event_ptr;
    }
    event.events = ev | EPOLLET; // ET触发
	epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event);
}

void EpollController::removeFd(int fd){
	epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

void EpollController::modifyFd(int fd, int ev){
	epoll_event event;
	event.data.fd = fd;
	event.events = ev | EPOLLET; // ET触发 
	epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event);
}

