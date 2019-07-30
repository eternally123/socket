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

        if(FD_ISSET(socketFd,&readSet)){//socket is readable
            // printf("\nread from socket...\n");
            nRead = read(socketFd,buf,1024);
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
           if((nRead = read(fileno(fp),buf,1024))==0){//no data input ,EOF
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






