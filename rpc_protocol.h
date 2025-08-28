#ifndef __rpc_protoco_h__
#define __rpc_protoco_h__



#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include "rpc_reactor.h"










#define ZRPC_HEAD_LENGTH  8 
#define ZRPC_HEAD_VERSION "01" 






int zrpc_connect_server(const char * ip, unsigned short port);
char * zrpc_client_send(char * body);





#endif 
