#include "rpc_reactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern int reactor_start(unsigned short port, msg_propocal rpc_handler);

int rpc_handler(char * rbuf, int rlength, char * response){

#if 0	
	printf("rbuf: %s\n", rbuf);
	memset(response, 0, MAX_BUF_SIZE);
	memcpy(response, rbuf, rlength);
	int wlength = rlength;
	return wlength;
#else


#endif
	
}


int main(int argc, char * argv[]){

	
	unsigned short port = (unsigned short)atoi(argv[1]);
	reactor_start(port, rpc_handler);


}




