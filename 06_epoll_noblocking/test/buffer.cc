#include "buffer.hh"
#include "utils.hh"
#include <errno.h>
#include <iostream>

/**
 * Buffer:
 *
 *    +-----------+-------------+-----------------+
 *    |    sent   |  wait send  |    wait write   |
 *    +-----------+-------------+-----------------+  
 *    ^           ^             ^ 
 *  _start    _wait_send    _wait_write 
 *
 */

Buffer::Buffer(int size) :
	_size(size)
{
	_start = new char[size];
	_wait_send = _wait_write = _start;
}

Buffer::~Buffer()
{
	delete[] _start;
}

int 
Buffer::read_nonblock(int fd, int nbytes)
{
	int n;
	int left = &(_start[_size]) - _wait_write;
	nbytes = nbytes < left ? nbytes : left;

	if ( (n = ::read(fd, _wait_write, nbytes)) < 0) {
        if ((errno != EWOULDBLOCK) && (errno != EAGAIN)){
            printf("read error");
			//err_quit("read error");
        }
	}
	else if (n > 0) {
		_wait_write += n;
	}
#ifdef _BUFFER_DEBUG_
	std::cout << "Read " << n << " bytes" << std::endl;
#endif // _BUFFER_DEBUG_	
	return n;
}

int 
Buffer::read_nonblock(int fd)
{
	int n;
	int left = &(_start[_size]) - _wait_write;

	if ( (n = ::read(fd, _wait_write, left)) < 0) {
		if ((errno != EWOULDBLOCK) && (errno != EAGAIN))
		    printf("read error errno= %d ",errno);
            //	err_quit("read error");
	}
	else if (n > 0) {
		_wait_write += n;
	}

	return n;
}

int 
Buffer::write_nonblock(int fd, int nbytes)
{
	int n;
	int left = _wait_write - _wait_send;
	nbytes = nbytes < left ? nbytes : left;

	if ( (n = ::write(fd, _wait_send, nbytes)) < 0) {
		if ((errno != EWOULDBLOCK) && (errno != EAGAIN))
			err_quit("write error");
	}
	else if (n > 0) {
		_wait_send += n;
		if (_wait_send == _wait_write)
			_wait_send = _wait_write = _start;
	}

	return  n;
}

int
Buffer::write_nonblock(int fd)
{
	int n;
	int left = _wait_write - _wait_send;

	if ( (n = ::write(fd, _wait_send, left)) < 0){
		if ((errno != EWOULDBLOCK) && (errno != EAGAIN))
			err_quit("write error");
	}
	else if (n > 0) {
		_wait_send += n;
		if (_wait_send == _wait_write)
			_wait_send = _wait_write = _start;
	}

	return n;
}
