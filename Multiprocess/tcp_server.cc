#include "config.hpp"
#include "utils.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <bits/signum.h>

void str_echo(int socketFd);
void sig_child(int signo);

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
    serverAddress.sin_addr.s_addr = inet_addr(LOCAL_IP);
    serverAddress.sin_port = htons(SERVER_PORT);
    bind(listenFd,(struct sockaddr*)&serverAddress,sizeof(serverAddress));

    //listen
    listen(listenFd,LISTEN_QUEUE_NUM);
    signal(SIGCHLD,sig_child);

    //accept client connect
    while(1){
        clientLength = sizeof(clientAddress);
        connectFd = accept(listenFd,(struct sockaddr*)&clientAddress,&clientLength);
        
        if(connectFd<0){//accept error
            if(errno == EINTR)
                continue;
            else{
                printf("accept error...\n");
                exit(1);
            }
        }else{//accept success
            if((childPid = fork()) == 0){//child process
                close(listenFd);
                str_echo(connectFd);
                close(connectFd);
                exit(0);
            }else{//parent process(listen process) closes connected socket
                close(connectFd);
            }
        }
    }
}

void str_echo(int socketFd){
    ssize_t n;
    char buf[BUFFER_SIZE];

    while(true){
        printf("before read...n=%d\n",n);
        n = read(socketFd,buf,BUFFER_SIZE);
        printf("after read...n=%d\n",n);
        if(n>0){//read data from client
            writen(socketFd,buf,n);
            continue;
        }else if (n<0){
            printf("n<0:n=%d\n",n);
            if(errno == EINTR){//interupt by system. read again
                printf("interupt by system  EINTR. read again\n");
                continue;
            }else if(errno == ECONNABORTED){//client send RST before accept
                printf("interupt by system  ECONNABORTED. read again\n");
            }else{
                printf("str_echo:read error\n");
                exit(1);
            }
        }else{//n==0 connection closed by client
            return;
        }
    }
}

void sig_child(int signo){
    pid_t pid;
    int stat;

    while((pid = waitpid(-1,&stat,WNOHANG)) > 0)
        printf("child %d terminated\n",pid);
    
    return ;
}
