#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib/chunks.h"
#include "lib/buffer.h"
#include "lib/udp_server.h"
#include "lib/async.h"
#include "lib/utils.h"


void hello_req(udp_server* server);
void query_req(udp_server* server);

unsigned peers_count = 0;
char** peers_ips = NULL;
int* peers_ports = NULL;

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

    chunks* chunks_head = NULL;

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
        buffer* req = buffer_create(2);
        char* ip = udp_server_receive(server, req->buffer, 2);
        short type = *(short*)buffer_get(req, 0);

        if(type == 1) hello_req(server);
        if(type == 2) query_req(server);
    }

    udp_server_destroy(server);
    chunks_destroy(chunks_head);
    free(peers_ips);
    free(peers_ips);
    
    return 0;
}


void hello_req(udp_server* server) {
    // Get chunk count
    buffer* chunk_count_req = buffer_create(2);
    udp_server_receive(server, chunk_count_req->buffer, 2);

    short chunks_c = *(short*)buffer_get(chunk_count_req, 0);

    // Chunks requested
    short chunks[chunks_c];

    for(int i=0;i<chunks_c; i++) {
        udp_server_receive(server, &chunks[i], 2);
    }

    buffer_destroy(chunk_count_req);
}

void query_req(udp_server* server) {
    buffer* chunk_count_req = buffer_create(10);

    // Get Protocol data
    unsigned ip     = *(unsigned*)buffer_get(chunk_count_req, 0);
    short port      = *(short*)buffer_get(chunk_count_req, 4);
    short peer_ttl  = *(short*)buffer_get(chunk_count_req, 6);
    short chunks_c  = *(short*)buffer_get(chunk_count_req, 8);

    short chunks[chunks_c];
    for(int i=0;i<chunks_c; i++) {
        udp_server_receive(server, &chunks[i], 2);
    }

    char* ip_str = uip_to_sip(ip);

    // Log
    printf("QUERY: %s:%i\n", ip_str, port);
    printf("PEER-TTL: %i\n", peer_ttl);
    printf("CHUNKS: ");
    for(int i=0;i<chunks_c; i++) {
        udp_server_receive(server, &chunks[i], 2);
        printf(" %s", chunks[i]);
    }
    printf("\n");



    buffer_destroy(chunk_count_req);
}