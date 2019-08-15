/**
 * @file utils.c
 * @brief Something useful wrapper function 
 * @author Jinliang Zheng
 * @version 1.0
 * @date 2019-07-11
 */

#include "utils.hh"

void
msg_msg(const char* msg)
/* no set errno */
{
	fprintf(stderr, "%s\n", msg);
}

void 
msg_quit(const char* msg)
/* no set errno */
{
	fprintf(stderr, "%s\n", msg);
	exit(0);
}

void
err_msg(const char* msg)
/* set errno */
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}

void 
/* set errno */
err_quit(const char* msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

char* 
Fgets(char* ptr, int n, FILE* stream)
{
	char *rptr;

	if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		err_quit("fgets error");

	return rptr;
}

void 
Fputs(const char* ptr, FILE* stream)
{
	if (fputs(ptr, stream) == EOF)
		err_quit("fputs error");
}

ssize_t 
readn(int fd, void* vptr, ssize_t n)
{
	ssize_t nleft;
	ssize_t nread;
	char* ptr;
	
	ptr = (char*)vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else 
				return -1;
		}
		else if (nread == 0)
			break;

		nleft -= nread;
		ptr += nread;
	}

	return (n - nleft);
}

ssize_t 
writen(int fd, void* vptr, ssize_t n)
{
	ssize_t nleft;
	ssize_t nwriten;
	const char* ptr;

	ptr = (char*)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwriten = write(fd, ptr, nleft)) <= 0) {
			if (nwriten < 0 && errno == EINTR)
				nwriten = 0;
			else 
				return -1;
		}
		nleft -= nwriten;
		ptr += nwriten;
	}
	return n;
}

void 
Writen(int fd, void* ptr, ssize_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		err_quit("Writen error");
}

void
Readn(int fd, void* ptr, ssize_t nbytes)
{
	if (readn(fd, ptr, nbytes) < 0)
		err_quit("Readn error");
}

ssize_t
readline(int fd, void* vptr, ssize_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = (char*)vptr;
	for (n = 1; n < maxlen; ++n) {
	again:
		if ( (rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0) {
			*ptr = 0;
			return n - 1;
		}
		else {
			if (errno == EINTR)
				goto again;
			return -1;
		}
	}
	*ptr = 0;
	return n;
}

ssize_t 
Readline(int fd, void* ptr, ssize_t maxlen)
{
	ssize_t n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
		err_quit("readline error");

	return n;
}

ssize_t 
Read(int fd, void* ptr, ssize_t nbytes)
{
	int n;

again:
	if ( (n = read(fd, ptr, nbytes)) < 0) {
		if ( n < 0 && errno == EINTR)
			goto again;
		else 
			err_quit("read error");
	}
	
	return n;
}

ssize_t
Write(int fd, void* ptr, ssize_t nbytes)
{
	int n;

	if ( (n = write(fd, ptr, nbytes)) != nbytes)
		err_quit("write error");

	return n;
}

int 
Socket(int family, int type, int protocol)
{
	int n;
	
	if ( (n = socket(family, type, protocol)) < 0)
		err_quit("socket error");

	return n;
}

void
Connect(int sockfd, const struct sockaddr* servaddr, socklen_t addrlen)
{
	if ( connect(sockfd, servaddr, addrlen) < 0) {
		if (errno == ETIMEDOUT) 
			err_quit("connect error: ETIMEDOUT");
		else if (errno == ECONNREFUSED)
			err_quit("connect error: ECONNREFUSED");
		else if (errno == EHOSTUNREACH || errno == ENETUNREACH)
			err_quit("connect error: EHOSTUNREACH || ENETUREACH");
		else 
			err_quit("connect error: unknown");
	}
}

void
Bind(int sockfd, const struct sockaddr* myaddr, socklen_t addrlen)
{
	if (bind(sockfd, myaddr, addrlen) < 0) {
		err_quit("bind error");
	}
}

void 
Listen(int sockfd, int backlog)
{
	char* ptr;

	if ( (ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);	

	if (listen(sockfd, backlog) < 0)
		err_quit("listen error");
}

int
Accept(int sockfd, struct sockaddr* cliaddr, socklen_t* addrlen)
{
	int n;

again:
	if ( (n = accept(sockfd, cliaddr, addrlen)) < 0) {
#ifdef EPROTO
		if (errno == EPROTO || errno == ECONNABORTED || errno == EINTR)
#else
		if (errno == ECONNABORTED || errno == EINTR)
#endif
			goto again;
		else
			err_quit("Accept error");
	}

	return n;
}

pid_t
Fork(void)
{
	pid_t pid;
	
	if ( (pid = fork()) == -1)
		err_quit("fork error");

	return pid;
}

void
Close(int fd)
{
	if (close(fd) == -1)
		err_quit("close error");
}
	
void
Shutdown(int sockfd, int howto)
{
	if (shutdown(sockfd, howto) < 0)
		err_quit("shutdown error");
}
	
Sigfunc*
Signal(int signo, Sigfunc* func)
{
	struct sigaction act, oact;

	act.sa_handler = func; 
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif 
	}

	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return (oact.sa_handler);
}

void
Inet_pton(int family, const char* strptr, void *addrptr)
{
	int n;

	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
		err_quit("inet_pton error for ");	/* errno set */
	else if (n == 0)
		msg_quit("inet_pton error for ");	/* errno not set */
}

int 
Select(int nfds, fd_set* readset, fd_set* writeset, fd_set* exceptset, struct timeval* timeout)
/* deal with EINTR? */
{
	int n;

	if ( (n = select(nfds, readset, writeset, exceptset, timeout)) < 0)
		err_quit("select error");

	return n;
}


