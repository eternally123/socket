#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void str_client(FILE *fp,int socketFd);

int main(){
    int socketFd;
    struct sockaddr_in serverAddress;

    //create socket
    socketFd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //connect with server
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(10000);
    connect(socketFd,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
    
    //send message
    str_client(stdin,socketFd);

    exit(0);
}

void str_client(FILE *fp,int socketFd){
    char sendLine[1024],receiveLine[1024];

    while(fgets(sendLine,1024,fp)!=NULL){
        write(socketFd,sendLine,strlen(sendLine));
        if(readline(socketFd,receiveLine,1024)==0){
            printf("str_client:server terminated prematurely\n");
            exit(1);
        }
        fputs(receiveLine,stdout);
    }
}

