#include "config.hpp"
#include "network.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <bits/signum-generic.h>

#include <iostream>

void str_client(FILE *fp,int socketFd);
void sig_do(int signo);

int main(int argc,char* argv[]){
    if(argc != 3){
        std::cerr<< "argument num incorrect!"<< std::endl;
        exit(0); 
    }

    int socketfd;

    socketfd = open_connectfd(argv[1],atoi(argv[2]));

    str_client(stdin,socketfd);

    exit(0);
}

void str_client(FILE *fp,int socketFd){
    char sendLine[BUFFER_SIZE],receiveLine[BUFFER_SIZE];

    while(fgets(sendLine,BUFFER_SIZE,fp)!=NULL){
        write(socketFd,sendLine,strlen(sendLine));
        if(readline(socketFd,receiveLine,BUFFER_SIZE)==0){
            printf("str_client:server terminated prematurely\n");
            exit(1);
        }
        fputs(receiveLine,stdout);
    }
}
