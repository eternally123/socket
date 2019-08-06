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

#include <iostream>

int main(int argc,char* argv[]){
    int i;
    int listenfd,new_connectfd,connectedfd;
    char buf[BUFFER_SIZE];
    ssize_t nread;//number of byte read from socket

    int nready;//nReady is the number of fd after select
    int client_connection_array[FD_SETSIZE];//FD_SETSIZE is max number of file descriptor
    int max_index_in_fdtable,max_index_in_client_connection_array;

    fd_set readset,allset;

    listenfd = open_listenfd(atoi(argv[1]));

    max_index_in_fdtable = listenfd;//initialize maxIndexInFdTable
    max_index_in_client_connection_array = -1;//initialize maxIndexInClientConnectionArray

    for(i=0;i<FD_SETSIZE;i++)//set clientConnectionArray
        client_connection_array[i] = -1;
    FD_ZERO(&allset);//clear allSet
    FD_SET(listenfd,&allset);//add listenfd

    while(1){
        readset = allset;//set all fd
        nready = select(max_index_in_fdtable+1,&readset,NULL,NULL,NULL);
        // std::cout<<"nready="<<nready<<std::endl;
        if(FD_ISSET(listenfd,&readset)){//new client connection arive
            // std::cout<<"new connection"<<std::endl;
            new_connectfd = accept_client(listenfd);
            // std::cout<<new_connectfd<<std::endl;

            for(i=0;i<FD_SETSIZE;i++)
                if(client_connection_array[i]<0){
                    client_connection_array[i] = new_connectfd;//save new_connectfd
                    break;
                }

            if(i == FD_SETSIZE){
                // printf("too many clients......\n");
                exit(1);
            }
            FD_SET(new_connectfd,&allset);//add new descriptor to set
            if(new_connectfd>max_index_in_fdtable)
                max_index_in_fdtable = new_connectfd;
            if(i>max_index_in_client_connection_array)
                max_index_in_client_connection_array = i;
            if(--nready<=0)
                continue;//no more readable descriptors
        }

        for(i=0;i<=max_index_in_client_connection_array;i++){//client connection readable
            // std::cout<<"read and write"<<std::endl;
            if((connectedfd = client_connection_array[i])<0)
                continue;
            if(FD_ISSET(connectedfd,&readset)){//readable 
                nread = read(connectedfd,buf,BUFFER_SIZE);
                if(nread ==0){//connection closed by client
                    close(connectedfd);
                    FD_CLR(connectedfd,&allset);//remove fd
                    client_connection_array[i] = -1;//remove fd
                }else if(nread > 0){
                    writen(connectedfd,buf,nread);
                }

                if(--nready<=0)
                    break;
            }
        }
    }
}


