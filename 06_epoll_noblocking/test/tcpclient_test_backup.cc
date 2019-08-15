/**
 *
 * 简单的回射服务器测试程序
 * 
 * 编译：
 * 	g++ tcpclient_test.cc epoll_utils.cc buffer.cc -o tcpclient_test
 *
 * 参数：
 * 	tcp_links: 本客户端程序同时对服务端打开的tcp连接数
 * 	msg_size:  每次发送的报文长度，单位字节
 * 	times:     所有连接一起向服务端发送且接受到的报文数之和
 *
 * 注意：
 * 	1. epoll水平触发
 * 	2. 单核
 * 	3. times是所有线程发送的总数
 * 	4. 赶工，不一定能跑满cpu或者io，你们先用着
 *
 */

#include "utils.hh"
#include "epoll_utils.hh"
#include "buffer.hh"

typedef struct data
{
	int sockfd;
	Buffer* send_buffer;
	Buffer* recv_buffer;
} data_t;


int 
main(int argc, char** argv)
{
	int sockfd, tcp_links, msg_size, times;
	struct sockaddr_in servaddr;

	if (argc != 6) {
		printf("Usage: %s <ip> <port> <tcp_links> <msg_size> <times>\n", argv[0]);
		exit(0);
	}

	tcp_links = atoi(argv[3]);
	msg_size = atoi(argv[4]);
	times = atoi(argv[5]);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_port = htons(atoi(argv[2]));

	int epfd = Epoll_create();
	struct epoll_event ev;
	for (int i = 0; i < tcp_links; ++i) {
		sockfd = Socket(AF_INET, SOCK_STREAM, 0);
		Connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
		setnonblocking(sockfd);

		data_t* data_ptr = new data_t;
		data_ptr->sockfd = sockfd;
		data_ptr->send_buffer = new Buffer(msg_size);
		data_ptr->send_buffer->setFull();
		data_ptr->recv_buffer = new Buffer(msg_size);
		data_ptr->recv_buffer->setClear();

		ev.data.ptr = data_ptr;
		ev.events = EPOLLOUT | EPOLLIN;
		epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
	}

	struct epoll_event events[tcp_links];
	int send_times = times;
	while (times > 0) {
		int number;
        //printf("before number=%d\n",number);
        number = Epoll_wait(epfd, events, tcp_links, -1);
		//printf("number=%d\n",number);
        for (int i = 0; i < number; ++i) {
			data_t* data_ptr = (data_t*)events[i].data.ptr;
			if ((events[i].events & EPOLLOUT) && (send_times > 0)) {
				//printf("send data send_times=%d\n",send_times);
                data_ptr->send_buffer->write_nonblock(data_ptr->sockfd);
				if (data_ptr->send_buffer->isClear()) {
					data_ptr->send_buffer->setFull();
					--send_times;
				}
			}
			if (events[i].events & EPOLLIN) {
				//printf("read data times=%d\n",times);
				data_ptr->recv_buffer->read_nonblock(data_ptr->sockfd);
				if (!(data_ptr->recv_buffer->isReadable())) {
					data_ptr->recv_buffer->setClear();
					--times;
				}
			}
		}
	}
}

