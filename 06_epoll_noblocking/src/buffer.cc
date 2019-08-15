#include "buffer.hpp"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Buffer::Buffer()
{
    size_ = BUFFER_SIZE;
    buf_ = new char[BUFFER_SIZE];
    start_ = end_ = buf_;
}

Buffer::~Buffer()
{
    delete[] buf_;
}

bool Buffer::isReadable()
{
    if (end_ != &buf_[size_ - 1])
        return true;
    return false;
}

bool Buffer::isWriteable()
{
    if (end_ != start_)
        return true;
    return false;
}

int Buffer::read_noblock(int fd)
{
    int n;
    int left = &buf_[size_ - 1] - end_ + 1;

    if ((n = ::read(fd, end_, left)) < 0) {
        if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
            printf("read error \n");
            exit(1);
        }
    } else if (n > 0) {
        end_ += n;
    }
    return n;
}

int Buffer::write_noblock(int fd)
{
    int n;
    int left = end_ - start_;

    if ((n = ::write(fd, start_, left)) < 0) {
        if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
            printf("write error \n");
            exit(1);
        }
    } else if (n > 0) {
        start_ += n;
        if (start_ == end_) {
            start_ = buf_;
            end_ = buf_;
        }
    }
    return n;
}