#include "network.hpp"
#include "config.hpp"

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
    char buf[BUFFER_SIZE];
    int nRead;

    stdinEOF = 0;
    FD_ZERO(&readSet);

    while(1){
        if(stdinEOF == 0)
            FD_SET(fileno(fp),&readSet);
        FD_SET(socketFd,&readSet);
        
        maxFdNum = fileno(fp)>socketFd ? fileno(fp) + 1 : socketFd + 1;
        select(maxFdNum,&readSet,NULL,NULL,NULL);

        if(FD_ISSET(socketFd,&readSet)){//socket is readable
            // printf("\nread from socket...\n");
            nRead = read(socketFd,buf,BUFFER_SIZE);
            if(nRead == 0){//no data
                if(stdinEOF==1){//client won't send data & server won't send data
                    return ;
                }else{//client doesn't stop writing data,but read nothing from server,error
                    printf("str_cli:server terminated prematurely\n");
                    exit(1);
                }
            }else{
                write(fileno(stdout),buf,nRead);
            }
        }
        
        if(FD_ISSET(fileno(fp),&readSet)){//input is readable
        //    printf("\nread from command...\n");
           if((nRead = read(fileno(fp),buf,BUFFER_SIZE))==0){//no data input ,EOF
                stdinEOF = 1;
                shutdown(socketFd,SHUT_WR);//won't send data . shutdown .send FIN
                FD_CLR(fileno(fp),&readSet);
                continue;
           }else{//read data from fp, need to write to server
               int num = writen(socketFd,buf,nRead);
           }
        }
    }
}







