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


char * zrpc_method_zcat(char * a, char * b, char * c){

	if (!a || !b || !c) return NULL;

	int a_length = strlen(a);
	int b_length = strlen(b);	
	int c_length = strlen(c);

	char * zcat_result = (char * )malloc(a_length + b_length + c_length + 1);
	if (!zcat_result) return NULL;
	memset(zcat_result, 0, a_length + b_length + c_length + 1);
	memcpy(zcat_result, a, a_length);
	memcpy(zcat_result + a_length, b, b_length);
	memcpy(zcat_result + a_length + b_length, c, c_length);

	return zcat_result;

}


char * zrpc_method_sayhello(char * msg, int length){

	if (!msg || (length <= 0)) return NULL;
	
	int i = 0;
	for (i = 0; i < length / 2; i ++){
		char temp = msg[i];
		msg[i] = msg[length -1 - i]; 
		msg[length - i - 1]  = temp;
	}
	printf("msg_result: %s\n", msg);
	return msg;
}



char * zrpc_server_session(char * bodyload){

	// 传client发过来cJSON解析为char * --> 执行函数，返回char* --> 返回CJSON  	
	// 怎么使method和对应的函数连接起来
	// 为什么 strcmp(func, "add") 不相等？？？？
	
	cJSON *root = cJSON_Parse(bodyload);
	if (!root) return NULL;

	cJSON * method = cJSON_GetObjectItem(root, "method");

	char * func = cJSON_Print(method);
//	printf("func: %s, method: %s, strcmp(func, \"add\"): %d\n", func, "method", strcmp(func, "add"));
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
		cJSON * params = cJSON_GetObjectItem(root, "params");
		cJSON * a = cJSON_GetObjectItem(params, "a");		
		cJSON * b = cJSON_GetObjectItem(params, "b");	
		cJSON * c = cJSON_GetObjectItem(params, "c");	

		
		char * result_zcat = zrpc_method_zcat(a->valuestring, b->valuestring, c->valuestring);
		

		// 组织成cJSON（不能把之前的root删掉，因为有重要的callerid）
		cJSON * response = cJSON_CreateObject();
		cJSON_AddStringToObject(response, "method", "zcat");
		cJSON_AddStringToObject(response, "results", result_zcat);
		cJSON_AddNumberToObject(response, "callerid", callerid->valueint);
		char * server_body_zcat = cJSON_Print(response);
		cJSON_Delete(response);

		printf("server_body_zcat: %s\n", server_body_zcat);		
		free(result_zcat);
			
		return server_body_zcat;

		
	

	}else if (strcmp(method->valuestring, "sayhello") == 0){
		cJSON * params = cJSON_GetObjectItem(root, "params");
		cJSON * msg = cJSON_GetObjectItem(params, "msg");		
		cJSON * length = cJSON_GetObjectItem(params, "length");	

		
		char * result_sayhello = zrpc_method_sayhello(msg->valuestring, length->valueint);
		

		// 组织成cJSON（不能把之前的root删掉，因为有重要的callerid）
		cJSON * response = cJSON_CreateObject();
		cJSON_AddStringToObject(response, "method", "sayhello");
		cJSON_AddStringToObject(response, "results", result_sayhello);
		cJSON_AddNumberToObject(response, "callerid", callerid->valueint);
		char * server_body_sayhello = cJSON_Print(response);
		cJSON_Delete(response);

		printf("server_body_result_sayhello: %s\n", server_body_sayhello);		
		free(result_sayhello);
			
		return server_body_sayhello;



	}else {
		printf("this fuction: %s is not included now!\n", method->valuestring);
		return NULL;
	}

}









