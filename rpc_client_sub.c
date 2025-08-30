#include "rpc_protocol.h"


int add(int a, int b){

	char * addjson = "{  \
			\"method\": \"add\", \
			\"params\": [\"3\", \"4\"], \
 			\"types\": [\"int\", \"int\"], \
			\"rettype\": \"int\" \
		}";
	
	zrpc_client_session(addjson);
	return 7;
}





int main(){

	int a = 4;
	int b = 3;
	int sum = add(a, b);

	printf("sum: %d\n", sum);
}





