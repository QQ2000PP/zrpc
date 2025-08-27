
#ifndef __qs_kvs_reactor_h__
#define __qs_kvs_reactor_h__

#define MAX_BUF_SIZE  128
#define MAX_FD        1024


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


int kvs_request(f_reactor *s_reactor);
int kvs_response(f_reactor *s_reactor);


typedef int (* msg_propocal)(char *msg, int msglength, char *response);


#endif


