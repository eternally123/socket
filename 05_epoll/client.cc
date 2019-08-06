#include "utils.hpp"
#include "network.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream>

void str_client(FILE *fp,int socketFd);

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
    int maxFdNum,stdinEOF;
    fd_set readSet;
    char buf[1024];
    int nRead;

    stdinEOF = 0;
    FD_ZERO(&readSet);

    while(1){
        if(stdinEOF == 0)
            FD_SET(fileno(fp),&readSet);
        FD_SET(socketFd,&readSet);
        
        maxFdNum = fileno(fp)>socketFd ? fileno(fp) + 1 : socketFd + 1;
        select(maxFdNum,&readSet,NULL,NULL,NULL);

        if(FD_ISSET(socketFd,&readSet)){
            if((nRead = read(socketFd,buf,1024)) == 0){
                if(stdinEOF==1)
                    return ;
                else{
                    printf("str_cli:server terminated prematurely\n");
                    exit(1);
                }
            }
            write(fileno(stdout),buf,nRead);
        }
        
        if(FD_ISSET(fileno(fp),&readSet)){
           if((nRead = read(fileno(fp),buf,1024))==0){
                stdinEOF = 1;
                shutdown(socketFd,SHUT_WR);
                FD_CLR(fileno(fp),&readSet);
                continue;
           }
            int num = writen(socketFd,buf,nRead);
        }
    }
}







