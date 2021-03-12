all: peer client

peer:
	gcc \
	-I ./src \
	src/lib/async.c \
	src/lib/tcp_server.c \
	src/lib/tcp_client.c \
	src/lib/udp_server.c \
	src/lib/udp_client.c \
	src/lib/utils.c \
	src/peer/main.c \
	-o peer -lpthread -lm

client:
	gcc \
	-I ./src \
	src/lib/async.c \
	src/lib/tcp_server.c \
	src/lib/tcp_client.c \
	src/lib/udp_server.c \
	src/lib/udp_client.c \
	src/lib/utils.c \
	src/client/main.c \
	-o cliente -lpthread -lm

