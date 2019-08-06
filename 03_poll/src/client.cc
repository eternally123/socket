#include "network.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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
    int maxfdnum,stdinEOF;
    fd_set readSet;
    char buf[1024];
    int nread;

    stdinEOF = 0;
    FD_ZERO(&readSet);

    while(1){
        if(stdinEOF == 0)
            FD_SET(fileno(fp),&readSet);
        FD_SET(socketfd,&readSet);
        
        maxfdnum = fileno(fp)>socketfd ? fileno(fp) + 1 : socketfd + 1;
        select(maxfdnum,&readSet,NULL,NULL,NULL);

        if(FD_ISSET(socketfd,&readSet)){//socket is readable
            // printf("\nread from socket...\n");
            if((nread = read(socketfd,buf,1024)) == 0){//no data
                if(stdinEOF==1)//client won't send data & server won't send data
                    return ;
                else{
                    printf("str_cli:server terminated prematurely\n");
                    exit(1);
                }
            }
            write(fileno(stdout),buf,nread);
        }
        
        if(FD_ISSET(fileno(fp),&readSet)){//input is readable
        //    printf("\nread from command...\n");
           if((nread = read(fileno(fp),buf,1024))==0){//no data input ,EOF
                stdinEOF = 1;
                shutdown(socketfd,SHUT_WR);//won't send data . shutdown .send FIN
                FD_CLR(fileno(fp),&readSet);
                continue;
           }
           int num = writen(socketfd,buf,nread);
        }
    }
}







