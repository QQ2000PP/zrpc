

all:
	gcc -o rpc_server_pub rpc_server_pub.c rpc_reactor.c rpc_protocol.c crc32.c cJSON.c
	gcc -o rpc_client_sub rpc_client_sub.c rpc_protocol.c crc32.c cJSON.c

