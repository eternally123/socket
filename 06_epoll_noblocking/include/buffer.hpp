#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_

#include "config.hpp"

class Buffer {
public:
    char* buf_;
    char* start_;
    char* end_;
    int size_;

public:
    Buffer();
    ~Buffer();
    bool isReadable();
    bool isWriteable();
    int read_noblock(int fd);
    int write_noblock(int fd);
};

#endif