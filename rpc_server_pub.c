#include "rpc_reactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern int reactor_start(unsigned short port, msg_propocal rpc_handler);

int rpc_handler(char * rbuf, int rlength, char * response){

#if 1	
	printf("rbuf + 8: %s\n", rbuf + 8);
	memset(response, 0, MAX_BUF_SIZE);
	memcpy(response, rbuf + 8, rlength - 8);

	printf("response: %s\n", response);
	
	int wlength = rlength - 8;
	return wlength;
#else





#endif
	
}


int main(int argc, char * argv[]){

	
	unsigned short port = (unsigned short)atoi(argv[1]);
	reactor_start(port, rpc_handler);


}




