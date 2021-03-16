
#include "queue.h"
#include "lib/chunks.h"

typedef struct client_data {
    chunks* chunks_head;
    unsigned chunks_count;
    char* peer_ip;
    int peer_port;
} client_data;

typedef struct request_data {
    client_data* client_data;
    udp_client* client;
    request* request;
} request_data;

void* chunk_info(request_data* client);
void* response(request_data* client);
