#include "config.hpp"
#include "epoll.hpp"
#include "network.hpp"

#include <unistd.h>

#include <sys/epoll.h>

#include <iostream>
#include <map>


struct UserData{
    char buf[BUFFER_SIZE];
    ssize_t num;
};


int main(int argc,char* argv[]){

    if(argc != 2){
        std::cerr<< "argument num incorrect!"<< std::endl;
        exit(0); 
    }

    int listenfd,new_connectfd,epollfd,connectedfd;
    int num_ready;
    struct epoll_event event;
    std::map<int,UserData*> users_data;
    
    listenfd = open_listenfd(atoi(argv[1]));
    Epoll epoll;
    epoll.create(EPOLL_NUM,EPOLL_EVENTS_NUM);
    epoll.addFd(listenfd,EPOLLIN);

    while(1){
        num_ready=epoll.wait(-1);
        printf("num_ready = %d\n",num_ready);
        for(int i=0;i<num_ready;++i){
            event = epoll.get_epoll_events()[i];
            if( event.data.fd == listenfd){
                /* 如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。 */
                new_connectfd = accept_client(listenfd);
                if(new_connectfd == -1){
                    std::cerr << "accept error " << std::endl;
                    break;
                }
                printf("accapt a connection \n");
                struct UserData user_data;
                users_data.insert(std::map<int,UserData*>::value_type(new_connectfd,&user_data));
                epoll.addFd(new_connectfd,EPOLLIN);

            }else if(event.events & EPOLLIN){
                /* 如果是已经连接的用户，并且收到数据，那么进行读入。 */
                connectedfd = event.data.fd;
                std::map<int,UserData*>::iterator it;
                it = users_data.find(connectedfd);

                (*it->second).num = read(connectedfd,(*it->second).buf, BUFFER_SIZE);
                if ((*it->second).num < 0) {
                    if (errno == ECONNRESET) {/* RST */
                        epoll.removeFd(connectedfd);
                        close(connectedfd);
                    } else{
                        std::cerr << "read error" << std::endl;
                    }
                } else if ((*it->second).num == 0) {/* 客户端关闭连接 */
                    epoll.removeFd(connectedfd);
                    close(connectedfd);
                }else{/* 接收到数据 */
                    epoll.modifyFd(connectedfd,EPOLLOUT);
                }

            }else if(event.events & EPOLLOUT){
                /* 向客户端写数据 */
                connectedfd = event.data.fd;
                std::map<int,UserData*>::iterator it;
                it = users_data.find(connectedfd);
                write(connectedfd,(*it->second).buf,(*it->second).num);
                epoll.modifyFd(connectedfd,EPOLLIN);
            }
        }
    }
    return 0;
}
