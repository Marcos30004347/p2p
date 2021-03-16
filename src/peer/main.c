#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib/chunks.h"
#include "lib/buffer.h"

#include "lib/udp_server.h"
#include "lib/udp_client.h"

#include "lib/async.h"
#include "lib/utils.h"
#include "protocol.h"


void handle_hello_request(udp_server*, request*);
void handle_query_request(udp_server*, request*);

unsigned peers_count = 0;
char** peers_ips = NULL;
int* peers_ports = NULL;
chunks* chunks_head = NULL;

int main(int argc, char *argv[]) {
    FILE* keyfile = fopen(argv[2], "rb");

    fseek(keyfile, 0L, SEEK_END);
    unsigned long size = ftell(keyfile);
    fseek(keyfile, 0L, 0);

    char keyfile_content[size];
    fread(keyfile_content,sizeof(char), size, keyfile);

    peers_count = argc - 3;
    peers_ips = malloc(sizeof(char*)*argc-2);
    peers_ports = malloc(sizeof(int)*argc-2);

    for(int i=3; i<argc - 3; i++) {
        char* address = argv[i];
        char *ptr = strtok(address, ":");
        peers_ips[i - 3] = ptr; 
        ptr = strtok(NULL, ":");
        peers_ports[i - 3] = atoi(ptr); 
    }

    unsigned chunks_count = 0;

    char *ptr = strtok(keyfile_content, ": ");
    while(ptr != NULL) {
        chunks_count++;

        short chunk_id = atoi(ptr);
        char *chunk_path = strtok(NULL, "\n");

        chunks* c = chunks_create(chunk_id, chunk_path);
        chunks_head = chunks_add(chunks_head, c);

        ptr = strtok(NULL, ": ");
    }

    udp_server* server = udp_server_create(atoi(argv[1]));

    while(1) {
        request* req = parse_request(server);
        switch (req->type) {
        case 1:handle_hello_request(server, req); break;
        case 2:handle_query_request(server, req); break;
        default: break;
        }
        
        destroy_request(req);
    }

    udp_server_destroy(server);
    chunks_destroy(chunks_head);
    free(peers_ips);
    free(peers_ips);
    
    return 0;
}


void handle_hello_request(udp_server* server, request* req) {
    hello_req* hr = (hello_req*)req->payload;
    
    short peer_ttl = 3;

    // Set query message buffer
    buffer* buff = buffer_create(10 + 2*hr->chk_n);
    buffer_set(buff, 0, sip_to_uip(req->ip), 4);
    buffer_set(buff, 4, req->port, 2);
    buffer_set(buff, 6, &peer_ttl, 2);
    buffer_set(buff, 8, &hr->chk_n, 2);
    for(int i=0; i<hr->chk_n; i++)
        buffer_set(buff, 8 + i*2, &hr->chk_ids[i], 2);

    // Send messages to peers
    for(int i=0;i<peers_count; i++){
        udp_client* client = udp_client_create(peers_ips[i], peers_ports[i]);
        if(client) {
            udp_send_message_to(udp_client_get_id(client), buffer_get(buff, 0));
            udp_client_destroy(client);
        }
    }

    buffer_destroy(buff);
}

void handle_query_request(udp_server* server, request* req) {
    query_req* qr = (query_req*)req->payload;

    chunks* tmp = chunks_head;

    short chunk_info_type = 3;
    short chunk_info_len = 4 + 2*qr->chk_n;

    buffer* chunk_info_buff = buffer_create(chunk_info_len);
    buffer_set(chunk_info_buff, 0, &chunk_info_type, 2);
    buffer_set(chunk_info_buff, 2, &qr->chk_n, 2);
    for(int i=0; i<qr->chk_n; i++)
        buffer_set(chunk_info_buff, 8 + i*2, &qr->chk_ids[i], 2);
    
    char* client_ip = uip_to_sip(qr->ip);

    while(tmp) {
        for(int c=0; c < qr->chk_n; c++) {
            if(tmp->id == qr->chk_ids[c]) {
                udp_server_send_message_to(server, client_ip, qr->port, buffer_get(chunk_info_buff, 0), chunk_info_len);
            }
        }
        tmp = tmp->next;
    }

    free(client_ip);
    buffer_destroy(chunk_info_buff);


    long query_len = 10 + 2*qr->chk_n;
    short peer_ttl = qr->peer_ttl - 1;
    
    if(peer_ttl <= 0) return;
    
    buffer* buff = buffer_create(query_len);
    buffer_set(buff, 0, qr->ip, 4);
    buffer_set(buff, 4, qr->port, 2);
    buffer_set(buff, 6, &peer_ttl, 2);
    buffer_set(buff, 8, &qr->chk_n, 2);

    // Send messages to peers
    for(int i=0;i<peers_count; i++){
        if(strcmp(client_ip, req->ip) != 0)
            udp_server_send_message_to(server, client_ip, qr->port, buffer_get(buff, 0), query_len);
    }

    buffer_destroy(buff);

}
void handle_get_request(udp_server* server, request* req) {
    get_req* gr = (get_req*)req->payload;

    short response_type = 5;


    chunks* tmp = chunks_head;
    while(tmp) {
        for(int i=0;i<gr->chk_n; i++) {
            if(gr->chk_ids[i] == tmp->id) {
                buffer* response_buff = buffer_create(1030);

                buffer_set(response_buff, 0, &response_type, 2);
                buffer_set(response_buff, 2, tmp->id, 2);

                FILE* chunk = fopen(tmp->path, "rb");
                fseek(chunk, 0L, SEEK_END);
                long size = ftell(chunk);
            
                if(size > 1024) {
                    printf("ERROR: chunks is too big\n");
                    exit(0);
                }
            
                fseek(chunk, 0L, SEEK_SET);

                short _size = (short)size;
                buffer_set(response_buff, 4, &tmp->id, 2);
                buffer_set(response_buff, 6, &_size, 2);
 
                char chunk_content[size];
                fread(chunk_content, sizeof(char), size, chunk);
                fclose(chunk);

                buffer_set(response_buff, 8, chunk_content, size);
    
                udp_server_send_message_to(server, req->ip, req->port, buffer_get(response_buff, 0), 6+size);
                buffer_destroy(response_buff);
            }
        }    
        tmp = tmp->next;
    }

}