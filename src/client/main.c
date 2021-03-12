#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib/tcp_client.h"
#include "lib/udp_server.h"

int main(int argc, char *argv[]) {
    // Parse flags
    char* address = argv[1];
	char *ptr = strtok(address, ":");
    char* url = ptr;
    ptr = strtok(NULL, ":");
    int port = atoi(ptr);

    // Create the Client
    tcp_client* client = tcp_client_create(url, port);    


    // Destroy the Client
    tcp_client_destroy(client);
    return 0;
}