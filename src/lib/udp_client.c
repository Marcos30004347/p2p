#include "udp_client.h"
#include "utils.h"

#include <sys/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

struct udp_client;

typedef void(*client_handler)(struct udp_client*, char*);

struct udp_client {
	int blocking;
	int flags;
	unsigned long ms;
    // struct sockaddr_in6 server_adress;
    int client_fd;
	client_handler on_receive;
};

void udp_client_send_to(struct udp_client* client, udp_address address, void* message, int len) {
	struct addrinfo hints,*res=NULL;

	char sport[10];
	itoa(address.port, sport, 10);

	int rc = getaddrinfo(address.ip, sport, &hints, &res);

	if (rc != 0) {
		printf("Host not found --> %s\n", gai_strerror(rc));
		if (rc == EAI_SYSTEM)
		perror("getaddrinfo() failed");
	}

	// c->server_adress = *(struct sockaddr_in6*)res->ai_addr;

    sendto(client->client_fd, message, len, MSG_CONFIRM, (const struct sockaddr *) &res->ai_addr, sizeof(res->ai_addr)); 
}

udp_address udp_client_receive(udp_client* client, char* buffer, unsigned long size)  {
    socklen_t len;
    struct sockaddr addr;
    
    long resp = recvfrom(client->client_fd, buffer, size, MSG_WAITALL, &addr, &len);
    
    udp_address address;
    
    char* ips = inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr);

    address.port = ((struct sockaddr_in*)&addr)->sin_port;
    strcpy(address.ip, ips);

    return address;
}


udp_client* udp_client_create() {
    udp_client* c = (struct udp_client*)malloc(sizeof(udp_client));

	c->blocking = 0;
	c->flags = 0;
	c->ms = 0;

	c->client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(c->client_fd < 0){
        printf("Error while creating socket\n");
		exit(EXIT_FAILURE);
    }

	// struct in6_addr serveraddr;
	// struct addrinfo hints, *res=NULL;
	// memset(&hints, 0x00, sizeof(hints));
	
	// hints.ai_flags    = AI_NUMERICSERV;
	// hints.ai_family   = AF_UNSPEC;
	// hints.ai_socktype = SOCK_DGRAM;

	// if (inet_pton(AF_INET, url, &serveraddr))    /* valid IPv4 text address? */
	// {
	// 	hints.ai_family = AF_INET;
	// 	hints.ai_flags |= AI_NUMERICHOST;
	// }
	// else if(inet_pton(AF_INET6, url, &serveraddr))
	// {

	// 	hints.ai_family = AF_INET6;
	// 	hints.ai_flags |= AI_NUMERICHOST;
	// }

	// char sport[10];
	// itoa(port, sport, 10);

	// int rc = getaddrinfo(url, sport, &hints, &res);

	// if (rc != 0) {
	// 	printf("Host not found --> %s\n", gai_strerror(rc));
	// 	if (rc == EAI_SYSTEM)
	// 	perror("getaddrinfo() failed");
	// }

	// c->client_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	
	// if (c->client_fd == -1) { 
	// 	printf("socket creation failed...\n"); 
	// 	exit(0); 
	// }

	// c->server_adress = *(struct sockaddr_in6*)res->ai_addr;

	c->flags = fcntl(c->client_fd, F_SETFL, c->flags);
	return c;
}

void udp_client_destroy(udp_client* client) {
	free(client);
}

void udp_client_disconnect(udp_client* client) {
	close(client->client_fd);
}

void udp_client_set_timeout(struct udp_client* client, unsigned long ms) {
	client->blocking = 1;
	client->ms = ms;

	int flags = fcntl(client->client_fd, F_GETFL, 0);
	client->flags = flags;

	int result = fcntl(client->client_fd, F_SETFL , flags | O_NONBLOCK);

	if(result < 0) {
		printf("Erro em udp_client_set_timeout\n");
		exit(-1);
	}
}

void udp_client_remove_timeout(struct udp_client* client) {
	client->blocking = 0;

	int result = fcntl(client->client_fd, F_SETFL, client->flags);
	client->flags = result;

	if(result < 0) {
		printf("Erro em udp_client_remove_timeout\n");
		exit(-1);
	}
}

int udp_client_get_id(udp_client* client) {
	return client->client_fd;
}