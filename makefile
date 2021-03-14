all: cpeer cclient

cpeer:
	gcc \
	-I ./src \
	src/lib/chunks.c \
	src/lib/async.c \
	src/lib/buffer.c \
	src/lib/tcp_server.c \
	src/lib/tcp_client.c \
	src/lib/udp_server.c \
	src/lib/udp_client.c \
	src/lib/utils.c \
	src/peer/main.c \
	-o peer -lpthread -lm

cclient:
	gcc \
	-I ./src \
	src/lib/chunks.c \
	src/lib/async.c \
	src/lib/buffer.c \
	src/lib/tcp_server.c \
	src/lib/tcp_client.c \
	src/lib/udp_server.c \
	src/lib/udp_client.c \
	src/lib/utils.c \
	src/client/main.c \
	-o cliente -lpthread -lm

