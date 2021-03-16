#ifndef NETWORK_UDP_CLIENT_H
#define NETWORK_UDP_CLIENT_H

#include "udp.h"

struct udp_client;

typedef struct udp_client udp_client;

udp_client* udp_client_create();
void udp_client_send_to(struct udp_client* client, udp_address address, void* message, int len);
udp_address udp_client_receive(udp_client* client, char* buffer, unsigned long size);
void udp_client_destroy(udp_client* client);
void udp_client_disconnect(udp_client* client);
int udp_client_get_id(udp_client* client);

#endif