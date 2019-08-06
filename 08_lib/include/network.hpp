// #ifndef _NETWORK_HPP_
// #define _NETWORK_HPP_

// #include <sys/types.h>

// #include <string>



// int open_listenfd(int port,int listen_queue_num);
// int accept_client(int listenfd);
// int open_connectfd(char *ip,int port);


// ssize_t readn(int fd,void *buff,size_t n);

// ssize_t writen(int fd,const void *buff,size_t n);

// /* automaticly add '\0' after '\n' */
// ssize_t readline(int fd,void *buff,size_t maxLength);



// int Socket(int family,int type,int protocol);
// int Bind(int fd,const struct sockaddr* socket_address,__socklen_t socket_length);
// int Listen(int fd,int num);
// int Accept(int fd,struct sockaddr* socket_address,__socklen_t* socket_length);
// int Connect(int fd,const struct sockaddr* socket_address,__socklen_t socket_length);


// #endif