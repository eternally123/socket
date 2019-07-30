#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/resource.h>

void str_echo(int socketFd);

int main(){
    int i;

    int listenFd,newConnectFd,connectedFd;//connectedFd:handle client request(doesn't include connect)
    struct sockaddr_in serverAddress,clientAddress;
    socklen_t clientAddressLength;
    
    char buf[1024];//buffer to storage data(receive or to send)
    ssize_t nRead;//number of byte read from socket

    int nReady;//nReady is the number of fd after select
    int maxIndexInClientConnectionArray;


    struct pollfd client[RLIMIT_NOFILE];

    //create socket
    listenFd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    
    //bind ip and port
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(10000);
    bind(listenFd,(struct sockaddr*)&serverAddress,sizeof(serverAddress));

    //listen
    listen(listenFd,20);

    client[0].fd = listenFd;
    client[0].events = POLLRDNORM;
    for(i = 1;i<RLIMIT_NOFILE;i++)
        client[i].fd = -1;
    maxIndexInClientConnectionArray = 0;

    //accept client connect
    for ( ; ; ){
        nReady = poll(client,maxIndexInClientConnectionArray+1,-1);
        
        if(client[0].revents & POLLRDNORM){//new client connection arive
            printf("new connection...\n");
            clientAddressLength = sizeof(clientAddress);
            newConnectFd = accept(listenFd,(struct sockaddr*)&clientAddress,&clientAddressLength);

            for(i=0;i<RLIMIT_NOFILE;i++)
                if(client[i].fd<0){
                    client[i].fd = newConnectFd;//save newConnectFd
                    break;
                }
            
            if(i == RLIMIT_NOFILE){
                printf("too many clients......\n");//??? accept
                exit(1);
            }

            client[i].events = POLLRDNORM;
            if(i>maxIndexInClientConnectionArray)
                maxIndexInClientConnectionArray = i;
            
            if(--nReady<=0)
                continue;//no more readable descriptors
        }

        for(i=1;i<=maxIndexInClientConnectionArray;i++){//client connection readable
            if((connectedFd = client[i].fd)<0)
                continue;
            if(client[i].revents & (POLLRDNORM | POLLERR)){//readable 
                if((nRead = read(connectedFd,buf,1024))<0){
                    if(errno == ECONNRESET){
                        //connection reset by client
                        close(connectedFd);
                        client[i].fd = -1;
                    }else{
                        printf("read error...\n");
                    }
                }else if(nRead == 0){
                    //connection closed by client
                    close(connectedFd);
                    client[i].fd = -1;                
                }else{
                    writen(connectedFd,buf,nRead);
                }

                if(--nReady<=0)
                    break;
            }
        }
    }
}



void str_echo(int socketFd){
    ssize_t n;
    char buf[1024];
    while(true){
        while((n = read(socketFd,buf,1024))>0)
            writen(socketFd,buf,n);

        if(n<0 && errno == EINTR)
            continue;
        else if (n<0){
            printf("str_echo:read error\n");
            exit(1);
        }
    }
}



