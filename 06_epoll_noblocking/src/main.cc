#include "buffer.hpp"

#include <stdio.h>
int main()
{
    Buffer* buffer = new Buffer();
    printf("%p %p %p", buffer->start_, buffer->end_, buffer->buf_);
}