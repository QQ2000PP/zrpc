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





char * zrpc_client_send(char * body){

	int connfd = zrpc_connect_server(ZRPC_CLIENT_IP, ZRPC_CLIENT_PORT);

		char rpc_header[ZRPC_HEAD_LENGTH] = {0};
		memcpy(rpc_header, ZRPC_HEAD_VERSION, 2);

		*(unsigned short *)(rpc_header + 2) =  (unsigned short)strlen(body);
		*(unsigned int *)(rpc_header + 4) = (unsigned int)crc32(body, strlen(body));		
		
		
		int total_send = send(connfd, rpc_header, ZRPC_HEAD_LENGTH, 0);
		total_send += send(connfd, body, strlen(body), 0);		
 
		close(connfd);

}







