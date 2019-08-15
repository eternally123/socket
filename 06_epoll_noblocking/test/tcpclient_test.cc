/**
 * 简单回射负载发生器 skyzheng
 *
 * 编译：
 * 	g++ tcpclient_test.cc epoll_utils.cc utils.cc buffer.cc -o tcpclient_test 
 *	g++ 版本比较低的需要加编译选项 -std=c++11
 *
 * 参数：
 * 	tcp_links: 连接数
 * 	msg_size:  报文大小，单位字节
 *	times:     所有连接总的发送次数
 * 
 * 注意：
 *	水平触发，阻塞连接，非阻塞收发数据。
 *	connect为阻塞状态，但不计入总的数据收发时间。
 *	delay指标系统误差偏大，因要记录每个报文的发送和接收时刻。
 *	
 */


#include "utils.hh"
#include "epoll_utils.hh"
#include "buffer.hh"
#include <list>
#include <time.h>
#include <iostream>

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
	std::cout << "Connection successful..." << std::endl;

	struct epoll_event events[tcp_links];
	int send_times = times;

	std::list<clock_t> send_time;
	std::list<clock_t> recv_time;
	clock_t main_start = clock();
	while (times > 0) {
		int number = Epoll_wait(epfd, events, tcp_links, -1);
		for (int i = 0; i < number; ++i) {
			data_t* data_ptr = (data_t*)events[i].data.ptr;
			if ((events[i].events & EPOLLOUT) && (send_times > 0)) {
				data_ptr->send_buffer->write_nonblock(data_ptr->sockfd);
				if (data_ptr->send_buffer->isClear()) {
					data_ptr->send_buffer->setFull();
					--send_times;
					send_time.push_back(clock());
				}
			}
			if (events[i].events & EPOLLIN) {
				data_ptr->recv_buffer->read_nonblock(data_ptr->sockfd);
				if (!(data_ptr->recv_buffer->isReadable())) {
					data_ptr->recv_buffer->setClear();
					--times;
					recv_time.push_back(clock());
				}
			}
		}
	}
	clock_t main_end = clock();

	std::cout << "total time: " << (main_end - main_start) / double(CLOCKS_PER_SEC) * 1000 << "ms" << std::endl;

	if (send_time.size() == recv_time.size()) {
		std::list<clock_t>::iterator send_iter = send_time.begin();
		std::list<clock_t>::iterator recv_iter = recv_time.begin();
		clock_t average = 0;
		int sum = 0;
		while ((send_iter != send_time.end()) && (recv_iter != recv_time.end())) {
			average = ((average * sum) + *recv_iter - *send_iter) / (sum + 1);
			++sum;
			++send_iter;
			++recv_iter;
		}
		std::cout << "average delay: " << average / double(CLOCKS_PER_SEC) * 1000 << "ms" << std::endl;
	}
	else {
		std::cout << "Fuck you... " << std::endl;
	}
}






	       






