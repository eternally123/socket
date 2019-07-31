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

#include <sys/select.h>
#include <fcntl.h>

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

void str_echo(int sockfd){
	//add
    #define MAXLINE 1024
    //end

	int			maxfdp1, val, stdineof;
	ssize_t		n, nwritten;
	fd_set		rset, wset;
	char		buf[MAXLINE];
	char		*iptr, *optr;

	val = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

	iptr = optr = buf;	/* initialize buffer pointers */
	stdineof = 0;

	maxfdp1 = sockfd + 1;
	while(1){
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		// if (stdineof == 0 && toiptr < &to[MAXLINE])
		// 	FD_SET(STDIN_FILENO, &rset);	/* read from stdin */
		if (iptr < &buf[MAXLINE])
			FD_SET(sockfd, &rset);			/* read from socket */
		if (optr != iptr)
			FD_SET(sockfd, &wset);			/* data to write to socket */
		// if (froptr != friptr)
		// 	FD_SET(STDOUT_FILENO, &wset);	/* data to write to stdout */

        printf("before select...\n");
		select(maxfdp1, &rset, &wset, NULL, NULL);
        printf("after select...\n");

		// if (FD_ISSET(STDIN_FILENO, &rset)) {
		// 	printf("read from stdin\n");
        //     if ( (n = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0) {
		// 		if (errno != EWOULDBLOCK){
        //             printf("read error on stdin\n");
        //             exit(1);
        //         }
		// 	} else if (n == 0) {
		// 		// fprintf(stderr, "%s: EOF on stdin\n", gf_time());
		// 		stdineof = 1;			/* all done with stdin */
		// 		if (tooptr == toiptr)
		// 			shutdown(sockfd, SHUT_WR);/* send FIN */
		// 	} else {
		// 		// fprintf(stderr, "%s: read %d bytes from stdin\n", gf_time(), n);
		// 		toiptr += n;			/* # just read */
		// 		FD_SET(sockfd, &wset);	/* try and write to socket below */
		// 	}
		// }

		if (FD_ISSET(sockfd, &rset)) {
            printf("read from client\n");
			if ( (n = read(sockfd, iptr, &buf[MAXLINE] - iptr)) < 0) {
				if (errno != EWOULDBLOCK){
                    printf("read error on socket\n");
                    exit(1);
                }
			} else if (n == 0) {
				// fprintf(stderr, "%s: EOF on socket\n", gf_time());
				if (stdineof)
					return;		/* normal termination */
				else{
                    printf("tr_cli: server terminated prematurely\n");
                    exit(1);
                }
			} else {
				// fprintf(stderr, "%s: read %d bytes from socket\n",gf_time(), n);
				iptr += n;		/* # just read */
				FD_SET(sockfd, &wset);	/* try and write below */
			}
		}

		// if (FD_ISSET(STDOUT_FILENO, &wset) && ( (n = friptr - froptr) > 0)) {
        //     printf("write to stdout\n");
		// 	if ( (nwritten = write(STDOUT_FILENO, froptr, n)) < 0) {
		// 		if (errno != EWOULDBLOCK){
        //             printf("write error to stdout\n");
        //             exit(1);
        //         }
		// 	} else {
		// 		// fprintf(stderr, "%s: wrote %d bytes to stdout\n",gf_time(), nwritten);
		// 		froptr += nwritten;		/* # just written */
		// 		if (froptr == friptr)
		// 			froptr = friptr = fr;	/* back to beginning of buffer */
		// 	}
		// }

		if (FD_ISSET(sockfd, &wset) && ( (n = iptr - optr) > 0)) {
            printf("write to client\n");
			if ( (nwritten = write(sockfd, optr, n)) < 0) {
				if (errno != EWOULDBLOCK){
                    printf("write error to socket\n");
                    exit(1);
                }
			} else {
				// fprintf(stderr, "%s: wrote %d bytes to socket\n",gf_time(), nwritten);
				optr += nwritten;	/* # just written */
				if (optr == iptr) {
					iptr = optr = buf;	/* back to beginning of buffer */
					if (stdineof)
						shutdown(sockfd, SHUT_WR);	/* send FIN */
				}
			}
		}
	}
}



// void str_echo(int socketFd){
//     ssize_t n;
//     char buf[BUFFER_SIZE];

//     while(true){
//         printf("before read...n=%d\n",n);
//         n = read(socketFd,buf,BUFFER_SIZE);
//         printf("after read...n=%d\n",n);
//         if(n>0){//read data from client
//             writen(socketFd,buf,n);
//             continue;
//         }else if (n<0){
//             printf("n<0:n=%d\n",n);
//             if(errno == EINTR){//interupt by system. read again
//                 printf("interupt by system  EINTR. read again\n");
//                 continue;
//             }else if(errno == ECONNABORTED){//client send RST before accept
//                 printf("interupt by system  ECONNABORTED. read again\n");
//             }else{
//                 printf("str_echo:read error\n");
//                 exit(1);
//             }
//         }else{//n==0 connection closed by client
//             return;
//         }
//     }
// }

void sig_child(int signo){
    pid_t pid;
    int stat;

    while((pid = waitpid(-1,&stat,WNOHANG)) > 0)
        printf("child %d terminated\n",pid);
    
    return ;
}



