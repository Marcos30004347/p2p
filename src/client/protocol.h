#ifndef PROTOCOL_H 
#define PROTOCOL_H 

#include<stdlib.h>

#include "lib/buffer.h"
#include "lib/udp_client.h"

typedef struct hello_req {
    short chk_n;
    short* chk_ids;
} hello_req;

typedef struct get_req {
    short chk_n;
    short* chk_ids;
} get_req;

typedef struct chunk_info_req {
    short chk_n;
    short* chk_ids;
} chunk_info_req;

typedef struct response_req {
    short chk_id;
    short chk_size;
    char* chk;
} response_req;

typedef struct query_req {
    unsigned ip;
    int port;
    short peer_ttl;
    short chk_n;
    short* chk_ids;
} query_req;

typedef struct request {
    char* ip;
    short port;

    short type;
    void* payload;
} request;

request* parse_request(udp_client*);
void destroy_request(request*);

#endif