#include "config.hpp"
#include "utils.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <bits/signum-generic.h>

void str_client(FILE *fp,int socketFd);
void sig_do(int signo);

int main(){
    int socketFd;
    struct sockaddr_in serverAddress;

    //create socket
    socketFd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //connect with server
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddress.sin_port = htons(SERVER_PORT);
    connect(socketFd,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
    
    // signal(SIGPIPE,sig_do);
    //send message
    str_client(stdin,socketFd);
    close(socketFd);

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

// void str_client(FILE *fp,int socketFd){
//     char sendLine[BUFFER_SIZE],receiveLine[BUFFER_SIZE];

//     while(fgets(sendLine,BUFFER_SIZE,fp)!=NULL){
//         write(socketFd,sendLine,1);
//         sleep(1);
//         write(socketFd,sendLine + 1,strlen(sendLine) - 1);
        
//         printf("...%d\n",errno);

//         if(readline(socketFd,receiveLine,BUFFER_SIZE)==0){
//             printf("str_client:server terminated prematurely\n");
//             exit(1);
//         }
//         fputs(receiveLine,stdout);
//     }
// }


// void sig_do(int signo){
//     pid_t pid;
//     int stat;
//     printf("%d:sig_do\n",signo);

//     return ;
// }