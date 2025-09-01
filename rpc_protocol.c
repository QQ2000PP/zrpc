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
	
	char * payload_client = (char *)malloc(body_length + 1);
	if (!payload_client) return NULL;
	memset(payload_client, 0, body_length);
	int count = recv(connfd, payload_client, body_length, 0);
	assert(count == body_length);
	
//	printf("payload_client: %d, count: %d\n", payload_client, count);
//	free(payload_client);	

	
	close(connfd);

	return payload_client;
}



int zrpc_method_add(int a, int b){
	return a+b;

}


char * zrpc_server_session(char * bodyload){

	// 传client发过来cJSON解析为char * --> 执行函数，返回char* --> 返回CJSON  	
	// 怎么使method和对应的函数连接起来
	// 为什么 strcmp(func, "add") 不相等？？？？
	
	cJSON *root = cJSON_Parse(bodyload);
	if (!root) return NULL;

	cJSON * method = cJSON_GetObjectItem(root, "method");

	char * func = cJSON_Print(method);
	printf("func: %s, method: %s, strcmp(func, \"add\"): %d\n", func, "method", strcmp(func, "add"));
	cJSON * callerid = cJSON_GetObjectItem(root, "callerid");
//	printf("callerid: %d\n", callerid->valueint);
		
	if (strcmp(method->valuestring, "add") == 0){
		cJSON * params = cJSON_GetObjectItem(root, "params");
		cJSON * a = cJSON_GetObjectItem(params, "a");		
		cJSON * b = cJSON_GetObjectItem(params, "b");	
		
		int result_add = zrpc_method_add(a->valueint, b->valueint);
		

		// 组织成cJSON（不能把之前的root删掉，因为有重要的callerid）
		cJSON * response = cJSON_CreateObject();
		cJSON_AddStringToObject(response, "method", "add");
		cJSON_AddNumberToObject(response, "results", result_add);
		cJSON_AddNumberToObject(response, "callerid", callerid->valueint);
		char * server_body_add = cJSON_Print(response);
		cJSON_Delete(response);

		printf("server_body_add: %s\n", server_body_add);
		return server_body_add;
					
		

	}else if (strcmp(method->valuestring, "zcat") == 0){

	}else if (strcmp(method->valuestring, "sayhello") == 0){


	}else {
		printf("this fuction: %s is not included now!\n", method->valuestring);
		return NULL;
	}

}









