#include "rpc_protocol.h"











int zrpc_connect_server(const char * ip, unsigned short port){


	int connfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	serveraddr.sin_port = htons(port);

	socklen_t length = sizeof(struct sockaddr_in);
	
	if (0 != connect(connfd, (struct sockaddr *)&serveraddr, length)){
		perror("connect failed\n");
		return -1;
	}

	return connfd;
}



void zrpc_header_constr(char * rpc_header, char * body){

	memcpy(rpc_header, ZRPC_HEAD_VERSION, 2);

	*(unsigned short *)(rpc_header + 2) =  (unsigned short)strlen(body);
	*(unsigned int *)(rpc_header + 4) = (unsigned int)crc32(body, strlen(body));


}




char * zrpc_client_session(char * body){

	// send
	int connfd = zrpc_connect_server(ZRPC_CLIENT_IP, ZRPC_CLIENT_PORT);

	char rpc_header[ZRPC_HEAD_LENGTH] = {0};
	if (!rpc_header) return NULL;

	zrpc_header_constr(rpc_header, body);

	int total_send = send(connfd, rpc_header, ZRPC_HEAD_LENGTH, 0);
	total_send += send(connfd, body, strlen(body), 0);		


	// recv 

	memset(rpc_header, 0, ZRPC_HEAD_LENGTH);
	int ret = recv(connfd, rpc_header, ZRPC_HEAD_LENGTH, 0);
	unsigned short body_length = *(unsigned short *)(rpc_header + 2);
	printf("body_length: %d\n", body_length);		
	
	char * count_client = (char *)malloc(body_length + 1);
	if (!count_client) return NULL;
	memset(count_client, 0, body_length);
	int count = recv(connfd, count_client, body_length, 0);
	assert(count == body_length);
	
	printf("count_client: %s, count: %d\n", count_client, count);
	free(count_client);	

	


}



char * zrpc_server_session(char * bodyload){

	


}









