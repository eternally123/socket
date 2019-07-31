#include "utils.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/epoll.h>

// struct userData{
//     int fd;
//     ssize_t nRead;
//     // char buf[1024];
// };


int main(){
    int i, maxi, listenFd, newConnectFd, connectedFd,epollFd,nReady;
    ssize_t nRead;
    char buf[1024];
    socklen_t clientAddressLength;

    //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
    struct epoll_event ev,events[20];
    //生成用于处理accept的epoll专用的文件描述符

    epollFd = epoll_create(1);
    struct sockaddr_in clientAddress;
    struct sockaddr_in serverAddress;
    listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    //把socket设置为非阻塞方式//setnonblocking(listenfd);
    // ev.data.fd=listenFd;//设置与要处理的事件相关的文件描述符
    // struct userData data;
    // data.fd = listenFd;
    // data.nRead = -1;
    // ev.data.ptr = &data;
    ev.data.fd = listenFd;
    ev.events=EPOLLIN;//设置要处理的事件类型
    //ev.events=EPOLLIN;


    epoll_ctl(epollFd,EPOLL_CTL_ADD,listenFd,&ev);//注册epoll事件

    //bind ip and port
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port=htons(10000);
    bind(listenFd,(sockaddr *)&serverAddress, sizeof(serverAddress));
    
    //listen
    listen(listenFd, 20);
    maxi = 0;
    while(1){
        // sleep(0.5);
        //等待epoll事件的发生
        nReady=epoll_wait(epollFd,events,20,-1);
        printf("nReady=%d\n",nReady);
        if(nReady == -1){//epoll_wait返回异常
            if(errno == EINTR)
                continue;
            else{
                printf("epoll_wait error...nReady<0\n");
            }
        }else{//epoll_wait返回正常。for循环处理所发生的所有事件
            printf("nReady=%d\n",nReady);
            for(i=0;i<nReady;++i){
                if(events[i].data.fd ==listenFd){//如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
                    newConnectFd = accept(listenFd,(sockaddr *)&clientAddress, &clientAddressLength);
                    if(newConnectFd<0){
                        printf("newConnectFd<0 errno = %d\n",errno);
                        exit(1);
                    }
                    //setnonblocking(newConnectFd);
                    printf("accapt a connection \n");
                    
                    // userData tmpUserData;
                    // tmpUserData.fd = newConnectFd;
                    // tmpUserData.nRead = -1;
                    // ev.data.ptr = &tmpUserData;

                    ev.data.fd=newConnectFd;//设置用于读操作的文件描述符
                    ev.events=EPOLLIN|EPOLLET;//设置用于注测的读操作事件
                    //ev.events=EPOLLIN;
                    epoll_ctl(epollFd,EPOLL_CTL_ADD,newConnectFd,&ev);//注册ev
                }else if(events[i].events&EPOLLIN){ //如果是已经连接的用户，并且收到数据，那么进行读入。
                    printf("read... \n");
                    connectedFd = events[i].data.fd;
                    if (connectedFd < 0){
                        continue;
                    }else{//read
                        nRead = read(connectedFd, buf, 1024);
                        if (nRead < 0) {
                            printf("n<0\n");
                            if (errno == ECONNRESET) {//RST
                                close(connectedFd);
                                events[i].data.fd = -1;
                            } else{
                                printf("readline error \n");
                            }
                        } else if (nRead == 0) {//关闭连接
                            printf("n=0\n");                            
                            close(connectedFd);
                            events[i].data.fd = -1;
                        }
                        // line[n] = '/0';
                        // printf("read \n");
                        
                        ev.data.fd = connectedFd;//设置用于写操作的文件描述符
                        ev.events=EPOLLOUT|EPOLLET;//设置用于注测的写操作事件
                        epoll_ctl(epollFd,EPOLL_CTL_MOD,connectedFd,&ev);//修改connectedFd上要处理的事件为EPOLLOUT
                    }
                }else if(events[i].events&EPOLLOUT){//有数据发送
                    connectedFd = events[i].data.fd;
                    write(connectedFd, buf, nRead);
                    // write(connectedFd,buf,nRead);
                    //设置用于读操作的文件描述符

                    ev.data.fd=connectedFd;//设置用于注测的读操作事件
                    ev.events=EPOLLIN|EPOLLET;//修改sockfd上要处理的事件为EPOLIN

                    epoll_ctl(epollFd,EPOLL_CTL_MOD,connectedFd,&ev);
            }
            }
        }
    }
    return 0;
}


