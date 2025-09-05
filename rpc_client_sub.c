#include "rpc_protocol.h"



// method名
// params格式
// params数量
// result格式


int main(){


	int a = 111;
	int b = 222;
	int sum = add(a, b);

	printf("sum: %d\n", sum);

	char * a_char = "ceshi" ;
	char * b_char = " zhe" ;
	char * c_char = " ci" ;

	char * zcat_result = zcat(a_char, b_char, c_char);
	printf("zcat_result: %s\n", zcat_result);
	free(zcat_result);
	
	char * msg = "abcdefghihg12345678";
	int  length  = strlen(msg);

	char * sayhello_result = sayhello(msg, length);
	printf("sayhello_result: %s\n", sayhello_result);
	free(sayhello_result);



	char * str = "AABBccdd";
	int str_length = strlen(str);
	char * toupper_result = zrpc_toupper(str, str_length);
	printf("toupper_result: %s\n", toupper_result);
	free(toupper_result);


	
}





