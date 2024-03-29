#include "network.hpp"
#include "config.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/resource.h>

#include <iostream>

int main(int argc,char* argv[]){
    int i;

    int listenfd,new_connectfd,connectedfd;

    char buf[BUFFER_SIZE];//buffer to storage data(receive or to send)
    ssize_t num_read;//number of byte read from socket

    int num_ready;//nReady is the number of fd after select
    int max_index_in_client_connection_array;


    struct pollfd client[RLIMIT_NOFILE];

    listenfd = open_listenfd(atoi(argv[1]));

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for(i = 1;i<RLIMIT_NOFILE;i++)
        client[i].fd = -1;
    max_index_in_client_connection_array = 0;


    while(1){
        num_ready = poll(client,max_index_in_client_connection_array+1,-1);
        
        if(client[0].revents & POLLRDNORM){//new client connection arive
            // printf("new connection...\n");
            new_connectfd = accept_client(listenfd);
            // std::cout<<new_connectfd<<std::endl;

            for(i=0;i<RLIMIT_NOFILE;i++)
                if(client[i].fd<0){
                    client[i].fd = new_connectfd;//save newConnectFd
                    client[i].events = POLLRDNORM;
                    break;
                }
            
            if(i == RLIMIT_NOFILE){
                // printf("too many clients......\n");
                exit(1);
            }

            if(i>max_index_in_client_connection_array)
                max_index_in_client_connection_array = i;
            
            if(--num_ready<=0)
                continue;//no more readable descriptors
        }

        for(i=1;i<=max_index_in_client_connection_array;i++){//client connection readable
            if((connectedfd = client[i].fd)<0)
                continue;
            if(client[i].revents & (POLLRDNORM | POLLERR)){//readable 
                if((num_read = read(connectedfd,buf,BUFFER_SIZE))<0){
                    if(errno == ECONNRESET){
                        //connection reset by client
                        close(connectedfd);
                        client[i].fd = -1;
                    }else{
                        printf("read error...\n");
                    }
                }else if(num_read == 0){
                    //connection closed by client
                    close(connectedfd);
                    client[i].fd = -1;                
                }else{
                    writen(connectedfd,buf,num_read);
                }

                if(--num_ready<=0)
                    break;
            }
        }
    }
}




