#include "config.hpp"
#include "network.hpp"

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

#include <iostream>

void str_echo(int socketFd);
void sig_child(int signo);

int main(int argc,char* argv[]){
    int listenfd,connectfd;
    pid_t childPid;
    
    listenfd = open_listenfd(atoi(argv[1]));
    signal(SIGCHLD,sig_child);

    while(1){
        connectfd = accept_client(listenfd);
        
        if(connectfd<0){//accept error
            if(errno == EINTR)
                continue;
            else{
                printf("accept error...\n");
                exit(1);
            }
        }else{//accept success
            if((childPid = fork()) == 0){//child process
                close(listenfd);
                str_echo(connectfd);
                close(connectfd);
                exit(0);
            }else{//parent process(listen process) closes connected socket
                close(connectfd);
            }
        }
    }
}

void str_echo(int socketFd){
    ssize_t n;
    char buf[BUFFER_SIZE];

    while(true){
        n = read(socketFd,buf,BUFFER_SIZE);
        std::cout << "read " << n << " char from client" << std::endl;
        if(n>0){//read data from client
            writen(socketFd,buf,n);
            continue;
        }else if (n<0){
            if(errno == EINTR){//interupt by system. read again
                std::cout << "interupt by system :EINTR. read again" << std::endl;
                continue;
            }else if(errno == ECONNABORTED){//client send RST before accept
                std::cout << "interupt by system :ECONNABORTED. read again" << std::endl;
            }else{
                std::cout << "str_echo:read error" << std::endl;
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
