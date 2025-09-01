#ifndef __rpc_protoco_h__
#define __rpc_protoco_h__


#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include "rpc_reactor.h"
#include "crc32.h"
#include <assert.h>
#include <stdlib.h>
#include "cJSON.h"







#define ZRPC_CLIENT_IP  "192.168.80.134"
#define ZRPC_CLIENT_PORT  2000



#define ZRPC_HEAD_LENGTH  8 
#define ZRPC_HEAD_VERSION "01" 






int zrpc_connect_server(const char * ip, unsigned short port);
char * zrpc_client_session(char * body);
void zrpc_header_constr(char * rpc_header, char * body);
char * zrpc_server_session(char * bodyload);





#endif 
