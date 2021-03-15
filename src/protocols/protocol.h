#ifndef PROTOCOL_H 
#define PROTOCOL_H 

#include<stdlib.h>

#include "lib/buffer.h"
#include "lib/udp_server.h"

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


typedef struct req {
    char* ip;
    short port;

    short type;
    void* payload;
} req;



hello_req* parse_hello_req(udp_server* server) {
    hello_req* req = (hello_req*)malloc(sizeof(hello_req));
    buffer* buff = buffer_create(2);

    udp_server_receive(server, buff->buffer, 2);

    req->chk_n = *(short*)buffer_get(buff, 0);
    req->chk_ids = (short*)malloc(req->chk_n*sizeof(short));

    for(int i=0;i<req->chk_n; i++) {
        udp_server_receive(server, (char*)&req->chk_ids[i], 2);
    }
    
    return req;
}

void destroy_hello_req(hello_req* req) {
    free(req->chk_ids);
    free(req);
}



get_req* parse_get_req(udp_server* server) {
    get_req* req = (get_req*)malloc(sizeof(get_req));
    buffer* buff = buffer_create(2);

    udp_server_receive(server, buff->buffer, 2);

    req->chk_n = *(short*)buffer_get(buff, 0);
    req->chk_ids = (short*)malloc(req->chk_n*sizeof(short));

    for(int i=0;i<req->chk_n; i++) {
        udp_server_receive(server, (char*)&req->chk_ids[i], 2);
    }
    
    return req;
}

void destroy_get_req(get_req* req) {
    free(req->chk_ids);
    free(req);
}

query_req* parse_query_req(udp_server* server) {
    query_req* req = (query_req*)malloc(sizeof(query_req));
    buffer* buff = buffer_create(10);
    udp_server_receive(server, buff->buffer, 10);

    req->ip         = *(unsigned*)buffer_get(buff, 0);
    req->port       = *(short*)buffer_get(buff, 4);
    req->peer_ttl   = *(short*)buffer_get(buff, 6);
    req->chk_n      = *(short*)buffer_get(buff, 8);

    req->chk_ids = (short*)malloc(req->chk_n*sizeof(short));

    for(int i=0;i<req->chk_n; i++) {
        udp_server_receive(server, (char*)&req->chk_ids[i], 2);
    }
    
    return req;
}

void destroy_query_req(query_req* req) {
    free(req->chk_ids);
    free(req);
}


chunk_info_req* parse_chunk_info_req(udp_server* server) {
    chunk_info_req* req = (chunk_info_req*)malloc(sizeof(chunk_info_req));
    buffer* buff = buffer_create(2);
    udp_server_receive(server, buff->buffer, 2);
    
    req->chk_n = *(short*)buffer_get(buff, 0);
    req->chk_ids = (short*)malloc(req->chk_n*sizeof(short));

    for(int i=0;i<req->chk_n; i++) {
        udp_server_receive(server, (char*)&req->chk_ids[i], 2);
    }
    
    return req;
}

void destroy_chunk_info_req(chunk_info_req* req) {
    free(req->chk_ids);
    free(req);
}


response_req* parse_response_req(udp_server* server) {
    response_req* req = (response_req*)malloc(sizeof(response_req));
    buffer* buff = buffer_create(4);
    udp_server_receive(server, buff->buffer, 4);

    req->chk_id = *(short*)buffer_get(buff, 0);
    req->chk_size = *(short*)buffer_get(buff, 2);
    req->chk = (char*)malloc(sizeof(char)* req->chk_size);

    for(int i=0;i<req->chk_size; i++) {
        udp_server_receive(server, &req->chk[i], 1);
    }
    
    return req;
}

void destroy_response_req(response_req* req) {
    free(req->chk);
    free(req);
}

req* parse_req(udp_server* server) {
    req* r = (req*)malloc(sizeof(req));
    buffer* buff = buffer_create(2);
    udp_address addr = udp_server_receive(server, buff->buffer, 2);
    
    r->type = *(short*)buffer_get(buff, 0);
    r->ip = addr.ip;
    r->port = addr.port;

    switch (r->type) {
    case 1: r->payload = parse_hello_req(server); break;
    case 2: r->payload = parse_query_req(server); break;
    case 3: r->payload = parse_chunk_info_req(server); break;
    case 4: r->payload = parse_get_req(server); break;    
    default: break;
    }

    buffer_destroy(buff);
    return r;
}

void destroy_req(req* r) {
    switch (r->type) {
    case 1: destroy_hello_req((hello_req*)r->payload); break;
    case 2: destroy_query_req((query_req*)r->payload); break;
    case 3: destroy_chunk_info_req((chunk_info_req*)r->payload); break;
    case 4: destroy_get_req((get_req*)r->payload); break;
    default: break;
    }

    free(r->ip);
    free(r);
}

#endif