#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib/chunks.h"
#include "lib/buffer.h"
#include "lib/udp_client.h"

int main(int argc, char *argv[]) {
    // Parse flags
    char* address = argv[1];
	char *ptr = strtok(address, ":");
    char* url = ptr;
    ptr = strtok(NULL, ":");
    int port = atoi(ptr);


    unsigned chunks_count = 0;
    ptr = strtok(argv[2], ",");

    chunks* chunks_head = NULL;

    while(ptr != NULL) {
        chunks_count++;

        chunks* c = chunks_create(atoi(ptr), "");
        chunks_head = chunks_add(chunks_head, c);

        ptr = strtok(NULL, ",");
    }


    // Create the Client
    udp_client* client = udp_client_create(url, port);    

    short hello_code = 1;
    unsigned hello_size = 2 + 2 + (2*chunks_count);
    buffer* hello_buff = buffer_create(hello_size);
    buffer_set(hello_buff, 0, &hello_code, 2);
    buffer_set(hello_buff, 2, &chunks_count, 2);
    
    int it = 0;
    chunks* tmp = chunks_head;
    while(tmp) {
        buffer_set(hello_buff, 4 + (it*2), &tmp->id, 2);
        tmp = tmp->next;
    }

    udp_client_send(client, hello_buff, hello_size);
    chunks_destroy(chunks_head);

    // Destroys
    udp_client_destroy(client);

    return 0;
}