#include "protocol.h"
#include "buffer.h"
#include "udp_client.h"


hello_req* parse_hello_req(udp_client* client) {
    hello_req* req = (hello_req*)malloc(sizeof(hello_req));
    buffer* buff = buffer_create(2);

    udp_server_receive(client, buff->buffer, 2);

    req->chk_n = *(short*)buffer_get(buff, 0);
    req->chk_ids = (short*)malloc(req->chk_n*sizeof(short));

    for(int i=0;i<req->chk_n; i++) {
        udp_server_receive(client, (char*)&req->chk_ids[i], 2);
    }
    
    return req;
}

void destroy_hello_req(hello_req* req) {
    free(req->chk_ids);
    free(req);
}



get_req* parse_get_req(udp_client* client) {
    get_req* req = (get_req*)malloc(sizeof(get_req));
    buffer* buff = buffer_create(2);

    udp_server_receive(client, buff->buffer, 2);

    req->chk_n = *(short*)buffer_get(buff, 0);
    req->chk_ids = (short*)malloc(req->chk_n*sizeof(short));

    for(int i=0;i<req->chk_n; i++) {
        udp_server_receive(client, (char*)&req->chk_ids[i], 2);
    }
    
    return req;
}

void destroy_get_req(get_req* req) {
    free(req->chk_ids);
    free(req);
}

query_req* parse_query_req(udp_client* client) {
    query_req* req = (query_req*)malloc(sizeof(query_req));
    buffer* buff = buffer_create(10);
    udp_server_receive(client, buff->buffer, 10);

    req->ip         = *(unsigned*)buffer_get(buff, 0);
    req->port       = *(short*)buffer_get(buff, 4);
    req->peer_ttl   = *(short*)buffer_get(buff, 6);
    req->chk_n      = *(short*)buffer_get(buff, 8);

    req->chk_ids = (short*)malloc(req->chk_n*sizeof(short));

    for(int i=0;i<req->chk_n; i++) {
        udp_server_receive(client, (char*)&req->chk_ids[i], 2);
    }
    
    return req;
}

void destroy_query_req(query_req* req) {
    free(req->chk_ids);
    free(req);
}


chunk_info_req* parse_chunk_info_req(udp_client* client) {
    chunk_info_req* req = (chunk_info_req*)malloc(sizeof(chunk_info_req));
    buffer* buff = buffer_create(2);
    udp_server_receive(client, buff->buffer, 2);
    
    req->chk_n = *(short*)buffer_get(buff, 0);
    req->chk_ids = (short*)malloc(req->chk_n*sizeof(short));

    for(int i=0;i<req->chk_n; i++) {
        udp_server_receive(client, (char*)&req->chk_ids[i], 2);
    }
    
    return req;
}

void destroy_chunk_info_req(chunk_info_req* req) {
    free(req->chk_ids);
    free(req);
}


response_req* parse_response_req(udp_client* client) {
    response_req* req = (response_req*)malloc(sizeof(response_req));
    buffer* buff = buffer_create(4);
    udp_server_receive(client, buff->buffer, 4);

    req->chk_id = *(short*)buffer_get(buff, 0);
    req->chk_size = *(short*)buffer_get(buff, 2);
    req->chk = (char*)malloc(sizeof(char)* req->chk_size);

    for(int i=0;i<req->chk_size; i++) {
        udp_server_receive(client, &req->chk[i], 1);
    }
    
    return req;
}

void destroy_response_req(response_req* req) {
    free(req->chk);
    free(req);
}

request* parse_request(udp_client* client) {
    request* r = (request*)malloc(sizeof(request));
    buffer* buff = buffer_create(2);
    udp_address addr = udp_server_receive(client, buff->buffer, 2);
    
    r->type = *(short*)buffer_get(buff, 0);
    r->ip = addr.ip;
    r->port = addr.port;

    switch (r->type) {
    case 1: r->payload = parse_hello_req(client); break;
    case 2: r->payload = parse_query_req(client); break;
    case 3: r->payload = parse_chunk_info_req(client); break;
    case 4: r->payload = parse_get_req(client); break;    
    default: break;
    }

    return r;
}

void destroy_request(request* r) {
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