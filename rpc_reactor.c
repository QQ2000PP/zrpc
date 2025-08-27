// reactor.c events VS callback ---> struct{fd callback }

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include "rpc_reactor.h"


#define  MAX_PORTS   1


static msg_propocal kvs_msg;
//extern int kvs_request(f_reactor *s_reactor);

int kvs_request(f_reactor *s_reactor){

//	printf("ret: %d, recv: %s\n", s_reactor->rlength, s_reactor->rbuf);

	int count = kvs_msg(s_reactor->rbuf, s_reactor->rlength, s_reactor->wbuf);
	s_reactor->wlength = count;

}



int epfd = 0;




f_reactor s_reactor[MAX_FD] = {0};



int set_event(int fd, int event, int event_switch);
int register_str(int fd, int event, int event_switch);
int accept_callback(int fd);
int recv_callback(int fd);
int send_callback(int fd);
int reactor_init_server(unsigned short port);


int set_event(int fd, int event, int event_switch){

	if (event_switch == 0){

		struct epoll_event ev;
		ev.events = event;
		ev.data.fd = fd;

		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

	}else if (event_switch == 1){

		struct epoll_event ev;
		ev.events = event;
		ev.data.fd = fd;

		epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);

	}
		
}


int register_str(int fd, int event, int event_switch){

	s_reactor[fd].fd = fd;
	memset(s_reactor[fd].rbuf,0,MAX_BUF_SIZE);
	s_reactor[fd].rlength = 0;

	memset(s_reactor[fd].wbuf,0,MAX_BUF_SIZE);
	s_reactor[fd].wlength = 0;

	s_reactor[fd].cb_epollin.cb_recv = recv_callback;
	s_reactor[fd].cb_send = send_callback;	

	set_event(fd, event, event_switch);

}


int accept_callback(int fd){

		struct sockaddr_in clientaddr;
		socklen_t len = sizeof(struct sockaddr_in);

		int clientfd = accept(fd, (struct sockaddr *)&clientaddr, &len);
//		printf("accept finished\n");

		register_str(clientfd, EPOLLIN, 0);

}

int recv_callback(int fd){


	memset(s_reactor[fd].rbuf, 0, MAX_BUF_SIZE);
	s_reactor[fd].rlength = 0; 
	memset(s_reactor[fd].wbuf,0,MAX_BUF_SIZE);
	s_reactor[fd].wlength = 0;



	int ret = recv(fd, s_reactor[fd].rbuf, MAX_BUF_SIZE,0);
//	printf("ret: %d, recv: %s\n", ret, s_reactor[fd].rbuf);

		// close
		if (ret == 0){
//			printf("fd: %d close\n", fd);
			close(fd);
			epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL); 
			return -1;
		}

	set_event(fd, EPOLLOUT, 1);

	s_reactor[fd].rlength = ret;
#if 0 
	memcpy(s_reactor[fd].wbuf, s_reactor[fd].rbuf, s_reactor[fd].rlength);
	s_reactor[fd].wlength = s_reactor[fd].rlength;
	
	return ret;

#else

	kvs_request(&s_reactor[fd]);

#endif 		

}


int send_callback(int fd){

	int count = send(fd, s_reactor[fd].wbuf, s_reactor[fd].wlength, 0);
//	printf("ret: %d, send: %s\n", s_reactor[fd].wlength, s_reactor[fd].wbuf);

	set_event(fd, EPOLLIN, 1);
	return count;
}


int reactor_init_server(unsigned short port){

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr,sizeof(serveraddr) )){
		perror("bind failed\n");
		return -1;
	}

	listen(sockfd, 10);
//	printf("Listen finished\n");

	return sockfd;
}


int reactor_start(unsigned short port, msg_propocal handler){    // non-complete

//	unsigned short port = 2048;
	kvs_msg = handler;
	

	epfd = epoll_create(1);


	int j = 0;
	for (j = 0;j < MAX_PORTS;j ++){

	// init_server
	int sockfd = reactor_init_server(port + j);

	//register to s_reactor	
   // epoll event register

	s_reactor[sockfd].fd = sockfd;
	s_reactor[sockfd].cb_epollin.cb_accept = accept_callback;
	set_event(sockfd, EPOLLIN, 0);	

	}
	
	// while

	while(1){	

		struct epoll_event events[1024];
		int nready = epoll_wait(epfd, events, 1024, -1);

		int i = 0;
		for (i = 0; i < nready; i ++){
			int eventfd = events[i].data.fd;
			if (events[i].events & EPOLLIN){
				s_reactor[eventfd].cb_epollin.cb_recv(eventfd);				
			}

			if (events[i].events & EPOLLOUT){
				s_reactor[eventfd].cb_send(eventfd);				
			}

		}
		
	}

	return 0;
}











