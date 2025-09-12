#include "rpc_protocol.h"



#define MAX_PARAMS  16



struct zrpc_func{

	char * method;
	char * params[MAX_PARAMS];
	char * types[MAX_PARAMS];
	char * rettype;
	int argv;

	struct zrpc_func * next;	
};

struct zrpc_func * zrpc_func_table = NULL;
int global_callerid =   0x1234567;



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
//	printf("body_length: %d\n", body_length);		
	
	char * payload_client = (char *)malloc(body_length + 1);

//	printf("payload_client malloc sucessful\n");
	
	if (!payload_client) return NULL;
	memset(payload_client, 0, body_length);
	int count = recv(connfd, payload_client, body_length, 0);

//	printf("payload_client: %s, count: %d\n", payload_client, count);	
	assert(count == body_length);

//	free(payload_client);	
//	printf("payload_client: %s\n", payload_client);

	
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
//	printf("msg_result: %s\n", msg);
	return msg;
}


char * zrpc_method_toupper(char *msg, int length) {
    if (!msg || length <= 0) return NULL;

    char *str_upper = (char *)malloc(length + 1); // 分配内存，+1 保留 '\0'
    if (!str_upper) return NULL;

    for (int i = 0; i < length; i++) {
        str_upper[i] = toupper(msg[i]);
    }

//	printf("zrpc_method_toupper: %s\n", str_upper);
	
	memset(msg, 0, length);
	memcpy(msg, str_upper,length);
	free(str_upper);
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
//	printf("func: %s, strcmp(): %d\n", func, strcmp(method->valuestring, "zrpc_toupper"));
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

	}else if (strcmp(method->valuestring, "zrpc_toupper") == 0){

		cJSON * params = cJSON_GetObjectItem(root, "params");
		cJSON * msg = cJSON_GetObjectItem(params, "msg");		
		cJSON * length = cJSON_GetObjectItem(params, "length");	

		
		char * result_toupper= zrpc_method_toupper(msg->valuestring, length->valueint);
		

		// 组织成cJSON（不能把之前的root删掉，因为有重要的callerid）
		cJSON * response = cJSON_CreateObject();
		cJSON_AddStringToObject(response, "method", "zrpc_toupper");
		cJSON_AddStringToObject(response, "results", result_toupper);
		cJSON_AddNumberToObject(response, "callerid", callerid->valueint);
		char * server_body_toupper = cJSON_Print(response);
		cJSON_Delete(response);

		printf("server_body_result_toupper: %s\n", server_body_toupper);		
		free(result_toupper);
			
		return server_body_toupper;


	}else {
		printf("this fuction: %s is not included now!\n", method->valuestring);
		return NULL;
	}

}




int read_conf_decode(char * file_content){

	if (!file_content) return -1;

/*
	struct zrpc_func * func = (struct zrpc_func *)malloc(sizeof(struct zrpc_func)); 
	if (!func) return -2;
	memset(func, 0, sizeof(struct zrpc_func));
*/

	cJSON * root = cJSON_Parse(file_content);
	cJSON * zrpc_namespace = cJSON_GetObjectItem(root, "namespace");
	cJSON * zrpc_ip = cJSON_GetObjectItem(root, "remote");
	cJSON * zrpc_port = cJSON_GetObjectItem(root, "port");
	cJSON * zrpc_config = cJSON_GetObjectItem(root, "config");

	int config_size = cJSON_GetArraySize(zrpc_config);
	
	printf("config_size: %d\n", config_size);
	
	int i = 0;
	for (i = 0; i < config_size; i++){

		// 每个一个func分配内存
		struct zrpc_func * func = (struct zrpc_func *)malloc(sizeof(struct zrpc_func)); 
		if (!func) return -2;
		memset(func, 0, sizeof(struct zrpc_func));

	
		cJSON * config_ele = cJSON_GetArrayItem(zrpc_config, i);
		cJSON * method = cJSON_GetObjectItem(config_ele, "method");
		cJSON * params = cJSON_GetObjectItem(config_ele, "params");		
		cJSON * types = cJSON_GetObjectItem(config_ele, "types");		
		cJSON * rettypes = cJSON_GetObjectItem(config_ele, "rettype");	

		int params_size = cJSON_GetArraySize(params);		
		int types_size = cJSON_GetArraySize(types);	
		assert((params_size == types_size) && (params_size < MAX_PARAMS));		
		func->method = strdup(method->valuestring);
		func->rettype = strdup(rettypes->valuestring);		

		int j = 0;
		for (j = 0; j < params_size; j++){
			cJSON * param = cJSON_GetArrayItem(params, j);
			func->params[j] = strdup(param->valuestring);
			cJSON * type = cJSON_GetArrayItem(types, j);	
			func->types[j] = strdup(type->valuestring);
		}

		func->argv = params_size;

		func->next =  zrpc_func_table;
		zrpc_func_table = func;
	}	


/*
		// func
		struct zrpc_func * current =  zrpc_func_table; // 不要破坏链表zrpc_func_table的表头
		while(current){
			printf("current->method: %s\n", current->method);
			current = current->next;
		}
*/
		cJSON_Delete(root);
	return 0;	

}





char *  read_conf(char * filename){
	if (!filename) return NULL;
	
	int file_fd = open(filename, O_RDONLY);
	if (!file_fd) return NULL;

	off_t file_size = lseek(file_fd, 0, SEEK_END);	
	
	#if 0	
	
	close(file_fd);
	file_fd = open(filename, O_RDONLY);
	
	#else

	lseek(file_fd, 0, SEEK_SET);

	#endif
	char * file_content = (char *)malloc(file_size + 1);
	memset(file_content, 0, file_size + 1);

	if (!file_content) {
		close(file_fd);
		free(file_content);
		return NULL;
	}

	ssize_t count = read(file_fd, file_content, file_size);
	file_content[file_size] = '\0';

//	printf("read_conf ---> file_content:\n %s\n", file_content);	
	return file_content; 
}




int zrpc_caller_register( char * filename){

	if (!filename) return -1;

	char * file_content = 	read_conf(filename);
	
	if (read_conf_decode(file_content) != 0) return -1;

//	printf("2zrpc_caller_register ---> file_content: %s\n", file_content);
	
	free(file_content);
}


char * zrpc_caller_name(void){

	const void * return_address = __builtin_return_address(1);
	Dl_info  info;
	int a = dladdr(return_address, &info);
//	printf("ddladdr: %d, dli_sname: %s\n", a, info.dli_sname);
	if ((0 != dladdr(return_address, &info) && (info.dli_sname != NULL))){

		return (char *)info.dli_sname;
	}

	return NULL;
}



char * caller(int numargc, ...){

	char * method = zrpc_caller_name();

//	printf("--------------------------------------\n");

	struct zrpc_func *  func = zrpc_func_table; 

//   printf("=== caller函数开始 ===\n");
//    printf("zrpc_func_table地址: %p\n", zrpc_func_table);

	int x = 0;
	while(func){
		x = x + 1;		
//		printf("----------------->Test zrpc_func_table->method: %s, method: %s, x: %d\n", func->method, method, x);
		if (strcmp(func->method, method) == 0) break;		
		func = func->next;
	}

	if (!func) return NULL;

	assert(numargc == func->argv);

	cJSON * root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "method", method);
	cJSON * params = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "params", params);

	// 如何获取参数，一种固定模式如下：从可变参数列表中逐个提取参数 va_list结构体
	va_list args;
	va_start(args, numargc);

	int i = 0;
	for (i = 0; i < numargc; i++){
		if (strcmp(func->types[i], "int") == 0){
			cJSON_AddNumberToObject(params, func->params[i], va_arg(args, int));		
		}else if (strcmp(func->types[i], "char *") == 0){
			cJSON_AddStringToObject(params, func->params[i], va_arg(args, char *));	
		}else if (strcmp(func->types[i], "float") == 0){
			cJSON_AddNumberToObject(params, func->params[i], va_arg(args, double));	
		}else {
			printf("no support parmeter: %s, type: %s\n", func->params[i], func->types[i]);
			return NULL;
		}
	}
	
	va_end(args);

	cJSON_AddNumberToObject(root, "callerid", global_callerid++); // global_callerid++ atomic
	char * body = cJSON_Print(root);
	printf("client_send: %s\n", body);
	cJSON_Delete(root);	


	char * payload_client =	zrpc_client_session(body);	
	printf("client_recv: %s\n", payload_client);
	// 解析client response body
	root = cJSON_Parse(payload_client);
	cJSON * results = cJSON_GetObjectItem(root, "results");
	char * ret_num = cJSON_Print(results);	
//	int  ret = atoi(ret_num);
	cJSON_Delete(root);

	free(payload_client);
	free(body);
//	free(ret_num);
	printf("-------------->ret_num: %s\n", ret_num);
	return ret_num;

	
//	return method;
}




// rpc_client commit


/*
struct zrpc_func{

	char * method;
	char * params[MAX_PARAMS];
	char * types[MAX_PARAMS];
	char * rettype;
	int argv;

	struct zrpc_func * next;	
};

*/





int add(int a, int b){

#if 1

	char * result = caller(2, a, b);
	int  ret = atoi(result);
	free(result);	
	return ret;
	
#else
/*
	char * addjson = "{  \
			\"method\": \"add\", \
			\"params\": [\"3\", \"4\"], \
 			\"callerid\": 0x12345678 \
		}";

	zrpc_client_session(addjson);
*/

/*
	char * add_response_json = "{  \
			\"method\": \"add\", \
			\"results\": 7, \
			\"rettype\": \"int\" \
		}";

*/

	// 发送client send body
	cJSON * root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "method", "add");
	cJSON * params = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "params", params);
	cJSON_AddNumberToObject(params, "a", a);
	cJSON_AddNumberToObject(params, "b", b);	
	
	cJSON_AddNumberToObject(root, "callerid", global_callerid++); // global_callerid++ atomic
	char * body = cJSON_Print(root);
	printf("client_send: %s\n", body);
	cJSON_Delete(root);	


	char * payload_client =	zrpc_client_session(body);	
	printf("client_recv: %s\n", payload_client);
	// 解析client response body
	root = cJSON_Parse(payload_client);
	cJSON * results = cJSON_GetObjectItem(root, "results");
	char * ret_num = cJSON_Print(results);	
	int  ret = atoi(ret_num);
	cJSON_Delete(root);

	free(payload_client);
	free(body);
	free(ret_num);
	
	return ret;
#endif	
}

char * zcat(char * a, char * b, char * c ){

#if 1

	char * result = caller(3, a, b, c);
	char *  ret = strdup(result);
	free(result);
	return ret;

#else
	// 发送client send body
	cJSON * root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "method","add");
	cJSON * params = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "params", params);
	cJSON_AddStringToObject(params, "a", a);
	cJSON_AddStringToObject(params, "b", b);
	cJSON_AddStringToObject(params, "c", c);

	cJSON_AddNumberToObject(root, "callerid", global_callerid++); // global_callerid++ atomic
	char * body = cJSON_Print(root);
	printf("client_send: %s\n", body);
	cJSON_Delete(root);	


	char * payload_client =	zrpc_client_session(body);	
	// 解析client response body
	root = cJSON_Parse(payload_client);
	cJSON * results = cJSON_GetObjectItem(root, "results");
	char * ret_num = cJSON_Print(results);	
	char * ret_strdup = strdup(ret_num);


	cJSON_Delete(root);

	free(payload_client);
	free(body);
	free(ret_num);
	
	return ret_strdup;	

#endif
}


char * sayhello(char * msg, int length ){

#if 1

	char * result = caller(2, msg, length);
	char *	ret = strdup(result);
	free(result);
	return ret;

#else 
	// 发送client send body
	cJSON * root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "method", "sayhello");
	cJSON * params = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "params", params);
	cJSON_AddStringToObject(params, "msg", msg);
	cJSON_AddNumberToObject(params, "length", length);

	cJSON_AddNumberToObject(root, "callerid", global_callerid++); // global_callerid++ atomic
	char * body = cJSON_Print(root);
	printf("client_send: %s\n", body);
	cJSON_Delete(root); 


	char * payload_client = zrpc_client_session(body);	
	
	// 解析client response body
	root = cJSON_Parse(payload_client);
	cJSON * results = cJSON_GetObjectItem(root, "results");
	char * ret_num = cJSON_Print(results);	
	char * ret_strdup = strdup(ret_num);


	cJSON_Delete(root);

	free(payload_client);
	free(body);
	free(ret_num);

	return ret_strdup;	
#endif


}

char * zrpc_toupper(char * msg, int length ){

#if 1
	
		char * result = caller(2, msg, length);
		char *	ret = strdup(result);
		free(result);
		return ret;
	
#else 

	// 发送client send body
	cJSON * root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "method", "zrpc_toupper");
	cJSON * params = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "params", params);
	cJSON_AddStringToObject(params, "msg", msg);
	cJSON_AddNumberToObject(params, "length", length);

	cJSON_AddNumberToObject(root, "callerid", global_callerid++); // global_callerid++ atomic
	char * body = cJSON_Print(root);
	printf("client_send: %s\n", body);
	cJSON_Delete(root); 


	char * payload_client = zrpc_client_session(body);	
	
	// 解析client response body
	root = cJSON_Parse(payload_client);
	cJSON * results = cJSON_GetObjectItem(root, "results");
	char * ret_num = cJSON_Print(results);	
	char * ret_strdup = strdup(ret_num);


	cJSON_Delete(root);

	free(payload_client);
	free(body);
	free(ret_num);

	return ret_strdup;	

#endif

}






