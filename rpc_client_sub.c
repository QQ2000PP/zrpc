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

/*
	char * add_response_json = "{  \
			\"method\": \"add\", \
			\"results\": 7, \
			\"rettype\": \"int\" \
		}";

*/


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
	// 解析response body
	root = cJSON_Parse(payload_client);
	cJSON * results = cJSON_GetObjectItem(root, "results");
	char * ret_num = cJSON_Print(results);	
	int  ret = atoi(ret_num);
	cJSON_Delete(root);

	free(payload_client);
	free(body);
	free(ret_num);
	
	return ret;	
}

char * zcat(char * a, char * b, char * c ){

/*
 #// zrpc_cat request{
{
    "method":	"zcat",
	"params":	{
		"a":	"abc",
		"b":	"rpq",
        "c":    "stu"
	},
	"callerid":	19088743
}

 #// zrpc_add response
{
	"method":	"zcat",
	"results":	"abcrpqstu",
	"callerid":	19088743
}

*/

	cJSON * root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "method", "zcat");
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
	// 解析response body
	root = cJSON_Parse(payload_client);
	cJSON * results = cJSON_GetObjectItem(root, "results");
	char * ret_num = cJSON_Print(results);	

	cJSON_Delete(root);

	free(payload_client);
	free(body);
//	free(ret_num);
	
	return ret_num;	




}




int main(){

#if 0
	int a = 4;
	int b = 3;
	int sum = add(a, b);

	printf("sum: %d\n", sum);
#elif 1
	char * a = "abc" ;
	char * b = "rpq" ;
	char * c = "stu" ;

	char * zcat_result = zcat(a, b, c);
	printf("zcat_result: %s\n", zcat_result);
	free(zcat_result);
#endif	
}





