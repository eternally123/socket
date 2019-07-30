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


#include <sys/select.h>
#include <fcntl.h>

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

// void str_client(FILE *fp,int socketFd){
//     char sendLine[BUFFER_SIZE],receiveLine[BUFFER_SIZE];

//     while(fgets(sendLine,BUFFER_SIZE,fp)!=NULL){
//         write(socketFd,sendLine,strlen(sendLine));
//         if(readline(socketFd,receiveLine,BUFFER_SIZE)==0){
//             printf("str_client:server terminated prematurely\n");
//             exit(1);
//         }
//         fputs(receiveLine,stdout);
//     }
// }

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


int connect_nonb(int sockfd,sockaddr *saptr, socklen_t salen, int nsec)
{
	int				flags, n, error;
	socklen_t		len;
	fd_set			rset, wset;
	struct timeval	tval;

	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	error = 0;
	if ( (n = connect(sockfd, saptr, salen)) < 0)
		if (errno != EINPROGRESS)
			return(-1);

	/* Do whatever we want while the connect is taking place. */

	if (n == 0)
		goto done;	/* connect completed immediately */

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;

	if ( (n = select(sockfd+1, &rset, &wset, NULL,nsec ? &tval : NULL)) == 0) {
		close(sockfd);		/* timeout */
		errno = ETIMEDOUT;
		return(-1);
	}

	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			return(-1);			/* Solaris pending error */
	} else{
		printf("select error: sockfd not set\n");
        exit(1);
    }

done:
	fcntl(sockfd, F_SETFL, flags);	/* restore file status flags */

	if (error) {
		close(sockfd);		/* just in case */
		errno = error;
		return(-1);
	}
	return(0);
}



void str_client(FILE *fp, int sockfd){
    //add
    #define MAXLINE 1024
    //end

	int			maxfdp1, val, stdineof;
	ssize_t		n, nwritten;
	fd_set		rset, wset;
	char		to[MAXLINE], fr[MAXLINE];
	char		*toiptr, *tooptr, *friptr, *froptr;

	val = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

	val = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);

	val = fcntl(STDOUT_FILENO, F_GETFL, 0);
	fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);

	toiptr = tooptr = to;	/* initialize buffer pointers */
	friptr = froptr = fr;
	stdineof = 0;

    maxfdp1 = STDIN_FILENO > STDOUT_FILENO ? STDIN_FILENO:STDOUT_FILENO;
	maxfdp1 = maxfdp1 > sockfd ? (maxfdp1 + 1):(sockfd + 1);
	while(1){
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		if (stdineof == 0 && toiptr < &to[MAXLINE])
			FD_SET(STDIN_FILENO, &rset);	/* read from stdin */
		if (friptr < &fr[MAXLINE])
			FD_SET(sockfd, &rset);			/* read from socket */
		if (tooptr != toiptr)
			FD_SET(sockfd, &wset);			/* data to write to socket */
		if (froptr != friptr)
			FD_SET(STDOUT_FILENO, &wset);	/* data to write to stdout */

        printf("before select...\n");
		select(maxfdp1, &rset, &wset, NULL, NULL);
        printf("after select...\n");

		if (FD_ISSET(STDIN_FILENO, &rset)) {
			printf("read from stdin\n");
            if ( (n = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0) {
				if (errno != EWOULDBLOCK){
                    printf("read error on stdin\n");
                    exit(1);
                }
			} else if (n == 0) {
				// fprintf(stderr, "%s: EOF on stdin\n", gf_time());
				stdineof = 1;			/* all done with stdin */
				if (tooptr == toiptr)
					shutdown(sockfd, SHUT_WR);/* send FIN */
			} else {
				// fprintf(stderr, "%s: read %d bytes from stdin\n", gf_time(), n);
				toiptr += n;			/* # just read */
				FD_SET(sockfd, &wset);	/* try and write to socket below */
			}
		}

		if (FD_ISSET(sockfd, &rset)) {
            printf("read from server\n");
			if ( (n = read(sockfd, friptr, &fr[MAXLINE] - friptr)) < 0) {
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
				friptr += n;		/* # just read */
				FD_SET(STDOUT_FILENO, &wset);	/* try and write below */
			}
		}

		if (FD_ISSET(STDOUT_FILENO, &wset) && ( (n = friptr - froptr) > 0)) {
            printf("write to stdout\n");
			if ( (nwritten = write(STDOUT_FILENO, froptr, n)) < 0) {
				if (errno != EWOULDBLOCK){
                    printf("write error to stdout\n");
                    exit(1);
                }
			} else {
				// fprintf(stderr, "%s: wrote %d bytes to stdout\n",gf_time(), nwritten);
				froptr += nwritten;		/* # just written */
				if (froptr == friptr)
					froptr = friptr = fr;	/* back to beginning of buffer */
			}
		}

		if (FD_ISSET(sockfd, &wset) && ( (n = toiptr - tooptr) > 0)) {
            printf("write to server\n");
			if ( (nwritten = write(sockfd, tooptr, n)) < 0) {
				if (errno != EWOULDBLOCK){
                    printf("write error to socket\n");
                    exit(1);
                }
			} else {
				// fprintf(stderr, "%s: wrote %d bytes to socket\n",gf_time(), nwritten);
				tooptr += nwritten;	/* # just written */
				if (tooptr == toiptr) {
					toiptr = tooptr = to;	/* back to beginning of buffer */
					if (stdineof)
						shutdown(sockfd, SHUT_WR);	/* send FIN */
				}
			}
		}
	}
}
/* end nonb3 */

