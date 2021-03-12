#include <stdlib.h>
#include <stdio.h>

#include "lib/tcp_server.h"
#include "lib/udp_server.h"

int handler(tcp_connection* conn) {
    return 0;
}

int main(int argc, char *argv[]) {
    tcp_server* server = tcp_server_create(handler, atoi(argv[1]));
    tcp_server_start(server);
    tcp_server_destroy(server);
    return 0;
}