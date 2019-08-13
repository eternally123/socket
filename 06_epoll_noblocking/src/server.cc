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
    char* start = buf;
    char* end = buf;
    bool last_receive = false;
};

int main(int argc,char* argv[]){

    if(argc != 2){
        std::cerr<< "argument num incorrect!"<< std::endl;
        exit(1); 
    }

    int listenfd,new_connectfd,epollfd,connectedfd;
    int num_ready;
    struct epoll_event event;
    std::map<int,UserData*> users_data;
    
    listenfd = open_listenfd(atoi(argv[1]),LISTEN_QUEUE_NUM);
    Epoll epoll;
    epoll.create(EPOLL_MAX_HANDLE,EPOLL_MAX_HANDLE);
    epoll.addFd(listenfd,EPOLLIN);

    while(1){
        num_ready=epoll.wait(5000);
        printf("num_ready = %d\n",num_ready);
        for(int i=0;i<num_ready;++i){
            event = epoll.get_epoll_events()[i];
            if( event.data.fd == listenfd){
                std::cerr << "start listen" << std::endl;
                /* 如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。 */
                new_connectfd = accept_client(listenfd);
                std::cout<<"new_connectfd:"<<new_connectfd<<std::endl;
                if(new_connectfd == -1){
                    std::cerr << "accept error " << std::endl;
                    break;
                }
                printf("accapt a connection \n");
                set_noblock(new_connectfd);
                struct UserData user_data;
                users_data.insert(std::map<int,UserData*>::value_type(new_connectfd,&user_data));
                epoll.addFd(new_connectfd,EPOLLIN);
                std::cerr << "end listen" << std::endl;
            }else if(event.events & EPOLLIN){
                /* 如果是已经连接的用户，并且收到数据，那么进行读入。 */
                std::cerr << "start read" << std::endl;
                connectedfd = event.data.fd;
                std::map<int,UserData*>::iterator it;
                it = users_data.find(connectedfd);

                (*it->second).num = read(connectedfd,(*it->second).end, BUFFER_SIZE);
                if ((*it->second).num < 0) {
                    if (errno == ECONNRESET) {/* RST */
                        epoll.removeFd(connectedfd);
                        std::cerr << "close client" << std::endl;
                        close(connectedfd);
                    } else{
                        std::cerr << "read error" << std::endl;
                    }
                } else if ((*it->second).num == 0) {/* 客户端关闭连接 */
                    if((*it->second).last_receive == false){
                        epoll.removeFd(connectedfd);
                        std::cerr << "close client" << std::endl;
                        close(connectedfd);
                    }else{
                        (*it->second).last_receive = false;
                        epoll.modifyFd(connectedfd,EPOLLOUT);
                    }

                }else{/* 接收到数据 */
                    (*it->second).end += (*it->second).num;
                    (*it->second).last_receive = true;
                    epoll.modifyFd(connectedfd,EPOLLOUT|EPOLLIN);
                }
                std::cerr << "end read" << std::endl;
            }else if(event.events & EPOLLOUT){
                /* 向客户端写数据 */
                std::cerr << "start write" << std::endl;
                connectedfd = event.data.fd;
                std::map<int,UserData*>::iterator it;
                it = users_data.find(connectedfd);
                (*it->second).num = write(connectedfd,(*it->second).start,(*it->second).end-(*it->second).start);
                if((*it->second).num > 0){
                    (*it->second).start += (*it->second).num;
                }
                if((*it->second).start == (*it->second).end){
                    (*it->second).start = (*it->second).buf;
                    (*it->second).end = (*it->second).buf;
                    epoll.modifyFd(connectedfd,EPOLLIN);
                }else{
                    epoll.modifyFd(connectedfd,EPOLLIN|EPOLLOUT);
                }
                std::cerr << "end write" << std::endl;

            }else if(event.events & EPOLLHUP){
                
            }
        }
    }
    return 0;
}
