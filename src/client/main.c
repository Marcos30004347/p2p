#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "client.h"

#include "lib/chunks.h"
#include "lib/buffer.h"
#include "lib/udp_client.h"
#include "protocol.h"

queue* work_queue = NULL;
client_data* cli_data;

void hello(udp_client* client);

void* worker_thread(void*);

int main(int argc, char *argv[]) {
    cli_data = (client_data*)malloc(sizeof(client_data));
    cli_data->chunks_count = 0;

    // Parse flags
    char* address_arg = argv[1];
	char *ptr = strtok(address_arg, ":");

    cli_data->peer_ip = ptr;
    ptr = strtok(NULL, ":");
    cli_data->peer_port = atoi(ptr);

    ptr = strtok(argv[2], ",");
   
    while(ptr != NULL) {
        cli_data->chunks_count++;

        chunks* c = chunks_create(atoi(ptr), "");
        cli_data->chunks_head = chunks_add(cli_data->chunks_head, c);

        ptr = strtok(NULL, ",");
    }

    udp_client* client = udp_client_create();    

    work_queue = queue_init();

    hello(client);

    thread handler_thread = thread_create(worker_thread, client);

    while(1) {
        queue_push(
            work_queue,
            HANDLE_REQUEST,
            parse_request(client)
        );
    }

    udp_client_destroy(client);
    chunks_destroy(cli_data->chunks_head);
    queue_destroy(work_queue);
    free(cli_data);
    
    return 0;
}

void hello(udp_client* client) {
    short hello_code = 1;
    unsigned hello_size = 2 + 2 + (2*cli_data->chunks_count);
    
    buffer* hello_buff = buffer_create(hello_size);
    buffer_set(hello_buff, 0, &hello_code, 2);
    buffer_set(hello_buff, 2, &cli_data->chunks_count, 2);
    
    int it = 0;
    chunks* tmp = cli_data->chunks_head;

    while(tmp) {
        buffer_set(hello_buff, 4 + (it*2), &tmp->id, 2);
        tmp = tmp->next;
    }

    udp_address address;
    strcpy(address.ip, cli_data->peer_ip);
    address.port = cli_data->peer_port;

    udp_client_send_to(client, address, buffer_get(hello_buff, 0), hello_size);
    buffer_destroy(hello_buff);
}


void* worker_thread(void* data) {
    udp_client* client = (udp_client*)data;

    node* node = NULL;

    while(node = queue_pop(work_queue)) {
        if(node->type == HANDLE_REQUEST) {
            request* req = node->request;
            request_data* data = (request_data*)malloc(sizeof(request_data));
        
            data->client = client;
            data->request = req;
            data->client_data = cli_data;

            switch (req->type) {
            case 3: thread_create(chunk_info, data); break;
            case 5: thread_create(response, data); break;
            default: break;
            }
        } else if(node->type == HANDLE_WORK) {
            work* w = node->work;
            thread_create(w->handler, client);
        }
    }

    return NULL;
}
