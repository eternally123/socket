#ifndef _NDSL_SERVER_BUFFER_HH_
#define _NDSL_SERVER_BUFFER_HH_

#include <ctype.h>
#include <unistd.h>

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

class Buffer
{
private:
	char* _start;
	char* _wait_send;
	char* _wait_write;
	int   _size;
public:
	Buffer(int size = 4096);
	virtual ~Buffer();
	
	inline int size() const;
	inline char* start() const;
	inline char* wait_send() const;
	inline char* wait_write() const;

	inline bool isReadable() const;
	inline bool isWritable() const;
	inline bool isClear() const;
	inline void setClear();
	inline void setFull();

	int read_nonblock(int fd, int nbytes);
	int write_nonblock(int fd, int nbytes);
	int read_nonblock(int fd);
	int write_nonblock(int fd);

};

int 
Buffer::size() const
{
	return _size;
}

char*
Buffer::start() const
{
	return _start;
}

char*
Buffer::wait_send() const
{
	return _wait_send;
}

char*
Buffer::wait_write() const
{
	return _wait_write;
}

bool
Buffer::isReadable() const
{
	return _wait_write != (_start + _size);
}

bool 
Buffer::isWritable() const
{
	return _wait_send != _wait_write;
}

bool
Buffer::isClear() const
{
	return (_start == _wait_send) && (_wait_send == _wait_write);
}

void
Buffer::setClear() 
{
	_wait_send = _wait_write = _start;
}

void 
Buffer::setFull()
{
	_wait_write = _start + _size;
	_wait_send = _start;
}

#endif // _NDSL_SERVER_BUFFER_HH_
