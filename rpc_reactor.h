
#ifndef __rpc_reactor_h__
#define __rpc_reactor_h__


#define MAX_BUF_SIZE  2048
#define MAX_FD        1024


#define RPC_NETWORK   1
#define ZRPC_MSG_HEADER_LENGTH  8



typedef int (* msg_propocal)(char * rbuf, int rlength, char * response);


typedef int (*cb)(int fd);

typedef struct _reactor{

	int fd;
	
	char rbuf[MAX_BUF_SIZE];
	int rlength;

	char wbuf[MAX_BUF_SIZE];
	int wlength;	

	union{
		cb cb_accept;
		cb cb_recv;
	}cb_epollin;
	cb cb_send;

}f_reactor;


int rpc_request(f_reactor *s_reactor);
int rpc_response(f_reactor *s_reactor);


int set_event(int fd, int event, int event_switch);
int register_str(int fd, int event, int event_switch);
int accept_callback(int fd);
int recv_callback(int fd);
int send_callback(int fd);
int reactor_init_server(unsigned short port);
int reactor_start(unsigned short port, msg_propocal handler);



#endif


