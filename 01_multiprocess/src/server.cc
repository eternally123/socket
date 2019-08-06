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

void str_echo(int socketfd);
void sig_child(int signo);

int main(int argc,char* argv[]){
    int listenfd,connectfd;
    pid_t childPid;
    
    listenfd = open_listenfd(atoi(argv[1]));
    signal(SIGCHLD,sig_child);

    while(1){
        connectfd = accept_client(listenfd);
        if(connectfd<0){
            if(errno == EINTR)
                continue;
            else if(errno == ECONNABORTED | errno == EPROTO)
                continue;
            else{
                // std::cout<<"accept wrong"<<std::endl;
            }
        }else{
            if((childPid = fork()) == 0){//child process
                close(listenfd);
                str_echo(connectfd);
                exit(0);
            }else{//parent process(listen process) closes connected socket
                close(connectfd);
            }
        }
    }
}

void str_echo(int socketfd){
    ssize_t n;
    char buf[BUFFER_SIZE];

    while(true){
        n = read(socketfd,buf,BUFFER_SIZE);
        // std::cout << "read " << n << " char from client" << std::endl;
        if(n>0){//read data from client
            writen(socketfd,buf,n);
            continue;
        }else if (n<0){
            if(errno == EINTR){//interupt by system. read again
                // std::cout << "interupt by system :EINTR. read again" << std::endl;
                continue;
            }else if(errno == ECONNRESET){//client send RST before accept
                // std::cout << "interupt by system :ECONNRESET. read again" << std::endl;
            }else{
                // std::cout << "str_echo:read error" << std::endl;
                exit(1);
            }
        }else{//n==0 connection closed by client
            close(socketfd);
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
