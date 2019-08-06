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

void str_client(FILE *fp,int socketfd);

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

void str_client(FILE *fp,int socketfd){
    char sendbuf[BUFFER_SIZE],receivebuf[BUFFER_SIZE];

    while(fgets(sendbuf,BUFFER_SIZE,fp)!=NULL){
        write(socketfd,sendbuf,strlen(sendbuf));
        if(readline(socketfd,receivebuf,BUFFER_SIZE)==0){
            std::cout<<"str_client:server terminated prematurely"<<std::endl;
            exit(1);
        }
        fputs(receivebuf,stdout);
    }
}
