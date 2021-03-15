#ifndef NETWORK_UDP_SERVER_H
#define NETWORK_UDP_SERVER_H

struct udp_server;
typedef struct udp_server udp_server;

typedef void(*udp_server_request_handler)();

struct udp_server* udp_server_create(int port);

typedef struct udp_address {
    char* ip;
    int port;
} udp_address;

int udp_server_get_port(struct udp_server* server);
void udp_server_destroy(struct udp_server* server);
udp_address udp_server_receive(udp_server* server, char* buffer, unsigned long size);
void udp_servererminate(struct udp_server* server);
void udp_send_messageo_client(int client, const char* message);

#endif