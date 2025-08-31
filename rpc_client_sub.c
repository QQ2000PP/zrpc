#include "rpc_protocol.h"

int global_callerid =   0x1234567;



int add(int a, int b){

/*
	char * addjson = "{  \
			\"method\": \"add\", \
			\"params\": [\"3\", \"4\"], \
 			\"callerid\": 0x12345678 \
		}";

	zrpc_client_session(addjson);
*/

	cJSON * root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "method", "add");
	cJSON * params = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "params", params);
	cJSON_AddNumberToObject(params, "a", a);
	cJSON_AddNumberToObject(params, "b", b);	
	
	cJSON_AddNumberToObject(root, "callerid", global_callerid++); // global_callerid++ atomic
	char * body = cJSON_Print(root);
	zrpc_client_session(body);	
	cJSON_Delete(root);
	
	return 7;
}





int main(){

	int a = 4;
	int b = 3;
	int sum = add(a, b);

	printf("sum: %d\n", sum);
}





