#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
// #include <sys/select.h>

// void str_echo(int socketFd);

int main(){
    int i;

    int listenFd,newConnectFd,connectedFd;//connectedFd:handle client request(doesn't include connect)
    struct sockaddr_in serverAddress,clientAddress;
    socklen_t clientAddressLength;
    
    char buf[1024];//buffer to storage data(receive or to send)
    ssize_t nRead;//number of byte read from socket

    int nReady;//nReady is the number of fd after select
    int clientConnectionArray[FD_SETSIZE];//FD_SETSIZE is max number of file descriptor
    int maxIndexInFdTable,maxIndexInClientConnectionArray;

    fd_set readSet,allSet;


    //create socket
    listenFd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    
    //bind ip and port
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(10000);
    bind(listenFd,(struct sockaddr*)&serverAddress,sizeof(serverAddress));

    //listen
    listen(listenFd,20);


    maxIndexInFdTable = listenFd;//initialize maxIndexInFdTable
    maxIndexInClientConnectionArray = -1;//initialize maxIndexInClientConnectionArray

    for(i=0;i<FD_SETSIZE;i++)//set clientConnectionArray
        clientConnectionArray[i] = -1;
    FD_ZERO(&allSet);//clear allSet
    FD_SET(listenFd,&allSet);//add listenFd


    //accept client connect
    while(1){
        readSet = allSet;//set all Fd
        nReady = select(maxIndexInFdTable+1,&readSet,NULL,NULL,NULL);

        if(FD_ISSET(listenFd,&readSet)){//new client connection arive
            // printf("new connection\n");
            clientAddressLength = sizeof(clientAddress);
            newConnectFd = accept(listenFd,(struct sockaddr*)&clientAddress,&clientAddressLength);

            for(i=0;i<FD_SETSIZE;i++)
                if(clientConnectionArray[i]<0){
                    clientConnectionArray[i] = newConnectFd;//save newConnectFd
                    break;
                }
            
            if(i == FD_SETSIZE){//??? accept
                printf("too many clients......\n");
                exit(1);
            }

            FD_SET(newConnectFd,&allSet);//add new descriptor to set
            if(newConnectFd>maxIndexInFdTable)
                maxIndexInFdTable = newConnectFd;
            if(i>maxIndexInClientConnectionArray)
                maxIndexInClientConnectionArray = i;
            
            if(--nReady<=0)
                continue;//no more readable descriptors
        }

        for(i=0;i<=maxIndexInClientConnectionArray;i++){//client connection readable
            if((connectedFd = clientConnectionArray[i])<0)
                continue;
            if(FD_ISSET(connectedFd,&readSet)){//readable 
                nRead = read(connectedFd,buf,1024);
                if(nRead ==0){//connection closed by client
                    close(connectedFd);
                    FD_CLR(connectedFd,&allSet);//remove Fd
                    clientConnectionArray[i] = -1;//remove Fd
                }else{
                    writen(connectedFd,buf,nRead);
                }

                if(--nReady<=0)
                    break;
            }
        }
    }
}




// void str_echo(int socketFd){
//     ssize_t n;
//     char buf[1024];
//     while(true){
//         while((n = read(socketFd,buf,1024))>0)
//             writen(socketFd,buf,n);

//         if(n<0 && errno == EINTR)
//             continue;
//         else if (n<0){
//             printf("str_echo:read error\n");
//             exit(1);
//         }
//     }
// }



