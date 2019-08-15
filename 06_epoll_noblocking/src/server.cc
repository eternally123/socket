#include "buffer.hpp"
#include "config.hpp"
#include "epoll.hpp"
#include "network.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>
#include <map>

void handleRead(int fd, Buffer* buffer)
{
    int n = buffer->read_noblock(fd);
}
void handleWrite(int fd, Buffer* buffer)
{
    int n = buffer->write_noblock(fd);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "argument num incorrect!" << std::endl;
        exit(1);
    }
    int listenfd;

    listenfd = open_listenfd(atoi(argv[1]), LISTEN_QUEUE_NUM);
    Epoll epoll;
    epoll.create(EPOLL_MAX_HANDLE, EPOLL_MAX_HANDLE);
    epoll.addFd(listenfd, EPOLLIN);

    int num_ready, new_connectfd;
    struct epoll_event event;
    std::map<int, Buffer*> users_buffer;
    while (1) {

        num_ready = epoll.wait(EPOLL_TIMEOUT);
        std::cout << "\n\nnum_ready:" << num_ready << std::endl;
        for (int i = 0; i < num_ready; ++i) {
            event = epoll.get_epoll_events()[i];
            if (event.data.fd == listenfd) { /* listen socket */
                std::cout << "new connect " << std::endl;
                int new_connectfd = accept_client(listenfd);
                std::cout << "connect:fd= " << new_connectfd << std::endl;
                if (new_connectfd == -1)
                    continue;
                set_noblock(new_connectfd);
                Buffer* buffer = new Buffer();
                // buffer->isWriteable();
                // users_buffer.insert(std::map<int, Buffer*>::value_type(new_connectfd, buffer));
                users_buffer[new_connectfd] = buffer;
                Buffer* buffer1 = users_buffer.find(new_connectfd)->second;
                // buffer1->isWriteable();
                epoll.addFd(new_connectfd, EPOLLIN | EPOLLOUT);
            } else {
                int connectedfd = event.data.fd;
                // std::cout << connectedfd << std::endl;
                Buffer* buffer = users_buffer.find(connectedfd)->second;
                if ((event.events & EPOLLIN) && buffer->isReadable()) { /* read */
                    handleRead(connectedfd, buffer);
                }
                if ((event.events & EPOLLOUT) && buffer->isWriteable()) { /* write */
                    handleWrite(connectedfd, buffer);
                }
            }
        }
    }
}