#include "rpc_protocol.h"

int global_callerid =   0x1234567;

// method名
// params格式
// params数量
// result格式



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
}

char * zcat(char * a, char * b, char * c ){

/*
 #// zrpc_cat request{
{
    "method":	"zcat",
	"params":	{
		"a":	"abc",
		"b":	"rpq",
        "c":    "xyz"
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
	// 发送client send body
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


}


char * sayhello(char * msg, int length ){

/*
	 #// zrpc_sayhello request{
	{
	    "method":	"sayhello",
		"params":	{
			"msg":	"rpc nb",
			"length":	6,
		},
		"callerid":	19088743
	}

	 #// zrpc_sayhello response
	{
		"method":	"sayhello",
		"results":	"bc cpr",
		"callerid":	19088743
}
*/

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








}




int main(){

#if 0
	int a = 4;
	int b = 3;
	int sum = add(a, b);

	printf("sum: %d\n", sum);
#elif 0
	char * a = "abc" ;
	char * b = "rpq" ;
	char * c = "stu" ;

	char * zcat_result = zcat(a, b, c);
	printf("zcat_result: %s\n", zcat_result);
	free(zcat_result);
#elif 1
	char * msg = "zrpc nb le";
	int  length  = strlen(msg);

	char * sayhello_result = sayhello(msg, length);
	printf("sayhello_result: %s\n", sayhello_result);
	free(sayhello_result);


#endif	
}





