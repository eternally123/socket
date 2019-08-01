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
    int listenfd,new_connectfd,connectedfd;//connectedFd:handle client request(doesn't include connect)
    
    char buf[BUFFER_SIZE];//buffer to storage data(receive or to send)
    ssize_t nRead;//number of byte read from socket

    int nReady;//nReady is the number of fd after select
    int clientConnectionArray[FD_SETSIZE];//FD_SETSIZE is max number of file descriptor
    int maxIndexInFdTable,maxIndexInClientConnectionArray;

    fd_set readSet,allSet;


    listenfd = open_listenfd(atoi(argv[1]));


    maxIndexInFdTable = listenfd;//initialize maxIndexInFdTable
    maxIndexInClientConnectionArray = -1;//initialize maxIndexInClientConnectionArray

    for(i=0;i<FD_SETSIZE;i++)//set clientConnectionArray
        clientConnectionArray[i] = -1;
    FD_ZERO(&allSet);//clear allSet
    FD_SET(listenfd,&allSet);//add listenFd


    //accept client connect
    while(1){
        readSet = allSet;//set all Fd
        // std::cout<<"before "<<std::endl;
        nReady = select(maxIndexInFdTable+1,&readSet,NULL,NULL,NULL);
        // std::cout<<"nready="<<nReady<<std::endl;

        if(FD_ISSET(listenfd,&readSet)){//new client connection arive
            std::cout<<"new connection"<<std::endl;
            new_connectfd = accept_client(listenfd);

            for(i=0;i<FD_SETSIZE;i++)
                if(clientConnectionArray[i]<0){
                    clientConnectionArray[i] = new_connectfd;//save newConnectFd
                    break;
                }
            
            if(i == FD_SETSIZE){
                printf("too many clients......\n");
                exit(1);
            }

            FD_SET(new_connectfd,&allSet);//add new descriptor to set
            if(new_connectfd>maxIndexInFdTable)
                maxIndexInFdTable = new_connectfd;
            if(i>maxIndexInClientConnectionArray)
                maxIndexInClientConnectionArray = i;
            
            // std::cout<<"......."<<std::endl;

            if(--nReady<=0)
                continue;//no more readable descriptors
        }

        for(i=0;i<=maxIndexInClientConnectionArray;i++){//client connection readable
            std::cout<<"read and write"<<std::endl;
            if((connectedfd = clientConnectionArray[i])<0)
                continue;
            if(FD_ISSET(connectedfd,&readSet)){//readable 
                nRead = read(connectedfd,buf,BUFFER_SIZE);
                if(nRead ==0){//connection closed by client
                    close(connectedfd);
                    FD_CLR(connectedfd,&allSet);//remove Fd
                    clientConnectionArray[i] = -1;//remove Fd
                }else{
                    writen(connectedfd,buf,nRead);
                }

                if(--nReady<=0)
                    break;
            }
        }
    }
}


