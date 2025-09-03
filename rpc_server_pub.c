#include "rpc_reactor.h"
#include "rpc_protocol.h"



char * add_response_json = "{  \
		\"method\": \"add\", \
		\"results\": 7, \
		\"callerid\": 1111111 \
	}";

char * zcat_response_json = "{  \
		\"method\": \"zcat\", \
		\"results\": \"abcrpqxyz\", \
		\"callerid\": 11111111 \
	}";

char * sayhello_response_json = "{  \
		\"method\": \"sayhello\", \
		\"results\": \"bc cprz\", \
		\"callerid\": 11111112 \
	}";



extern int reactor_start(unsigned short port, msg_propocal rpc_handler);

int rpc_handler(char * rbuf, int rlength, char * response){

#if 0	
	printf("rbuf + 8: %s\n", rbuf + 8);

	memset(response, 0, MAX_BUF_SIZE);
	memcpy(response, rbuf + 8, rlength - 8);
	
	int wlength = rlength - 8;
	return wlength;


#else 

	// server_recv
	char rpc_header[ZRPC_HEAD_LENGTH] = {0};
	memset(rpc_header, 0, ZRPC_HEAD_LENGTH);
	memcpy(rpc_header, rbuf, ZRPC_HEAD_LENGTH);
	
	unsigned short body_length = *(unsigned short *)(rpc_header + 2);
	printf("body_length: %d\n", body_length);	
	unsigned int callerid = *(unsigned int *)(rpc_header + 4);
		
	char * body_client = (char *)malloc(body_length + 1);
	if (!body_client) return -1;
	memset(body_client, 0, body_length);
	memcpy(body_client, rbuf + ZRPC_HEAD_LENGTH, body_length);
	printf("body_client: %s\n", body_client);	


	
	// 函数运行
	char * body_server =  zrpc_server_session(body_client);




	// server_send	
	memset(response, 0, MAX_BUF_SIZE);
	memset(rpc_header, 0, ZRPC_HEAD_LENGTH);

	#if 0
		#if 0
		memcpy(rpc_header, ZRPC_HEAD_VERSION, 2);
		*(unsigned short *)(rpc_header + 2) =  (unsigned short)strlen(add_response_json);
		*(unsigned int *)(rpc_header + 4) = callerid;	
		
		memcpy(response, rpc_header, ZRPC_HEAD_LENGTH);	
		memcpy(response + ZRPC_HEAD_LENGTH, add_response_json, strlen(add_response_json));

		#elif 0
		memcpy(rpc_header, ZRPC_HEAD_VERSION, 2);
		*(unsigned short *)(rpc_header + 2) =  (unsigned short)strlen(zcat_response_json);
		*(unsigned int *)(rpc_header + 4) = callerid;	
		
		memcpy(response, rpc_header, ZRPC_HEAD_LENGTH);	
		memcpy(response + ZRPC_HEAD_LENGTH, zcat_response_json, strlen(zcat_response_json));		
		#elif 1
		memcpy(rpc_header, ZRPC_HEAD_VERSION, 2);
		*(unsigned short *)(rpc_header + 2) =  (unsigned short)strlen(sayhello_response_json);
		*(unsigned int *)(rpc_header + 4) = callerid;	
		
		memcpy(response, rpc_header, ZRPC_HEAD_LENGTH);	
		memcpy(response + ZRPC_HEAD_LENGTH, sayhello_response_json, strlen(sayhello_response_json));	
		#endif	
	#else
		memcpy(rpc_header, ZRPC_HEAD_VERSION, 2);
		*(unsigned short *)(rpc_header + 2) =  (unsigned short)strlen(body_server);
		*(unsigned int *)(rpc_header + 4) = callerid;	
		
		memcpy(response, rpc_header, ZRPC_HEAD_LENGTH); 
		memcpy(response + ZRPC_HEAD_LENGTH, body_server, strlen(body_server));
	#endif
	
		free (body_client);
		free (body_server);

#endif
	
}


int main(int argc, char * argv[]){

	
	unsigned short port = (unsigned short)atoi(argv[1]);
	reactor_start(port, rpc_handler);


}




