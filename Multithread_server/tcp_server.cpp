#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>

static void *processRequest(void *);
void str_echo(int socketFd);

int main(){
    int listenFd,connectFd;
    pthread_t tid;

    struct sockaddr_in serverAddress,clientAddress;
    socklen_t clientLength;
    
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

    //accept client connect,
    for ( ; ; ){
        clientLength = sizeof(clientAddress);
        connectFd = accept(listenFd,(struct sockaddr*)&clientAddress,&clientLength);
        pthread_create(&tid,NULL,&processRequest,(void *)connectFd);
    }
    return 0;
}

static void *processRequest(void *arg){
    pthread_detach(pthread_self());
    str_echo((long) arg);
    close((long) arg);
    return(NULL);
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

