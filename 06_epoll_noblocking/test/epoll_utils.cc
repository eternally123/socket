#include "utils.hh"
#include "epoll_utils.hh"

int
setnonblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL, 0);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

/*
void
addfd(int epfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLOUT;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) < 0)
		err_quit("addfd error");
}
*/

void 
addfd(int epfd, int fd, unsigned mode)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = mode;

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) < 0)
		err_quit("addfd error");
}
	

void deletefd(int epfd, int fd)
{
	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
}

int 
Epoll_create()
{
	int n;

	if ( (n = epoll_create(100)) < 0)
		err_quit("epoll_create error");

	return n;
}

int 
Epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout)
{
	int number;

again:
	if ( (number = epoll_wait(epfd, events, maxevents, timeout)) < 0) {
		if (errno == EINTR)
			goto again;
		else 
			err_quit("epoll_wait error");
	}

	return number;
}
