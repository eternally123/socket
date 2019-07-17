#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

void str_echo(int socketFd);

int main(){
    int listenFd,connectFd;
    pid_t childPid;
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
        if((childPid = fork()) == 0){
            //child process
            close(listenFd);
            //process
            str_echo(connectFd);
            exit(0);
        }
        close(connectFd);//parent process(listen process) closes connected socket
    }
    return 0;
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



